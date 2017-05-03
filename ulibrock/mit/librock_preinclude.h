/* This is a compatibility and portability shim to help
   write sources that are platform independent and also
   locally adaptable.
   
   More info at http://www.mibsoftware.com/
 */
#if !defined LIBROCK_WANT_GMTIME_R
//Source didn't ask for it.
#elif defined __MINGW32__  && defined __MSVCRT__
//gmtime_r is a compatibility macro in pthread.h
#   define LIBROCK_WANT_INCLUDE_PTHREAD 
#elif defined _MSC_VER
// Use gmtime_s with the Microsoft parameter order.
#	define LIBROCK_WANT_GMTIME_R_FOR_MSC_VER
#endif

#if !defined LIBROCK_WANT_STRCASE
//Source didn't ask for it.
#elif defined _MSC_VER
// Instead of using strcasecmp and strncasecmp, use _strnicmp, _stricmp
#   define LIBROCK_WANT_STRCASE_FOR_MSC_VER
#endif

#if defined _MSC_VER
#	define LIBROCK_WANT_GETENV_S_FOR_MSC_VER
#endif
