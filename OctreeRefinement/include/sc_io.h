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

#ifndef SC_IO_H
#define SC_IO_H

#include <sc.h>
#include <sc_containers.h>

SC_EXTERN_C_BEGIN;

/** Error values for io.
 */
typedef enum
{
  SC_IO_ERROR_NONE,     /**< The value of zero means no error. */
  SC_IO_ERROR_FATAL = -1,       /**< The io object is now disfunctional. */
  SC_IO_ERROR_AGAIN = -2        /**< Another io operation may resolve it.
                                The function just returned was a noop. */
}
sc_io_error_t;

typedef enum
{
  SC_IO_MODE_WRITE,     /**< Semantics as "w" in fopen. */
  SC_IO_MODE_APPEND,    /**< Semantics as "a" in fopen. */
  SC_IO_MODE_LAST       /**< Invalid entry to close list */
}
sc_io_mode_t;

typedef enum
{
  SC_IO_ENCODE_NONE,
  SC_IO_ENCODE_LAST     /**< Invalid entry to close list */
}
sc_io_encode_t;

typedef enum
{
  SC_IO_TYPE_BUFFER,
  SC_IO_TYPE_FILENAME,
  SC_IO_TYPE_FILEFILE,
  SC_IO_TYPE_LAST       /**< Invalid entry to close list */
}
sc_io_type_t;

typedef struct sc_io_sink
{
  sc_io_type_t        iotype;
  sc_io_mode_t        mode;
  sc_io_encode_t      encode;
  sc_array_t         *buffer;
  size_t              buffer_bytes;    /**< distinguish from array elems */
  FILE               *file;
  size_t              bytes_in;
  size_t              bytes_out;
}
sc_io_sink_t;

typedef struct sc_io_source
{
  sc_io_type_t        iotype;
  sc_io_encode_t      encode;
  sc_array_t         *buffer;
  size_t              buffer_bytes;    /**< distinguish from array elems */
  FILE               *file;
  size_t              bytes_in;
  size_t              bytes_out;
  sc_io_sink_t       *mirror;
  sc_array_t         *mirror_buffer;
}
sc_io_source_t;

/** Create a generic data sink.
 * \param [in] iotype           Type of the sink.
 *                              Depending on iotype, varargs must follow:
 *                              BUFFER: sc_array_t * (existing array).
 *                              FILENAME: const char * (name of file to open).
 *                              FILEFILE: FILE * (file open for writing).
 *                              These buffers are only borrowed by the sink.
 * \param [in] mode             Mode to add data to sink.
 *                              For type FILEFILE, data is always appended.
 * \param [in] encode           Type of data encoding.
 * \return                      Newly allocated sink, or NULL on error.
 */
sc_io_sink_t       *sc_io_sink_new (sc_io_type_t iotype,
                                    sc_io_mode_t mode,
                                    sc_io_encode_t encode, ...);

/** Free data sink.
 * Calls sc_io_sink_complete and discards the final counts.
 * Errors from complete lead to SC_IO_ERROR_FATAL returned from this function.
 * Call sc_io_sink_complete yourself if bytes_out is of interest.
 * \param [in,out] sink         The sink object to complete and free.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_sink_destroy (sc_io_sink_t * sink);

/** Write data to a sink.  Data may be buffered and sunk in a later call.
 * The internal counters sink->bytes_in and sink->bytes_out are updated.
 * \param [in,out] sink         The sink object to write to.
 * \param [in] data             Data passed into sink.
 * \param [in] bytes_avail      Number of data bytes passed in.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_sink_write (sc_io_sink_t * sink,
                                      const void *data, size_t bytes_avail);

/** Flush all buffered output data to sink.
 * This function may return SC_IO_ERROR_AGAIN if another write is required.
 * Currently this may happen if BUFFER requires an integer multiple of bytes.
 * If successful, the updated value of bytes read and written is returned
 * in bytes_in/out, and the sink status is reset as if the sink had just
 * been created.  In particular, the bytes counters are reset to zero.
 * The internal state of the sink is not changed otherwise.
 * It is legal to continue writing to the sink hereafter.
 * The sink actions taken depend on its type.
 * BUFFER, FILEFILE: none.
 * FILENAME: call fclose on sink->file.
 * \param [in,out] sink         The sink object to write to.
 * \param [in,out] bytes_in     Bytes received since the last new or complete
 *                              call.  May be NULL.
 * \param [in,out] bytes_out    Bytes written since the last new or complete
 *                              call.  May be NULL.
 * \return                      0 if completed, nonzero on error.
 */
int                 sc_io_sink_complete (sc_io_sink_t * sink,
                                         size_t * bytes_in,
                                         size_t * bytes_out);

/** Align sink to a byte boundary by writing zeros.
 * \param [in,out] sink         The sink object to align.
 * \param [in] bytes_align      Byte boundary.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_sink_align (sc_io_sink_t * sink,
                                      size_t bytes_align);

/** Create a generic data source.
 * \param [in] iotype           Type of the source.
 *                              Depending on iotype, varargs must follow:
 *                              BUFFER: sc_array_t * (existing array).
 *                              FILENAME: const char * (name of file to open).
 *                              FILEFILE: FILE * (file open for reading).
 * \param [in] encode           Type of data encoding.
 * \return                      Newly allocated source, or NULL on error.
 */
sc_io_source_t     *sc_io_source_new (sc_io_type_t iotype,
                                      sc_io_encode_t encode, ...);

/** Free data source.
 * Calls sc_io_source_complete and requires it to return no error.
 * This is to avoid discarding buffered data that has not been passed to read.
 * \param [in,out] source       The source object to free.
 * \return                      0 on success.  Nonzero if an error is
 *                              encountered or is_complete returns one.
 */
int                 sc_io_source_destroy (sc_io_source_t * source);

/** Read data from a source.
 * The internal counters source->bytes_in and source->bytes_out are updated.
 * Data is read until the data buffer has not enough room anymore, or source
 * becomes empty.  It is possible that data already read internally remains
 * in the source object for the next call.  Call sc_io_source_complete and
 * check its return value to find out.
 * Returns an error if bytes_out is NULL and less than bytes_avail are read.
 * \param [in,out] source       The source object to read from.
 * \param [in] data             Data buffer for reading from sink.
 *                              If NULL the output data will be thrown away.
 * \param [in] bytes_avail      Number of bytes available in data buffer.
 * \param [in,out] bytes_out    If not NULL, byte count read into data buffer.
 *                              Otherwise, requires to read exactly bytes_avail.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_source_read (sc_io_source_t * source,
                                       void *data, size_t bytes_avail,
                                       size_t * bytes_out);

/** Determine whether all data buffered from source has been returned by read.
 * If it returns SC_IO_ERROR_AGAIN, another sc_io_source_read is required.
 * If the call returns no error, the internal counters source->bytes_in and
 * source->bytes_out are returned to the caller if requested, and reset to 0.
 * The internal state of the source is not changed otherwise.
 * It is legal to continue reading from the source hereafter.
 *
 * \param [in,out] source       The source object to read from.
 * \param [in,out] bytes_in     If not NULL and true is returned,
 *                              the total size of the data sourced.
 * \param [in,out] bytes_out    If not NULL and true is returned,
 *                              total bytes passed out by source_read.
 * \return                      SC_IO_ERROR_AGAIN if buffered data remaining.
 *                              Otherwise return ERROR_NONE and reset counters.
 */
int                 sc_io_source_complete (sc_io_source_t * source,
                                           size_t * bytes_in,
                                           size_t * bytes_out);

/** Align source to a byte boundary by skipping.
 * \param [in,out] source       The source object to align.
 * \param [in] bytes_align      Byte boundary.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_source_align (sc_io_source_t * source,
                                        size_t bytes_align);

/** Activate a buffer that mirrors (i.e., stores) the data that was read.
 * \param [in,out] source       The source object to activate mirror in.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_source_activate_mirror (sc_io_source_t * source);

/** Read data from the source's mirror.
 * Same behaviour as sc_io_source_read.
 * \param [in,out] source       The source object to read mirror data from.
 * \return                      0 on success, nonzero on error.
 */
int                 sc_io_source_read_mirror (sc_io_source_t * source,
                                              void *data,
                                              size_t bytes_avail,
                                              size_t * bytes_out);

/** This function writes numeric binary data in VTK base64 encoding.
 * \param vtkfile        Stream openened for writing.
 * \param numeric_data   A pointer to a numeric data array.
 * \param byte_length    The length of the data array in bytes.
 * \return               Returns 0 on success, -1 on file error.
 */
int                 sc_vtk_write_binary (FILE * vtkfile, char *numeric_data,
                                         size_t byte_length);

/** This function writes numeric binary data in VTK compressed format.
 * \param vtkfile        Stream openened for writing.
 * \param numeric_data   A pointer to a numeric data array.
 * \param byte_length    The length of the data array in bytes.
 * \return               Returns 0 on success, -1 on file error.
 */
int                 sc_vtk_write_compressed (FILE * vtkfile,
                                             char *numeric_data,
                                             size_t byte_length);

/** Write memory content to a file.
 * \param [in] ptr      Data array to write to disk.
 * \param [in] size     Size of one array member.
 * \param [in] nmemb    Number of array members.
 * \param [in,out] file File pointer, must be opened for writing.
 * \param [in] errmsg   Error message passed to SC_CHECK_ABORT.
 * \note                This function aborts on file errors.
 */
void                sc_fwrite (const void *ptr, size_t size,
                               size_t nmemb, FILE * file, const char *errmsg);

/** Read file content into memory.
 * \param [out] ptr     Data array to read from disk.
 * \param [in] size     Size of one array member.
 * \param [in] nmemb    Number of array members.
 * \param [in,out] file File pointer, must be opened for reading.
 * \param [in] errmsg   Error message passed to SC_CHECK_ABORT.
 * \note                This function aborts on file errors.
 */
void                sc_fread (void *ptr, size_t size,
                              size_t nmemb, FILE * file, const char *errmsg);

#ifdef SC_ENABLE_MPIIO

/** Write memory content to an MPI file.
 * \param [in,out] mpifile      MPI file object opened for writing.
 * \param [in] ptr      Data array to write to disk.
 * \param [in] zcount   Number of array members.
 * \param [in] t        The MPI type for each array member.
 * \param [in] errmsg   Error message passed to SC_CHECK_ABORT.
 * \note                This function aborts on MPI file and count errors.
 */
void                sc_mpi_write (MPI_File mpifile, const void *ptr,
                                  size_t zcount, sc_MPI_Datatype t,
                                  const char *errmsg);

#endif

SC_EXTERN_C_END;

#endif /* SC_IO_H */
