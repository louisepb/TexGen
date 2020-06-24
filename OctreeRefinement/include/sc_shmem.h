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

#ifndef SC_SHMEM_H
#define SC_SHMEM_H

#include <sc.h>
#include <sc_mpi.h>

SC_EXTERN_C_BEGIN;

/** \file sc_shmem.h */

/* sc_shmem: an interface for arrays that are redundant on each mpi
 * process */

typedef enum
{
  SC_SHMEM_BASIC = 0,      /**< use allgathers, then sum to simulate scan */
  SC_SHMEM_PRESCAN,        /**< mpi_scan, then allgather */
#if defined(SC_ENABLE_MPIWINSHARED)
  SC_SHMEM_WINDOW,         /**< MPI_Win (requires MPI 3) */
  SC_SHMEM_WINDOW_PRESCAN, /**< mpi_scan, then MPI_Win (requires MPI 3) */
#endif
#if defined(__bgq__)
  SC_SHMEM_BGQ,            /**< raw pointer passing: only works for
                                shared-heap environments */
  SC_SHMEM_BGQ_PRESCAN,    /**< mpi_scan, then raw pointer passing: only works
                                for shared-heap environments */
#endif
  SC_SHMEM_NUM_TYPES,
  SC_SHMEM_NOT_SET
}
sc_shmem_type_t;

extern const char  *sc_shmem_type_to_string[SC_SHMEM_NUM_TYPES];

extern sc_shmem_type_t sc_shmem_default_type;

/* ALL sc_shmem routines should be considered collective: called on
 * every process in the communicator */

/** Set the type of shared memory arrays to use on this mpi communicator.
 *
 * \param[in,out] comm        the mpi communicator
 * \param[in]     type        the type of shmem array behavior.
 */
void                sc_shmem_set_type (sc_MPI_Comm comm,
                                       sc_shmem_type_t type);

/** Get the type of shared memory arrays to use on this mpi communicator.
 *
 * \param[in] comm        the mpi communicator
 *
 * \return the type of shmem array used on this communicator.
 */
sc_shmem_type_t     sc_shmem_get_type (sc_MPI_Comm comm);

/** Allocate a shmem array: an array that is redundant on every process.
 *
 * \param[in] package         package requesting memory
 * \param[in] elem_size       the size of each element in the array
 * \param[in] elem_count      the number of elements in the array
 * \param[in] comm            the mpi communicator
 *
 * \return a shared memory array
 * */
void               *sc_shmem_malloc (int package, size_t elem_size,
                                     size_t elem_count, sc_MPI_Comm comm);

#define SC_SHMEM_ALLOC(t,e,c) (t *) sc_shmem_malloc(sc_package_id,sizeof(t),e,c)

/** Destroy a shmem array created with sc_shmem_alloc()
 *
 * \param[in] package         package freeing memory
 * \param[in] array           array to be freed
 * \param[in] comm            the mpi communicator
 *
 * */
void                sc_shmem_free (int package, void *array,
                                   sc_MPI_Comm comm);

#define SC_SHMEM_FREE(a,c) sc_shmem_free (sc_package_id,a,c)

/** Start a write window for a shared array.
 *
 * \param[in] array           array that will be changed.
 * \param[in] comm            the mpi communicator
 *
 * \return 1 if I have write access, 0 if my proc should not change the
 * array.
 */
int                 sc_shmem_write_start (void *array, sc_MPI_Comm comm);

/** End a write window for a shared array.
 *
 * \param[in] array           array that has changed
 * \param[in] comm            the mpi communicator
 */
void                sc_shmem_write_end (void *array, sc_MPI_Comm comm);

/** Copy a shmem array.
 *
 * \param[out]  destarray     array to write to
 * \param[in]   srcarray      array to write from
 * \param[in]   bytes         number of bytes to write
 * \param[in]   comm          the mpi communicator
 */
void                sc_shmem_memcpy (void *destarray, void *srcarray,
                                     size_t bytes, sc_MPI_Comm comm);

/** Fill a shmem array with an allgather.
 *
 * \param[in] sendbuf         the source from this process
 * \param[in] sendcount       the number of items to allgather
 * \param[in] sendtype        the type of items to allgather
 * \param[in,out] recvbuf     the destination shmem array
 * \param[in] recvcount       the number of items to allgather
 * \param[in] recvtype        the type of items to allgather
 * \param[in] comm            the mpi communicator
 */
void                sc_shmem_allgather (void *sendbuf, int sendcount,
                                        sc_MPI_Datatype sendtype,
                                        void *recvbuf, int recvcount,
                                        sc_MPI_Datatype recvtype,
                                        sc_MPI_Comm comm);

/** Fill a shmem array with an allgather of the prefix op over all processes.
 *
 * The return array will be
 * (0, send0, send0 op send1, send0 op send1 op send2, ...)
 *
 * Note that the first entry of \a recvbuf will be set to 0 using memset: if
 * this is not the desired value for the first entry of the array, the user
 * can change it *after* calling sc_shmem_prefix.
 *
 * \param[in] sendbuf         the source from this process
 * \param[in,out] recvbuf     the destination shmem array
 * \param[in] count           the number of items to allgather
 * \param[in] type            the type of items to allgather
 * \param[in] op              the operation to prefix (e.g., sc_MPI_SUM)
 * \param[in] comm            the mpi communicator
 */
void                sc_shmem_prefix (void *sendbuf, void *recvbuf,
                                     int count, sc_MPI_Datatype type,
                                     sc_MPI_Op op, sc_MPI_Comm comm);
SC_EXTERN_C_END;

#endif /* SC_SHMEM_H */
