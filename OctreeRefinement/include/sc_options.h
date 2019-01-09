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

#ifndef SC_OPTIONS_H
#define SC_OPTIONS_H

/** \file sc_options.h
 * Register and parse command line options and read/write configuration files.
 */

#include <sc_containers.h>
#include <sc_keyvalue.h>

SC_EXTERN_C_BEGIN;

/** The options data structure is opaque. */
typedef struct sc_options sc_options_t;

/** This callback can be invoked during sc_options_parse.
 * \param [in] opt      Valid options data structure.
 *                      This is passed in case a file should be loaded.
 * \param [in] optarg   The option argument or NULL if there is none.
 * \param [in] data     User-defined data passed to sc_options_add_callback.
 * \return              Return 0 if successful, -1 on error.
 */
typedef int         (*sc_options_callback_t) (sc_options_t * opt,
                                              const char *optarg, void *data);

/** Create an empty options structure.
 * \param [in] program_path   Name or path name of the program to display.
 *                            Usually argv[0] is fine.
 * \return                    A valid and empty options structure.
 */
sc_options_t       *sc_options_new (const char *program_path);

/** Destroy the options structure and all allocated structures contained.
 * The keyvalue structure passed into sc_keyvalue_add is destroyed.
 * \param [in,out] opt          This options structure is deallocated,
 *                              including all key-value containers referenced.
 * \deprecated                  This function may go away soon.
 */
void                sc_options_destroy_deep (sc_options_t * opt);

/** Destroy the options structure.
 * Whatever has been passed into sc_keyvalue_add is left alone.
 * \param [in,out] opt          This options structure is deallocated.
 */
void                sc_options_destroy (sc_options_t * opt);

/** Set the spacing for \ref sc_options_print_summary.
 * There are two values to be set: the spacing from the beginning of the
 * printed line to the type of the option variable, and from the beginning
 * of the printed line to the help string.
 * \param [in,out] opt          Valid options structure.
 * \param [in] space_type       Number of spaces to the type display, for
 *                              example \<INT\>, \<STRING\>, etc.
 *                              Setting this negative sets the default 20.
 * \param [in] space_help       Number of space to the help string.
 *                              Setting this negative sets the default 32.
 */
void                sc_options_set_spacing (sc_options_t * opt,
                                            int space_type, int space_help);

/** Add a switch option. This option is used without option arguments.
 * Every use increments the variable by one.  Its initial value is 0.
 * Either opt_char or opt_name must be valid, that is, not '\0'/NULL.
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] variable      Address of the variable to store the option value.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_switch (sc_options_t * opt,
                                           int opt_char,
                                           const char *opt_name,
                                           int *variable,
                                           const char *help_string);

/** Add a boolean option.
 * It can be initialized to true or false in the C sense.
 * Specifying it on the command line without argument sets the option to true.
 * The argument 0/f/F/n/N sets it to false (0).
 * The argument 1/t/T/y/Y sets it to true (nonzero).
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] variable      Address of the variable to store the option value.
 * \param [in] init_value    Initial value to set the option, read as true or false.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_bool (sc_options_t * opt,
                                         int opt_char,
                                         const char *opt_name,
                                         int *variable, int init_value,
                                         const char *help_string);

/** Add an option that takes an integer argument.
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] variable      Address of the variable to store the option value.
 * \param [in] init_value    The initial value of the option variable.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_int (sc_options_t * opt,
                                        int opt_char,
                                        const char *opt_name,
                                        int *variable, int init_value,
                                        const char *help_string);

/** Add an option that takes a size_t argument.
 * The value of the size_t variable must not be greater than LLONG_MAX.
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] variable      Address of the variable to store the option value.
 * \param [in] init_value    The initial value of the option variable.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_size_t (sc_options_t * opt,
                                           int opt_char,
                                           const char *opt_name,
                                           size_t * variable,
                                           size_t init_value,
                                           const char *help_string);

/** Add an option that takes a double argument.
 * The double must be in the legal range.  "inf" and "nan" are legal too.
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] variable      Address of the variable to store the option value.
 * \param [in] init_value    The initial value of the option variable.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_double (sc_options_t * opt,
                                           int opt_char,
                                           const char *opt_name,
                                           double *variable,
                                           double init_value,
                                           const char *help_string);

/** Add a string option.
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] variable      Address of the variable to store the option value.
 * \param [in] init_value    This default value of the option may be NULL.
 *                           If not NULL, the value is copied to internal storage.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_string (sc_options_t * opt,
                                           int opt_char,
                                           const char *opt_name,
                                           const char **variable,
                                           const char *init_value,
                                           const char *help_string);

/** Add an option to read in a file in .ini format.
 * The argument to this option must be a filename.
 * On parsing the specified file is read to set known option variables.
 * It does not have an associated option variable itself.
 * \param [in,out] opt       A valid options structure.
 * \param [in] opt_char      Short option character, may be '\0'.
 * \param [in] opt_name      Long option name without initial dashes, may be NULL.
 * \param [in] help_string   Help string for usage message, may be NULL.
 */
void                sc_options_add_inifile (sc_options_t * opt,
                                            int opt_char,
                                            const char *opt_name,
                                            const char *help_string);

/** Add an option that calls a user-defined function when parsed.
 * The callback function should be implemented to allow multiple calls.
 * The option does not have an associated variable.
 * The callback can be used to set multiple option variables in bulk that would
 * otherwise require an inconvenient number of individual options.
 * This is, however, currently not possible for options with
 * string values or key-value pairs due to the way the API is set up.
 * This function should not have non-option related side effects.
 * This option is not loaded from or saved to files.
 * \param [in,out] opt      A valid options structure.
 * \param [in] opt_char     Short option character, may be '\0'.
 * \param [in] opt_name     Long option name without initial dashes, may be NULL.
 * \param [in] has_arg      Specify if the option needs an option argument.
 * \param [in] fn           Function to call when this option is encountered.
 * \param [in] data         User-defined data passed to the callback.
 * \param [in] help_string  Help string for usage message, may be NULL.
 */
void                sc_options_add_callback (sc_options_t * opt,
                                             int opt_char,
                                             const char *opt_name,
                                             int has_arg,
                                             sc_options_callback_t fn,
                                             void *data,
                                             const char *help_string);

/** Add an option that takes string keys into a lookup table of integers.
 * On calling this function, it must be certain that the initial value exists.
 * \param [in] opt          Initialized options structure.
 * \param [in] opt_char     Option character for command line, or 0.
 * \param [in] opt_name     Name of the long option, or NULL.
 * \param [in] variable     Address of an existing integer that holds
 *                          the value of this option parameter.
 * \param [in] init_value   The key that is looked up for the initial value.
 *                          It must be certain that the key exists
 *                          and its value is of type integer.
 * \param [in] keyvalue     A valid key-value structure where the values
 *                          must be integers.  If a key is asked for that
 *                          does not exist, we will produce an option error.
 *                          This structure must stay alive as long as opt.
 * \param [in] help_string  Instructive one-line string to explain the option.
 */
void                sc_options_add_keyvalue (sc_options_t * opt,
                                             int opt_char,
                                             const char *opt_name,
                                             int *variable,
                                             const char *init_value,
                                             sc_keyvalue_t * keyvalue,
                                             const char *help_string);

/** Copy one set of options to another as a subset, with a prefix.
 * \param [in,out] opt  A set of options.
 * \param [in]  subopt  Another set of options to be copied.
 * \param [in]  prefix  The prefix to add to option names as they are copied.
 *                      If an option has a long name "name" in subopt, its
 *                      name in opt is "prefix:name"; if an option only has a
 *                      character 'c' in subopt, its name in opt is
 *                      "prefix:-c".
 */
void                sc_options_add_suboptions (sc_options_t * opt,
                                               sc_options_t * subopt,
                                               const char *prefix);

/** Print a usage message.
 * This function uses the SC_LC_GLOBAL log category.
 * That means the default action is to print only on rank 0.
 * Applications can change that by providing a user-defined log handler.
 * \param [in] package_id       Registered package id or -1.
 * \param [in] log_priority     Log priority for output according to sc.h.
 * \param [in] opt              The option structure.
 * \param [in] arg_usage        If not NULL, an \<ARGUMENTS\> string is appended
 *                              to the usage line.  If the string is non-empty,
 *                              it will be printed after the option summary
 *                              and an "ARGUMENTS:\n" title line.  Line breaks
 *                              are identified by strtok(3) and honored.
 */
void                sc_options_print_usage (int package_id, int log_priority,
                                            sc_options_t * opt,
                                            const char *arg_usage);

/** Print a summary of all option values.
 * Prints the title "Options:" and a line for every option,
 * then the title "Arguments:" and a line for every argument.
 * This function uses the SC_LC_GLOBAL log category.
 * That means the default action is to print only on rank 0.
 * Applications can change that by providing a user-defined log handler.
 * \param [in] package_id       Registered package id or -1.
 * \param [in] log_priority     Log priority for output according to sc.h.
 * \param [in] opt              The option structure.
 */
void                sc_options_print_summary (int package_id,
                                              int log_priority,
                                              sc_options_t * opt);

/** Load a file in .ini format and updates entries found under [Options].
 * An option whose name contains a colon such as "prefix:basename" will be
 * updated by a "basename =" entry in a [prefix] section.
 * \param [in] package_id       Registered package id or -1.
 * \param [in] err_priority     Error log priority according to sc.h.
 * \param [in] opt              The option structure.
 * \param [in] inifile          Filename of the ini file to load.
 * \return                      Returns 0 on success, -1 on failure.
 */
int                 sc_options_load (int package_id, int err_priority,
                                     sc_options_t * opt, const char *inifile);

/** Save all options and arguments to a file in .ini format.
 * This function must only be called after successful option parsing.
 * This function should only be called on rank 0.
 * This function will log errors with category SC_LC_GLOBAL.
 * An options whose name contains a colon such as "prefix:basename" will be
 * written in a section titled [prefix] as "basename =".
 * \param [in] package_id       Registered package id or -1.
 * \param [in] err_priority     Error log priority according to sc.h.
 * \param [in] opt              The option structure.
 * \param [in] inifile          Filename of the ini file to save.
 * \return                      Returns 0 on success, -1 on failure.
 */
int                 sc_options_save (int package_id, int err_priority,
                                     sc_options_t * opt, const char *inifile);

/** Parse command line options.
 * \param [in] package_id       Registered package id or -1.
 * \param [in] err_priority     Error log priority according to sc.h.
 * \param [in] opt              The option structure.
 * \param [in] argc             Length of argument list.
 * \param [in,out] argv         Argument list may be permuted.
 * \return                      Returns -1 on an invalid option, otherwise
 *                              the position of the first non-option argument.
 */
int                 sc_options_parse (int package_id, int err_priority,
                                      sc_options_t * opt, int argc,
                                      char **argv);

/** Load a file in .ini format and updates entries found under [Arguments].
 * There needs to be a key Arguments.count specifing the number.
 * Then as many integer keys starting with 0 need to be present.
 * \param [in] package_id       Registered package id or -1.
 * \param [in] err_priority     Error log priority according to sc.h.
 * \param [in] opt              The args are stored in this option structure.
 * \param [in] inifile          Filename of the ini file to load.
 * \return                      Returns 0 on success, -1 on failure.
 */
int                 sc_options_load_args (int package_id, int err_priority,
                                          sc_options_t * opt,
                                          const char *inifile);

SC_EXTERN_C_END;

#endif /* !SC_OPTIONS_H */
