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

#ifndef SC_FLOPS_H
#define SC_FLOPS_H

#include <sc.h>

SC_EXTERN_C_BEGIN;

typedef struct sc_flopinfo
{
  double              seconds;  /* current time from sc_MPI_Wtime */

  /* these variables measure onward from from sc_flops_start */
  double              cwtime;   /* cumulative wall time */
  float               crtime;   /* cumulative real time */
  float               cptime;   /* cumulative process time */
  long long           cflpops;  /* cumulative floating point operations */

  /* measure since sc_flops_start or the previous sc_flops_count */
  double              iwtime;   /* interval wall time */
  float               irtime;   /* interval real time */
  float               iptime;   /* interval process time */
  long long           iflpops;  /* interval floating point operations */
  float               mflops;   /* MFlop/s rate in this interval */

  /* without SC_PAPI only seconds, ?wtime and ?rtime are meaningful */
}
sc_flopinfo_t;

/**
 * Calls PAPI_flops.  Aborts on PAPI error.
 * The first call sets up the performance counters.
 * Subsequent calls return cumulative real and process times,
 * cumulative floating point operations and the flop rate since the last call.
 */
void                sc_flops_papi (float *rtime, float *ptime,
                                   long long *flpops, float *mflops);

/**
 * Prepare sc_flopinfo_t structure and start flop counters.
 * Must only be called once during the program run.
 * This function calls sc_flops_papi.
 *
 * \param [out] fi  Members will be initialized.
 */
void                sc_flops_start (sc_flopinfo_t * fi);

/**
 * Update sc_flopinfo_t structure with current measurement.
 * Must only be called after sc_flops_start.
 * Can be called any number of times.
 * This function calls sc_flops_papi.
 *
 * \param [in,out] fi   Members will be updated.
 */
void                sc_flops_count (sc_flopinfo_t * fi);

/**
 * Call sc_flops_count (fi) and copies fi into snapshot.
 *
 * \param [in,out] fi       Members will be updated.
 * \param [out] snapshot    On output is a copy of fi.
 */
void                sc_flops_snap (sc_flopinfo_t * fi,
                                   sc_flopinfo_t * snapshot);

/**
 * Call sc_flops_count (fi) and override snapshot interval timings
 * with the differences since the previous call to sc_flops_snap.
 * The interval mflop rate is computed by iflpops / 1e6 / irtime.
 * The cumulative timings in snapshot are copied form fi.
 *
 * \param [in,out] fi       Members will be updated.
 * \param [in,out] snapshot Interval timings measured since sc_flops_snap.
 */
void                sc_flops_shot (sc_flopinfo_t * fi,
                                   sc_flopinfo_t * snapshot);

/**
 * Call sc_flops_count (fi) and work on all arguments in the list
 * of type sc_flopinfo_t * as in sc_flops_shot.  Last argument must be NULL.
 */
void                sc_flops_shotv (sc_flopinfo_t * fi, ...);

SC_EXTERN_C_END;

#endif /* !SC_FLOPS_H */
