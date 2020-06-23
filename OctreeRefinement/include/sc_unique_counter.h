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

#ifndef SC_UNIQUE_COUNTER_H
#define SC_UNIQUE_COUNTER_H

#include <sc_containers.h>

typedef struct sc_unique_counter
{
  int                 start_value;
  sc_mempool_t       *mempool;
}
sc_unique_counter_t;

/** Create a factory for unique tag numbers.
 * The first tag number created will be start_value.
 * Subsequent ones are counted forward from there.
 * If a counter is released, it will be reactivated by the next creation.
 * \param [in] start_value      Value of the first counter to be added.
 * \return                      Fully initialized counter factory.
 */
sc_unique_counter_t *sc_unique_counter_new (int start_value);

/** Destroy the counter factor and all counters created from it.
 * All counters added must have been released before calling this function.
 * \param [in,out] uc           This memory will be released.
 */
void                sc_unique_counter_destroy (sc_unique_counter_t * uc);

/** Return the size in bytes allocated by this counter factory.
 * \param [in,out] uc           Its total memory used will be counted.
 */
size_t              sc_unique_counter_memory_used (sc_unique_counter_t * uc);

/** Request and return a counter with a unique integer value.
 * The memory return is borrowed and still being owned by \a uc.  The same
 * number will never be returned twice, unless the counter has been released
 * first.
 * \param [in,out] uc           The factory to return a unique counter.
 * \return                      Pointer to internal memory.
 *                              The unique counter value is accessed by simply
 *                              dereferencing the int pointer.
 */
int                *sc_unique_counter_add (sc_unique_counter_t * uc);

/** Release and return a counter to the factory.
 * It will be reactivated on a subsequent call to sc_unique_counter_add.
 * \param [in,out] uc           The factory to return a unique counter.
 * \param [in] counter          This must be a pointer previously obtained from
 *                              sc_unique_counter_add and not since released.
 */
void                sc_unique_counter_release (sc_unique_counter_t * uc,
                                               int *counter);

#endif /* !SC_UNIQUE_COUNTER */
