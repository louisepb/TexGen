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

#ifndef SC_KEYVALUE_H
#define SC_KEYVALUE_H

/** \file sc_keyvalue.h
 * This file provides a lookup structure for key-value pairs.
 */

#include <sc.h>
#include <sc_containers.h>

SC_EXTERN_C_BEGIN;

/** The values can have different types. */
typedef enum
{
  SC_KEYVALUE_ENTRY_NONE = 0,   /**< Designate an invalid situation. */
  SC_KEYVALUE_ENTRY_INT,        /**< Used for values of type int. */
  SC_KEYVALUE_ENTRY_DOUBLE,     /**< Used for values of type double. */
  SC_KEYVALUE_ENTRY_STRING,     /**< Used for values of type const char *. */
  SC_KEYVALUE_ENTRY_POINTER     /**< Used for values of anonymous pointer type. */
}
sc_keyvalue_entry_type_t;

/** The key-value container is an opaque structure. */
typedef struct sc_keyvalue sc_keyvalue_t;

/** Create a new key-value container.
 * \return          The container is ready to use.
 */
sc_keyvalue_t      *sc_keyvalue_new ();

/** Create a container and set one or more key-value pairs.
 * Arguments come in pairs of 2: a static string "type:key" and a value.
 * The type is the letter i, g, s, p for int, double, const char *, and void *,
 * respectively.
 * \param [in] dummy            Not touched, just to use the varargs feature.
 * \return                      A key-value container initialized with the given entries.
 */
sc_keyvalue_t      *sc_keyvalue_newf (int dummy, ...);

/** Create a container and set one or more key-value pairs.
 * This function works analogously to \ref sc_keyvalue_newf.
 * Arguments come in pairs of 2: a static string "type:key" and a value.
 * The type is the letter i, g, s, p for int, double, const char *, and void *,
 * respectively.
 * \param [in] ap               Varargs pointer; see stdarg.h for the syntax.
 * \return                      A key-value container initialized with the given entries.
 */
sc_keyvalue_t      *sc_keyvalue_newv (va_list ap);

/** Free a key-value container and all internal memory for key storage.
 * \param [in,out] kv           The key-value container is invalidated by this call.
 */
void                sc_keyvalue_destroy (sc_keyvalue_t * kv);

/** Routine to check existence of an entry.
 * \param [in] kv               Valid key-value container.
 * \param [in] key              Lookup key to query.
 * \return                      The entry's type if found
 *                              and SC_KEYVALUE_ENTRY_NONE otherwise.
 */
sc_keyvalue_entry_type_t sc_keyvalue_exists (sc_keyvalue_t * kv,
                                             const char *key);

/** Routine to remove an entry.
 * \param [in] kv               Valid key-value container.
 * \param [in] key              Lookup key to remove if it exists.
 * \return                      The entry's type if found and removed,
 *                              SC_KEYVALUE_ENTRY_NONE otherwise.
 */
sc_keyvalue_entry_type_t sc_keyvalue_unset (sc_keyvalue_t * kv,
                                            const char *key);

/** Routines to retrieve an integer value by its key.
 * This function asserts that the key, if existing, points to the correct type.
 * \param [in] kv               Valid key-value container.
 * \param [in] key              Lookup key, may or may not exist.
 * \param [in] dvalue           Default value returned if key is not found.
 * \return                      If key is not present then \b dvalue is returned,
 *                              otherwise the value stored under \b key.
 */
int                 sc_keyvalue_get_int (sc_keyvalue_t * kv,
                                         const char *key, int dvalue);

/** Retrieve a double value by its key.
 * This function asserts that the key, if existing, points to the correct type.
 * \param [in] kv               Valid key-value container.
 * \param [in] key              Lookup key, may or may not exist.
 * \param [in] dvalue           Default value returned if key is not found.
 * \return                      If key is not present then \b dvalue is returned,
 *                              otherwise the value stored under \b key.
 */
double              sc_keyvalue_get_double (sc_keyvalue_t * kv,
                                            const char *key, double dvalue);

/** Retrieve a string value by its key.
 * This function asserts that the key, if existing, points to the correct type.
 * \param [in] kv               Valid key-value container.
 * \param [in] key              Lookup key, may or may not exist.
 * \param [in] dvalue           Default value returned if key is not found.
 * \return                      If key is not present then \b dvalue is returned,
 *                              otherwise the value stored under \b key.
 */
const char         *sc_keyvalue_get_string (sc_keyvalue_t * kv,
                                            const char *key,
                                            const char *dvalue);

/** Retrieve a pointer value by its key.
 * This function asserts that the key, if existing, points to the correct type.
 * \param [in] kv               Valid key-value container.
 * \param [in] key              Lookup key, may or may not exist.
 * \param [in] dvalue           Default value returned if key is not found.
 * \return                      If key is not present then \b dvalue is returned,
 *                              otherwise the value stored under \b key.
 */
void               *sc_keyvalue_get_pointer (sc_keyvalue_t * kv,
                                             const char *key, void *dvalue);

/** Query an integer key with error checking.
 * We check whether the key is not found or it is of the wrong type.
 * A default value to be returned on error can be passed in as *status.
 * If status is NULL, then the result on error is undefined.
 * \param [in] kv           Valid key-value table.
 * \param [in] key          Non-NULL key string.
 * \param [in,out] status   If not NULL, set to
 *                          0 if there is no error,
 *                          1 if the key is not found,
 *                          2 if a value is found but its type is not integer,
 *                          and return the input value *status on error.
 * \return                  On error we return *status if status is not NULL,
 *                          and else an undefined value backed by an assertion.
 *                          Without error, return the result of the lookup.
 */
int                 sc_keyvalue_get_int_check (sc_keyvalue_t * kv,
                                               const char *key, int *status);

/** Routine to set an integer value for a given key.
 * \param [in] kv           Valid key-value table.
 * \param [in] key          Non-NULL key to insert or replace.
 *                          If it already exists, it must be of type integer.
 * \param [in] newvalue     New value will be stored under key.
 */
void                sc_keyvalue_set_int (sc_keyvalue_t * kv,
                                         const char *key, int newvalue);

/** Routine to set a double value for a given key.
 * \param [in] kv           Valid key-value table.
 * \param [in] key          Non-NULL key to insert or replace.
 *                          If it already exists, it must be of type double.
 * \param [in] newvalue     New value will be stored under key.
 */
void                sc_keyvalue_set_double (sc_keyvalue_t * kv,
                                            const char *key, double newvalue);

/** Routine to set a string value for a given key.
 * \param [in] kv           Valid key-value table.
 * \param [in] key          Non-NULL key to insert or replace.
 *                          If it already exists, it must be of type string.
 * \param [in] newvalue     New value will be stored under key.
 */
void                sc_keyvalue_set_string (sc_keyvalue_t * kv,
                                            const char *key,
                                            const char *newvalue);

/** Routine to set a pointer value for a given key.
 * \param [in] kv           Valid key-value table.
 * \param [in] key          Non-NULL key to insert or replace.
 *                          If it already exists, it must be of type pointer.
 * \param [in] newvalue     New value will be stored under key.
 */
void                sc_keyvalue_set_pointer (sc_keyvalue_t * kv,
                                             const char *key, void *newvalue);

/** Function to call on every key value pair
 * \param [in] key   The key for this pair
 * \param [in] type  The type of entry
 * \param [in] entry Pointer to the entry
 * \param [in] u     Arbitrary user data.
 * \return Return true if the traversal should continue, false to stop.
 */
typedef int         (*sc_keyvalue_foreach_t) (const char *key,
                                              const sc_keyvalue_entry_type_t
                                              type, void *entry,
                                              const void *u);

/** Iterate through all stored key-value pairs.
 * \param [in] kv               Valid key-value container.
 * \param [in] fn               Function to call on each key-value pair.
 * \param [in,out] user_data    This pointer is passed through to \b fn.
 */
void                sc_keyvalue_foreach (sc_keyvalue_t * kv,
                                         sc_keyvalue_foreach_t fn,
                                         void *user_data);

SC_EXTERN_C_END;

#endif /* !SC_KEYVALUE_H */
