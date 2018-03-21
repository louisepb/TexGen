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

#ifndef SC_STATISTICS_H
#define SC_STATISTICS_H

#include <sc.h>
#include <sc_keyvalue.h>

SC_EXTERN_C_BEGIN;

/* sc_statinfo_t stores information for one random variable */
typedef struct sc_statinfo
{
  int                 dirty;    /* only update stats if this is true */
  long                count;    /* inout, global count is 52bit accurate */
  double              sum_values, sum_squares, min, max;        /* inout */
  int                 min_at_rank, max_at_rank; /* out */
  double              average, variance, standev;       /* out */
  double              variance_mean, standev_mean;      /* out */
  const char         *variable; /* name of the variable for output */
}
sc_statinfo_t;

/* sc_statistics_t allows dynamically adding random variables */
typedef struct sc_stats
{
  sc_MPI_Comm         mpicomm;
  sc_keyvalue_t      *kv;
  sc_array_t         *sarray;
}
sc_statistics_t;

/**
 * Populate a sc_statinfo_t structure assuming count=1 and mark it dirty.
 */
void                sc_stats_set1 (sc_statinfo_t * stats,
                                   double value, const char *variable);

/**
 * Initialize a sc_statinfo_t structure assuming count=0 and mark it dirty.
 * This is useful if \a stats will be used to accumulate instances locally
 * before global statistics are computed.
 */
void                sc_stats_init (sc_statinfo_t * stats,
                                   const char *variable);

/**
 * Add an instance of the random variable.
 */
void                sc_stats_accumulate (sc_statinfo_t * stats, double value);

/**
 * Compute global average and standard deviation.
 * Only updates dirty variables. Then removes the dirty flag.
 * \param [in]     mpicomm   MPI communicator to use.
 * \param [in]     nvars     Number of variables to be examined.
 * \param [in,out] stats     Set of statisitcs for each variable.
 * On input, set the following fields for each variable separately.
 *    count         Number of values for each process.
 *    sum_values    Sum of values for each process.
 *    sum_squares   Sum of squares for each process.
 *    min, max      Minimum and maximum of values for each process.
 *    variable      String describing the variable, or NULL.
 * On output, the fields have the following meaning.
 *    count                        Global number of values.
 *    sum_values                   Global sum of values.
 *    sum_squares                  Global sum of squares.
 *    min, max                     Global minimum and maximum values.
 *    min_at_rank, max_at_rank     The ranks that attain min and max.
 *    average, variance, standev   Global statistical measures.
 *    variance_mean, standev_mean  Statistical measures of the mean.
 */
void                sc_stats_compute (sc_MPI_Comm mpicomm, int nvars,
                                      sc_statinfo_t * stats);

/**
 * Version of sc_statistics_statistics that assumes count=1.
 * On input, the field sum_values needs to be set to the value
 * and the field variable must contain a valid string or NULL.
 * Only updates dirty variables. Then removes the dirty flag.
 */
void                sc_stats_compute1 (sc_MPI_Comm mpicomm, int nvars,
                                       sc_statinfo_t * stats);

/**
 * Print measured statistics.
 * This function uses the SC_LC_GLOBAL log category.
 * That means the default action is to print only on rank 0.
 * Applications can change that by providing a user-defined log handler.
 * \param [in] package_id       Registered package id or -1.
 * \param [in] log_priority     Log priority for output according to sc.h.
 * \param [in] full             Print full information for every variable.
 * \param [in] summary          Print summary information all on 1 line.
 */
void                sc_stats_print (int package_id, int log_priority,
                                    int nvars, sc_statinfo_t * stats,
                                    int full, int summary);

/** Create a new statistics structure that can grow dynamically.
 */
sc_statistics_t    *sc_statistics_new (sc_MPI_Comm mpicomm);
void                sc_statistics_destroy (sc_statistics_t * stats);

/** Register a statistics variable by name and set its value to 0.
 * This variable must not exist already.
 */
void                sc_statistics_add (sc_statistics_t * stats,
                                       const char *name);

/** Register a statistics variable by name and set its count to 0.
 * This variable must not exist already.
 */
void                sc_statistics_add_empty (sc_statistics_t * stats,
                                             const char *name);

/** Returns true if the stats include a variable with the given name */
int                 sc_statistics_has (sc_statistics_t * stats,
                                       const char *name);
/** Set the value of a statistics variable, see sc_stats_set1.
 * The variable must previously be added with sc_statistics_add.
 * This assumes count=1 as in the sc_stats_set1 function above.
 */
void                sc_statistics_set (sc_statistics_t * stats,
                                       const char *name, double value);

/** Add an instance of a statistics variable, see sc_stats_accumulate
 * The variable must previously be added with sc_statistics_add_empty.
 */
void                sc_statistics_accumulate (sc_statistics_t * stats,
                                              const char *name, double value);

/** Compute statistics for all variables, see sc_stats_compute.
 */
void                sc_statistics_compute (sc_statistics_t * stats);

/** Print all statistics variables, see sc_stats_print.
 */
void                sc_statistics_print (sc_statistics_t * stats,
                                         int package_id, int log_priority,
                                         int full, int summary);

SC_EXTERN_C_END;

#endif /* !SC_STATISTICS_H */
