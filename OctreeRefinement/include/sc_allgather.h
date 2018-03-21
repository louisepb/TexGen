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

#ifndef SC_ALLGATHER_H
#define SC_ALLGATHER_H

#include <sc.h>

#ifndef SC_AG_ALLTOALL_MAX
#define SC_AG_ALLTOALL_MAX      5
#endif

SC_EXTERN_C_BEGIN;

/** Allgather by direct point-to-point communication.
 * Only makes sense for small group sizes.
 */
void                sc_allgather_alltoall (sc_MPI_Comm mpicomm, char *data,
                                           int datasize, int groupsize,
                                           int myoffset, int myrank);

/** Performs recursive bisection allgather.
 * When size becomes small enough, calls sc_ag_alltoall.
 */
void                sc_allgather_recursive (sc_MPI_Comm mpicomm, char *data,
                                            int datasize, int groupsize,
                                            int myoffset, int myrank);

/** Drop-in allgather replacement.
 */
int                 sc_allgather (void *sendbuf, int sendcount,
                                  sc_MPI_Datatype sendtype, void *recvbuf,
                                  int recvcount, sc_MPI_Datatype recvtype,
                                  sc_MPI_Comm mpicomm);

SC_EXTERN_C_END;

#endif /* !SC_ALLGATHER_H */
