/* -*- C++ -*- */
//=============================================================================
/**
 *  @file   config-win32-msvc-14.h
 *
 *  @brief  Microsoft Visual C++ 14.0 configuration file.
 *
 *  This file is the ACE configuration file for Microsoft Visual C++ version 2015.
 *
 *  @note Do not include this file directly, include config-win32.h instead.
 */
//=============================================================================

#ifndef ACE_CONFIG_WIN32_MSVC_14_H
#define ACE_CONFIG_WIN32_MSVC_14_H
#include /**/ "ace/pre.h"

#ifndef ACE_CONFIG_WIN32_H
#error Use config-win32.h in config.h instead of this header
#endif /* ACE_CONFIG_WIN32_H */

#ifndef ACE_WIN32_VC14
#  define ACE_WIN32_VC14
#endif

// Windows' timeval is non-conformant (defined in terms of long instead of
// time_t) and VC8 changed time_t to a 64-bit value even when compiling a
// 32-bit application. Therefore, ace/Time_Value needs to rearrange a few
// things for this compiler. See Time_Value.h for complete details.
#if !defined (_USE_32BIT_TIME_T)
#  define ACE_HAS_TIME_T_LONG_MISMATCH
#endif

#define ACE_HAS_ITOA

#define ACE_ITOA_EQUIVALENT ::_itoa
#define ACE_STRCASECMP_EQUIVALENT ::_stricmp
#define ACE_STRNCASECMP_EQUIVALENT ::_strnicmp
#define ACE_WCSDUP_EQUIVALENT ::_wcsdup
#if defined (ACE_HAS_WINCE)
# define ACE_FILENO_EQUIVALENT ::_fileno
#else
# define ACE_FILENO_EQUIVALENT(X) (_get_osfhandle (::_fileno (X)))
#endif

// Windows Mobile 6 doesn't do sig_atomic_t, but maybe future versions will.
#  if !defined (_WIN32_WCE) || (_WIN32_WCE > 0x601)
#    define ACE_HAS_SIG_ATOMIC_T
#  endif /* !Win CE 6.0 or less */

#define ACE_LACKS_STRPTIME

// Evaluate this with a WinCE build; maybe things have improved since VC8.
//#if !defined (ACE_HAS_WINCE)
# define ACE_HAS_INTRIN_H
# define ACE_HAS_INTRINSIC_INTERLOCKED
//#endif

#if !defined (_WIN32_WCE) || (_WIN32_WCE >= 0x501)
#  define ACE_HAS_INTRINSIC_BYTESWAP
#endif

#define ACE_LACKS_STRRECVFD

#define ACE_TEMPLATES_REQUIRE_SOURCE

// Platform provides ACE_TLI function prototypes.
// For Win32, this is not really true, but saves a lot of hassle!
#define ACE_HAS_TLI_PROTOTYPES

// Platform support linebuffered streaming is broken
#define ACE_LACKS_LINEBUFFERED_STREAMBUF

// ace/iostream.h does not work with the standard cpp library (yet).
# if !defined (ACE_USES_OLD_IOSTREAMS)
# define ACE_LACKS_ACE_IOSTREAM
# endif /* ! ACE_USES_OLD_IOSTREAMS */

#define ACE_NEW_THROWS_EXCEPTIONS
#define ACE_HAS_NEW_NOTHROW

// There are too many instances of this warning to fix it right now.
// Maybe in the future.

// Disable warning of using Microsoft Extension.
# pragma warning(disable:4231)

// 'class1' : inherits 'class2::member' via dominance
#pragma warning(disable:4250)

// CE (at least thru Windows Mobile 5) doesn't have the new, secure CRT.
#if !defined (ACE_HAS_WINCE) && !defined (ACE_HAS_TR24731_2005_CRT)
#  define ACE_HAS_TR24731_2005_CRT
#endif

// On CE w/o MFC config-WinCE.h needs to declare a placement new. This
// triggers a warning that there's no placement delete, which can be ignored.
#if defined (ACE_HAS_WINCE) && !defined (ACE_HAS_MFC)
#  pragma warning(disable:4291)
#endif

// A template can not be exported. Only an instantiation may be exported.
#define ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION_EXPORT

// Windows Vista and Windows Server 2008 and newer do have native condition
// variables, but this is commented out because the support in ACE hasn't
// been completed
// #if defined (_WIN32_WINNT) && (_WIN32_WINNT >= 0x0600)
// # define ACE_HAS_WTHREADS_CONDITION_VARIABLE
// # undef ACE_LACKS_COND_T
// #endif

#define ACE_HAS_POSIX_TIME 1
#define ACE_LACKS_TIMESPEC_T 1

// According to MS the Visual Studio 2015 C-runtime has a
// C99 compliant vsnprintf/vsnwprintf, this is a change compared to
// previous versions
#define ACE_HAS_C99_VSNPRINTF
#define ACE_HAS_C99_VSNWPRINTF

// Visual Studio 2015 has 3 parameter wcstok
#define ACE_HAS_3_PARAM_WCSTOK

// Visual Studio 2015 has adequate C++11 support
#define ACE_HAS_CPP11

#define ACE_PUTENV_EQUIVALENT ::_putenv
#define ACE_TEMPNAM_EQUIVALENT ::_tempnam
#define ACE_STRDUP_EQUIVALENT ::_strdup
#define ACE_MKDIR_EQUIVALENT ::_mkdir
#define ACE_ACCESS_EQUIVALENT ::_access
#define ACE_CHDIR_EQUIVALENT ::_chdir
#define ACE_RMDIR_EQUIVALENT ::_rmdir
#define ACE_GETCWD_EQUIVALENT ::_getcwd
#define ACE_SWAB_EQUIVALENT ::_swab
#define ACE_UNLINK_EQUIVALENT ::_unlink

#define ACE_HAS_STRNLEN
#define ACE_HAS_WCSNLEN

#include /**/ "ace/post.h"
#endif /* ACE_CONFIG_WIN32_MSVC_14_H */
