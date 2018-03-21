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

#ifndef SC_LAPACK_H
#define SC_LAPACK_H

#include <sc_blas.h>

SC_EXTERN_C_BEGIN;

typedef enum sc_jobz
{
  SC_EIGVALS_ONLY,
  SC_EIGVALS_AND_EIGVECS,
  SC_JOBZ_ANCHOR
}
sc_jobz_t;

extern const char   sc_jobzchar[];

#ifdef SC_WITH_LAPACK

#ifndef SC_F77_FUNC
#if defined(__bgq__)            /* && defined(__HAVE_ESSL) */
#define SC_F77_FUNC(small,CAPS) small
/* TODO - FIX THIS FOR NOW WE DO NOT USE ESSL
  #   define SC_F77_FUNC(small,CAPS) small ## _
*/
#define SC_F77_FUNC_NOESSL(small,CAPS) small
#else
#define SC_F77_FUNC(small,CAPS) small ## _
#endif
#endif /* SC_F77_FUNC */

#define SC_LAPACK_DGELS   SC_F77_FUNC(dgels,DGELS)
#define SC_LAPACK_DGESV   SC_F77_FUNC(dgesv,DGESV)
#define SC_LAPACK_DGETRF  SC_F77_FUNC(dgetrf,DGETRF)
#define SC_LAPACK_DGETRS  SC_F77_FUNC(dgetrs,DGETRS)
#if defined(__bgq__)            /* && define(__HAVE_ESSL) */
#define SC_LAPACK_DSTEV   SC_F77_FUNC_NOESSL(dstev,DSTEV)
#else
#define SC_LAPACK_DSTEV   SC_F77_FUNC(dstev,DSTEV)
#endif
#define SC_LAPACK_DTRSM   SC_F77_FUNC(dtrsm,DTRSM)
#define SC_LAPACK_DLAIC1  SC_F77_FUNC(dlaic1,DLAIC1)
#define SC_LAPACK_ILAENV  SC_F77_FUNC(ilaenv,ILAENV)

void                SC_LAPACK_DGELS (const char *trans,
                                     const sc_bint_t * m, const sc_bint_t * n,
                                     const sc_bint_t * nrhs, double *a,
                                     const sc_bint_t * lda, double *b,
                                     const sc_bint_t * ldb, double *work,
                                     const sc_bint_t * lwork,
                                     sc_bint_t * info);

void                SC_LAPACK_DGESV (const sc_bint_t * n,
                                     const sc_bint_t * nrhs,
                                     double *a, const sc_bint_t * lda,
                                     sc_bint_t * ipiv,
                                     double *b, const sc_bint_t * ldb,
                                     sc_bint_t * info);

void                SC_LAPACK_DGETRF (const sc_bint_t * m,
                                      const sc_bint_t * n, double *a,
                                      const sc_bint_t * lda, sc_bint_t * ipiv,
                                      sc_bint_t * info);

void                SC_LAPACK_DGETRS (const char *trans, const sc_bint_t * n,
                                      const sc_bint_t * nrhs, const double *a,
                                      const sc_bint_t * lda,
                                      const sc_bint_t * ipiv, double *b,
                                      const sc_bint_t * ldx,
                                      sc_bint_t * info);

void                SC_LAPACK_DSTEV (const char *jobz,
                                     const sc_bint_t * n,
                                     double *d,
                                     double *e,
                                     double *z,
                                     const sc_bint_t * ldz,
                                     double *work, sc_bint_t * info);

void                SC_LAPACK_DTRSM (const char *side,
                                     const char *uplo,
                                     const char *transa,
                                     const char *diag,
                                     const sc_bint_t * m,
                                     const sc_bint_t * n,
                                     const double *alpha,
                                     const double *a,
                                     const sc_bint_t * lda,
                                     const double *b, const sc_bint_t * ldb);

void                SC_LAPACK_DLAIC1 (const int *job,
                                      const int *j,
                                      const double *x,
                                      const double *sest,
                                      const double *w,
                                      const double *gamma,
                                      double *sestpr, double *s, double *c);

int                 SC_LAPACK_ILAENV (const sc_bint_t * ispec,
                                      const char *name,
                                      const char *opts,
                                      const sc_bint_t * N1,
                                      const sc_bint_t * N2,
                                      const sc_bint_t * N3,
                                      const sc_bint_t * N4,
                                      sc_buint_t name_length,
                                      sc_buint_t opts_length);

#else /* !SC_WITH_LAPACK */

#define SC_LAPACK_DGELS    (void) sc_lapack_nonimplemented
#define SC_LAPACK_DGESV    (void) sc_lapack_nonimplemented
#define SC_LAPACK_DGETRF   (void) sc_lapack_nonimplemented
#define SC_LAPACK_DGETRS   (void) sc_lapack_nonimplemented
#define SC_LAPACK_DSTEV    (void) sc_lapack_nonimplemented
#define SC_LAPACK_DTRSM    (void) sc_lapack_nonimplemented
#define SC_LAPACK_DLAIC1   (void) sc_lapack_nonimplemented
#define SC_LAPACK_ILAENV   (int)  sc_lapack_nonimplemented

int                 sc_lapack_nonimplemented (SC_NOARGS);

#endif

SC_EXTERN_C_END;

#endif /* !SC_LAPACK_H */
