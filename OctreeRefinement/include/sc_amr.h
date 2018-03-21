/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System
  Additional copyright (C) 2011 individual authors

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef SC_AMR_H
#define SC_AMR_H

#include <sc_statistics.h>

SC_EXTERN_C_BEGIN;

typedef struct sc_amr_control
{
  const double       *errors;
  sc_statinfo_t       estats;
  sc_MPI_Comm         mpicomm;
  long                num_procs_long;
  long                num_total_elements;
  double              coarsen_threshold;
  double              refine_threshold;
  long                num_total_coarsen;
  long                num_total_refine;
  long                num_total_estimated;
}
sc_amr_control_t;

/** Compute global error statistics.
 * \param [in] mpicomm        MPI communicator to use.
 * \param [in] mpisize        Number of MPI processes in this communicator.
 * \param [in] num_local_elements   Number of local elements.
 * \param [in] errors         The error values, one per local element.
 * \param [out] amr           Structure will be initialized and estats filled.
 */
void                sc_amr_error_stats (sc_MPI_Comm mpicomm,
                                        long num_local_elements,
                                        const double *errors,
                                        sc_amr_control_t * amr);

/** Count the local number of elements that will be coarsened.
 *
 * This is all elements whose error is below threshold
 * and where there are no other conditions preventing coarsening
 * (such as not all siblings may be coarsened or are on another processor).
 *
 * \return          Returns the net loss of local elements.
 */
typedef long        (*sc_amr_count_coarsen_fn) (sc_amr_control_t * amr,
                                                void *user_data);

/** Count the local number of elements that will be refined.
 *
 * This is all elements whose error is above the threshold
 * and where there are no other conditions preventing refinement
 * (such as refinement would not make the element too small).
 *
 * \return          Returns the net gain of local elements.
 */
typedef long        (*sc_amr_count_refine_fn) (sc_amr_control_t * amr,
                                               void *user_data);

/** Specify a coarsening threshold and count the expected elements.
 *
 * \param [in] package_id               Registered package id or -1.
 * \param [in,out] amr                  AMR control structure.
 * \param [in] coarsen_threshold        Coarsening threshold.
 * \param [in] cfn                      Callback to count local coarsenings.
 * \param [in] user_data                Will be passed to the cfn callback.
 */
void                sc_amr_coarsen_specify (int package_id,
                                            sc_amr_control_t * amr,
                                            double coarsen_threshold,
                                            sc_amr_count_coarsen_fn cfn,
                                            void *user_data);

/** Binary search for coarsening threshold without refinement.
 *
 * \param [in] package_id               Registered package id or -1.
 * \param [in,out] amr                  AMR control structure.
 * \param [in] num_total_ideal          Target number of global elements.
 * \param [in] coarsen_threshold_high   Upper bound on the error indicator.
 * \param [in] target_window            Relative target window (< 1).
 * \param [in] max_binary_steps         Upper bound on binary search steps.
 * \param [in] cfn                      Callback to count local coarsenings.
 * \param [in] user_data                Will be passed to the cfn callback.
 */
void                sc_amr_coarsen_search (int package_id,
                                           sc_amr_control_t * amr,
                                           long num_total_ideal,
                                           double coarsen_threshold_high,
                                           double target_window,
                                           int max_binary_steps,
                                           sc_amr_count_coarsen_fn cfn,
                                           void *user_data);

/** Binary search for refinement threshold without coarsening.
 *
 * \param [in] package_id               Registered package id or -1.
 * \param [in,out] amr                  AMR control structure.
 * \param [in] num_total_ideal          Target number of global elements.
 * \param [in] refine_threshold_low     Lower bound on the error indicator.
 * \param [in] target_window            Relative target window (< 1).
 * \param [in] max_binary_steps         Upper bound on binary search steps.
 * \param [in] rfn                      Callback to count local refinements.
 * \param [in] user_data                Will be passed to the rfn callback.
 */
void                sc_amr_refine_search (int package_id,
                                          sc_amr_control_t * amr,
                                          long num_total_ideal,
                                          double refine_threshold_low,
                                          double target_window,
                                          int max_binary_steps,
                                          sc_amr_count_refine_fn rfn,
                                          void *user_data);

SC_EXTERN_C_END;

#endif /* !SC_AMR_H */
