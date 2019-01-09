#ifndef _SRC_P_EST_CONFIG_H
#define _SRC_P_EST_CONFIG_H 1

/* src/p4est_config.h. Generated automatically at end of configure. */
/* src/pre_config.h.  Generated from pre_config.h.in by configure.  */
/* src/pre_config.h.in.  Generated from configure.ac by autoheader.  */

/* DEPRECATED (use P4EST_ENABLE_BUILD_2D instead) */
#ifndef P4EST_BUILD_2D
#define P4EST_BUILD_2D 1
#endif

/* DEPRECATED (use P4EST_ENABLE_BUILD_3D instead) */
#ifndef P4EST_BUILD_3D
#define P4EST_BUILD_3D 1
#endif

/* DEPRECATED (use P4EST_ENABLE_BUILD_P6EST instead) */
#ifndef P4EST_BUILD_P6EST
#define P4EST_BUILD_P6EST 1
#endif

/* C compiler */
#ifndef P4EST_CC
#define P4EST_CC "gcc"
#endif

/* C compiler flags */
#ifndef P4EST_CFLAGS
#define P4EST_CFLAGS "-g -O2"
#endif

/* C preprocessor */
#ifndef P4EST_CPP
#define P4EST_CPP "gcc -E"
#endif

/* C preprocessor flags */
#ifndef P4EST_CPPFLAGS
#define P4EST_CPPFLAGS ""
#endif

/* Define to 1 if your C++ compiler doesn't accept -c and -o together. */
/* #undef CXX_NO_MINUS_C_MINUS_O */

/* DEPRECATED (use P4EST_ENABLE_DEBUG instead) */
/* #undef DEBUG */

/* Undefine if: disable the 2D library */
#ifndef P4EST_ENABLE_BUILD_2D
#define P4EST_ENABLE_BUILD_2D 1
#endif

/* Undefine if: disable the 3D library */
#ifndef P4EST_ENABLE_BUILD_3D
#define P4EST_ENABLE_BUILD_3D 1
#endif

/* Undefine if: disable hybrid 2D+1D p6est library */
#ifndef P4EST_ENABLE_BUILD_P6EST
#define P4EST_ENABLE_BUILD_P6EST 1
#endif

/* enable debug mode (assertions and extra checks) */
/* #undef ENABLE_DEBUG */

/* Undefine if: use aligned malloc (optionally use --enable-memalign=<bytes>)
   */
#ifndef P4EST_ENABLE_MEMALIGN
#define P4EST_ENABLE_MEMALIGN 1
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

/* Undefine if: write vtk ascii file data */
/* #undef ENABLE_VTK_BINARY */

/* Undefine if: disable zlib compression for vtk binary data */
#ifndef P4EST_ENABLE_VTK_COMPRESSION
#define P4EST_ENABLE_VTK_COMPRESSION 1
#endif

/* use doubles for vtk file data */
/* #undef ENABLE_VTK_DOUBLES */

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef F77_DUMMY_MAIN */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#ifndef P4EST_F77_FUNC
#define P4EST_F77_FUNC(name,NAME) name ## _
#endif

/* As F77_FUNC, but for C identifiers containing underscores. */
#ifndef P4EST_F77_FUNC_
#define P4EST_F77_FUNC_(name,NAME) name ## _
#endif

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef F77_NO_MINUS_C_MINUS_O */

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef FC_DUMMY_MAIN */

/* Define if F77 and FC dummy `main' functions are identical. */
/* #undef FC_DUMMY_MAIN_EQ_F77 */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#ifndef P4EST_FC_FUNC
#define P4EST_FC_FUNC(name,NAME) name ## _
#endif

/* As FC_FUNC, but for C identifiers containing underscores. */
#ifndef P4EST_FC_FUNC_
#define P4EST_FC_FUNC_(name,NAME) name ## _
#endif

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef FC_NO_MINUS_C_MINUS_O */

/* Define to 1 if you have the `aligned_alloc' function. */
#ifndef P4EST_HAVE_ALIGNED_ALLOC
#define P4EST_HAVE_ALIGNED_ALLOC 1
#endif

/* Define to 1 if you have the <arpa/inet.h> header file. */
#ifndef P4EST_HAVE_ARPA_INET_H
#define P4EST_HAVE_ARPA_INET_H 1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef P4EST_HAVE_DLFCN_H
#define P4EST_HAVE_DLFCN_H 1
#endif

/* Define to 1 if you have the `fsync' function. */
#ifndef P4EST_HAVE_FSYNC
#define P4EST_HAVE_FSYNC 1
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef P4EST_HAVE_INTTYPES_H
#define P4EST_HAVE_INTTYPES_H 1
#endif

/* Have we found function pthread_create. */
#ifndef P4EST_HAVE_LPTHREAD
#define P4EST_HAVE_LPTHREAD 1
#endif

/* Have we found function lua_createtable. */
/* #undef HAVE_LUA */

/* Define to 1 if you have the <memory.h> header file. */
#ifndef P4EST_HAVE_MEMORY_H
#define P4EST_HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the <netinet/in.h> header file. */
#ifndef P4EST_HAVE_NETINET_IN_H
#define P4EST_HAVE_NETINET_IN_H 1
#endif

/* Have we found function omp_get_thread_num. */
#ifndef P4EST_HAVE_OPENMP
#define P4EST_HAVE_OPENMP 1
#endif

/* Define to 1 if you have the `posix_memalign' function. */
#ifndef P4EST_HAVE_POSIX_MEMALIGN
#define P4EST_HAVE_POSIX_MEMALIGN 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef P4EST_HAVE_STDINT_H
#define P4EST_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef P4EST_HAVE_STDLIB_H
#define P4EST_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef P4EST_HAVE_STRINGS_H
#define P4EST_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef P4EST_HAVE_STRING_H
#define P4EST_HAVE_STRING_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef P4EST_HAVE_SYS_STAT_H
#define P4EST_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef P4EST_HAVE_SYS_TYPES_H
#define P4EST_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef P4EST_HAVE_UNISTD_H
#define P4EST_HAVE_UNISTD_H 1
#endif

/* Have we found function adler32_combine. */
#ifndef P4EST_HAVE_ZLIB
#define P4EST_HAVE_ZLIB 1
#endif

/* Linker flags */
#ifndef P4EST_LDFLAGS
#define P4EST_LDFLAGS ""
#endif

/* Libraries */
#ifndef P4EST_LIBS
#define P4EST_LIBS "-lgomp   -lz   "
#endif

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#ifndef P4EST_LT_OBJDIR
#define P4EST_LT_OBJDIR ".libs/"
#endif

/* DEPRECATED (use P4EST_ENABLE_MEMALIGN instead) */
#ifndef P4EST_MEMALIGN
#define P4EST_MEMALIGN 1
#endif

/* desired alignment of allocations in bytes */
#ifndef P4EST_MEMALIGN_BYTES
#define P4EST_MEMALIGN_BYTES (P4EST_SIZEOF_VOID_P)
#endif

/* DEPRECATED (use P4EST_WITH_METIS instead) */
/* #undef METIS */

/* DEPRECATED (use P4EST_ENABLE_MPI instead) */
/* #undef MPI */

/* DEPRECATED (use P4EST_ENABLE_MPIIO instead) */
/* #undef MPIIO */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* DEPRECATED (use P4EST_ENABLE_OPENMP instead) */
/* #undef OPENMP */

/* Name of package */
#ifndef P4EST_PACKAGE
#define P4EST_PACKAGE "p4est"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef P4EST_PACKAGE_BUGREPORT
#define P4EST_PACKAGE_BUGREPORT "info@p4est.org"
#endif

/* Define to the full name of this package. */
#ifndef P4EST_PACKAGE_NAME
#define P4EST_PACKAGE_NAME "p4est"
#endif

/* Define to the full name and version of this package. */
#ifndef P4EST_PACKAGE_STRING
#define P4EST_PACKAGE_STRING "p4est UNKNOWN"
#endif

/* Define to the one symbol short name of this package. */
#ifndef P4EST_PACKAGE_TARNAME
#define P4EST_PACKAGE_TARNAME "p4est"
#endif

/* Define to the home page for this package. */
#ifndef P4EST_PACKAGE_URL
#define P4EST_PACKAGE_URL ""
#endif

/* Define to the version of this package. */
#ifndef P4EST_PACKAGE_VERSION
#define P4EST_PACKAGE_VERSION "UNKNOWN"
#endif

/* DEPRECATED (use P4EST_WITH_PETSC instead) */
/* #undef PETSC */

/* Use builtin getopt */
/* #undef PROVIDE_GETOPT */

/* Use builtin obstack */
#ifndef P4EST_PROVIDE_OBSTACK
#define P4EST_PROVIDE_OBSTACK 1
#endif

/* DEPRECATED (use P4EST_ENABLE_PTHREAD instead) */
/* #undef PTHREAD */

/* DEPRECATED (use P4EST_WITH_SC instead) */
/* #undef SC */

/* The size of `void *', as computed by sizeof. */
#ifndef P4EST_SIZEOF_VOID_P
#define P4EST_SIZEOF_VOID_P 8
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef P4EST_STDC_HEADERS
#define P4EST_STDC_HEADERS 1
#endif

/* Version number of package */
#ifndef P4EST_VERSION
#define P4EST_VERSION "UNKNOWN"
#endif

/* Package major version */
#ifndef P4EST_VERSION_MAJOR
#define P4EST_VERSION_MAJOR UNKNOWN
#endif

/* Package minor version */
#ifndef P4EST_VERSION_MINOR
#define P4EST_VERSION_MINOR N
#endif

/* Package point version */
#ifndef P4EST_VERSION_POINT
#define P4EST_VERSION_POINT
#endif

/* DEPRECATED (use P4EST_ENABLE_VTK_BINARY instead) */
/* #undef VTK_BINARY */

/* DEPRECATED (use P4EST_ENABLE_VTK_COMPRESSION instead) */
#ifndef P4EST_VTK_COMPRESSION
#define P4EST_VTK_COMPRESSION 1
#endif

/* DEPRECATED (use P4EST_ENABLE_VTK_DOUBLES instead) */
/* #undef VTK_DOUBLES */

/* Define to 1 if BLAS is used */
/* #undef WITH_BLAS */

/* Define to 1 if LAPACK is used */
/* #undef WITH_LAPACK */

/* enable metis-dependent code */
/* #undef WITH_METIS */

/* enable PETSc-dependent code */
/* #undef WITH_PETSC */

/* path to installed package sc (optional) */
/* #undef WITH_SC */

/* once: _SRC_P_EST_CONFIG_H */
#endif
