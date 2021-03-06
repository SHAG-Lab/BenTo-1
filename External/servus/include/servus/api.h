// generated by CommonLibrary.cmake, do not edit.

/**
 * @file include/servus/api.h
 * Defines export visibility macros for library Servus.
 */

#ifndef SERVUS_API_H
#define SERVUS_API_H

#include <servus/defines.h>

#if defined(_MSC_VER) || defined(__declspec)
#  define SERVUS_DLLEXPORT __declspec(dllexport)
#  define SERVUS_DLLIMPORT __declspec(dllimport)
#else // _MSC_VER
#  define SERVUS_DLLEXPORT
#  define SERVUS_DLLIMPORT
#endif // _MSC_VER

#if defined(SERVUS_STATIC)
#  define SERVUS_API
#elif defined(SERVUS_SHARED)
#  define SERVUS_API SERVUS_DLLEXPORT
#else
#  define SERVUS_API SERVUS_DLLIMPORT
#endif

#if defined(SERVUS_SHARED_INL)
#  define SERVUS_INL SERVUS_DLLEXPORT
#else
#  define SERVUS_INL SERVUS_DLLIMPORT
#endif

#endif
