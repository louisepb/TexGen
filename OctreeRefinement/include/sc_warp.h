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

#ifndef SC_WARP_H
#define SC_WARP_H

#include <sc.h>

typedef struct sc_warp_interval sc_warp_interval_t;

struct sc_warp_interval
{
  int                 level;    /* level of root is 0 */
  double              r_low, r_high;    /* interval coordinates */
  sc_warp_interval_t *left, *right;     /* binary tree descendants */
};

SC_EXTERN_C_BEGIN;

sc_warp_interval_t *sc_warp_new (double r_low, double r_high);
void                sc_warp_destroy (sc_warp_interval_t * root);

/** Refine the warp as necessary to accomodate a set of new points.
 * \param [in] root             The root warp interval.
 * \param [in] num_points       Number of new points to integrate.
 * \param [in] r_points         The new points need to be sorted.
 */
void                sc_warp_update (sc_warp_interval_t * root,
                                    int num_points, double *r_points,
                                    double r_tol, int max_level);
void                sc_warp_print (int package_id, int log_priority,
                                   sc_warp_interval_t * root);
void                sc_warp_write (sc_warp_interval_t * root, FILE * nout);

SC_EXTERN_C_END;

#endif /* !SC_WARP_H */
