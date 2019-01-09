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

#ifndef SC_SEARCH_H
#define SC_SEARCH_H

#include <sc.h>

SC_EXTERN_C_BEGIN;

/** Find the branch of a tree that is biased towards a target.
 * We assume a binary tree of depth maxlevel and 0 <= target < 2**maxlevel.
 * We search the branch towards the target on 0 <= level <= maxlevel.
 * The branch number on level is specified by 0 <= interval < 2**level.
 *
 * \return          Branch position with 0 <= position <= 2**maxlevel.
 */
int                 sc_search_bias (int maxlevel, int level,
                                    int interval, int target);

/** Find lowest position k in a sorted array such that array[k] >= target.
 * \param [in]  target  The target lower bound to binary search for.
 * \param [in]  array   The 64bit integer array to binary search in.
 * \param [in]  nmemb   The number of int64_t's in the array.
 * \param [in]  guess   Initial array position to look at.
 * \return  Returns the matching position
 *          or -1 if array[size-1] < target or if size == 0.
 */
ssize_t             sc_search_lower_bound64 (int64_t target,
                                             const int64_t * array,
                                             size_t nmemb, size_t guess);

/** Search position k in sorted array with array[k] <= target < array[k + 1].
 * This function is modeled after the libc bsearch function.
 * \param [in]  key     The target to find in the array range.
 * \param [in]  base    The array to binary search in.
 * \param [in]  nmemb   Number of entries in the array MINUS ONE.
 *                      Thus the array always contains at least one element.
 * \param [in]  size    Size of one entry in the array in bytes.
 * \param [in]  compar  Comparison function to return < 0 for less than,
 *                      0 for equal, and > 0 for greater between the arguments.
 * \return              The matching array position if found, or nmemb if not,
 *                      i.e., if target < array[0] or target >= array[nmemb].
 */
size_t              sc_bsearch_range (const void *key, const void *base,
                                      size_t nmemb, size_t size,
                                      int (*compar) (const void *,
                                                     const void *));

SC_EXTERN_C_END;

#endif /* !SC_SEARCH_H */
