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

#ifndef SC_DMATRIX_H
#define SC_DMATRIX_H

/** \file sc_dmatrix.h
 * Routines to create and manipulate small dense matrices of double.
 * We use BLAS and LAPACK for more advanced linear algebra computations.
 */

#include <sc_blas.h>
#include <sc_containers.h>

SC_EXTERN_C_BEGIN;

/** This is the matrix object.  It can have its own storage or be a view. */
typedef struct sc_dmatrix
{
  double            **e;        /**< Array into the rows of the matrix. */
  sc_bint_t           m;        /**< Number of rows in this matrix. */
  sc_bint_t           n;        /**< Number of columns in this matrix. */
  int                 view;     /**< Boolean to indicate this is a view. */
}
sc_dmatrix_t;

/** Check whether a double array is free of NaN entries.
 * \param [in] darray   Array of doubles.
 * \param [in] nelem    Number of doubles in the array.
 * \return              Return false if at least one entry is NaN.
 */
int                 sc_darray_is_valid (const double *darray, size_t nelem);

/** Check whether the values in a double array are in a certain range.
 * \param [in] darray   Array of doubles.
 * \param [in] nelem    Number of doubles in the array.
 * \param [in] low      Lowest allowed value in the array.
 * \param [in] high     Highest allowed value in the array.
 * \return              Return false if at least one entry is out of range.
 */
int                 sc_darray_is_range (const double *darray, size_t nelem,
                                        double low, double high);

/** Calculate the memory used by a dmatrix.
 * \param [in] dmatrix     The dmatrix.
 * \return                 Memory used in bytes.
 */
size_t              sc_dmatrix_memory_used (sc_dmatrix_t * dmatrix);

/** Create a new uninitalized matrix object.
 * This function aborts on memory allocation errors.
 * \param [in] m            Number of rows.
 * \param [in] n            Number of columns.
 * \return                  A valid dmatrix object with uninitialized entries.
 */
sc_dmatrix_t       *sc_dmatrix_new (sc_bint_t m, sc_bint_t n);

/** Create a new matrix object with all entries set to zero.
 * This function aborts on memory allocation errors.
 * \param [in] m            Number of rows.
 * \param [in] n            Number of columns.
 * \return                  A valid dmatrix object storing all zeros.
 */
sc_dmatrix_t       *sc_dmatrix_new_zero (sc_bint_t m, sc_bint_t n);

/** Create a new matrix object with the same size and entries as another.
 * This function aborts on memory allocation errors.
 * \param [in] dmatrix      A valid dmatrix or view.
 * \return                  A valid dmatrix with size and entries of \b view.
 */
sc_dmatrix_t       *sc_dmatrix_clone (const sc_dmatrix_t * dmatrix);

/** Create a matrix view on an existing data array.
 * The data array must have been previously allocated and large enough.
 * The data array must not be deallocated while the view is in use.
 */
sc_dmatrix_t       *sc_dmatrix_new_data (sc_bint_t m, sc_bint_t n,
                                         double *data);

/** Create a matrix view on an existing sc_dmatrix_t.
 * The original matrix must have greater equal as many elements as the view.
 * The original matrix must not be destroyed or resized while view is in use.
 * \note            Currently, creating views of views is not safe.
 */
sc_dmatrix_t       *sc_dmatrix_new_view (sc_bint_t m, sc_bint_t n,
                                         sc_dmatrix_t * orig);

/** Create a matrix view on an existing sc_dmatrix_t.
 * The start of the view is offset by a number of rows.
 * The original matrix must have greater equal as many elements as view end.
 * The original matrix must not be destroyed or resized while view is in use.
 * \param [in] o    Number of rows that the view is offset.
 *                  Requires (o + m) * n <= orig->m * orig->n.
 * \param [in] m    Number of rows that the view shall have.
 * \param [in] n    Number of columns that the view shall have.
 * \param [in] orig     This valid matrix is viewed.
 * \return              A newly created mxn view onto \b orig.
 * \note            Currently, creating views of views is not safe.
 */
sc_dmatrix_t       *sc_dmatrix_new_view_offset (sc_bint_t o,
                                                sc_bint_t m, sc_bint_t n,
                                                sc_dmatrix_t * orig);

/** Create a matrix view onto one column of an existing sc_dmatrix_t.
 * \param [in] orig     This matrix must have at least one column.
 *                      Its \b jth column is returned as a view.
 * \param [in] j        Valid column index into \b orig.
 * \return              A matrix of as many rows as \b orig and one column
 *                      whose entries point at the jth column of \b orig.
 * \note            Currently, creating views of views is not safe.
 */
sc_dmatrix_t       *sc_dmatrix_new_view_column (sc_dmatrix_t * orig,
                                                sc_bint_t j);

/** Change a matrix view to point at a single column of another matrix.
 * \param [in,out] view     This must be a view and is modified in place.
 *                          It must have the same number of rows as \b orig.
 *                          On return, its number of columns will be one.
 * \param [in] orig         The \b jth column of this matrix is viewed.
 * \param [in] j            Valid column index into \b orig.
 * \note            Currently, creating views of views is not safe.
 */
void                sc_dmatrix_view_set_column (sc_dmatrix_t * view,
                                                sc_dmatrix_t * orig,
                                                sc_bint_t j);

/** Change a matrix view to point at a single row of another matrix.
 * \param [in,out] view     This must be a view and is modified in place.
 *                          It must have precisely one row.  On return,
 *                          its number of columns will match \b orig.
 * \param [in] orig         The \b jth row of this matrix is viewed.
 * \param [in] i            Valid row index into \b orig.
 * \note            Currently, creating views of views is not safe.
 */
void                sc_dmatrix_view_set_row (sc_dmatrix_t * view,
                                             sc_dmatrix_t * orig,
                                             sc_bint_t i);

/** Reshape a matrix to different m and n without changing m * n.
 */
void                sc_dmatrix_reshape (sc_dmatrix_t * dmatrix,
                                        sc_bint_t m, sc_bint_t n);

/** Change the matrix dimensions.
 * For views it must be known that the new size is permitted.
 * For non-views the data will be realloced if necessary.
 * The entries are unchanged to the minimum of the old and new sizes.
 */
void                sc_dmatrix_resize (sc_dmatrix_t * dmatrix,
                                       sc_bint_t m, sc_bint_t n);

/** Change the matrix dimensions, while keeping the subscripts in place, i.e.
 * dmatrix->e[i][j] will have the same value before and after, as long as
 * (i, j) is still a valid subscript.
 * This is not valid for views.
 * For non-views the data will be realloced if necessary.
 * The entries are unchanged to the minimum of the old and new sizes.
 */
void                sc_dmatrix_resize_in_place (sc_dmatrix_t * dmatrix,
                                                sc_bint_t m, sc_bint_t n);

/** Destroy a dmatrix and all allocated memory. */
void                sc_dmatrix_destroy (sc_dmatrix_t * dmatrix);

/** Check whether a dmatrix is free of NaN entries.
 * \return          true if the dmatrix does not contain any NaN entries.
 */
int                 sc_dmatrix_is_valid (const sc_dmatrix_t * A);

/** Check a square dmatrix for symmetry.
 * \param [in] A            This square dmatrix is checked for symmetry.
 * \param [in] tolerance    Measures the absolute value of the max difference.
 * \return                  true if and only if matrix is numerically symmetric.
 */
int                 sc_dmatrix_is_symmetric (const sc_dmatrix_t * A,
                                             double tolerance);

/** Set a matrix to all zero entries.
 * \param [in,out] dmatrix  Valid dmatrix whose entries are zero'd.
 */
void                sc_dmatrix_set_zero (sc_dmatrix_t * dmatrix);

/** Set all entries of a matrix to a constant.
 * \param [in,out] dmatrix  Valid dmatrix whose entries are set to \b value.
 * \param [in] value        This value is written into every entry of \b dmatrix.
 */
void                sc_dmatrix_set_value (sc_dmatrix_t * dmatrix,
                                          double value);

/** Perform element-wise multiplication with a scalar, X := alpha .* X.
 */
void                sc_dmatrix_scale (double alpha, sc_dmatrix_t * X);

/** Perform element-wise addition with a scalar, X := X + alpha.
 */
void                sc_dmatrix_shift (double alpha, sc_dmatrix_t * X);

/** Perform element-wise multipl. & addition w/ scalar, X := alpha .* X + beta.
 */
void                sc_dmatrix_scale_shift (double alpha, double beta,
                                            sc_dmatrix_t * X);

/** Perform element-wise divison with a scalar, X := alpha ./ X.
 */
void                sc_dmatrix_alphadivide (double alpha, sc_dmatrix_t * X);

/** Perform element-wise exponentiation with a scalar, X := X ^ alpha.
 */
void                sc_dmatrix_pow (double exponent, sc_dmatrix_t * X);

/** Perform element-wise absolute value, Y := fabs(X).
 */
void                sc_dmatrix_fabs (const sc_dmatrix_t * X,
                                     sc_dmatrix_t * Y);

/** Perform element-wise square root, Y := sqrt(X).
 */
void                sc_dmatrix_sqrt (const sc_dmatrix_t * X,
                                     sc_dmatrix_t * Y);

/** Extract the element-wise sign of a matrix, Y := (X >= 0 ? 1 : -1)
 */
void                sc_dmatrix_getsign (const sc_dmatrix_t * X,
                                        sc_dmatrix_t * Y);

/** Compare a matrix element-wise against a bound, Y := (X >= bound ? 1 : 0)
 */
void                sc_dmatrix_greaterequal (const sc_dmatrix_t * X,
                                             double bound, sc_dmatrix_t * Y);

/** Compare a matrix element-wise against a bound, Y := (X <= bound ? 1 : 0)
 */
void                sc_dmatrix_lessequal (const sc_dmatrix_t * X,
                                          double bound, sc_dmatrix_t * Y);

/** Assign element-wise maximum, Y_i := (X_i > Y_i ? X_i : Y_i)
 */
void                sc_dmatrix_maximum (const sc_dmatrix_t * X,
                                        sc_dmatrix_t * Y);

/** Assign element-wise minimum, Y_i := (X_i < Y_i ? X_i : Y_i)
 */
void                sc_dmatrix_minimum (const sc_dmatrix_t * X,
                                        sc_dmatrix_t * Y);

/** Perform element-wise multiplication, Y := Y .* X.
 */
void                sc_dmatrix_dotmultiply (const sc_dmatrix_t * X,
                                            sc_dmatrix_t * Y);

/** Perform element-wise division, Y := Y ./ X.
 */
void                sc_dmatrix_dotdivide (const sc_dmatrix_t * X,
                                          sc_dmatrix_t * Y);

/** Perform element-wise multiplication & addition, Y := A .* X + Y.
 */
void                sc_dmatrix_dotmultiply_add (const sc_dmatrix_t * A,
                                                const sc_dmatrix_t * X,
                                                sc_dmatrix_t * Y);

/** Copy one matrix into another.
 * \param [in] X        Matrix taken as a source.
 * \param [in,out] Y    Matrix of dimensions of \b X.
 *                      On output, its entries are set to X.
 */
void                sc_dmatrix_copy (const sc_dmatrix_t * X,
                                     sc_dmatrix_t * Y);

/** Copy one matrix transposed into another.
 * \param [in] X        Matrix taken as a source.
 * \param [in,out] Y    Matrix of dimensions of \b X transposed.
 *                      On output, its entries are set to X transposed.
 */
void                sc_dmatrix_transpose (const sc_dmatrix_t * X,
                                          sc_dmatrix_t * Y);

/*! \brief Matrix Matrix Add (AXPY)  \c Y := alpha X + Y
 */
void                sc_dmatrix_add (double alpha, const sc_dmatrix_t * X,
                                    sc_dmatrix_t * Y);

/** Perform matrix-vector multiplication Y = alpha * A * X + beta * Y.
 * The dimensions of A, X, and Y must be compatible.
 * \param [in] transa   Transpose operation for matrix A.
 * \param [in] transx   Transpose operation for matrix X.
 * \param [in] transy   Transpose operation for matrix Y.
 * \param [in] alpha    Factor for the matrix to multiply.
 * \param [in] A        Valid matrix or view.
 * \param [in] X        Column or row vector.
 * \param [in] beta     Factor for the original matrix.
 * \param [in] Y        Column or row vector.
 */
void                sc_dmatrix_vector (sc_trans_t transa,
                                       sc_trans_t transx,
                                       sc_trans_t transy,
                                       double alpha, const sc_dmatrix_t * A,
                                       const sc_dmatrix_t * X, double beta,
                                       sc_dmatrix_t * Y);

/** Matrix-matrix multiplication \c C := alpha * A * B + beta * C
 * The dimensions of A, B, and C must be compatible.
 * \param [in] transa   Transpose operation for matrix A.
 * \param [in] transb   Transpose operation for matrix B.
 * \param [in] alpha    Factor for the matrix to multiply.
 * \param [in] A        First matrix to multiply.
 * \param [in] B        Secend Matrix to multiply.
 * \param [in] beta     Factor for the original matrix.
 * \param [in,out] C    Matrix is modified in place.
 */
void                sc_dmatrix_multiply (sc_trans_t transa,
                                         sc_trans_t transb, double alpha,
                                         const sc_dmatrix_t * A,
                                         const sc_dmatrix_t * B, double beta,
                                         sc_dmatrix_t * C);

/** \brief Left Divide \c A \ \c B.
 * The matrices cannot have 0 rows or columns.
 * Solves  \c A \c C = \c B or \c A' \c C = \c B.
 *
 *   \param transa Use the transpose of \c A
 *   \param A matrix
 *   \param B matrix
 *   \param C matrix
 */
void                sc_dmatrix_ldivide (sc_trans_t transa,
                                        const sc_dmatrix_t * A,
                                        const sc_dmatrix_t * B,
                                        sc_dmatrix_t * C);

/** \brief Right Divide \c A / \c B.
 * The matrices cannot have 0 rows or columns.
 * Solves  \c A = \c C \c B or \c A = \c C \c B'.
 *
 *   \param transb Use the transpose of \c B
 *   \param A matrix
 *   \param B matrix
 *   \param C matrix
 */
void                sc_dmatrix_rdivide (sc_trans_t transb,
                                        const sc_dmatrix_t * A,
                                        const sc_dmatrix_t * B,
                                        sc_dmatrix_t * C);

/** \brief Solve B^T <- A^{-T} B^T.
 * This call is destructive on the entries of the matrix A.
 * Solving multiple right hand sides is supported.
 *
 *   \param[in,out] A   Square invertible matrix.  Values are changed.
 *                      Its transpose is inverted and applied to B^T.
 *   \param[in,out] B   Rectangular matrix with as many columns as A.
 *                      On input, each row is an independent right hand side.
 *                      On output, each row holds the corresponding solution.
 */
void                sc_dmatrix_solve_transpose_inplace
  (sc_dmatrix_t * A, sc_dmatrix_t * B);

/** \brief Writes a matrix to an opened stream.
 *
 *   \param dmatrix Pointer to matrix to write
 *   \param fp      Pointer to file to write to
 */
void                sc_dmatrix_write (const sc_dmatrix_t * dmatrix,
                                      FILE * fp);

/** The sc_dmatrix_pool recycles matrices of the same size. */
typedef struct sc_dmatrix_pool
{
  sc_bint_t           m;        /**< Number of rows of the matrices stored. */
  sc_bint_t           n;        /**< NUmber of columns of matrices stored. */
  size_t              elem_count;       /**< Number of matrices alive. */
  sc_array_t          freed;    /**< Buffer for the matrices returned. */
}
sc_dmatrix_pool_t;

/** Create a new dmatrix pool.
 * \param [in] m    Row count of the stored matrices.
 * \param [in] n    Column count of the stored matrices.
 * \return          Returns a dmatrix pool that is ready to use.
 */
sc_dmatrix_pool_t  *sc_dmatrix_pool_new (sc_bint_t m, sc_bint_t n);

/** Destroy a dmatrix pool.
 * This will also destroy all matrices stored for reuse.
 * Requires all allocated matrices to be returned to the pool previously.
 * \param [in,out] dmpool       The dmatrix pool to destroy.
 */
void                sc_dmatrix_pool_destroy (sc_dmatrix_pool_t * dmpool);

/** Allocate a dmatrix from the pool.
 * Reuses a matrix previously returned to the pool, or allocated a fresh one.
 * \param [in,out] dmpool   The dmatrix pool to use.
 * \return                  Returns a matrix of size dmpool->m by dmpool->n.
 */
sc_dmatrix_t       *sc_dmatrix_pool_alloc (sc_dmatrix_pool_t * dmpool);

/** Return a dmatrix to the pool.
 * The matrix is stored internally for reuse and not freed in this function.
 * \param [in] dmpool The dmatrix pool to use.
 * \param [in] dm     The dmatrix pool to return to the pool.
 */
void                sc_dmatrix_pool_free (sc_dmatrix_pool_t * dmpool,
                                          sc_dmatrix_t * dm);

/** Multithreaded workspace allocations of multiple blocks. */
typedef struct sc_darray_work
{
  double             *data;       /**< Entries of all blocks of all threads */
  int                 n_threads;  /**< Number of threads */
  int                 n_blocks;   /**< Number of blocks per thread */
  int                 n_entries;  /**< Number of entries per block */
}
sc_darray_work_t;

/** Create a new multithreaded workspace allocation object.
 * For each thread \c n_blocks of memory blocks with at least \c n_entries
 * double values are allocated.  The actual number of entries per block is
 * adjusted such that the base-pointer for each block is aligned to
 * \c alignment_bytes.  It is assumed that SC_ALLOC returns an aligned
 * base-pointer.  This function aborts on memory allocation errors.
 * \param [in] n_threads        Number of thread.
 * \param [in] n_blocks         Number of blocks per thread.
 * \param [in] n_entries        Minimum number of entries per block.
 * \param [in] alignment_bytes  Align blocks to this byte boundary.
 * \return                      A valid darray_work object.
 */
sc_darray_work_t   *sc_darray_work_new (const int n_threads,
                                        const int n_blocks,
                                        const int n_entries,
                                        const int alignment_bytes);

/** Destroy a darray_work object and all allocated memory. */
void                sc_darray_work_destroy (sc_darray_work_t * work);

/** Get workspace allocation of a certain thread and a specified block.
 * \param [in] work         Workspace taken as a source.
 * \param [in] thread       Valid thread index into \b work.
 * \param [in] block        Valid block index into \b work.
 * \return                  Pointer to entries (memory aligned).
 */
double             *sc_darray_work_get (sc_darray_work_t * work,
                                        const int thread, const int block);

/** Get the number of blocks per thread of a workspace allocation.
 * \param [in] work         Workspace taken as a source.
 * \return                  Number of allocated blocks per thread.
 */
int                 sc_darray_work_get_blockcount (sc_darray_work_t * work);

/** Get the number of entries per block of a workspace allocation.
 * \param [in] work         Workspace taken as a source.
 * \return                  Number of allocated entries per blocks per thread.
 */
int                 sc_darray_work_get_blocksize (sc_darray_work_t * work);

SC_EXTERN_C_END;

#endif /* !SC_DMATRIX_H */
