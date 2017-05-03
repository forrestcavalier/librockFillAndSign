#if !defined(librock_awsFillAndSign_H)
#define librock_awsFillAndSign_H 1
#define LIBROCK_SRC_AWSFILLANDSIGN 1 //Identify the source module
#define LIBROCK_WANT_GMTIME_R 1 //We need this
#define LIBROCK_WANT_STRCASE 1 //We need this
#include "ulibrock/mit/librock_preinclude.h"
/*************************************************************
A note about librock_preinclude.h and librock_postinclude.h:

   LIBROCK sources are designed to be very portable and very stable.
   Conditional includes in this file are based only on variables with a prefix
   of LIBROCK_.
   
   All compiler and platform-specific changes are to be accomplished in
   librock_preinclude.h and librock_postinclude.h.
   
For the purposes of awsFillAndSign, librock_preinclude should have
accomplished something like the following in order to have the
unchanged sources compile portably with MSVC, GCC, and CLANG on
Windows, and GCC on Linux and *BSD:
    #if !defined LIBROCK_WANT_GMTIME_R
    #elif defined __MINGW32__  && defined __MSVCRT__
    #   define LIBROCK_WANT_INCLUDE_PTHREAD
    #elif defined _MSC_VER
    #   define LIBROCK_WANT_GMTIME_R_FOR_MSC_VER
    #endif
    #if !defined LIBROCK_WANT_STRCASE
    #elif defined _MSC_VER
    #   define LIBROCK_WANT_STRCASE_FOR_MSC_VER
    #endif
    #if defined _MSC_VER
    #	define LIBROCK_WANT_GETENV_S_FOR_MSC_VER
    #endif
*/

#if defined LIBROCK_WANT_INCLUDE_PTHREAD
//gmtime_r is a compatibility macro in pthread.h
#   include <pthread.h>
#endif


#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

    /* Declare the workhorse function, which is public.  */
    struct librock_awsFillAndSignParameters_s {
        void *generalParameters[8]; // caller may use these for any purpose.

        const char *AWS_ACCESS_KEY_ID; //Required
        const char *AWS_SECRET_ACCESS_KEY; //Required
        const char *AWS_SECURITY_TOKEN; //Optional, set to 0 if not wanted
        const char *AWS_DEFAULT_REGION;  //Required.
        const char *AWS_REGION_NOT_USEAST1; //Optional. If not us-east-1, will be set to region + '.', otherwise ''.
        const char *AWS_SERVICE_NAME; //Required. This is the name for AWS Signature 4 signing.
        const char *AWS_AMZDATE;  //Required.
        const char *AWS_SHA256; //Required unless CONTENT_LENGTH is 0. *If set to 0, will be the SHA256 of an empty body.)
        const char *AWS_QUERY_EXTRA; //Optional, set to 0 or URL-encoded parameter list to append to query string.

        unsigned long CONTENT_LENGTH;
        const char *CONTENT_TYPE;

        int (*fnOutput)(void *outputId, const char *pSource, int count); //called to write output
        void *outputId;

        int (*fnDebugOutput)(void *debugOutputId, const char *pSource, int count); //called to write output
        void *debugOutputId;
    };
    
    const char *librock_awsFillAndSign( //Compute an AWS Version 4 or Version 2 signature
        const char *pRequestString, //CURL one option per line, or HTTP headers one per line.
        struct librock_awsFillAndSignParameters_s *signingParameters
        );
    
     
    /* Utility functions in this module */
    PRIVATE int countToStr(const char *pRead, const char *sToFind);
    PRIVATE int countToEol(const char *pRead); /* count to \n or \0 */
    PRIVATE int countOptionName(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead != '=' && *pRead > ' ') {
            pRead++;
        }
        return pRead - pStart;
    }
    PRIVATE int isCurlOptionName(const char *pRead, const char *pToFind)
    {
        int length = strlen(pToFind);
        if (!strncmp(pRead,":curl:",6)) {
            pRead += 6;
        }
        if (countOptionName(pRead) != length) {
            return 0;
        }
        return strncmp(pRead, pToFind, length) == 0;
    }
    PRIVATE int countToStr(const char *pRead, const char *sToFind)
    {
        const char *pFind = strstr(pRead, sToFind);
        if (!pFind) {
            return strlen(pRead);
        }
        return pFind - pRead;
    }
            
    PRIVATE int countToValue(const char *pRead); /* count to value following '=', for parsing CURL options */
#include "ulibrock/mit/appendable.h"
    PRIVATE void bToHex0(char *pWrite, unsigned char ch)
    {
        char *digits = "0123456789abcdef";
        *pWrite = digits[(ch>>4)&0x0f];
        pWrite[1] = digits[(ch)&0x0f];
        pWrite[2] = '\0';
    }

/**************************************************************/
//[[Predeclarations of functions from librock_sha256.c]]
struct librock_SHA256_CTX;
    extern int librock_sha256Init(struct librock_SHA256_CTX *c);
    extern int librock_sha256Update(struct librock_SHA256_CTX *c, const void *data_, int len);
    extern int librock_sha256StoreFinal(unsigned char *md, struct librock_SHA256_CTX *c);

/**************************************************************/
//[[Predeclarations of functions from librock_hmacsha256.c]]
    extern void librock_hmacSha256(
            unsigned char *pDigestResult,
            const char *pKey,
            int cKey,
            const char *pToSign,
            int cToSign);

/**************************************************************/
    PRIVATE void freeOnce(void **p)
    {
        if (*p) {
            free(*p);
            *p = 0;
        }
    }

    PRIVATE int countToEol(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead && *pRead != '\n' && *pRead != '\r') {
            pRead++;
        }
        return pRead - pStart;
    }
    PRIVATE int countToValue(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead && *pRead != '\n') {
            if (*pRead == '=') {
                pRead++;
                if (*pRead == ' ') {
                    pRead++;
                }
                if (*pRead == '\x22') {
                    pRead++;
                }
                return pRead-pStart;
            }
            pRead++;
        }
        return pRead-pStart;

    }


int librock_triggerAlternateBranch(const char *name, long *pLong);

#if defined(librock_WANT_BRANCH_COVERAGE)
int librock_coverage_main();
void librock_awsFillAndSign_coverage();
long librock_global_iAlternateBranch = 0;
#define GLOBAL_ALTERNATE_BRANCH librock_triggerAlternateBranch("global", &librock_global_iAlternateBranch)

void *librock_FaultInjection_malloc(size_t);
void *librock_FaultInjection_realloc(void *p, size_t);
#define malloc librock_FaultInjection_malloc
#define realloc librock_FaultInjection_realloc


time_t librock_coverage_time( time_t *arg ); //Always return 0
#define time librock_coverage_time
#else
#define GLOBAL_ALTERNATE_BRANCH 0
#endif
static void freeOnce(void **p);

#include "ulibrock/mit/librock_postinclude.h"

#if defined LIBROCK_WANT_STRCASE_FOR_MSC_VER
#   define strcasecmp _stricmp
#   define strncasecmp _strnicmp
#endif

#if defined LIBROCK_WANT_GMTIME_R_FOR_MSC_VER
    struct tm *gmtime_r(time_t *t, struct tm *p_tm) {
        gmtime_s(p_tm, t); /* NOTE: parameter order in _MSC_VER version is reversed compared to standard 
            https://connect.microsoft.com/VisualStudio/feedback/details/793092/gmtime-s-non-conformant
        */
        return p_tm;
    }
#endif
	
#endif
