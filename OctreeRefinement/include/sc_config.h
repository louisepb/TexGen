#ifndef _SRC_SC_CONFIG_H
#define _SRC_SC_CONFIG_H 1

/* src/sc_config.h. Generated automatically at end of configure. */
/* src/pre_config.h.  Generated from pre_config.h.in by configure.  */
/* src/pre_config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* C compiler */
#ifndef SC_CC
#define SC_CC "gcc"
#endif

/* C compiler flags */
#ifndef SC_CFLAGS
#define SC_CFLAGS "-g -O2"
#endif

/* C preprocessor */
#ifndef SC_CPP
#define SC_CPP "gcc -E"
#endif

/* C preprocessor flags */
#ifndef SC_CPPFLAGS
#define SC_CPPFLAGS ""
#endif

/* CXX compiler */
#ifndef SC_CXX
#define SC_CXX "g++"
#endif

/* CXX compiler flags */
#ifndef SC_CXXFLAGS
#define SC_CXXFLAGS "-g -O2"
#endif

/* Define to 1 if your C++ compiler doesn't accept -c and -o together. */
/* #undef CXX_NO_MINUS_C_MINUS_O */

/* DEPRECATED (use SC_ENABLE_DEBUG instead) */
/* #undef DEBUG */

/* enable debug mode (assertions and extra checks) */
/* #undef ENABLE_DEBUG */

/* Undefine if: use aligned malloc (optionally use --enable-memalign=<bytes>)
   */
#ifndef SC_ENABLE_MEMALIGN
#define SC_ENABLE_MEMALIGN 1
#endif

/* Define to 1 if we are using MPI */
/* #undef ENABLE_MPI */

/* Define to 1 if we can use MPI_COMM_TYPE_SHARED */
/* #undef ENABLE_MPICOMMSHARED */

/* Define to 1 if we are using MPI I/O */
/* #undef ENABLE_MPIIO */

/* Define to 1 if we are using MPI_Init_thread */
/* #undef ENABLE_MPITHREAD */

/* Define to 1 if we can use MPI_Win_allocate_shared */
/* #undef ENABLE_MPIWINSHARED */

/* enable OpenMP (optionally use --enable-openmp=<OPENMP_CFLAGS>) */
/* #undef ENABLE_OPENMP */

/* enable POSIX threads (optionally use --enable-pthread=<PTHREAD_CFLAGS>) */
/* #undef ENABLE_PTHREAD */

/* Undefine if: replace array/dmatrix resize with malloc/copy/free */
#ifndef SC_ENABLE_USE_REALLOC
#define SC_ENABLE_USE_REALLOC 1
#endif

/* F77 compiler */
#ifndef SC_F77
#define SC_F77 "gfortran"
#endif

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef F77_DUMMY_MAIN */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#ifndef SC_F77_FUNC
#define SC_F77_FUNC(name,NAME) name ## _
#endif

/* As F77_FUNC, but for C identifiers containing underscores. */
#ifndef SC_F77_FUNC_
#define SC_F77_FUNC_(name,NAME) name ## _
#endif

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef F77_NO_MINUS_C_MINUS_O */

/* FC compiler */
#ifndef SC_FC
#define SC_FC "gfortran"
#endif

/* FC compiler flags */
#ifndef SC_FCFLAGS
#define SC_FCFLAGS "-g -O2"
#endif

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef FC_DUMMY_MAIN */

/* Define if F77 and FC dummy `main' functions are identical. */
/* #undef FC_DUMMY_MAIN_EQ_F77 */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#ifndef SC_FC_FUNC
#define SC_FC_FUNC(name,NAME) name ## _
#endif

/* As FC_FUNC, but for C identifiers containing underscores. */
#ifndef SC_FC_FUNC_
#define SC_FC_FUNC_(name,NAME) name ## _
#endif

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef FC_NO_MINUS_C_MINUS_O */

/* F77 compiler flags */
#ifndef SC_FFLAGS
#define SC_FFLAGS "-g -O2"
#endif

/* Define to 1 if you have the `aligned_alloc' function. */
#ifndef SC_HAVE_ALIGNED_ALLOC
#define SC_HAVE_ALIGNED_ALLOC 1
#endif

/* Define to 1 if you have the `backtrace' function. */
/* #undef HAVE_BACKTRACE */

/* Define to 1 if you have the `backtrace_symbols' function. */
/* #undef HAVE_BACKTRACE_SYMBOLS */

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef SC_HAVE_DLFCN_H
#define SC_HAVE_DLFCN_H 1
#endif

/* Define to 1 if you have the <execinfo.h> header file. */
/* #undef HAVE_EXECINFO_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef SC_HAVE_INTTYPES_H
#define SC_HAVE_INTTYPES_H 1
#endif

/* Have we found function pthread_create. */
#ifndef SC_HAVE_LPTHREAD
#define SC_HAVE_LPTHREAD 1
#endif

/* Have we found function lua_createtable. */
/* #undef HAVE_LUA */

/* Define to 1 if you have the <lua5.1/lua.h> header file. */
/* #undef HAVE_LUA5_1_LUA_H */

/* Define to 1 if you have the <lua5.2/lua.h> header file. */
/* #undef HAVE_LUA5_2_LUA_H */

/* Define to 1 if you have the <lua5.3/lua.h> header file. */
/* #undef HAVE_LUA5_3_LUA_H */

/* Define to 1 if you have the <lua.h> header file. */
/* #undef HAVE_LUA_H */

/* Define to 1 if you have the <memory.h> header file. */
#ifndef SC_HAVE_MEMORY_H
#define SC_HAVE_MEMORY_H 1
#endif

/* Have we found function omp_get_thread_num. */
#ifndef SC_HAVE_OPENMP
#define SC_HAVE_OPENMP 1
#endif

/* Define to 1 if you have the `posix_memalign' function. */
#ifndef SC_HAVE_POSIX_MEMALIGN
#define SC_HAVE_POSIX_MEMALIGN 1
#endif

/* Define to 1 if you have the <signal.h> header file. */
#ifndef SC_HAVE_SIGNAL_H
#define SC_HAVE_SIGNAL_H 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef SC_HAVE_STDINT_H
#define SC_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef SC_HAVE_STDLIB_H
#define SC_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef SC_HAVE_STRINGS_H
#define SC_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef SC_HAVE_STRING_H
#define SC_HAVE_STRING_H 1
#endif

/* Define to 1 if you have the `strtol' function. */
#ifndef SC_HAVE_STRTOL
#define SC_HAVE_STRTOL 1
#endif

/* Define to 1 if you have the `strtoll' function. */
#ifndef SC_HAVE_STRTOLL
#define SC_HAVE_STRTOLL 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef SC_HAVE_SYS_STAT_H
#define SC_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef SC_HAVE_SYS_TIME_H
#define SC_HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef SC_HAVE_SYS_TYPES_H
#define SC_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <time.h> header file. */
#ifndef SC_HAVE_TIME_H
#define SC_HAVE_TIME_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef SC_HAVE_UNISTD_H
#define SC_HAVE_UNISTD_H 1
#endif

/* Have we found function adler32_combine. */
#ifndef SC_HAVE_ZLIB
#define SC_HAVE_ZLIB 1
#endif

/* Define to 1 on a bigendian machine */
/* #undef IS_BIGENDIAN */

/* Linker flags */
#ifndef SC_LDFLAGS
#define SC_LDFLAGS ""
#endif

/* Libraries */
#ifndef SC_LIBS
#define SC_LIBS "-lgomp   -lz   "
#endif

/* minimal log priority */
/* #undef LOG_PRIORITY */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#ifndef SC_LT_OBJDIR
#define SC_LT_OBJDIR ".libs/"
#endif

/* DEPRECATED (use SC_ENABLE_MEMALIGN instead) */
#ifndef SC_MEMALIGN
#define SC_MEMALIGN 1
#endif

/* desired alignment of allocations in bytes */
#ifndef SC_MEMALIGN_BYTES
#define SC_MEMALIGN_BYTES (SC_SIZEOF_VOID_P)
#endif

/* DEPRECATED (use SC_ENABLE_MPI instead) */
/* #undef MPI */

/* DEPRECATED (use SC_ENABLE_MPIIO instead) */
/* #undef MPIIO */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* DEPRECATED (use SC_ENABLE_OPENMP instead) */
/* #undef OPENMP */

/* Name of package */
#ifndef SC_PACKAGE
#define SC_PACKAGE "libsc"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef SC_PACKAGE_BUGREPORT
#define SC_PACKAGE_BUGREPORT "info@p4est.org"
#endif

/* Define to the full name of this package. */
#ifndef SC_PACKAGE_NAME
#define SC_PACKAGE_NAME "libsc"
#endif

/* Define to the full name and version of this package. */
#ifndef SC_PACKAGE_STRING
#define SC_PACKAGE_STRING "libsc UNKNOWN"
#endif

/* Define to the one symbol short name of this package. */
#ifndef SC_PACKAGE_TARNAME
#define SC_PACKAGE_TARNAME "libsc"
#endif

/* Define to the home page for this package. */
#ifndef SC_PACKAGE_URL
#define SC_PACKAGE_URL ""
#endif

/* Define to the version of this package. */
#ifndef SC_PACKAGE_VERSION
#define SC_PACKAGE_VERSION "UNKNOWN"
#endif

/* DEPRECATED (use SC_WITH_PAPI instead) */
/* #undef PAPI */

/* Use builtin getopt */
/* #undef PROVIDE_GETOPT */

/* Use builtin obstack */
#ifndef SC_PROVIDE_OBSTACK
#define SC_PROVIDE_OBSTACK 1
#endif

/* DEPRECATED (use SC_ENABLE_PTHREAD instead) */
/* #undef PTHREAD */

/* The size of `int', as computed by sizeof. */
#ifndef SC_SIZEOF_INT
#define SC_SIZEOF_INT 4
#endif

/* The size of `long', as computed by sizeof. */
#ifndef SC_SIZEOF_LONG
#define SC_SIZEOF_LONG 8
#endif

/* The size of `long long', as computed by sizeof. */
#ifndef SC_SIZEOF_LONG_LONG
#define SC_SIZEOF_LONG_LONG 8
#endif

/* The size of `unsigned long', as computed by sizeof. */
#ifndef SC_SIZEOF_UNSIGNED_LONG
#define SC_SIZEOF_UNSIGNED_LONG 8
#endif

/* The size of `unsigned long long', as computed by sizeof. */
#ifndef SC_SIZEOF_UNSIGNED_LONG_LONG
#define SC_SIZEOF_UNSIGNED_LONG_LONG 8
#endif

/* The size of `void *', as computed by sizeof. */
#ifndef SC_SIZEOF_VOID_P
#define SC_SIZEOF_VOID_P 8
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef SC_STDC_HEADERS
#define SC_STDC_HEADERS 1
#endif

/* DEPRECATED (use SC_ENABLE_USE_REALLOC instead) */
#ifndef SC_USE_REALLOC
#define SC_USE_REALLOC 1
#endif

/* Version number of package */
#ifndef SC_VERSION
#define SC_VERSION "UNKNOWN"
#endif

/* Package major version */
#ifndef SC_VERSION_MAJOR
#define SC_VERSION_MAJOR UNKNOWN
#endif

/* Package minor version */
#ifndef SC_VERSION_MINOR
#define SC_VERSION_MINOR N
#endif

/* Package point version */
#ifndef SC_VERSION_POINT
#define SC_VERSION_POINT
#endif

/* Define to 1 if BLAS is used */
/* #undef WITH_BLAS */

/* Define to 1 if LAPACK is used */
/* #undef WITH_LAPACK */

/* enable Flop counting with papi */
/* #undef WITH_PAPI */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#ifndef _sc_restrict
#define _sc_restrict __restrict
#endif
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* once: _SRC_SC_CONFIG_H */
#endif
