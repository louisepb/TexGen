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

#ifndef SC_STRING_H
#define SC_STRING_H

#include <sc.h>

/** \file sc_string.h
 * This file declares a simple string object that can be appended to.
 */

/** This defines the maximum string storage including the trailing '\0'. */
#define SC_STRING_SIZE 4088

/** This is a simple opaque type for growing a string by printf-like commands.
 * It can be declared on the stack to avoid malloc and free.
 * This means that the length of the string is limited to \ref SC_STRING_SIZE - 1.
 * The current string can be accessed by \ref sc_string_get_content.
 * This is really an opaque object: its members shall not be accessed directly.
 */
typedef struct sc_string
{
  /* None of the member variables are public.
   * We provide the declaration here in sc_string.h
   * so the object can be declared on the stack.
   */
  int                 printed;                  /**< Opaque object: do not access. */
  char                buffer[SC_STRING_SIZE];   /**< Opaque object: do not access. */
}
sc_string_t;

/** Initialize to an empty string.
 * This function can be used to reset a non-empty string to be empty again.
 * \param [out] scs             After returning, a valid object
 *                              containing the empty string.
 */
void                sc_string_init (sc_string_t * scs);

/** Append a single character to the string buffer object.
 * \param [in,out] scs          A valid string buffer object.
 * \param [in] c                Converted to an unsigned char and appended.
 * \return                      Zero if the character has been appended and
 *                              a negative value when the input was truncated.
 */
int                 sc_string_putc (sc_string_t * scs, int c);

/** Append a string to the string buffer object.
 * \param [in,out] scs          A valid string buffer object.
 * \param [in] s                This string is appended to the string buffer.
 * \return                      Zero if the string has been appended and
 *                              a negative value when the input was truncated.
 */
int                 sc_string_puts (sc_string_t * scs, const char *s);

/** Append to the string object using a format string and arguments.
 * The maximum length will not be exceeded.
 * The string object will remain valid even on truncated input.
 * \param [in,out] scs          Valid string object that is appended to.
 * \param [in] fmt              Format string as used with printf and friends.
 * \return                      Zero if everything has been appended and a
 *                              negative value when the input was truncated.
 */
int                 sc_string_putf (sc_string_t * scs, const char *fmt, ...)
  __attribute__ ((format (printf, 2, 3)));

/** Append to the string object using a format string and a vararg pointer.
 * The maximum length will not be exceeded.
 * The string object will remain valid even on truncated input.
 * \param [in,out] scs          Valid string object that is appended to.
 * \param [in] fmt              Format string as used with printf and friends.
 * \param [in,out] ap           Argument list pointer as defined in stdarg.h.
 * \return                      Zero if everything has been appended and a
 *                              negative value when the input was truncated.
 */
int                 sc_string_putv (sc_string_t * scs, const char *fmt,
                                    va_list ap);

/** Access content of the string buffer.
 * \param [in] scs              Valid sc_string object.
 * \param [in] length           If not NULL, assign length without trailing '\0'.
 * \return                      Pointer to an internally allocated string, may
 *                              not be used after \b scs goes out of scope.
 */
const char         *sc_string_get_content (sc_string_t * scs, int *length);

#endif /* !SC_STRING_H */
