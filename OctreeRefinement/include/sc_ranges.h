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

#ifndef SC_RANGES_H
#define SC_RANGES_H

#include <sc.h>

SC_EXTERN_C_BEGIN;

/** Compute the optimal ranges of processors to talk to.
 *
 * \param [in] package_id   Registered package id or -1.
 * \param [in] num_procs    Number of processors processed.
 * \param [in] procs        Array [num_procs] interpreted as booleans.
 *                          Nonzero entries need to be talked to.
 * \param [in] rank         The id of the calling process.
 *                          Will be excluded from the ranges.
 * \param [in] first_peer   First processor to be considered.
 * \param [in] last_peer    Last processor to be considered (inclusive).
 * \param [in] num_ranges   The maximum number of ranges to fill.
 * \param [in,out] ranges   Array [2 * num_ranges] that will be filled
 *                          with beginning and ending procs (inclusive)
 *                          that represent each range.  Values of -1
 *                          indicate that the range is not needed.
 * \return                  Returns the number of filled ranges.
 */
int                 sc_ranges_compute (int package_id, int num_procs,
                                       const int *procs, int rank,
                                       int first_peer, int last_peer,
                                       int num_ranges, int *ranges);

/** Compute the globally optimal ranges of processors.
 *
 * \param [in] package_id   Registered package id or -1.
 * \param [in] mpicomm      MPI Communicator for Allreduce and Allgather.
 * \param [in] procs        Same as in sc_ranges_compute ().
 * \param [in,out] inout1
 *     On input, first_peer as in sc_ranges_compute ().
 *     On output, global maximum of peer counts.
 * \param [in,out] inout2
 *     On input, last_peer as in sc_ranges_compute ().
 *     On output, global maximum number of ranges.
 * \param [in] num_ranges   The maximum number of ranges to fill.
 * \param [in,out] ranges   Array [2 * num_ranges] that will be filled
 *                          with beginning and ending procs (inclusive)
 *                          that represent each local range.  Values of -1
 *                          indicate that the range is not needed.
 * \param [out] global_ranges
 *     If not NULL, will be allocated and filled with everybody's ranges.
 *     Size will be 2 * inout2 * num_procs.  Must be freed with SC_FREE ().
 * \return                  Returns the number of locally filled ranges.
 */
int                 sc_ranges_adaptive (int package_id, sc_MPI_Comm mpicomm,
                                        const int *procs,
                                        int *inout1, int *inout2,
                                        int num_ranges, int *ranges,
                                        int **global_ranges);

/** Determine an array of receivers and an array of senders from ranges.
 * This function is intended for compatibility and debugging only.
 * In particular, sc_ranges_adaptive may include non-receiving processors.
 * It is generally more efficient to use sc_notify instead of sc_ranges.
 *
 * \param [in] num_procs    The number of parallel processors (aka mpisize).
 * \param [in] rank         Number of this processors (aka mpirank).
 *                          Rank is excluded from receiver and sender output.
 * \param [in] max_ranges   Global maximum of filled range windows as
 *                          returned in inout2 by sc_ranges_adaptive.
 * \param [in] global_ranges    All processor ranges from sc_ranges_adaptive.
 * \param [out] num_receivers   Number of receiver ranks.  Greater/equal to
 *                              number of nonzero procs in sc_ranges_compute.
 * \param [in,out] receiver_ranks   Array of at least mpisize for output.
 * \param [out] num_senders         Number of senders to this processor.
 * \paarm [in,out] sender_ranks     Array of at least mpisize for output.
 */
void                sc_ranges_decode (int num_procs, int rank,
                                      int max_ranges,
                                      const int *global_ranges,
                                      int *num_receivers, int *receiver_ranks,
                                      int *num_senders, int *sender_ranks);

/** Compute global statistical information on the ranges.
 *
 * \param [in] package_id       Registered package id or -1.
 * \param [in] log_priority     Priority to use for logging.
 */
void                sc_ranges_statistics (int package_id, int log_priority,
                                          sc_MPI_Comm mpicomm, int num_procs,
                                          const int *procs, int rank,
                                          int num_ranges, int *ranges);

SC_EXTERN_C_END;

#endif /* !SC_RANGES_H */
