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

#ifndef SC_BLAS_H
#define SC_BLAS_H

#include <sc.h>
#if defined(__bgq__)
/* TODO - FOR NOW WE DO NOT USE ESSL
  # include <essl.h>
*/
#endif

SC_EXTERN_C_BEGIN;

typedef int         sc_bint_t;  /* Integer type for all of the blas calls */
typedef unsigned int sc_buint_t;        /* Unsigned Integer type for blas */

typedef enum sc_trans
{
  SC_NO_TRANS,
  SC_TRANS,
  SC_TRANS_ANCHOR
}
sc_trans_t;

typedef enum sc_uplo
{
  SC_UPPER,
  SC_LOWER,
  SC_UPLO_ANCHOR
}
sc_uplo_t;

typedef enum sc_cmach
{
  SC_CMACH_EPS,                 /* relative machine precision */
  SC_CMACH_SFMIN,               /* safe minimum, such that 1/sfmin does not
                                 * overflow
                                 */
  SC_CMACH_BASE,                /* base of the machine */
  SC_CMACH_PREC,                /* eps*base */
  SC_CMACH_T,                   /* number of (base) digits in the mantissa */
  SC_CMACH_RND,                 /* 1.0 when rounding occurs in addition,
                                 * 0.0 otherwise
                                 */
  SC_CMACH_EMIN,                /* minimum exponent before (gradual)
                                 * underflow
                                 */
  SC_CMACH_RMIN,                /* underflow threshold - base**(emin-1) */
  SC_CMACH_EMAX,                /* largest exponent before overflow */
  SC_CMACH_RMAX,                /* overflow threshold  - (base**emax)*(1-eps) */
  SC_CMACH_ANCHOR
}
sc_cmach_t;

extern const char   sc_transchar[];
extern const char   sc_antitranschar[]; /* does not work for complex */
extern const char   sc_uplochar[];
extern const char   sc_cmachchar[];

#ifdef SC_WITH_BLAS

#ifndef SC_F77_FUNC
#if defined(__bgq__)            /* && defined(__HAVE_ESSL) */
#define SC_F77_FUNC(small,CAPS) small
/* TODO - For now we do not use ESSL
  #   define SC_F77_FUNC(small,CAPS) small ## _
*/
#define SC_F77_FUNC_NOESSL(small,CAPS) small
#else
#define SC_F77_FUNC(small,CAPS) small ## _
#endif
#endif /* SC_F77_FUNC */

#define SC_BLAS_DLAMCH  SC_F77_FUNC(dlamch,DLAMCH)
#define SC_BLAS_DSCAL   SC_F77_FUNC(dscal,DSCAL)
#define SC_BLAS_DCOPY   SC_F77_FUNC(dcopy,DCOPY)
#define SC_BLAS_DAXPY   SC_F77_FUNC(daxpy,DAXPY)
#define SC_BLAS_DDOT    SC_F77_FUNC(ddot,DDOT)
#define SC_BLAS_DGEMV   SC_F77_FUNC(dgemv,DGEMV)
#define SC_BLAS_DGEMM   SC_F77_FUNC(dgemm,DGEMM)

double              SC_BLAS_DLAMCH (const char *cmach);
void                SC_BLAS_DSCAL (const sc_bint_t * n, const double *alpha,
                                   double *X, const sc_bint_t * incx);
void                SC_BLAS_DCOPY (const sc_bint_t * n,
                                   const double *X, const sc_bint_t * incx,
                                   double *Y, const sc_bint_t * incy);
void                SC_BLAS_DAXPY (const sc_bint_t * n, const double *alpha,
                                   const double *X, const sc_bint_t * incx,
                                   double *Y, const sc_bint_t * incy);
double              SC_BLAS_DDOT (const sc_bint_t * n, const double *X,
                                  const sc_bint_t * incx, const double *Y,
                                  const sc_bint_t * incy);

void                SC_BLAS_DGEMV (const char *transa, const sc_bint_t * m,
                                   const sc_bint_t * n, const double *alpha,
                                   const double *a, const sc_bint_t * lda,
                                   const double *x, const sc_bint_t * incx,
                                   const double *beta, double *y,
                                   const sc_bint_t * incy);

void                SC_BLAS_DGEMM (const char *transa, const char *transb,
                                   const sc_bint_t * m, const sc_bint_t * n,
                                   const sc_bint_t * k, const double *alpha,
                                   const double *a, const sc_bint_t * lda,
                                   const double *b, const sc_bint_t * ldb,
                                   const double *beta, double *c,
                                   const sc_bint_t * ldc);

#else /* !SC_WITH_BLAS */

#define SC_BLAS_DLAMCH (double) sc_blas_nonimplemented
#define SC_BLAS_DSCAL  (void)   sc_blas_nonimplemented
#define SC_BLAS_DCOPY  (void)   sc_blas_nonimplemented
#define SC_BLAS_DAXPY  (void)   sc_blas_nonimplemented
#define SC_BLAS_DDOT   (double) sc_blas_nonimplemented
#define SC_BLAS_DGEMM  (void)   sc_blas_nonimplemented
#define SC_BLAS_DGEMV  (void)   sc_blas_nonimplemented

int                 sc_blas_nonimplemented (SC_NOARGS);

#endif

SC_EXTERN_C_END;

#endif /* !SC_BLAS_H */
