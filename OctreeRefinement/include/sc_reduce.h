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

#ifndef SC_REDUCE_H
#define SC_REDUCE_H

#include <sc.h>

/* highest level that uses all-to-all instead of recursion */
#ifndef SC_REDUCE_ALLTOALL_LEVEL
#define SC_REDUCE_ALLTOALL_LEVEL        3
#endif

SC_EXTERN_C_BEGIN;

typedef void        (*sc_reduce_t) (void *sendbuf, void *recvbuf,
                                    int sendcount, sc_MPI_Datatype sendtype);

/** Custom allreduce operation.
 */
int                 sc_allreduce_custom (void *sendbuf, void *recvbuf,
                                         int sendcount,
                                         sc_MPI_Datatype sendtype,
                                         sc_reduce_t reduce_fn,
                                         sc_MPI_Comm mpicomm);

/** Custom reduce operation.
 * \param [in] target   The MPI rank that obtains the result.
 */
int                 sc_reduce_custom (void *sendbuf, void *recvbuf,
                                      int sendcount, sc_MPI_Datatype sendtype,
                                      sc_reduce_t reduce_fn,
                                      int target, sc_MPI_Comm mpicomm);

/** Drop-in MPI_Allreduce replacement.
 */
int                 sc_allreduce (void *sendbuf, void *recvbuf, int sendcount,
                                  sc_MPI_Datatype sendtype,
                                  sc_MPI_Op operation, sc_MPI_Comm mpicomm);

/** Drop-in MPI_Reduce replacement.
 * \param [in] target   The MPI rank that obtains the result.
 */
int                 sc_reduce (void *sendbuf, void *recvbuf, int sendcount,
                               sc_MPI_Datatype sendtype, sc_MPI_Op operation,
                               int target, sc_MPI_Comm mpicomm);

SC_EXTERN_C_END;

#endif /* !SC_REDUCE_H */
