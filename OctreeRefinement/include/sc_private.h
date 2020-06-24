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

/** \file sc_private.h
 *
 * Support for calls between different parts of the sc library.
 * This is not meant for use from other packages or applications.
 */

#ifndef SC_PRIVATE_H
#define SC_PRIVATE_H

#include <sc.h>

SC_EXTERN_C_BEGIN;

/** Add to the per-package variable about active reference counters.
 * This function is thread safe; it uses per-package locking internally
 * \param [in] package_id       The id of a registered package or -1.
 * \param [in] toadd            This is added to the package's internal
 *                              variable that counts active rcs.
 *                              We assert that the new count does not
 *                              drop below zero.
 */
void                sc_package_rc_count_add (int package_id, int toadd);

SC_EXTERN_C_END;

#endif /* SC_PRIVATE_H */
