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

#ifndef SC_CONTAINERS_H
#define SC_CONTAINERS_H

/** \file sc_containers.h
 *
 * Defines lists, arrays, hash tables, etc.
 *
 * \ingroup containers
 */

/** \defgroup containers containers
 *
 * Defines lists, arrays, hash tables, etc.
 *
 * \ingroup sc
 */

#include <sc_obstack.h>

SC_EXTERN_C_BEGIN;

/* Hash macros from lookup3.c by Bob Jenkins, May 2006, public domain. */
#define sc_hash_rot(x,k) (((x) << (k)) | ((x) >> (32 - (k))))
#define sc_hash_mix(a,b,c) ((void)                                      \
                            (a -= c, a ^= sc_hash_rot(c, 4), c += b,    \
                             b -= a, b ^= sc_hash_rot(a, 6), a += c,    \
                             c -= b, c ^= sc_hash_rot(b, 8), b += a,    \
                             a -= c, a ^= sc_hash_rot(c,16), c += b,    \
                             b -= a, b ^= sc_hash_rot(a,19), a += c,    \
                             c -= b, c ^= sc_hash_rot(b, 4), b += a))
#define sc_hash_final(a,b,c) ((void)                            \
                              (c ^= b, c -= sc_hash_rot(b,14),  \
                               a ^= c, a -= sc_hash_rot(c,11),  \
                               b ^= a, b -= sc_hash_rot(a,25),  \
                               c ^= b, c -= sc_hash_rot(b,16),  \
                               a ^= c, a -= sc_hash_rot(c, 4),  \
                               b ^= a, b -= sc_hash_rot(a,14),  \
                               c ^= b, c -= sc_hash_rot(b,24)))

/** Function to compute a hash value of an object.
 * \param [in] v   The object to hash.
 * \param [in] u   Arbitrary user data.
 * \return Returns an unsigned integer.
 */
typedef unsigned    (*sc_hash_function_t) (const void *v, const void *u);

/** Function to check equality of two objects.
 * \param [in] u   Arbitrary user data.
 * \return Returns false if *v1 is unequal *v2 and true otherwise.
 */
typedef int         (*sc_equal_function_t) (const void *v1,
                                            const void *v2, const void *u);

/** Function to call on every data item of a hash table.
 * \param [in] v   The address of the pointer to the current object.
 * \param [in] u   Arbitrary user data.
 * \return Return true if the traversal should continue, false to stop.
 */
typedef int         (*sc_hash_foreach_t) (void **v, const void *u);

/** The sc_array object provides a dynamic array of equal-size elements.
 * Elements are accessed by their 0-based index.  Their address may change.
 * The number of elements (== elem_count) of the array can be changed by 
 * \ref sc_array_resize and \ref sc_array_rewind.
 * Elements can be sorted with \ref sc_array_sort.
 * If the array is sorted, it can be searched with \ref sc_array_bsearch.
 * A priority queue is implemented with pqueue_add and pqueue_pop (untested).
 */
typedef struct sc_array
{
  /* interface variables */
  size_t              elem_size;        /**< size of a single element */
  size_t              elem_count;       /**< number of valid elements */

  /* implementation variables */
  ssize_t             byte_alloc;       /**< number of allocated bytes
                                           or -(number of viewed bytes + 1)
                                           if this is a view: the "+ 1"
                                           distinguishes an array of size 0
                                           from a view of size 0 */
  char               *array;    /**< linear array to store elements */
}
sc_array_t;

/** test whether the sc_array_t owns its \a array */
#define SC_ARRAY_IS_OWNER(a) ((a)->byte_alloc >= 0)
/** the allocated size of the array */
#define SC_ARRAY_BYTE_ALLOC(a) ((size_t) \
         (SC_ARRAY_IS_OWNER (a) ? (a)->byte_alloc : -((a)->byte_alloc + 1)))

/** Calculate the memory used by an array.
 * \param [in] array       The array.
 * \param [in] is_dynamic  True if created with sc_array_new,
 *                         false if initialized with sc_array_init
 * \return                 Memory used in bytes.
 */
size_t              sc_array_memory_used (sc_array_t * array, int is_dynamic);

/** Creates a new array structure with 0 elements.
 * \param [in] elem_size    Size of one array element in bytes.
 * \return                  Return an allocated array of zero length.
 */
sc_array_t         *sc_array_new (size_t elem_size);

/** Creates a new array structure with a given length (number of elements).
 * \param [in] elem_size    Size of one array element in bytes.
 * \param [in] elem_count   Initial number of array elements.
 * \return                  Return an allocated array
 *                          with allocated but uninitialized elements.
 */
sc_array_t         *sc_array_new_count (size_t elem_size, size_t elem_count);

/** Deprecated: use \ref sc_array_new_count. */
#define sc_array_new_size(s,c) (sc_array_new_count ((s), (c)))

/** Creates a new view of an existing sc_array_t.
 * \param [in] array    The array must not be resized while view is alive.
 * \param [in] offset   The offset of the viewed section in element units.
 *                      This offset cannot be changed until the view is reset.
 * \param [in] length   The length of the viewed section in element units.
 *                      The view cannot be resized to exceed this length.
 */
sc_array_t         *sc_array_new_view (sc_array_t * array,
                                       size_t offset, size_t length);

/** Creates a new view of an existing plain C array.
 * \param [in] base         The data must not be moved while view is alive.
 * \param [in] elem_size    Size of one array element in bytes.
 * \param [in] elem_count   The length of the view in element units.
 *                          The view cannot be resized to exceed this length.
 */
sc_array_t         *sc_array_new_data (void *base,
                                       size_t elem_size, size_t elem_count);

/** Destroys an array structure.
 * \param [in] array    The array to be destroyed.
 */
void                sc_array_destroy (sc_array_t * array);

/** Initializes an already allocated (or static) array structure.
 * \param [in,out]  array       Array structure to be initialized.
 * \param [in] elem_size        Size of one array element in bytes.
 */
void                sc_array_init (sc_array_t * array, size_t elem_size);

/** Initializes an already allocated (or static) array structure
 * and allocates a given number of elements.
 * \param [in,out]  array       Array structure to be initialized.
 * \param [in] elem_size        Size of one array element in bytes.
 * \param [in] elem_count       Number of initial array elements.
 */
void                sc_array_init_size (sc_array_t * array,
                                        size_t elem_size, size_t elem_count);

/** Initializes an already allocated (or static) view from existing sc_array_t.
 * The array view returned does not require sc_array_reset (doesn't hurt though).
 * \param [in,out] view  Array structure to be initialized.
 * \param [in] array     The array must not be resized while view is alive.
 * \param [in] offset    The offset of the viewed section in element units.
 *                       This offset cannot be changed until the view is reset.
 * \param [in] length    The length of the view in element units.
 *                       The view cannot be resized to exceed this length.
 *                       It is not necessary to call sc_array_reset later.
 */
void                sc_array_init_view (sc_array_t * view, sc_array_t * array,
                                        size_t offset, size_t length);

/** Initializes an already allocated (or static) view from given plain C data.
 * The array view returned does not require sc_array_reset (doesn't hurt though).
 * \param [in,out] view     Array structure to be initialized.
 * \param [in] base         The data must not be moved while view is alive.
 * \param [in] elem_size    Size of one array element in bytes.
 * \param [in] elem_count   The length of the view in element units.
 *                          The view cannot be resized to exceed this length.
 *                          It is not necessary to call sc_array_reset later.
 */
void                sc_array_init_data (sc_array_t * view, void *base,
                                        size_t elem_size, size_t elem_count);

/** Sets the array count to zero and frees all elements.
 * This function turns a view into a newly initialized array.
 * \param [in,out]  array       Array structure to be reset.
 * \note Calling sc_array_init, then any array operations,
 *       then sc_array_reset is memory neutral.
 *       As an exception, the two functions sc_array_init_view and
 *       sc_array_init_data do not require a subsequent call to sc_array_reset.
 *       Regardless, it is legal to call sc_array_reset anyway.
 */
void                sc_array_reset (sc_array_t * array);

/** Sets the array count to zero, but does not free elements.
 * Not allowed for views.
 * \param [in,out]  array       Array structure to be truncated.
 * \note This is intended to allow an sc_array to be used as a reusable
 * buffer, where the "high water mark" of the buffer is preserved, so that
 * O(log (max n)) reallocs occur over the life of the buffer.
 */
void                sc_array_truncate (sc_array_t * array);

/** Shorten an array without reallocating it.
 * \param [in,out] array    The element count of this array is modified.
 * \param [in] new_count    Must be less or equal than the \b array's count.
 *                          If it is less, the number of elements in the
 *                          array is reduced without reallocating memory.
 *                          The exception is a \b new_count of zero
 *                          specified for an array that is not a view:
 *                          In this case \ref sc_array_reset is equivalent.
 */
void                sc_array_rewind (sc_array_t * array, size_t new_count);

/** Sets the element count to new_count.
 * If the array is not a view, reallocation takes place occasionally.
 * If the array is a view, new_count must not be greater than the element
 * count of the view when it was created.  The original offset of the view
 * cannot be changed.
 * \param [in,out] array    The element count and address is modified.
 * \param [in] new_count    New element count of the array.
 *                          If it is zero and the array is not a view,
 *                          the effect equals \ref sc_array_reset.
 */
void                sc_array_resize (sc_array_t * array, size_t new_count);

/** Copy the contents of an array into another.
 * Both arrays must have equal element sizes.
 * \param [in] dest Array (not a view) will be resized and get new data.
 * \param [in] src  Array used as source of new data, will not be changed.
 */
void                sc_array_copy (sc_array_t * dest, sc_array_t * src);

/** Sorts the array in ascending order wrt. the comparison function.
 * \param [in] array    The array to sort.
 * \param [in] compar   The comparison function to be used.
 */
void                sc_array_sort (sc_array_t * array,
                                   int (*compar) (const void *,
                                                  const void *));

/** Check whether the array is sorted wrt. the comparison function.
 * \param [in] array    The array to check.
 * \param [in] compar   The comparison function to be used.
 * \return              True if array is sorted, false otherwise.
 */
int                 sc_array_is_sorted (sc_array_t * array,
                                        int (*compar) (const void *,
                                                       const void *));

/** Check whether two arrays have equal size, count, and content.
 * Either array may be a view.  Both arrays will not be changed.
 * \param [in] array   One array to be compared.
 * \param [in] other   A second array to be compared.
 * \return              True if array and other are equal, false otherwise.
 */
int                 sc_array_is_equal (sc_array_t * array,
                                       sc_array_t * other);

/** Removed duplicate entries from a sorted array.
 * This function is not allowed for views.
 * \param [in,out] array  The array size will be reduced as necessary.
 * \param [in] compar     The comparison function to be used.
 */
void                sc_array_uniq (sc_array_t * array,
                                   int (*compar) (const void *,
                                                  const void *));

/** Performs a binary search on an array. The array must be sorted.
 * \param [in] array   A sorted array to search in.
 * \param [in] key     An element to be searched for.
 * \param [in] compar  The comparison function to be used.
 * \return Returns the index into array for the item found, or -1.
 */
ssize_t             sc_array_bsearch (sc_array_t * array,
                                      const void *key,
                                      int (*compar) (const void *,
                                                     const void *));

/** Function to determine the enumerable type of an object in an array.
 * \param [in] array   Array containing the object.
 * \param [in] index   The location of the object.
 * \param [in] data    Arbitrary user data.
 */
typedef             size_t (*sc_array_type_t) (sc_array_t * array,
                                               size_t index, void *data);

/** Compute the offsets of groups of enumerable types in an array.
 * \param [in] array         Array that is sorted in ascending order by type.
 *                           If k indexes \a array, then
 *                           0 <= \a type_fn (\a array, k, \a data) <
 *                           \a num_types.
 * \param [in,out] offsets   An initialized array of type size_t that is
 *                           resized to \a num_types + 1 entries.  The indices
 *                           j of \a array that contain objects of type k are
 *                           \a offsets[k] <= j < \a offsets[k + 1].
 *                           If there are no objects of type k, then
 *                           \a offsets[k] = \a offset[k + 1].
 * \param [in] num_types     The number of possible types of objects in
 *                           \a array.
 * \param [in] type_fn       Returns the type of an object in the array.
 * \param [in] data          Arbitrary user data passed to \a type_fn.
 */
void                sc_array_split (sc_array_t * array, sc_array_t * offsets,
                                    size_t num_types, sc_array_type_t type_fn,
                                    void *data);

/** Determine whether \a array is an array of size_t's whose entries include
 * every integer 0 <= i < array->elem_count.
 * \param [in] array         An array.
 * \return                   Returns 1 if array contains size_t elements whose
 *                           entries include every integer
 *                           0 <= i < \a array->elem_count, 0 otherwise.
 */
int                 sc_array_is_permutation (sc_array_t * array);

/** Given permutation \a newindices, permute \a array in place.  The data that
 * on input is contained in \a array[i] will be contained in \a
 * array[newindices[i]] on output.  The entries of newindices will be altered
 * unless \a keepperm is true.
 * \param [in,out] array      An array.
 * \param [in,out] newindices Permutation array (see sc_array_is_permutation).
 * \param [in]     keepperm   If true, \a newindices will be unchanged by the
 *                            algorithm; if false, \a newindices will be the
 *                            identity permutation on output, but the
 *                            algorithm will only use O(1) space.
 */
void                sc_array_permute (sc_array_t * array,
                                      sc_array_t * newindices, int keepperm);

/** Computes the adler32 checksum of array data (see zlib documentation).
 * This is a faster checksum than crc32, and it works with zeros as data.
 */
unsigned            sc_array_checksum (sc_array_t * array);

/** Adds an element to a priority queue.
 * PQUEUE FUNCTIONS ARE UNTESTED AND CURRENTLY DISABLED.
 * This function is not allowed for views.
 * The priority queue is implemented as a heap in ascending order.
 * A heap is a binary tree where the children are not less than their parent.
 * Assumes that elements [0]..[elem_count-2] form a valid heap.
 * Then propagates [elem_count-1] upward by swapping if necessary.
 * \param [in] temp    Pointer to unused allocated memory of elem_size.
 * \param [in] compar  The comparison function to be used.
 * \return Returns the number of swap operations.
 * \note  If the return value is zero for all elements in an array,
 *        the array is sorted linearly and unchanged.
 */
size_t              sc_array_pqueue_add (sc_array_t * array,
                                         void *temp,
                                         int (*compar) (const void *,
                                                        const void *));

/** Pops the smallest element from a priority queue.
 * PQUEUE FUNCTIONS ARE UNTESTED AND CURRENTLY DISABLED.
 * This function is not allowed for views.
 * This function assumes that the array forms a valid heap in ascending order.
 * \param [out] result  Pointer to unused allocated memory of elem_size.
 * \param [in]  compar  The comparison function to be used.
 * \return Returns the number of swap operations.
 * \note This function resizes the array to elem_count-1.
 */
size_t              sc_array_pqueue_pop (sc_array_t * array,
                                         void *result,
                                         int (*compar) (const void *,
                                                        const void *));

/** Returns a pointer to an array element.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index (sc_array_t * array, size_t iz)
{
  SC_ASSERT (iz < array->elem_count);

  return (void *) (array->array + (array->elem_size * iz));
}

/** Returns a pointer to an array element indexed by a plain int.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_int (sc_array_t * array, int i)
{
  SC_ASSERT (i >= 0 && (size_t) i < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) i));
}

/** Returns a pointer to an array element indexed by a plain long.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_long (sc_array_t * array, long l)
{
  SC_ASSERT (l >= 0 && (size_t) l < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) l));
}

/** Returns a pointer to an array element indexed by a ssize_t.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_ssize_t (sc_array_t * array, ssize_t is)
{
  SC_ASSERT (is >= 0 && (size_t) is < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) is));
}

/** Returns a pointer to an array element indexed by a int16_t.
 * \param [in] index needs to be in [0]..[elem_count-1].
 */
/*@unused@*/
static inline void *
sc_array_index_int16 (sc_array_t * array, int16_t i16)
{
  SC_ASSERT (i16 >= 0 && (size_t) i16 < array->elem_count);

  return (void *) (array->array + (array->elem_size * (size_t) i16));
}

/** Return the index of an object in an array identified by a pointer.
 * \param [in] element needs to be the address of an element in array.
 */
/*@unused@*/
static inline       size_t
sc_array_position (sc_array_t * array, void *element)
{
  ptrdiff_t           position;

  SC_ASSERT (array->array <= (char *) element);
  SC_ASSERT (((char *) element - array->array) %
             (ptrdiff_t) array->elem_size == 0);

  position = ((char *) element - array->array) / (ptrdiff_t) array->elem_size;
  SC_ASSERT (0 <= position && position < (ptrdiff_t) array->elem_count);

  return (size_t) position;
}

/** Remove the last element from an array and return a pointer to it.
 * This function is not allowed for views.
 * \return                The pointer to the removed object.  Will be valid
 *                        as long as no other function is called on this array.
 */
/*@unused@*/
static inline void *
sc_array_pop (sc_array_t * array)
{
  SC_ASSERT (SC_ARRAY_IS_OWNER (array));
  SC_ASSERT (array->elem_count > 0);

  return (void *) (array->array + (array->elem_size * --array->elem_count));
}

/** Enlarge an array by a number of elements.  Grows the array if necessary.
 * This function is not allowed for views.
 * \return Returns a pointer to the uninitialized newly added elements.
 */
/*@unused@*/
static inline void *
sc_array_push_count (sc_array_t * array, size_t add_count)
{
  const size_t        old_count = array->elem_count;
  const size_t        new_count = old_count + add_count;

  SC_ASSERT (SC_ARRAY_IS_OWNER (array));

  if (array->elem_size * new_count > (size_t) array->byte_alloc) {
    sc_array_resize (array, new_count);
  }
  else {
    array->elem_count = new_count;
  }

  return (void *) (array->array + array->elem_size * old_count);
}

/** Enlarge an array by one element.  Grows the array if necessary.
 * This function is not allowed for views.
 * \return Returns a pointer to the uninitialized newly added element.
 */
/*@unused@*/
static inline void *
sc_array_push (sc_array_t * array)
{
  return sc_array_push_count (array, 1);
}

/** The sc_mempool object provides a large pool of equal-size elements.
 * The pool grows dynamically for element allocation.
 * Elements are referenced by their address which never changes.
 * Elements can be freed (that is, returned to the pool)
 *    and are transparently reused.
 * If the zero_and_persist option is selected, new elements are initialized to
 * all zeros on creation, and the contents of an element are not touched
 * between freeing and re-returning it.
 */
typedef struct sc_mempool
{
  /* interface variables */
  size_t              elem_size;        /**< size of a single element */
  size_t              elem_count;       /**< number of valid elements */
  int                 zero_and_persist; /**< Boolean; is set in constructor. */

  /* implementation variables */
  struct obstack      obstack;  /**< holds the allocated elements */
  sc_array_t          freed;    /**< buffers the freed elements */
}
sc_mempool_t;

/** Calculate the memory used by a memory pool.
 * \param [in] array       The memory pool.
 * \return                 Memory used in bytes.
 */
size_t              sc_mempool_memory_used (sc_mempool_t * mempool);

/** Creates a new mempool structure with the zero_and_persist option off.
 * The contents of any elements returned by sc_mempool_alloc are undefined.
 * \param [in] elem_size  Size of one element in bytes.
 * \return Returns an allocated and initialized memory pool.
 */
sc_mempool_t       *sc_mempool_new (size_t elem_size);

/** Creates a new mempool structure with the zero_and_persist option on.
 * The memory of newly created elements is zero'd out, and the contents of an
 * element are not touched between freeing and re-returning it.
 * \param [in] elem_size  Size of one element in bytes.
 * \return Returns an allocated and initialized memory pool.
 */
sc_mempool_t       *sc_mempool_new_zero_and_persist (size_t elem_size);

/** Same as sc_mempool_new, but for an already allocated sc_mempool_t pointer. */
void                sc_mempool_init (sc_mempool_t * mempool,
                                     size_t elem_size);

/** Destroys a mempool structure.
 * All elements that are still in use are invalidated.
 */
void                sc_mempool_destroy (sc_mempool_t * mempool);

/** Same as sc_mempool_destroy, but does not free the pointer */
void                sc_mempool_reset (sc_mempool_t * mempool);

/** Invalidates all previously returned pointers, resets count to 0.
 */
void                sc_mempool_truncate (sc_mempool_t * mempool);

/** Allocate a single element.
 * Elements previously returned to the pool are recycled.
 * \return Returns a new or recycled element pointer.
 */
/*@unused@*/
static inline void *
sc_mempool_alloc (sc_mempool_t * mempool)
{
  void               *ret;
  sc_array_t         *freed = &mempool->freed;

  ++mempool->elem_count;

  if (freed->elem_count > 0) {
    ret = *(void **) sc_array_pop (freed);
  }
  else {
    ret = obstack_alloc (&mempool->obstack, (int) mempool->elem_size);
    if (mempool->zero_and_persist) {
      memset (ret, 0, mempool->elem_size);
    }
  }

#ifdef SC_ENABLE_DEBUG
  if (!mempool->zero_and_persist) {
    memset (ret, -1, mempool->elem_size);
  }
#endif

  return ret;
}

/** Return a previously allocated element to the pool.
 * \param [in] elem  The element to be returned to the pool.
 */
/*@unused@*/
static inline void
sc_mempool_free (sc_mempool_t * mempool, void *elem)
{
  sc_array_t         *freed = &mempool->freed;

  SC_ASSERT (mempool->elem_count > 0);

#ifdef SC_ENABLE_DEBUG
  if (!mempool->zero_and_persist) {
    memset (elem, -1, mempool->elem_size);
  }
#endif

  --mempool->elem_count;

  *(void **) sc_array_push (freed) = elem;
}

/** The sc_link structure is one link of a linked list.
 */
typedef struct sc_link
{
  void               *data;
  struct sc_link     *next;
}
sc_link_t;

/** The sc_list object provides a linked list.
 */
typedef struct sc_list
{
  /* interface variables */
  size_t              elem_count;
  sc_link_t          *first;
  sc_link_t          *last;

  /* implementation variables */
  int                 allocator_owned;
  sc_mempool_t       *allocator;        /* must allocate sc_link_t */
}
sc_list_t;

/** Calculate the total memory used by a list.
 * \param [in] list        The list.
 * \param [in] is_dynamic  True if created with sc_list_new,
 *                         false if initialized with sc_list_init
 * \return                 Memory used in bytes.
 */
size_t              sc_list_memory_used (sc_list_t * list, int is_dynamic);

/** Allocate a new, empty linked list.
 * \param [in] allocator    Memory allocator for sc_link_t, can be NULL
 *                          in which case an internal allocator is created.
 * \return                  Pointer to a newly allocated, empty list object.
 */
sc_list_t          *sc_list_new (sc_mempool_t * allocator);

/** Destroy a linked list structure in O(N).
 * \param [in,out] list     All memory allocated for this list is freed.
 * \note If allocator was provided in sc_list_new, it will not be destroyed.
 */
void                sc_list_destroy (sc_list_t * list);

/** Initialize a list object with an external link allocator.
 * \param [in,out]  list       List structure to be initialized.
 * \param [in]      allocator  External memory allocator for sc_link_t,
 *                             which must exist already.
 */
void                sc_list_init (sc_list_t * list, sc_mempool_t * allocator);

/** Remove all elements from a list in O(N).
 * \param [in,out]  list       List structure to be emptied.
 * \note Calling sc_list_init, then any list operations,
 *       then sc_list_reset is memory neutral.
 */
void                sc_list_reset (sc_list_t * list);

/** Unlink all list elements without returning them to the mempool.
 * This runs in O(1) but is dangerous because the link memory stays alive.
 * \param [in,out]  list       List structure to be unlinked.
 */
void                sc_list_unlink (sc_list_t * list);

/** Insert a list element at the beginning of the list.
 * \param [in,out] list     Valid list object.
 * \param [in] data         A new link is created holding this data.
 * \return                  The link that has been created for data.
 */
sc_link_t          *sc_list_prepend (sc_list_t * list, void *data);

/** Insert a list element at the end of the list.
 * \param [in,out] list     Valid list object.
 * \param [in] data         A new link is created holding this data.
 * \return                  The link that has been created for data.
 */
sc_link_t          *sc_list_append (sc_list_t * list, void *data);

/** Insert an element after a given list position.
 * \param [in,out] list     Valid list object.
 * \param [in,out] pred     The predecessor of the element to be inserted.
 * \param [in] data         A new link is created holding this data.
 * \return                  The link that has been created for data.
 */
sc_link_t          *sc_list_insert (sc_list_t * list,
                                    sc_link_t * pred, void *data);

/** Remove an element after a given list position.
 * \param [in,out] list     Valid, non-empty list object.
 * \param [in] pred  The predecessor of the element to be removed.
 *                   If \a pred == NULL, the first element is removed,
 *                   which is equivalent to calling sc_list_pop (list).
 * \return           The data of the removed and freed link.
 */
void               *sc_list_remove (sc_list_t * list, sc_link_t * pred);

/** Remove an element from the front of the list.
 * \param [in,out] list     Valid, non-empty list object.
 * \return Returns the data of the removed first list element.
 */
void               *sc_list_pop (sc_list_t * list);

/** The sc_hash implements a hash table.
 * It uses an array which has linked lists as elements.
 */
typedef struct sc_hash
{
  /* interface variables */
  size_t              elem_count;       /**< total number of objects contained */

  /* implementation variables */
  sc_array_t         *slots;    /**< the slot count is slots->elem_count */
  void               *user_data;        /**< user data passed to hash function */
  sc_hash_function_t  hash_fn;
  sc_equal_function_t equal_fn;
  size_t              resize_checks, resize_actions;
  int                 allocator_owned;
  sc_mempool_t       *allocator;        /**< must allocate sc_link_t */
}
sc_hash_t;

/** Compute a hash value from a null-terminated string.
 * This hash function is NOT cryptographically safe! Use libcrypt then.
 * \param [in] s        Null-terminated string to be hashed.
 * \param [in] u        Not used.
 * \return              The computed hash value as an unsigned integer.
 */
unsigned            sc_hash_function_string (const void *s, const void *u);

/** Calculate the memory used by a hash table.
 * \param [in] hash        The hash table.
 * \return                 Memory used in bytes.
 */
size_t              sc_hash_memory_used (sc_hash_t * hash);

/** Create a new hash table.
 * The number of hash slots is chosen dynamically.
 * \param [in] hash_fn     Function to compute the hash value.
 * \param [in] equal_fn    Function to test two objects for equality.
 * \param [in] user_data   User data passed through to the hash function.
 * \param [in] allocator   Memory allocator for sc_link_t, can be NULL.
 */
sc_hash_t          *sc_hash_new (sc_hash_function_t hash_fn,
                                 sc_equal_function_t equal_fn,
                                 void *user_data, sc_mempool_t * allocator);

/** Destroy a hash table.
 *
 * If the allocator is owned, this runs in O(1), otherwise in O(N).
 * \note If allocator was provided in sc_hash_new, it will not be destroyed.
 */
void                sc_hash_destroy (sc_hash_t * hash);

/** Remove all entries from a hash table in O(N).
 *
 * If the allocator is owned, it calls sc_hash_unlink and sc_mempool_truncate.
 * Otherwise, it calls sc_list_reset on every hash slot which is slower.
 */
void                sc_hash_truncate (sc_hash_t * hash);

/** Unlink all hash elements without returning them to the mempool.
 *
 * If the allocator is not owned, this runs faster than sc_hash_truncate,
 *    but is dangerous because of potential memory leaks.
 * \param [in,out]  hash       Hash structure to be unlinked.
 */
void                sc_hash_unlink (sc_hash_t * hash);

/** Same effect as unlink and destroy, but in O(1).
 * This is dangerous because of potential memory leaks.
 * \param [in]  hash       Hash structure to be unlinked and destroyed.
 */
void                sc_hash_unlink_destroy (sc_hash_t * hash);

/** Check if an object is contained in the hash table.
 * \param [in]  v      The object to be looked up.
 * \param [out] found  If found != NULL, *found is set to the address of the
 *                     pointer to the already contained object if the object
 *                     is found.  You can assign to **found to override.
 * \return Returns true if object is found, false otherwise.
 */
int                 sc_hash_lookup (sc_hash_t * hash, void *v, void ***found);

/** Insert an object into a hash table if it is not contained already.
 * \param [in]  v      The object to be inserted.
 * \param [out] found  If found != NULL, *found is set to the address of the
 *                     pointer to the already contained, or if not present,
 *                     the new object.  You can assign to **found to override.
 * \return Returns true if object is added, false if it is already contained.
 */
int                 sc_hash_insert_unique (sc_hash_t * hash, void *v,
                                           void ***found);

/** Remove an object from a hash table.
 * \param [in]  v      The object to be removed.
 * \param [out] found  If found != NULL, *found is set to the object
                       that is removed if that exists.
 * \return Returns true if object is found, false if is not contained.
 */
int                 sc_hash_remove (sc_hash_t * hash, void *v, void **found);

/** Invoke a callback for every member of the hash table.
 * The functions hash_fn and equal_fn are not called by this function.
 */
void                sc_hash_foreach (sc_hash_t * hash, sc_hash_foreach_t fn);

/** Compute and print statistical information about the occupancy.
 */
void                sc_hash_print_statistics (int package_id,
                                              int log_priority,
                                              sc_hash_t * hash);

typedef struct sc_hash_array_data
{
  sc_array_t         *pa;
  sc_hash_function_t  hash_fn;
  sc_equal_function_t equal_fn;
  void               *user_data;
  void               *current_item;
}
sc_hash_array_data_t;

/** The sc_hash_array implements an array backed up by a hash table.
 * This enables O(1) access for array elements.
 */
typedef struct sc_hash_array
{
  /* implementation variables */
  sc_array_t          a;
  sc_hash_array_data_t internal_data;
  sc_hash_t          *h;
}
sc_hash_array_t;

/** Calculate the memory used by a hash array.
 * \param [in] ha          The hash array.
 * \return                 Memory used in bytes.
 */
size_t              sc_hash_array_memory_used (sc_hash_array_t * ha);

/** Create a new hash array.
 * \param [in] elem_size   Size of one array element in bytes.
 * \param [in] hash_fn     Function to compute the hash value.
 * \param [in] equal_fn    Function to test two objects for equality.
 */
sc_hash_array_t    *sc_hash_array_new (size_t elem_size,
                                       sc_hash_function_t hash_fn,
                                       sc_equal_function_t equal_fn,
                                       void *user_data);

/** Destroy a hash array.
 */
void                sc_hash_array_destroy (sc_hash_array_t * hash_array);

/** Check the internal consistency of a hash array.
 */
int                 sc_hash_array_is_valid (sc_hash_array_t * hash_array);

/** Remove all elements from the hash array.
 * \param [in,out] hash_array   Hash array to truncate.
 */
void                sc_hash_array_truncate (sc_hash_array_t * hash_array);

/** Check if an object is contained in a hash array.
 *
 * \param [in]  v          A pointer to the object.
 * \param [out] position   If position != NULL, *position is set to the
 *                         array position of the already contained object
 *                         if found.
 * \return                 Returns true if object is found, false otherwise.
 */
int                 sc_hash_array_lookup (sc_hash_array_t * hash_array,
                                          void *v, size_t * position);

/** Insert an object into a hash array if it is not contained already.
 * The object is not copied into the array.  Use the return value for that.
 * New objects are guaranteed to be added at the end of the array.
 *
 * \param [in]  v          A pointer to the object.  Used for search only.
 * \param [out] position   If position != NULL, *position is set to the
 *                         array position of the already contained, or if
 *                         not present, the new object.
 * \return                 Returns NULL if the object is already contained.
 *                         Otherwise returns its new address in the array.
 */
void               *sc_hash_array_insert_unique (sc_hash_array_t * hash_array,
                                                 void *v, size_t * position);

/** Extract the array data from a hash array and destroy everything else.
 * \param [in] hash_array   The hash array is destroyed after extraction.
 * \param [in] rip          Array structure that will be overwritten.
 *                          All previous array data (if any) will be leaked.
 *                          The filled array can be freed with sc_array_reset.
 */
void                sc_hash_array_rip (sc_hash_array_t * hash_array,
                                       sc_array_t * rip);

/** The sc_recycle_array object provides an array of slots that can be reused.
 *
 * It keeps a list of free slots in the array which will be used for insertion
 * while available.  Otherwise, the array is grown.
 */
typedef struct sc_recycle_array
{
  /* interface variables */
  size_t              elem_count;       /* number of valid entries */

  /* implementation variables */
  sc_array_t          a;
  sc_array_t          f;
}
sc_recycle_array_t;

/** Initialize a recycle array.
 *
 * \param [in] elem_size   Size of the objects to be stored in the array.
 */
void                sc_recycle_array_init (sc_recycle_array_t * rec_array,
                                           size_t elem_size);

/** Reset a recycle array.
 *
 * As with all _reset functions, calling _init, then any array operations,
 * then _reset is memory neutral.
 */
void                sc_recycle_array_reset (sc_recycle_array_t * rec_array);

/** Insert an object into the recycle array.
 * The object is not copied into the array.  Use the return value for that.
 *
 * \param [out] position   If position != NULL, *position is set to the
 *                         array position of the inserted object.
 * \return                 Returns the new address of the object in the array.
 */
void               *sc_recycle_array_insert (sc_recycle_array_t * rec_array,
                                             size_t * position);

/** Remove an object from the recycle array.  It must be valid.
 *
 * \param [in] position   Index into the array for the object to remove.
 * \return                The pointer to the removed object.  Will be valid
 *                        as long as no other function is called
 *                        on this recycle array.
 */
void               *sc_recycle_array_remove (sc_recycle_array_t * rec_array,
                                             size_t position);

SC_EXTERN_C_END;

#endif /* !SC_CONTAINERS_H */
