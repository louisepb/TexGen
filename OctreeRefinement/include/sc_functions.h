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

#ifndef SC_FUNCTIONS_H
#define SC_FUNCTIONS_H

#include <sc.h>

SC_EXTERN_C_BEGIN;

/** Integer power routine.
 * Found in github.com:bfam/bfam.git, and originally in
 * http://stackoverflow.com/questions/101439/\
 * the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
 *
 * \param [in] base         This integer is taken to the power of \exp.
 *                          It may be negative as well.
 * \param [in] exp          This non-negative integer is the exponent.
 * \return                  We compute \b base ** \b exp.
 */
int                 sc_intpow (int base, int exp);

/* Power routine for 64-bit integers.
 * \see sc_intpow.
 * \param [in] base         This integer is taken to the power of \exp.
 *                          It may be negative as well.
 * \param [in] exp          This non-negative integer is the exponent.
 * \return                  We compute \b base ** \b exp.
 */
int64_t             sc_intpow64 (int64_t base, int exp);

/* Power routine for unsigned 64-bit integers.
 * \see sc_intpow.
 * \param [in] base         This integer is taken to the power of \exp.
 * \param [in] exp          This non-negative integer is the exponent.
 * \return                  We compute \b base ** \b exp.
 */
uint64_t            sc_intpow64u (uint64_t base, int exp);

typedef double      (*sc_function1_t) (double x, void *data);

typedef double      (*sc_function3_t) (double x, double y, double z,
                                       void *data);

/*
 * this structure is used as data element for the meta functions.
 * for _sum and _product:
 * f1 needs to be a valid function.
 * f2 can be a function, then it is used,
 *    or NULL, in which case parameter2 is used.
 * for _tensor: f1, f2, f3 need to be valid functions.
 */
typedef struct sc_function3_meta
{
  sc_function3_t      f1;
  sc_function3_t      f2;
  double              parameter2;
  sc_function3_t      f3;
  void               *data;
}
sc_function3_meta_t;

/* Evaluate the inverse function with regula falsi: x = func^{-1}(y) */
double              sc_function1_invert (sc_function1_t func, void *data,
                                         double x_low, double x_high,
                                         double y, double rtol);

/** Seed the random number generator differently on each process.
 * Seeds each process with seed and mpirank from sc_MPI_COMM_WORLD.
 *    ( mpirank + seed * large_prime )
 *
 * \param [in] seed Seed for random number generator, calls srand ().
 */
void                sc_srand (unsigned int seed);

/** Seed the random number generator differently on each process.
 * Seeds each process with time and mpirank from sc_MPI_COMM_WORLD.
 *    ( time + mpirank * small_prime )
 */
void                sc_srand_time ();

/** Sample a uniform value from [0,1) via rand ().
 *
 * \return    randum number from uniform distribution on [0,1)
 */
double              sc_rand_uniform (void);

/** Sample a (gaussian) standard normal distribution.
 * Implements polar form of the Box Muller transform based on rand ().
 *
 * \return    random number from a univariate standard normal distribution
 */
double              sc_rand_normal (void);

/* Some basic 3D functions */
double              sc_zero3 (double x, double y, double z, void *data);
double              sc_one3 (double x, double y, double z, void *data);
double              sc_two3 (double x, double y, double z, void *data);
double              sc_ten3 (double x, double y, double z, void *data);

/**
 * \param data   needs to be *double with the value of the constant.
 */
double              sc_constant3 (double x, double y, double z, void *data);

double              sc_x3 (double x, double y, double z, void *data);
double              sc_y3 (double x, double y, double z, void *data);
double              sc_z3 (double x, double y, double z, void *data);

double              sc_sum3 (double x, double y, double z, void *data);
double              sc_product3 (double x, double y, double z, void *data);
double              sc_tensor3 (double x, double y, double z, void *data);

SC_EXTERN_C_END;

#endif /* !SC_FUNCTIONS_H */
