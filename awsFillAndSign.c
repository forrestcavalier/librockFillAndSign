/* awsFillAndSign.c Copyright 2016 MIB SOFTWARE, INC.

 PURPOSE:   Sign Amazon Web Services requests with AWS Signature Version 4.
  
 LICENSE:   MIT (Free/OpenSource)
 
 PLATFORM:  C on GCC/MSVC/Clang/BSD/Linux/Windows.
 
 PACKAGE:   Compiles as a utility and a library function. Needs
                hmacsha256.c librock_sha256.c
            See details at [[Header include and compatibility]], below.
                
 STABILITY: UNSTABLE as of 2016-12-16
            Be sure to compile with -DLIBROCK_UNSTABLE.
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign
              
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

*******************************************************************************/
#if !defined(LIBROCK_UNSTABLE) //Guard for the entire file.
#   error "E-58 This file came from the unstable branch of librock. You have not defined LIBROCK_UNSTABLE."
#else
/*
The MIT License (MIT)

Copyright (C) 2016 MIB SOFTWARE INC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/*
*******************************************************************************
TABLE OF CONTENTS FOR THE REST OF THIS FILE
*******************************************************************************
    [[HOW TO USE]]
        HOW TO use this as a command-line utility
        HOW TO use this as a library function
        
    [[Header include and compatibility]]
        #include "librock_preinclude.h"
        #include <stdlib.h>, <sys/types.h>, <time.h>, <string.h>
        #if defined LIBROCK_WANT_INCLUDE_PTHREAD: <pthread.h>
        #if defined(LIBROCK_AWSFILLANDSIGN_MAIN): <stdio.h>, <fcntl.h>, "librock_fdio.h"
        #include "librock_postinclude.h"
        
    [[Predeclarations]]

    [[Predeclarations of functions from librock_sha256.c]]
        librock_sha256Init, librock_sha256Update, librock_sha256StoreFinal
    
    [[Predeclarations of functions from hmacsha256.c]]
        librock_hmacSha256

    [[built-in templates]]

    [[function main()]]  #if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
               
    
    [[Implementation]]
    
        function librock_awsFillAndSign  The Workhorse
        
    [[PRIVATE implementation functions]]
        
    [[PRIVATE Utility Functions]]
        
*/
/********************************************************************************
[[HOW TO USE]]

   HOW TO use this as a command-line utility:
       1. Compile using -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN.
       This compiles as a command-line (console) utility on Windows,
       Linux, *BSD, using MSVC, gcc, or clang. E.g. To compile for gcc:
       gcc -o awsFillAndSign -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN -Werror -Wall awsFillAndSign.c hmacsha256.c librock_sha256.c 
       
       2. Create a template file in CURL config or RAW format, with numbered
       replaceable parts (inside @ @).  For example.
            @//Template AWS S3 LIST request, with a limit on results returned.
            url="https://@1_bucket@.s3.amazonaws.com/?prefix=@2_object@&max-keys=@3_max@&list-type=2"
            header = "Host: @1_bucket@.s3.amazonaws.com"
            header = "Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)"
            header = "x-amz-content-sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
       
       3. Specify the template file name as the first command line parameter.
       
       4. Provide other template parameters in order. (In this example the bucket,
       the prefix, and the max-keys.)
       
       5. Give AWSCredentials scope,id,secret (comma separated, no spaces) on the input.
       
       6. Redirect the output to a file, that you provide to CURL.
       
       See github for more examples and the full set of templates for AWS S3 and
       other AWS services.
       
   HOW TO use this as a library function:
       You want to call librock_awsFillAndSign(), see details below.
 */
/**************************************************************/
//[[Header include and compatibility]]
#define LIBROCK_SRC_AWSFILLANDSIGN 1 //Identify the source module
#define LIBROCK_WANT_GMTIME_R 1 //We need this
#define LIBROCK_WANT_STRCASE 1 //We need this
#include "librock_preinclude.h"
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
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#if defined LIBROCK_WANT_INCLUDE_PTHREAD
//gmtime_r is a compatibility macro in pthread.h
#   include <pthread.h>
#endif
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
#   include <stdio.h> /* fwrite, fprintf */
#   include <fcntl.h>
#   include "librock_fdio.h" /* portable _open, read, close */
#endif

int librock_triggerAlternateBranch(const char *name, long *pLong);

#if defined librock_WANT_ALTERNATE_BRANCHING
int librock_coverage_main();
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

#include "librock_postinclude.h"

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



/********************************************************************/
//[[Predeclarations]]
#if !defined(PRIVATE)
#   define PRIVATE static /* names with module-level scope */
#endif
    
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)    // Compile as command line utility
    int main(int argc, char **argv);
    PRIVATE int write_to_FILE(void *id, const char *pSource, int count); /* helper function */
#endif

    /* Declare the workhorse function, which is public.  */
    const char *librock_awsFillAndSign( //Compute an AWS Version 4 signature
        const char *pRequestString //CURL one option per line, or HTTP headers one per line.
        ,char const * *ppSigningParameters
        ,int (*fnOutput)(void *outputId, const char *pSource, int count) //called to write output
        ,void *outputId
        ,int (*fnDebugOutput)(void *debugOutputId, const char *pSource, int count) //called to write output
        ,void *debugOutputId
        );
    
    /* Declare struct for the parsed request. Callers should treat it as opaque, 
       because it is subject to change. 
     */
    struct librock_awsRequest_s;

    /* (private) Implementing functions in this modules. 
            NOTE: Callers should use librock_awsSignatureVersion4(), not these. They
            are subject to change.
    */
    PRIVATE void librock_awsRequest_start_(
                        struct librock_awsRequest_s *pRequest,
                        const char *pRequestWithoutBody,
                        const char **ppSigningParameters);
    PRIVATE const char *librock_awsSignature_parseRequest_(
                        struct librock_awsRequest_s *pRequest);
    PRIVATE const char *librock_awsSignature_getStringToSign_(
                        struct librock_awsRequest_s *pRequest,
                        char **ppToSign);
    PRIVATE const char *librock_awsSignature_outputWithAuthorization_(
                        struct librock_awsRequest_s *pRequest,
                        const char *pToSign);
     
    /* Utility functions in this module */
    PRIVATE char *mallocNamedValue(const char *pName);
    PRIVATE const char *librock_fillTemplateTokenize(
                        const char *pTemplate,
                        int iOffset,
                        int *pCount
                        );
    PRIVATE const char *librock_fillTemplate(
                        char **ppFilled,
                        const char *pTemplate,
                        int iEncodeParameters,
                        char ** namedArguments,
                        int argc,
                        char * const * const argv,
                        int *pErrorIndex);
    PRIVATE int qsort_strcmp_(const void *s1, const void *s2);  /* For sorting query parameters */
    PRIVATE int qsort_strcasecmp_(const void *pString1, const void *pString2);  /* For sorting HTTP headers */
    PRIVATE int countToStr(const char *pRead, const char *sToFind);
    PRIVATE int countToEol(const char *pRead); /* count to \n or \0 */
    PRIVATE int countLWS(const char *pRead); /* Count linear ' ' and '\t' */
    PRIVATE int countOptionName(const char *pRead); /* count to ' ' or '=', for parsing CURL options */
    PRIVATE int isCurlOptionName(const char *pRead, const char *pToMatch);
            
    PRIVATE int countToValue(const char *pRead); /* count to value following '=', for parsing CURL options */
    struct librock_appendable {
        void *p;
        int cb;
        int iWriting;
        void *(*realloc)(void *, size_t);
    };
    PRIVATE char *librock_appendableSet(struct librock_appendable *pAppendable,
            void *p,
            int cb,
            void *(*realloc)(void *, size_t)
            );
    PRIVATE char *librock_safeAppend0(
                        struct librock_appendable *pAppendable,
                        const char *pSource,
                        int cSource);
#if 0 //Works but not used
    PRIVATE char *librock_safeAppendEnv0(
                        struct librock_appendable *pAppendable,
                        const char *pName);
#endif
    PRIVATE char *librock_safeAppendUrlEncoded0(
                        struct librock_appendable *pAppendable,
                        const char *pSource,
                        int cSource);
    PRIVATE char *librock_safeAppendBase64Encoded0(
                        struct librock_appendable *pAppendable,
                        const char *pSource,
                        int cSource);

#ifdef LIBROCK_AWSFILLANDSIGN_MAIN
    PRIVATE const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32); /* store 32 bytes, returns error explanation or 0 */
    PRIVATE void *librock_fileGetContents(const char *fname); /* allocate memory, read entire file */
#endif
    PRIVATE void bToHex0(char *pWrite, unsigned char ch); /* To hexadecimal, avoid sprintf */
    PRIVATE void bToUHex0(char *pWrite, unsigned char ch); /* To upper case hexadecimal, avoid sprintf */

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
//[[Implementation]]
//struct for the parsed request Callers should NOT use because it is subject to change.
 struct librock_awsRequest_s {
    const char *pRequest;
    const char **ppSigningParameters;
    const char *pUriHost;
    const char *pUriPath;
    const char *pCanonicalQuery;
    char *pSignedHeaders;
    const char *pXAmzDate;
    char const * *pListOfHeaders;
    char amzDate[100];
    int bFormatCURL;
    int signatureV2;
    const char *pVerb;
    int cHeaders;
    int (*fnOutput)(void *outputId, const char *pSource, int count); //called to write output
    void *outputId;
    int (*fnDebugOutput)(void *debugOutputId, const char *pSource, int count); //called to write output
    void *debugOutputId;
};



/**************************************************************/
/* The main workhorse function. Call this one */
const char *librock_awsFillAndSign(
        const char *pRequestWithoutBody //CURL one option per line, or HTTP headers one per line.
        ,char const * *ppSigningParameters
        ,int (*fnOutput)(void *fnOutputId, const char *pSource, int count) //called to write output
        ,void *outputId
        ,int (*fnDebugOutput)(void *fnDebugOutputId, const char *pSource, int count) //called to write output
        ,void *debugOutputId
        )
{
    struct librock_awsRequest_s awsRequest;
    const char *pErrorMessage = 0;
    char *pToSign = 0;
#define iSHA256 0
#define iSERVICE_REGION 1
#define iSERVICE_NAME 2
#define iSECURITY_TOKEN 3
#define iACCESS_KEY_ID 4
#define iSECRET_ACCESS_KEY 5
#define iCONTENT_LENGTH 6

    if (!pRequestWithoutBody) {
        return("E-309 need request");
    }
    if (!ppSigningParameters) {
        return("E-316 need signingParameters.");
    }
    if (!ppSigningParameters[iSERVICE_REGION]) { 
        return("E-316 need complete signingParameters. Missing Service Region.");
    }
    if (!ppSigningParameters[iSERVICE_NAME]) {
        return("E-316 need complete signingParameters. Missing Service Name.");
    }
    if (!ppSigningParameters[iACCESS_KEY_ID]) { 
        return("E-316 need complete signingParameters. Missing Access Key Id.");
    }
    if (!ppSigningParameters[iSECRET_ACCESS_KEY]) {
        return("E-316 need complete signingParameters. Missing Access Key.");
    }
        
    /* Initialize the structure */
    librock_awsRequest_start_(&awsRequest, pRequestWithoutBody, ppSigningParameters);
    if (!strcmp(ppSigningParameters[iSERVICE_NAME],"sdb")) {
        awsRequest.signatureV2 = 1;
    }

    awsRequest.pListOfHeaders = 0;
    
    /* Initialize the output methods */
    awsRequest.fnOutput = fnOutput;
    awsRequest.outputId = outputId;

    awsRequest.fnDebugOutput = fnDebugOutput;
    awsRequest.debugOutputId = debugOutputId;

    /* Parse the request */
    pErrorMessage = librock_awsSignature_parseRequest_(&awsRequest);
    if (!pErrorMessage) {
        /* Build the string to sign, per the AWS specification */
        pErrorMessage = librock_awsSignature_getStringToSign_(&awsRequest, &pToSign);
    }
    if (!pErrorMessage) {
        /* Create the signing key and write out the signed request */
        pErrorMessage = librock_awsSignature_outputWithAuthorization_(&awsRequest, pToSign);
    }

    /* The internal functions called malloc.  Free the memory here */
    freeOnce((void **)&awsRequest.pListOfHeaders);
    freeOnce((void **)&awsRequest.pSignedHeaders);
    freeOnce((void **)&pToSign);
    
    return pErrorMessage;

} /* librock_awsSignature */
/**************************************************************/

//[[PRIVATE implementation functions]]

PRIVATE void librock_awsRequest_start_(
    struct librock_awsRequest_s *pRequest,
    const char *pRequestWithoutBody,
    const char **ppSigningParameters)
{ //Callers should use librock_awsSignatureVersion4(), not this.
    pRequest->pUriHost = 0;
    pRequest->pUriPath = 0;
    pRequest->pXAmzDate = 0;
    pRequest->bFormatCURL = 0;
    pRequest->signatureV2 = 0;
    pRequest->pVerb = 0;
    pRequest->cHeaders = 0;
    pRequest->pRequest = pRequestWithoutBody;
    pRequest->pCanonicalQuery = 0;
    pRequest->pSignedHeaders = 0;
    pRequest->ppSigningParameters = ppSigningParameters;
    { /* prep the amzDate, in case it is not already in the request */
        time_t now;
        struct tm gmt;
        time(&now);
        gmtime_r(&now, &gmt);
        strftime(pRequest->amzDate, sizeof pRequest->amzDate, "X-Amz-Date:%Y%m%dT%H%M%SZ", &gmt);
    }
} /* librock_awsRequest_start */
/**************************************************************/


/**************************************************************/
PRIVATE const char *librock_awsSignature_parseRequest_(struct librock_awsRequest_s *pRequest)
{//Callers should use librock_awsSignatureVersion4(), not this.
    const char *pRead = pRequest->pRequest;
    const char *pStartParse;
    int iPass;

    if (strstr(pRequest->pRequest, "\nheader=")||strstr(pRequest->pRequest, "\nheader =")) {
        /* detected cURL --config format*/
        pRequest->bFormatCURL = 1;

        if (pRequest->fnDebugOutput) {
            const char *pLiteral = "I-171 Detected CURL\n";
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
        }

    } else {
        /* We will parse and output text headers */
        pRequest->pVerb = pRead;
        while (*pRead > ' ') {
            pRead++;
        }
        if (*pRead) {
            pRead++;
        }
        pRequest->pUriPath = pRead;
        pRead += countToEol(pRead);
        if (*pRead) pRead++;
    }
    iPass =0;
    pStartParse = pRead;
    while (iPass <= 1) {
        pRead = pStartParse;
        pRequest->cHeaders = 0;
        while (*pRead) {
            if (pRequest->bFormatCURL) {
                if (isCurlOptionName(pRead, "upload-file")) {
                    if (iPass == 0) {
                        pRequest->pVerb = "PUT";
                    }
                    goto skip_line;
                } else if (isCurlOptionName(pRead, "data")) {
                    if (iPass == 0) {
                        pRequest->pVerb = "POST";
                    }
                    goto skip_line;
                } else if (isCurlOptionName(pRead, "request")) {
                    pRead += countToValue(pRead);
                    if (iPass == 0) {
                        if (countToEol(pRead)==0) {
                            return "E-507 missing request type";
                        }
                        pRequest->pVerb = pRead;
                    }
                    goto skip_line;
                } else if (isCurlOptionName(pRead, "url")) {
                    if (iPass == 0) {
                        pRead += countToValue(pRead);
                        pRequest->pUriPath = pRead;
                    }
                    goto skip_line;
                } else if (!isCurlOptionName(pRead, "header")) {
                    goto skip_line;
                }
                pRead += countToValue(pRead);
            }
            if (iPass == 0) {
                if (!strncasecmp(pRead, "x-amz-date:", 11)) {
                    pRequest->amzDate[0] = '\0';
                }
            }
            if (!strncasecmp(pRead, "authorization:", 14)) {
                /* Do not sign */
            } else if (!strncasecmp(pRead, "x-amz-content-sha256:", 21)) {
                /* Do not sign */
                if (pRequest->ppSigningParameters[iSHA256] == (char *) 0) {
                    pRead += 21;
                    while(*pRead == ' ') {
                        pRead++;
                    }
                    pRequest->ppSigningParameters[iSHA256] = pRead;
                }
            } else if (1/*sign everything*/) {
                if (countToStr(pRead, ":") >= countToEol(pRead)) {
                    return "E-528 malformed header";
                }
                if (iPass==1) {
                    pRequest->pListOfHeaders[pRequest->cHeaders] = pRead;
                }
                pRequest->cHeaders++;
            }
skip_line:
            while (*pRead && *pRead != '\n') {
                pRead++;
            }
            if (*pRead) pRead++; /* To next line */
        }
        if (pRequest->amzDate[0]) {
            /* Date was not found in the supplied headers.  Supply it. */
            if (iPass == 1) {
                pRequest->pListOfHeaders[pRequest->cHeaders] = pRequest->amzDate;
            }
            pRequest->cHeaders++;
        }
        if (pRequest->ppSigningParameters[iSECURITY_TOKEN]) {
            if (iPass == 1) {
                pRequest->pListOfHeaders[pRequest->cHeaders] = "X-Amz-Security-Token: will_replace";
            }
            pRequest->cHeaders++;
        }
        if (iPass == 0) {
            /* Because of date, there will be at least one header */
            pRequest->pListOfHeaders = malloc(sizeof(char *) * pRequest->cHeaders);
            if (!pRequest->pListOfHeaders) {
                 return("E-523 malloc failed");
            }
        }
        iPass++;
    }
    {
        int cValid = 0;
        const char *pRead = pRequest->ppSigningParameters[iSHA256];
        if (pRead == 0) {
            return "E-551 x-amz-content-sha256 not set ";
        }
        while(*pRead && strchr("0123456789ABCDEFabcdef",*pRead)) {
            pRead++;
            cValid++;
        }
        if (cValid != 64) {
            return "E-561 x-amz-content-sha256 must be 64 hexadecimal ascii digits";
        }
    }

    { // If URI is absolute, skip host part
        int iHost;
        int iURI;
        int cLine;
        pRead = pRequest->pUriPath;
        cLine = countToEol(pRead);
        iHost = countToStr(pRead, "://");
        if (iHost < cLine) {
            iURI = countToStr(pRead+iHost+3, "/");
            if (iURI+iHost+3 >= cLine) {
                return "E-516 bad url";
            }
            pRequest->pUriHost = pRead+iHost+3;
            pRequest->pUriPath = pRead+iHost+3+iURI;
        }
    }
    if (pRequest->bFormatCURL) {
        if (!pRequest->pVerb) {
            pRequest->pVerb = "GET";
        }
    }
    return 0;
} /* librock_awsSignature_parseRequest_ */
/**************************************************************/


/**************************************************************/
PRIVATE void librock_awsSignature_dump2_(struct librock_awsRequest_s *pRequest, const char *name, const char *pSource)
{ /* Special purpose dump routine to generate debugging output.
   * IMPORTANT: Call only when fnDebugOutput is set-up.
   */
    const char *pRead = pSource;
    const char *pLiteral = "#<";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = ">";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));

    while (*pRead) {
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pRead, countToEol(pRead));
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\\n\n", 3);
        pRead += countToEol(pRead);
        if (*pRead) {
            pRead++;
        }
    }

    pLiteral = "</";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = ">\n";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));

    pLiteral = "<";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = "Bytes>";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    pRead = pSource;
    while (*pRead) {
        char buffer[3];
        bToHex0(buffer, *pRead & 0xff);
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, buffer, strlen(buffer));
        pRead++;
        if (*pRead) {
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, " ", 1);
        }
    }
    pLiteral = "</";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, name, strlen(name));
    pLiteral = "Bytes>\n";
    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
} /* librock_awsSignature_dump2_ */
/**************************************************************/

PRIVATE const char *librock_awsSignature_canonicalQueryString_(
        int bSignatureV2,
        struct librock_appendable *paParameter,
        const char **ppQueryString,
        int bFormatCURL)
{
    /* Canonicalize the % encoding into aParameter, then make a sorted copy, and store again to aParameter */
    const char *startChunk;
    int cParameters = 1;
    const char *pSource = *ppQueryString;
    int iStartString = paParameter->iWriting;
    
    /* For efficiency, work in chunks */
    startChunk = pSource;
    while (*pSource > ' ') {
        char ch;
        if (bFormatCURL && *pSource == '\x22') {
            break;
        }
        ch = *pSource;
        if (ch == '%') {
            char *strEncode;
            char *strCharacters ="0123456789abcdef0123456789ABCDEF";
            if (!librock_safeAppend0(paParameter, startChunk, pSource - startChunk)) { 
                return "E-601 would overflow pre-allocated block";
            }
            ch = 0;
            strEncode = strchr(strCharacters, pSource[1]);
            if (!strEncode) {
                return "E-670 Invalid URI escape";
            }
            ch += (strEncode - strCharacters) & 0x0f;
            ch *= 16;
            strEncode = strchr(strCharacters, pSource[2]);
            if (!strEncode) {
                return "E-670 Invalid URI escape";
            }
            ch += (strEncode - strCharacters) & 0x0f;
            if (!librock_safeAppendUrlEncoded0(paParameter, &ch, 1)) {
                return "E-601 would overflow pre-allocated block";
            }
            pSource += 3;
            startChunk = pSource;
        } else if ((*pSource >= '0' && *pSource <= '9')
            ||(*pSource == '.')
            ||(*pSource >= 'A' && *pSource <= 'Z')
            ||(*pSource >= 'a' && *pSource <= 'z')
            ||(*pSource == '-')
            ||(*pSource == '_')
            ||(*pSource == '~')
            ||(*pSource == '=') //Caller means this = not encoded.
        ) {
            pSource++;
        } else if (*pSource == '&') { //Caller means this not encoded.
            cParameters += 1;
            pSource++;
        } else {
            /* Append the string so far */
            if (!librock_safeAppend0(paParameter, startChunk, pSource - startChunk)) { 
                return "E-601 would overflow pre-allocated block";
            }
            /* URL encoded */
            if (!librock_safeAppend0(paParameter, "%XX", 3)) { 
                return "E-601 would overflow pre-allocated block";
            }
            bToUHex0((char *) paParameter->p + paParameter->iWriting-2, *pSource & 0xff);
            pSource++;
            startChunk = pSource;
        }
    }
    /* Append rest of string */
    if (pSource > startChunk) {
        if (!librock_safeAppend0(paParameter, startChunk, pSource - startChunk)) { 
            return "E-601 would overflow pre-allocated block";
        }
    }
    if (*pSource == '\x22') { //Will happen if bFormatCURL.
        pSource++;
    }
    startChunk = pSource;
    /* Store the end location */
    *ppQueryString = pSource;
    
    if (cParameters > 1) { /*sort*/
        char **pList = 0;
        struct librock_appendable aCopy;
        char *pFix;
        int i = 0;
        pList = malloc(cParameters * sizeof(char *));
        if (!pList) {
            return "E-680 malloc failed";
        }
        librock_appendableSet(&aCopy, 0, 0, 0);
        aCopy.cb = strlen((char*)paParameter->p + iStartString) + 1;
        aCopy.p = malloc(aCopy.cb);
        if (!aCopy.p) {
            freeOnce((void **)&pList);
            return "E-683 malloc failed";
        }
        if (!librock_safeAppend0(&aCopy, (char*)paParameter->p + iStartString, strlen((char*)paParameter->p + iStartString))) {
            freeOnce((void **)&pList);
            freeOnce((void **)&(aCopy.p));
            return "E-601 would overflow pre-allocated block";
        }
        pFix = aCopy.p;
        
        /* First turn '=' into spaces for easy sorting */
        while((pFix = strchr(pFix, '='))) {
            *pFix++ = ' ';
        }
        /* Now build a list, splitting at '&' */
        pFix = aCopy.p;
        while(pFix) {
            pList[i++] = pFix;
            pFix = strchr(pFix,'&');
            if (pFix) {
                *pFix++ = '\0';
            }
        }

        /* Sort */
        qsort((void *) &pList[0], cParameters, sizeof(char **), qsort_strcmp_);
        
        /* Reset the iWriting index, and join in sorted order, separated by '&'  */
        paParameter->iWriting = iStartString;
        for(i = 0; i < cParameters; i++) {
            if (bSignatureV2 && !strncasecmp(pList[i],"signature ",10)) {
                
            } else {
                if (i > 0) {
                    if (!librock_safeAppend0(paParameter, "&", 1)) {
                        freeOnce((void **)&pList);
                        freeOnce((void *)&(aCopy.p));
                        return "E-601 would overflow pre-allocated block";
                    }
                }
                if (!librock_safeAppend0(paParameter, pList[i], strlen(pList[i]))) { 
                    freeOnce((void **)&pList);
                    freeOnce((void *)&(aCopy.p));
                    return "E-601 would overflow pre-allocated block";
                }
            }
        }

        /* convert spaces back to = */
        pFix = (char *) paParameter->p + iStartString;
        while((pFix = strchr(pFix, ' '))) {
            *pFix++ = '=';
        }
        freeOnce((void **)&pList);
        freeOnce((void *)&(aCopy.p));
    }
    return 0;
} /* librock_awsSignature_canonicalQueryString_ */

/**************************************************************/
PRIVATE const char *librock_awsSignature_getStringToSign_(
    struct librock_awsRequest_s *pRequest,
    char **ppToSign)
{//Callers should use librock_awsRest_sign_v4(), not this.
    /* Version 4:
    
StringToSign  =
Algorithm + '\n' +
RequestDate + '\n' +
CredentialScope + '\n' +
HashedCanonicalRequest))

E.g.
AWS4-HMAC-SHA256
20150830T123600Z
20150830/us-east-1/iam/aws4_request
f536975d06c0309214f805bb90ccff089219ecd68b2577efef23edd43b7e1a59
    
    CanonicalRequest =
        HTTPRequestMethod + '\n' +
        CanonicalURI + '\n' +
        CanonicalQueryString + '\n' +
        CanonicalHeaders + '\n' +
        SignedHeaders + '\n' +
        HexEncode(Hash(RequestPayload))
    */
    const char *pRead;
    unsigned char mdCanonicalRequest[32];
    const char *pErrorMessage = 0;
    void *pHashInfo = 0;
    struct librock_appendable aParameter;
//    const char *pSha256;
    const char *pCanonicalRequestTemplate = /* For Version 4. Will be reset below for Version 2 */
        "@1_VERB@\n"
        "@2_URI@\n"
        "@3_QueryString@\n"
        "@4_Headers@\n"
        "@5_SignedHeaders@\n"
        "@6_PayloadHash@" //No trailing \n
        ;

    char *pParameterList[7]; /* pParameterList[0] unused */
    /* Build the Canonical Request. */
    /* We are going to build stringtosignbytes afto digest with hmac-sha256 */

    librock_appendableSet(&aParameter, 0, 0, 0);
    
    if (pRequest->fnDebugOutput) {
        const char *pLiteral = "I-330 ";
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pRequest->pRequest, strlen(pRequest->pRequest));
        (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\n", 1);
    }

    /* Overallocate, ensuring room for all the headers, amzDate, payload hash.
    We multiply the strlen(pRequest->pURI) as a worst case for expanding out '/' to %2F
    */
    aParameter.cb = countToEol(pRequest->pUriPath)*3 + strlen(pRequest->pRequest)*2+200+1; 
    //  This buffer is also reused to create stringToSign, which gets parts of the credentials.
    aParameter.cb += 22 + 32*2 + 1; /*room for adding x-amz-content-Sha256: */
    aParameter.cb += strlen(pRequest->ppSigningParameters[iSERVICE_REGION]);
    aParameter.cb += strlen(pRequest->ppSigningParameters[iSERVICE_NAME]);
    aParameter.cb += pRequest->ppSigningParameters[iSECURITY_TOKEN] ? strlen(pRequest->ppSigningParameters[iSECURITY_TOKEN]) : 0;

    aParameter.p = (char *) malloc(aParameter.cb); 
    if (!aParameter.p) {
        return "E-462 malloc failed";
    }
    
    /* No allocator in aParameter. Store pointers into the buffer, it won't move. */
    pParameterList[1/*Verb*/] = (char*)aParameter.p + aParameter.iWriting;
    if (!librock_safeAppend0(&aParameter, pRequest->pVerb, countOptionName(pRequest->pVerb))) {
        freeOnce((void **) & aParameter.p);
        return "E-470 would overflow pre-allocated block";
    }
    if (((char *) aParameter.p)[aParameter.iWriting-1] == '\x22') {/* Maybe check for \x22 only if bFormatCURL? */
        aParameter.iWriting--;
        ((char *) aParameter.p)[aParameter.iWriting] = '\0';
    }
    aParameter.iWriting++; /* Retain the terminating \0. */

    /* Canonical URI */
    pParameterList[2/*Canonical URI*/] = (char*)aParameter.p + aParameter.iWriting;
    pRead = pRequest->pUriPath;
    { /* Copy up to start of query part */
        const char *pStart = pRead;
        while (*pRead > ' '  && *pRead != '?') {
            if (pRequest->bFormatCURL && *pRead == '\x22') {
                break;
            }
            pRead++;
        }
        if (!librock_safeAppend0(&aParameter, pStart, pRead - pStart)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        aParameter.iWriting++; /* Retain the terminating \0. */
    }

    /* Canonical Query String. */
    pParameterList[3/*Query String*/] = (char*)aParameter.p + aParameter.iWriting;
    if (!librock_safeAppend0(&aParameter, "", 0)) { 
        freeOnce((void **) & aParameter.p);
        return "E-470 would overflow pre-allocated block";
    }
    if (*pRead == '?') {
        pRead++;
        pErrorMessage = librock_awsSignature_canonicalQueryString_(
                    pRequest->signatureV2, &aParameter, &pRead, pRequest->bFormatCURL);
    } else if (pRequest->signatureV2) {
        /* need to get it from data */
        pRead = strstr(pRead,"\ndata");
        if (pRead) {
            pRead = strchr(pRead,'\"');
        }
        if (!pRead) {
            pErrorMessage = "E-936 missing data for signature Version 2";
        } else {
            pRead++;
            pErrorMessage = librock_awsSignature_canonicalQueryString_(
                        pRequest->signatureV2, &aParameter, &pRead, pRequest->bFormatCURL);
        }
    }
    if (pErrorMessage) {
        freeOnce((void **) & aParameter.p);
        return pErrorMessage;
    }
    pRead = 0;
    aParameter.iWriting++; /* Retain the terminating \0. */
    
    /* CanonicalHeaders */
    
    /* Sorted headers */
    pParameterList[4/*Canonical Headers*/] = (char*)aParameter.p + aParameter.iWriting;
    {
        const char *pHeader;
        int i = 0;
        qsort((void *)pRequest->pListOfHeaders, pRequest->cHeaders, sizeof(char **), qsort_strcasecmp_);
        while (i < pRequest->cHeaders) {
            const char *pReplace = 0;
            pHeader = pRequest->pListOfHeaders[i];
            if (!strncasecmp(pHeader, "x-amz-date:", 11)) {
                pRequest->pXAmzDate = strchr(pHeader, ':')+1;
                while (*(pRequest->pXAmzDate)==' ') {
                    pRequest->pXAmzDate += 1;
                }
            }
            if (!strncasecmp(pHeader, "x-amz-security-token: will_replace", 34)) {
                pReplace = pRequest->ppSigningParameters[iSECURITY_TOKEN];
            }

            /* lower-casify header for signing */
            while (*pHeader != ':') {
                char *pWrite = (char *) aParameter.p + aParameter.iWriting;
                if (!librock_safeAppend0(&aParameter, "X", 1)) { 
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                if (*pHeader >= 'A' && *pHeader <= 'Z') {
                    *pWrite = *pHeader + 'a' - 'A';
                } else {
                    *pWrite = *pHeader;
                }
                pHeader++;
            }
            if (!librock_safeAppend0(&aParameter, ":", 1)) { 
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
            pHeader++; /* move beyond ':' */
            pHeader += countLWS(pHeader);
            if (pReplace) {
                if (!librock_safeAppend0(&aParameter, pReplace, -1 )) {
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
            } else {
                const char *pStart = pHeader;
                while (*pHeader && *pHeader != '\n') {
                    if (pRequest->bFormatCURL && *pHeader == '\x22') {
                        break;
                    }
                    pHeader++;
                    if (((*pHeader&0xff) <= ' ') && pHeader[-1]==' ') {
                        /* Need to elide spaces */
                        if (!librock_safeAppend0(&aParameter, pStart, pHeader - pStart)) {
                            freeOnce((void **) & aParameter.p);
                            return "E-470 would overflow pre-allocated block";
                        }
                        aParameter.iWriting--;
                        pStart = pHeader;
                    }
                }
                if (pHeader > pStart) {
                    if (!librock_safeAppend0(&aParameter, pStart, pHeader - pStart)) { 
                        freeOnce((void **) & aParameter.p);
                        return "E-470 would overflow pre-allocated block";
                    }
                }
                
            }
            if (!librock_safeAppend0(&aParameter, "\n", 1)) { 
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
            i++;
        }
        /* Strip the last newline */
        aParameter.iWriting--;
        if (!librock_safeAppend0(&aParameter, "", 0)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        aParameter.iWriting++; /* Retain the terminating \0. */
    }

    pParameterList[5/*Names of Signed Headers*/] = (char*)aParameter.p + aParameter.iWriting;
    /* All of the headers we have will be signed. If caller or proxies add
       headers later, they will not be signed. */
    {
        unsigned iStart;
        int i = 0;
        int cSigned;
        if (!librock_safeAppend0(&aParameter, "\n", 1)) { 
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        iStart = aParameter.iWriting;
        
        while (i < pRequest->cHeaders) {
            const char *pHeader;
            if (i > 0) {
                if (!librock_safeAppend0(&aParameter, ";", 1)) { 
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
            }
            pHeader = pRequest->pListOfHeaders[i];

            while (*pHeader != ':') { // Made sure there is a ':' above
                char *pWrite = librock_safeAppend0(&aParameter, 0/* we will write */, 1);
                if (!pWrite) {
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                *pWrite = *pHeader;
                if (*pHeader >= 'A' && *pHeader <= 'Z') {
                    *pWrite = *pHeader + 'a' - 'A';
                }
                pHeader++;
            }
            i++;
        }
        cSigned = aParameter.iWriting - iStart;

        /* Make a copy of these */
        pRequest->pSignedHeaders = malloc(cSigned+1);
        if (!pRequest->pSignedHeaders) {
            freeOnce((void **) & aParameter.p);
            return "E-451 malloc failed";
        }
        memmove(pRequest->pSignedHeaders, (char*)aParameter.p + iStart, cSigned);
        pRequest->pSignedHeaders[cSigned] = '\0';
        
        /* Keep the \0 and start the next string*/
        if (!librock_safeAppend0(&aParameter, "\0", 1)) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
    
    }
    
    /* Hash of the Request Payload */
    pParameterList[6/*Hash of payload*/] = (char *) aParameter.p + aParameter.iWriting;
    {
        if (!librock_safeAppend0(&aParameter, pRequest->ppSigningParameters[iSHA256], 32*2)) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
    }
    
    ((char *) aParameter.p)[aParameter.iWriting] = '\0';
    
    { /* Create the Canonical request from the template */
        char *pCanonicalRequest = 0;
        const char *pErrorMessage = 0;
        char *pHost = 0;
        if (pRequest->signatureV2) {
            if (!pRequest->pUriHost) {
                pErrorMessage = "E-1092 Need Host for AWS Signature Version 2";
            } else {
                int cHost = pRequest->pUriPath - pRequest->pUriHost;
                pHost = malloc(cHost+1);
                if (!pHost) {
                    pErrorMessage = "E-1096 malloc failed";
                } else {
                    
                    memmove(pHost, pRequest->pUriHost, cHost);
                    pHost[cHost] = '\0';
                    pParameterList[4] = pHost;
                    
                }
            }
            pCanonicalRequestTemplate =
                "@1_VERB@\n"
                "@4_Host@\n"
                "@2_URI@\n"
                "@3_QueryString@"
                ;
        }
        if (!pErrorMessage) {
            pErrorMessage = librock_fillTemplate(&pCanonicalRequest, pCanonicalRequestTemplate, -1, 0, 7, &pParameterList[0], 0);
        }
        freeOnce((void **) &pHost);
        if (pErrorMessage) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            freeOnce((void **) & pCanonicalRequest);
            return pErrorMessage;
        }

        if (pRequest->fnDebugOutput) {
            librock_awsSignature_dump2_(pRequest, "CanonicalRequest", pCanonicalRequest);
        }

        if (pRequest->signatureV2) {
            freeOnce((void **) & aParameter.p);
            *ppToSign = pCanonicalRequest; // Caller will free
            return 0;
        }

        
        /* Compute hash */
        pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
        if (!pHashInfo) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            freeOnce((void **) & pCanonicalRequest);
            return "E-1059 malloc failed";
        }
        librock_sha256Init(pHashInfo);
        librock_sha256Update(pHashInfo, (unsigned char *) pCanonicalRequest, strlen(pCanonicalRequest));
        librock_sha256StoreFinal(mdCanonicalRequest, pHashInfo);

        freeOnce((void **)&pHashInfo);

        /* mdCanonicalRequest has the SHA256 of the canonical request */

        freeOnce((void **) & pCanonicalRequest);
        
    }
    {
        /* Now the request to sign. Reuse the buffer. It's big enough. */
        aParameter.iWriting = 0;
        
        /*Algorithm Designation */
        if (!librock_safeAppend0(&aParameter, "AWS4-HMAC-SHA256\n", -1)) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        
        /* date-time from the header */
        if (!librock_safeAppend0(&aParameter, pRequest->pXAmzDate, countToEol(pRequest->pXAmzDate))) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }

        if (((char *) aParameter.p)[aParameter.iWriting-1]== '\"') {
            aParameter.iWriting--;
        }
        if (!librock_safeAppend0(&aParameter, "\n", 1)) { 
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        
        /* Credential scope starts with the date from the XAmzDate header, truncated at "T" */
        {
            int position = aParameter.iWriting;
            if (!librock_safeAppend0(&aParameter, pRequest->pXAmzDate, countToEol(pRequest->pXAmzDate))) {
                freeOnce((void **) & pRequest->pSignedHeaders);
                freeOnce((void **) & aParameter.p);
                return "E-470 would overflow pre-allocated block";
            }
            position += countToStr((char *) aParameter.p + position,"T");
            /* Truncate here */
            aParameter.iWriting = position;
        }
        
        librock_safeAppend0(&aParameter, "/", 1);
        librock_safeAppend0(&aParameter, pRequest->ppSigningParameters[iSERVICE_REGION], -1);
        librock_safeAppend0(&aParameter, "/", 1);
        librock_safeAppend0(&aParameter, pRequest->ppSigningParameters[iSERVICE_NAME], -1);
        if (!librock_safeAppend0(&aParameter, "/aws4_request\n", -1)) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            return "E-470 would overflow pre-allocated block";
        }
        
        /* Hashed canonical request */
        {
            unsigned int i;
            for (i = 0; i < sizeof(mdCanonicalRequest); i++) {
                char *pWrite = librock_safeAppend0(&aParameter, 0/* we will write */, 2);
                if (!pWrite) {
                    freeOnce((void **) & pRequest->pSignedHeaders);
                    freeOnce((void **) & aParameter.p);
                    return "E-470 would overflow pre-allocated block";
                }
                bToHex0(pWrite, mdCanonicalRequest[i] & 0xff);
            }
        }

        if (pRequest->fnDebugOutput) {
            librock_awsSignature_dump2_(pRequest, "StringToSign", aParameter.p);
        }
    }
    *ppToSign = aParameter.p; // Caller will free
    aParameter.p = 0;

    return 0;
} /* librock_awsSignature_getStringToSign */
/**************************************************************/


/**************************************************************/
PRIVATE const char *librock_awsSignature_outputWithAuthorization_(struct librock_awsRequest_s *pRequest, const char *pToSign)
{ /* Output with Authorization header*/
    unsigned char resultSha[32];
    char base64Version2[32*4/3*3+10]; /* Worst case */
    const char *pLiteral = "";

    if (pRequest->signatureV2) {
        char buffer1[100];
        struct librock_appendable a;
        librock_appendableSet(&a,buffer1,sizeof(buffer1),0);
        
        librock_hmacSha256(resultSha,
                pRequest->ppSigningParameters[iSECRET_ACCESS_KEY],
                strlen(pRequest->ppSigningParameters[iSECRET_ACCESS_KEY]),
                pToSign,
                strlen(pToSign));
                
        librock_safeAppendBase64Encoded0(&a, (char *) resultSha,32);

        librock_appendableSet(&a,base64Version2,sizeof(base64Version2),0);
        librock_safeAppendUrlEncoded0(&a,buffer1,strlen(buffer1));
        if (pRequest->fnDebugOutput) {
            pLiteral = "I-1256 ";
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, base64Version2, strlen(base64Version2));
            (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\n", 1);
        }
        
    } else {
        /*  Pseudocode for deriving a signing key
                Using kSecret = Your AWS Secret Access Key
        //E.g.  pRequest->pKey = "wJalrXUtnFEMI/K7MDENG+bPxRfiCYEXAMPLEKEY"; // Secret
        //  pRequest->cKey = strlen(pRequest->pKey);

                kDate = HMAC("AWS4" + kSecret, Date)
                kRegion = HMAC(kDate, Region)
                kService = HMAC(kRegion, Service)
                kSigning = HMAC(kService, "aws4_request")
        */
        { /* kDate = HMAC("AWS4" + kSecret, Date) */
            int cbBuf = strlen(pRequest->ppSigningParameters[iSECRET_ACCESS_KEY])+4;
            char *pConcatKey = (char *) malloc(cbBuf+1);
            if (!pConcatKey) {
                return "E-590 malloc failed";
            }
            memmove(pConcatKey, "AWS4", 4);
            memmove(pConcatKey+4, pRequest->ppSigningParameters[iSECRET_ACCESS_KEY], cbBuf-4);
            pConcatKey[cbBuf] = '\0';
            librock_hmacSha256(resultSha,
                                pConcatKey,
                                cbBuf,
                                pRequest->pXAmzDate,
                                strchr(pRequest->pXAmzDate,'T')-pRequest->pXAmzDate);
            freeOnce((void **)&pConcatKey);
        }
        
        {  
            const char *pRead = 0;
            
            /* kRegion = HMAC(kDate, Region) */
            pRead = pRequest->ppSigningParameters[iSERVICE_REGION];
            librock_hmacSha256(resultSha, (char *) resultSha, 32, pRead, strlen(pRead));
            pRead = strchr(pRead, '/')+1;

            /* kService = HMAC(kRegion, Service) */
            pRead = pRequest->ppSigningParameters[iSERVICE_NAME];
            librock_hmacSha256(resultSha, (char *) resultSha, 32, pRead, strlen(pRead));
            
            /* kSigning = HMAC(kService, "aws4_request")*/
            librock_hmacSha256(resultSha, (char *) resultSha, 32, "aws4_request", 12);

            /* resultSha has the signing key */
            if (pRequest->fnDebugOutput) {
                unsigned int i;
                pLiteral = "I-608 ";
                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, pLiteral, strlen(pLiteral));
                for (i = 0; i < sizeof(resultSha); i++) {
                    char buf[3];
                    bToHex0(buf, resultSha[i] & 0xff);
                    (*pRequest->fnDebugOutput)(pRequest->debugOutputId, buf, strlen(buf));
                }
                (*pRequest->fnDebugOutput)(pRequest->debugOutputId, "\n", 1);

            }
        }

        /* Use the signing key which in resultSha, and write the result back to resultSha */
        librock_hmacSha256(resultSha, (char *) resultSha, 32, pToSign, strlen(pToSign));
    }
    
    /* resultSha has the signature. Output with Authorization */
    {
        const char *pHeader = pRequest->pRequest;
        int didContent = 0;
        while (*pHeader || !didContent) {
            const char *pStart = pHeader;
            if (*pHeader == '\0') {
                if (pRequest->bFormatCURL) {
                    pStart = "header = \x22x-amz-content-sha256:\x22";
                } else {
                    pStart = "x-amz-content-sha256:";
                }
            }
            pHeader += countToEol(pHeader);
            if (pRequest->bFormatCURL) {
                if (isCurlOptionName(pStart, "header")) {
                    pStart += countToValue(pStart);
                } else {
                    if (pHeader[-1]=='\"') {
                        pHeader--;
                    }
                    (*(pRequest->fnOutput))(pRequest->outputId, pStart, pHeader-pStart);
                    if (isCurlOptionName(pStart, "url") && pRequest->signatureV2) {
                        didContent = 1;
                        if (!strncmp(pRequest->pVerb,"GET",countOptionName(pRequest->pVerb))) {
                            (*(pRequest->fnOutput))(pRequest->outputId, "&Signature=", 11);
                            (*(pRequest->fnOutput))(pRequest->outputId, base64Version2, strlen(base64Version2));
                        }
                    } else if (isCurlOptionName(pStart, "data") && pRequest->signatureV2) {
                        (*(pRequest->fnOutput))(pRequest->outputId, "&Signature=", 11);
                        (*(pRequest->fnOutput))(pRequest->outputId, base64Version2, strlen(base64Version2));
                        pRequest->ppSigningParameters[iCONTENT_LENGTH] += 11 + strlen(base64Version2);
                    }
                    if (*pHeader == '\"') {
                        (*(pRequest->fnOutput))(pRequest->outputId, pHeader, 1);
                        pHeader++;
                    }
                    (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
                    if (*pHeader) pHeader++;
                    continue;
                }
            }
            if (!strncasecmp(pStart, "authorization:", 14)) {
                /* Skip this now. Add it later */
            } else if (!strncasecmp(pStart, "x-amz-content-sha256:", 21)) {
                const char *pLiteral;
                didContent = 1;
                if (pRequest->bFormatCURL) {
                    pLiteral = "header = \x22x-amz-content-sha256:";
                } else {
                    pLiteral = "x-amz-content-sha256:";
                }
                (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            
                pLiteral = pRequest->ppSigningParameters[iSHA256];
                (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, 64);
                
                if (pRequest->bFormatCURL) {
                    (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);           
                }
                (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
            } else {
                if (pRequest->bFormatCURL) {
                    pLiteral = "header = \x22";
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                }
                (*(pRequest->fnOutput))(pRequest->outputId, pStart, pHeader-pStart);
                if (pRequest->signatureV2 && pStart == pRequest->pRequest) {
                    didContent = 1;
                    (*(pRequest->fnOutput))(pRequest->outputId, "&Signature=", 11);
                    (*(pRequest->fnOutput))(pRequest->outputId, base64Version2, strlen(base64Version2));
                }
                if (pRequest->bFormatCURL && pHeader[-1] != '\x22') {
                    (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);
                }
                (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
            }
            if (*pHeader) {
                pHeader++;
            }
        }
        if (pRequest->signatureV2) {
        } else {
            /* Authorization header */
            if (pRequest->bFormatCURL) {
                pLiteral = "header = \x22";
                (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            }
            pLiteral = "Authorization:";
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));

            pLiteral = "AWS4-HMAC-SHA256 Credential=";
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            pLiteral = pRequest->ppSigningParameters[iACCESS_KEY_ID];
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            (*(pRequest->fnOutput))(pRequest->outputId, "/", 1);
            (*(pRequest->fnOutput))(pRequest->outputId, pRequest->pXAmzDate, (int) (strchr(pRequest->pXAmzDate, 'T')-pRequest->pXAmzDate));
            (*(pRequest->fnOutput))(pRequest->outputId, "/", 1);
            pLiteral = pRequest->ppSigningParameters[iSERVICE_REGION];
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            (*(pRequest->fnOutput))(pRequest->outputId, "/", 1);
            pLiteral = pRequest->ppSigningParameters[iSERVICE_NAME];
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            pLiteral = "/aws4_request";
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            
            pLiteral = ", SignedHeaders=";
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            (*(pRequest->fnOutput))(pRequest->outputId, pRequest->pSignedHeaders, strlen(pRequest->pSignedHeaders));
            pLiteral = ", Signature=";
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
            {
                unsigned int i;
                for (i = 0; i < sizeof(resultSha); i++) {
                    char buf[3];
                    bToHex0(buf, resultSha[i] & 0xff);
                    (*(pRequest->fnOutput))(pRequest->outputId, buf, 2);
                }

            }
            if (pRequest->bFormatCURL) {
                (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);
            }
            (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);

            if (pRequest->ppSigningParameters[iSECURITY_TOKEN]) {
                if (pRequest->bFormatCURL) {
                    pLiteral = "header = \x22X-Amz-Security-Token:";
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                    pLiteral = pRequest->ppSigningParameters[iSECURITY_TOKEN];
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                    (*(pRequest->fnOutput))(pRequest->outputId, "\x22\n", 2);
                } else {
                    pLiteral = "X-Amz-Security-Token:";
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                    pLiteral = pRequest->ppSigningParameters[iSECURITY_TOKEN];
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                    (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
                }
            }

            /* write an amz-date, if needed */
            if (pRequest->amzDate[0]) {
                if (pRequest->bFormatCURL == 1) {
                    pLiteral = "header = \x22";
                    (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
                    (*(pRequest->fnOutput))(pRequest->outputId, pRequest->amzDate, strlen(pRequest->amzDate));
                    (*(pRequest->fnOutput))(pRequest->outputId, "\x22\n", 2);
                } else {
                    (*(pRequest->fnOutput))(pRequest->outputId, pRequest->amzDate, strlen(pRequest->amzDate));
                    (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);
                }
            }
        }
        if (pRequest->ppSigningParameters[iCONTENT_LENGTH] > 0) {
            unsigned long value = (unsigned long) (pRequest->ppSigningParameters[iCONTENT_LENGTH]);
            char buf[40];
            int i = sizeof(buf);
            /* to string, avoiding sprintf */
            i--;
            buf[i] = '\0';
            do {
                i--;
                buf[i] = (value % 10) + '0';
                value /= 10;
            } while(value);
            
            if (pRequest->bFormatCURL) {
                pLiteral = "header = \x22""Content-Length:";
            } else {
                pLiteral = "Content-Length:";
            }
            (*(pRequest->fnOutput))(pRequest->outputId, pLiteral, strlen(pLiteral));
        
            (*(pRequest->fnOutput))(pRequest->outputId, buf+i, strlen(buf+i));
            
            if (pRequest->bFormatCURL) {
                (*(pRequest->fnOutput))(pRequest->outputId, "\x22", 1);           
            }
            (*(pRequest->fnOutput))(pRequest->outputId, "\n", 1);

        }
    }
    return 0;
} /* librock_awsSignature_outputWithAuthorization */
/**************************************************************/
//[[built-in templates]]

const char *librock_awsBuiltInTemplates[] = {
    "@//awsFillAndSign_write_templates_" //Placeholder

#include "aws_templates.inc"

#if defined librock_WANT_ALTERNATE_BRANCHING
/* For branch and line coverage */
,"@//bad-template.curl;\n@//;"
    "\n@//.default.bad"
    "\n@//="

,"@//bad-template2.curl;2016-11-26;MIB SOFTWARE, INC"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""url=\"https://s3.amazonaws.com/"
    "\n""header = \"Host: s3.amazonaws.com\""

,"@//bad-template3.curl;"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@"
#endif
,0 // End of list
};

const char *librock_awsBuiltInTemplate(const char *pName)
{
    int i;
    int cName;
    i = 0;
    cName = strlen(pName);
    while(librock_awsBuiltInTemplates[i]) {
        if (countToStr(librock_awsBuiltInTemplates[i]+3,";")==cName) {
            if (!strncmp(pName,librock_awsBuiltInTemplates[i]+3,cName)) {
                return librock_awsBuiltInTemplates[i];
            }
        }
        i++;
    }
    return 0;
}

/**************************************************************/
//[[function main()]]

#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)

PRIVATE int write_to_FILE(void *id, const char *pRead, int len)
{ /* Helper function */
    int cWritten = fwrite(pRead, 1, len, (FILE *)id);
    fflush((FILE *)id);
    return cWritten;
} /* write_to_FILE */
#endif

int librock_stringListGetIndex(char ***ppNamedArguments,int cStep, const char *pName, int cName)
{
    int expansion = cStep * 32; /* 32 slots at a time */
    int iString = 0;

    /* Linear search. Is this already set? */
    while(*ppNamedArguments && (*ppNamedArguments)[iString]) {
        if (!strncmp(pName,(*ppNamedArguments)[iString],cName) &&
            (*ppNamedArguments)[iString][cName] == '\0') {
            /* Matched. It is already set. */
            return iString;
        }
        iString += cStep;
    }
    if (!*ppNamedArguments) {
        iString = -1;
    }
    if (iString == -1 || iString % expansion == expansion-cStep) {
        /* Need to expand */
        char **ppExpanded;

        ppExpanded = realloc((void *)*ppNamedArguments, sizeof(char *) * (iString+1+expansion));
        if (!ppExpanded) {
            return -1;
        }
        *ppNamedArguments = ppExpanded;
        memset((void *) (*ppNamedArguments + iString + 1), '\0', sizeof(char *) * expansion);
        if (iString == -1) {
            iString = 0;
        }
    }
    return iString;
} /* librock_stringListGetIndex */

#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)

PRIVATE const char *collectNamedDefaults(char ***ppNamedArguments, const char *pRequestTemplate)
{ /* Scan a template for @//.default. entries */
const char *pRead = pRequestTemplate;
int cRequestTemplate = strlen(pRequestTemplate);
while(*pRead) {
    int cToken = pRequestTemplate + cRequestTemplate - pRead;
    int iTokenType = atoi(librock_fillTemplateTokenize(pRequestTemplate, pRead - pRequestTemplate, &cToken));
    if (iTokenType == 1) {//* comment
        int cName;
        if (cToken > 12 && !strncmp(pRead,"@//.default.",12)) {
            pRead += 12;
            cToken -= 12;
            cName = countToStr(pRead, "=");
            
            if (cName >= cToken) {
                /* Ignore */
            } else {
                int iString = librock_stringListGetIndex(ppNamedArguments, 2, pRead, cName);
                if (iString < 0) {
                    return "E-1522 malloc failed";
                }
                if ((*ppNamedArguments)[iString] == 0) {
                    /* Not found. Add it */
                    char *pValue = 0;
                    (*ppNamedArguments)[iString] = malloc(cName+1);
                    if (!(*ppNamedArguments)[iString]) {
                        return "E-1522 malloc failed";
                    }
                    memmove((*ppNamedArguments)[iString], pRead, cName);
                    (*ppNamedArguments)[iString][cName] = '\0';

                    pValue = mallocNamedValue((*ppNamedArguments)[iString]);                    
                    if (!pValue) { /* Not found in environment. Set the default */
                        pValue = malloc(cToken-cName+1);
                        if (!pValue) {
                            return "E-1522 malloc failed";
                        }
                        memmove(pValue, pRead+cName+1, cToken-cName-1);
                        pValue[cToken-cName-1] = '\0';
                        pValue[countToEol(pValue)] = '\0';
                    }
                    (*ppNamedArguments)[iString+1] = pValue;
                }
            }
        }
    } else if (iTokenType == 3 && cToken==2) { /* Escaped @@ */
    } else if (iTokenType == 3) {
        /* Parameter */
        pRead++;
        cToken--;
        if (*pRead == 'e') {// environment variable
            int iString;
            pRead++;
            cToken--;
            iString = librock_stringListGetIndex(ppNamedArguments, 2, pRead, cToken-1);
            if (GLOBAL_ALTERNATE_BRANCH) {
                iString = -1;
            }
            if (iString < 0) {
                return "E-1522 malloc failed";
            }
            if (!(*ppNamedArguments)[iString]) {
                /* Not already defined */
                char *pValue;
                (*ppNamedArguments)[iString] = malloc(cToken);
                if (!(*ppNamedArguments)[iString]) {
                    return "E-1522 malloc failed";
                }
                memmove((*ppNamedArguments)[iString], pRead, cToken-1);
                (*ppNamedArguments)[iString][cToken-1] = '\0';
                pValue = mallocNamedValue((*ppNamedArguments)[iString]);
                (*ppNamedArguments)[iString+1] = pValue;
                if (!pValue) { /* Not found in environment. */
                    freeOnce((void **) &((*ppNamedArguments)[iString]));
                } else {
                    (*ppNamedArguments)[iString+1] = pValue;
                }
                
            }
        }
        
    }
    pRead += cToken;

}

return 0;
} /* collectNamedDefaults */
PRIVATE const char *collectNamedParameters(char ***pppNamedArguments, const char *pRequestTemplate)
{
        const char *pMessage = 0;
        pMessage = collectNamedDefaults(pppNamedArguments, 
                    "@//.default.QueryExtra=");
        if (pMessage) {
            return pMessage;
        }

        pMessage = collectNamedDefaults(pppNamedArguments, 
                    "@eAWS_DEFAULT_REGION@"
                    ",@eAWS_SERVICE_NAME@"
                    ",@eAWS_SECURITY_TOKEN@"
                    ",@eQueryExtra@");
        if (pMessage) {
            return pMessage;
        }
        pMessage = collectNamedDefaults(pppNamedArguments, pRequestTemplate);
        if (pMessage) {
            return pMessage;
        }
        if (!*pppNamedArguments) {
            pMessage = "E-1879 Must set AWS_DEFAULT_REGION and AWS_SERVICE_NAME";
        }
        return pMessage;

} /* collectNamedParameters */

const char *mallocNamedTemplate(char **ppRequestTemplate,int fromFile,const char *pName)
{
    char *pWrite = 0;
    char *pRead;
    if (fromFile) {
        *ppRequestTemplate = (char *) librock_fileGetContents(pName);
        if (!*ppRequestTemplate) {
            return "E-1096 did not load file";
        }
    } else {   /* Make a copy of built-in template */
        char *pBuiltIn = (char *) librock_awsBuiltInTemplate(pName);
        int size;
        if (!pBuiltIn) {
            return "I-1515 no built-in template";
        }
        size = strlen(pBuiltIn);
        *ppRequestTemplate = malloc(size+1);
        if (!*ppRequestTemplate) {
            return "I-1515 malloc failed";
        }
        memmove(*ppRequestTemplate, pBuiltIn, size+1);
    }
    /* Strip \r in place */
    pWrite = *ppRequestTemplate;
    pRead = pWrite;
    *ppRequestTemplate = pWrite;
    while (*pRead) {
        while (*pRead == '\r') {
            pRead++;
        }
        *pWrite++ = *pRead++;
    }
    *pWrite = '\0';
    return 0;

} /* mallocNamedTemplate */

const char *prepareSHA256(char **ppSHA256,int scanType, unsigned long *contentLength, unsigned char *mdContent32, const char *pFilledRequest)
{
    if (scanType == 1) { /* process CURL options to determine body */
        /* Find an upload-file or data= field in the request. */
        const char *pRead = pFilledRequest;
        while(pRead) {
            if (isCurlOptionName(pRead, "upload-file")) {
                char *fileName = 0;
                const char *pErrorMessage;
                int nameLength;
                pRead += countToValue(pRead);
                nameLength = countToStr(pRead, "\x22");
                fileName = malloc(nameLength+1);
                if (!fileName) {
                    return "E-1741 malloc failed";
                }
                memmove(fileName, pRead, nameLength );
                fileName[nameLength] = '\0';

                pErrorMessage = librock_fileSha256Contents(fileName, &mdContent32[0]);
                if (!pErrorMessage) {
                    int fd;
                    fd = librock_fdOpenReadOnly(fileName);
                    if (fd == -1) {
                        pErrorMessage = "E-1797 Could not open upload file to create SHA256";
                    } else {
                        *contentLength = librock_fdSeek(fd, 0, SEEK_END);
                        librock_fdClose(fd);
                    }
                }                
                freeOnce((void **)&fileName);
                if (pErrorMessage) {
                    return pErrorMessage;
                }
                break;
            } else if (isCurlOptionName(pRead, "data") ||
                       isCurlOptionName(pRead, "data-binary")
                    ) {
                void *pHashInfo;
                int iStart = 0;
                int iParse = 0;
                pRead += countToValue(pRead);
                /* Compute hash */
                pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
                if (!pHashInfo) {
                    return "E-1741 malloc failed";
                }
                *contentLength = 0;
                librock_sha256Init(pHashInfo);
                while(pRead[iParse] && pRead[iParse] != '\n' && pRead[iParse]!= '\x22') {
                    if (pRead[iParse] == '\\') {
                        //These are the escapes that CURL recognizes */
                        librock_sha256Update(pHashInfo, 
                           (unsigned char *) pRead+iStart, iParse-iStart);
                        if (pRead[iParse+1]=='t') {
                            librock_sha256Update(pHashInfo, 
                               (const unsigned char *) "\t", 1);
                        } else if (pRead[iParse+1]=='r') {
                            librock_sha256Update(pHashInfo, 
                               (const unsigned char *) "\r", 1);
                        } else if (pRead[iParse+1]=='n') {
                            librock_sha256Update(pHashInfo, 
                               (const unsigned char *) "\n", 1);
                        } else if (pRead[iParse+1]=='v') {
                            librock_sha256Update(pHashInfo, 
                               (const unsigned char *) "\v", 1);
                        } else {
                            /* Just itself */
                            librock_sha256Update(pHashInfo, 
                               (unsigned char *) pRead+iParse+1, 1);
                        }
                        *contentLength += iParse-iStart+1;
                        
                        iParse = iParse+2;
                        iStart = iParse;
                    } else {
                        iParse++;
                    }
                }
                librock_sha256Update(pHashInfo, 
                   (unsigned char *) pRead+iStart, iParse-iStart);
                *contentLength += iParse-iStart;
                librock_sha256StoreFinal(mdContent32, pHashInfo);
                freeOnce((void **)&pHashInfo);
                break;
            }
            pRead = strchr(pRead, '\n');
            if (pRead) {
                pRead++;
            }
            
        }
        if (!pRead) {
            // Presume empty body is OK.
        }
    }
    {
        int i;
        *ppSHA256 = malloc(64+1);
        if (!*ppSHA256) {
            return "E-1741 malloc failed";
        }
        for (i = 0; i < 32; i++) {
            bToHex0(*ppSHA256 + i*2, mdContent32[i] & 0xff);
        }
        (*ppSHA256)[32*2] = '\0';
    }
    return 0; // No error
} /* prepareSHA256 */

int main_help()
{
fprintf(stdout, "%s",
    "awsFillAndSign Copyright 2016 MIB SOFTWARE, INC."
    "\n"
    "\n"" PURPOSE:   Sign Amazon Web Services requests with AWS Signature Version 4."
    "\n"
    "\n"" LICENSE:   MIT (Free/OpenSource)"
    "\n" 
    "\n"" STABILITY: UNSTABLE as of 2017-01-17"
    "\n""            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign"
    "\n"              
    "\n"" SUPPORT:   Contact the author for commercial support and consulting at"
    "\n""            http://www.mibsoftware.com/"
    "\n"
    "\n"
    "\n"" USAGE: awsFillAndSign [OPTIONS] <template-name.curl> [param1[ param2...]]"
    "\n"
    "\n""   The output is the filled template with AWS Version 4 signatures."
    "\n""   Credentials come from the environment variables AWS_ACCESS_KEY_ID"
    "\n""   and AWS_SECRET_ACCESS_KEY (unless using the --read-key option.)"
    "\n""   AWS_SECURITY_TOKEN is included if it is defined."
    "\n"
    "\n"
    "\n"" OPTIONS:"
    "\n""  --from-file      Treat the template-name as a file, not a built-in template."
    "\n""                   (Use --list to see built-in templates.)"
    "\n"
    "\n""  --have-sha256    The template has a SHA256 body signature."
    "\n"
    "\n""  -b <file-name>   Calculate SHA256 body signature from specified file."
    "\n""                   (Only needed if the template does not give accurate"
    "\n""                   upload-file or data CURL options.)"
    "\n"
    "\n""  --read-key       Read credentials from one line on stdin in the format of:"
    "\n""                      <ID>,<SECRET>"
    "\n"
    "\n""  --verbose        Verbose debugging output on stderr, including generated"
    "\n""                   AWS Canonical Request fields."
    "\n"
    "\n""  -D <name=value>  Put name=value into the environment."
    "\n"
    "\n""  --encode <flag>  Control percent-encoding. 0 - (default) auto-detect;"
    "\n""                   1 - always percent-encode; -1 - never percent-encode."
    "\n"
    "\n""  -                Marker for end of arguments. (Useful when parameters that"
    "\n""                   follow may start with '-'.)"
    "\n"
    "\n"" INFORMATION commands (output is not a filled and signed template:)"
    "\n""  --help           Show this message."
    "\n""  --list           List the built-in templates."
    "\n""  --list <name>    Show the named built-in template along with comments."
    "\n"
    );
return 0;    
}
int main_list(int argc, char **argv)
{
    int i;
    int argumentIndex = 0;
    i = 1;
    if (argumentIndex == argc-1) {
        while(librock_awsBuiltInTemplates[i]) {
            fprintf(stdout, "%.*s\n", countToStr(librock_awsBuiltInTemplates[i]+3,";"), librock_awsBuiltInTemplates[i]+3);
            i++;
        }
    } else {
        const char *pNext;
        const char *pFields;
        int cField = 0;
        const char *pRequestTemplate = (char *) librock_awsBuiltInTemplate(argv[argumentIndex+1]);
        if (!pRequestTemplate) {
            fprintf(stderr, "I-1515 no built-in template '%s'\n", argv[argumentIndex+1]);
            return 9;
        }
        fprintf(stdout, "@//%s\n", argv[argumentIndex+1]);

        // Templates are stored in a compact format. Expand it
        pFields = pRequestTemplate + countToStr(pRequestTemplate,";")+1;
        pRequestTemplate += countToStr(pRequestTemplate, "\n")+1;
        pNext = strstr(pRequestTemplate,"REST API DOCS");
        if (!pNext) {
            pNext = strstr(pRequestTemplate,"TEMPLATE FOR:");
        }
        if (!pNext) {
            pNext = pRequestTemplate;
        }
        
        pNext += countToEol(pNext) + 1;
        fprintf(stdout, "%.*s", pNext - pRequestTemplate,pRequestTemplate);
        fprintf(stdout, "@//");
        fprintf(stdout, "\n@// TEMPLATE REVISION:    ");
        cField = countToStr(pFields,";");
        if (pFields + cField < pRequestTemplate) {
            fprintf(stdout,"%.*s", cField ,pFields);
        } else {
            cField = pRequestTemplate - pFields - 1;
        }
        pFields += cField + 1;
        cField = countToStr(pFields,";");
        if (pFields + cField < pRequestTemplate) {
            fprintf(stdout," by %.*s", cField ,pFields);
        } else {
            cField = pRequestTemplate - pFields - 1;
        }
        pFields += cField + 1;
        fprintf(stdout, "\n@// TEMPLATE LICENSE:     ");
        cField = pRequestTemplate-pFields-1;
        if (cField > 0) {
            fprintf(stdout,"%.*s", cField ,pFields);
        }
        if (countToStr(pNext,"  @1") < countToStr(pNext,"\n")) {
            fprintf(stdout, "\n@// TEMPLATE PARAMETERS:");
        }
        pRequestTemplate = pNext-1;
        pNext = strstr(pRequestTemplate,"\n@//.default.");
        if (!pNext) {
            pNext = pRequestTemplate + strlen(pRequestTemplate);
        }
        fprintf(stdout,"%.*s", pNext-pRequestTemplate, pRequestTemplate);
        fprintf(stdout,"%s",
            "\n""@//"
            "\n""@// (Before using CURL, use awsFillAndSign by MIB SOFTWARE to fill the"
            "\n""@// template, strip comments, and add headers for AWS Signature version 4.)"
            "\n""@//"
            );
        fprintf(stdout,"%s", pNext);
        pRequestTemplate = 0; /* Was not allocated */
        return 0;
    }
    return 0;

}
int main(int argc, char **argv)
{
    //char *credentialregion = 0;
    int argumentIndex;
    int bVerbose = 0;
    int scanSignature = 1; /* By default, look for data and upload-file */
    int fromFile = 0;
    int iEncodeParameters = 0;
    int credentialsFromEnv = 1;
    char credentials[200];
    char *signingParameters[7];
    unsigned char mdContent32[] = { /* SHA256 of empty string */
        0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
    };
    char *pRequestTemplate = 0;
    char **ppNamedArguments = 0;
    char *pFilledRequest = 0;
    int errorCode = 0;

#if defined librock_WANT_ALTERNATE_BRANCHING
    argumentIndex = 1;
    fprintf(stderr,"I-1474 %s(COVERAGE TEST)", argv[0]);
    while(argumentIndex < argc) {
        fprintf(stderr, " %s", argv[argumentIndex]);
        if (!strncmp(argv[argumentIndex],"-Dtest=",7)) {
            putenv(argv[argumentIndex]+2);
        }
        argumentIndex++;
    }
    fprintf(stderr,"\n");
#endif
    librock_triggerAlternateBranch(0, 0); /* Initialize branch alteration mechanism */
    /* Accept command line flags preceding the template file name*/
    argumentIndex = 1;
    if (argc > argumentIndex) {
        while(argumentIndex < argc && argv[argumentIndex][0]=='-') {
            if (!strcmp(argv[argumentIndex], "-")) {
                /* This means end of arguments. Only useful to allow a
                   template file name to start with - */
                argumentIndex++;
                break;
            }
            if (!strcmp(argv[argumentIndex], "--verbose")) {
                bVerbose = 1;
            } else if (!strcmp(argv[argumentIndex], "--help")) {
                return main_help();
            } else if (!strcmp(argv[argumentIndex], "--list")) {    
                if (argumentIndex + 2 < argc) {
                    /* Error */
                    argumentIndex = argc;
                } else {
                    return main_list(argc-argumentIndex,argv+argumentIndex);
                }
            } else if (!strcmp(argv[argumentIndex], "--from-file")) {
                /* Load template from file */
                fromFile = 1;
#if defined librock_WANT_ALTERNATE_BRANCHING
            } else if (!strcmp(argv[argumentIndex], "--coverage")) {
                return librock_coverage_main();
#endif
            } else if (!strcmp(argv[argumentIndex], "--read-key")) {
                credentialsFromEnv = 0;
            } else if (!strncmp(argv[argumentIndex], "-D", 2)) {
                const char *pVariable;
                if (!strcmp(argv[argumentIndex], "-D")) {
                    if (argumentIndex + 1 >= argc) {
                        argumentIndex = argc;
                        break; //Show usage message
                    } else {
                        pVariable = argv[argumentIndex + 1];
                        argumentIndex++;
                    }
                } else {
                    pVariable = argv[argumentIndex]+2;
                }
                if (strstr(pVariable, "AWS_SECRET_ACCESS_KEY=")!=0) {
                    /* Prohibit operation with potential secret disclosure */
                    fprintf(stderr, "-D must not set AWS_SECRET_ACCESS_KEY on command line\n");
                    return 15;
                }
                putenv((char *) pVariable);
            } else if (!strncmp(argv[argumentIndex], "--have-sha256", 3)) {
                scanSignature = 0;
            } else if (!strncmp(argv[argumentIndex], "--encode", 8)) {
                if (argumentIndex + 1 >= argc) {
                    argumentIndex = argc;
                    break; //Show usage message
                }
                iEncodeParameters = atoi(argv[argumentIndex+1]);
                argumentIndex++;
            } else if (!strncmp(argv[argumentIndex], "-b", 2)) {
                const char *pBody = 0;
                const char *pErrorMessage;
                scanSignature = 2;
                if (!strcmp(argv[argumentIndex], "-b")) {
                    if (argumentIndex + 1 >= argc) {
                        argumentIndex = argc;
                        break; //Show usage message
                    }
                    pBody = argv[argumentIndex + 1];
                    argumentIndex++;
                } else {
                    pBody = argv[argumentIndex]+2;
                }
                pErrorMessage = librock_fileSha256Contents(pBody, &mdContent32[0]);
                if (pErrorMessage) {
                    fprintf(stderr, "%s for file '%s'\n", pErrorMessage, pBody);
                    return 7;
                }
            } else {
                fprintf(stderr, "E-1457 Unrecognized option: %s\nTry --help.\n",argv[argumentIndex]);
                return 8;
            }
            argumentIndex++;
        }
    }

    if (argumentIndex >= argc) {
        fprintf(stderr, "Usage: awsFillAndSign <template-name.curl> [param1[ param2...]]\nTry --help.\n");
        return -1;
    }
    
    
    { /* Get template */
        const char *pName = argv[argumentIndex];
        const char *pErrorMessage = mallocNamedTemplate(&pRequestTemplate,fromFile,pName);
        if (pErrorMessage) {
            if (fromFile) {
                perror(pErrorMessage);
                fprintf(stderr, " when loading template file '%s'\n", pName);
            } else {
                fprintf(stderr, "%s when loading built-in template '%s'\n", pErrorMessage, pName);
            }
            return 14;
        }
        if (bVerbose) {
            if (fromFile) {
                fprintf(stderr, "I-1703 loaded template file '%s'\n", pName);
            } else {
                fprintf(stderr, "I-1710 loaded built-in template '%s'\n", pName);
            }
        }
    }
    argumentIndex++; /* Advance to template parameters */

    { /* Determine Signing Parameters */
        const char *pName;
        const char *pErrorMessage = collectNamedParameters(&ppNamedArguments,pRequestTemplate);

        if (!pErrorMessage) { /* prep the AWS_AMZDATE, in case of V2 requests where it is not already present */
            char buffer[100];
            time_t now;
            struct tm gmt;
            time(&now);
            gmtime_r(&now, &gmt);
            strftime(buffer, sizeof buffer, "@//.default.AWS_AMZDATE=%Y-%m-%dT%H:%M:%SZ", &gmt);
            pErrorMessage = collectNamedParameters(&ppNamedArguments, buffer);
        }

        if (pErrorMessage) {
            fprintf(stderr, "%s\n", pErrorMessage);
            freeOnce((void **)&pRequestTemplate);
            errorCode = 14;
        } else {

            signingParameters[iSHA256] = 0; //Set below
            pName = "AWS_DEFAULT_REGION";
            signingParameters[iSERVICE_REGION] = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
            pName = "AWS_SERVICE_NAME";
            signingParameters[iSERVICE_NAME] = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
            pName = "AWS_SECURITY_TOKEN";
            signingParameters[iSECURITY_TOKEN] = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
        }
        if (!pErrorMessage) {
            int iString;
            pName = "AWS_REGION_NOT_USEAST1";
            iString = librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) );
            if (GLOBAL_ALTERNATE_BRANCH) {
                iString = -1;
            }
            if (iString < 0) {
                pErrorMessage = "E-2558 malloc failed";
            } else {
                const char *pLiteral;
                ppNamedArguments[iString] = malloc(strlen(pName)+1);
                if (!ppNamedArguments[iString]) {
                    pErrorMessage = "E-2558 malloc failed";
                }
                ppNamedArguments[iString+1] = malloc(strlen(signingParameters[iSERVICE_REGION])+2);
                if (!ppNamedArguments[iString+1]) {
                    pErrorMessage = "E-2558 malloc failed";
                }
                if (!pErrorMessage) {
                    memmove(ppNamedArguments[iString], pName,strlen(pName)+1);
                    pLiteral = signingParameters[iSERVICE_REGION];
                    if (strcmp(pLiteral, "us-east-1")) {
                        ppNamedArguments[iString+1][0] = '.';
                        memmove(ppNamedArguments[iString+1]+1, pLiteral,strlen(pLiteral)+1);
                    } else {
                        ppNamedArguments[iString+1][0] = '\0';
                    }
                }
            }

            if (pErrorMessage) {
                fprintf(stderr, "%s\n", pErrorMessage);
                errorCode = 14;
            } else if (credentialsFromEnv) {
                const char *pName;
                pErrorMessage = collectNamedDefaults(&ppNamedArguments, 
                        "@eAWS_ACCESS_KEY_ID@,@eAWS_SECRET_ACCESS_KEY@");
                if (!pErrorMessage) {
                    pName = "AWS_ACCESS_KEY_ID";
                    signingParameters[iACCESS_KEY_ID] = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
                    pName = "AWS_SECRET_ACCESS_KEY";
                    signingParameters[iSECRET_ACCESS_KEY] = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
                } else {
                    fprintf(stderr, "%s\n", pErrorMessage);
                    errorCode = 14;
                }
            } else {
                const char *pCredentials;
                credentials[sizeof(credentials)-2] = '\0';

                /* Get comma-separated credentials on stdin, which must have this form. Lengths may vary.
                      AXXXXXXXXXXXXXXXXXXX,7777777777777777777777777777777777777777
                 There must be no quote characters or spaces.
                 Line length more than 198 characters will block processing.
                 */
                if (bVerbose) {
                    fprintf(stderr, "I-1737 reading credentials on stdin\n");
                }

                pCredentials = fgets(credentials,sizeof(credentials)-1, stdin);
                
                if (GLOBAL_ALTERNATE_BRANCH) {
                    pCredentials = 0;
                }
                if (!pCredentials) {
                    perror("E-1163 Could not read credentials string");
                    errorCode = 5;
                } else if (strlen(credentials) >= sizeof(credentials) - 2) {
                    fprintf(stderr, "E-1167 The credentials string on stdin is too long\n");
                    freeOnce((void **)&pRequestTemplate);
                    errorCode = 6;
                } else if (!strchr(credentials,',')) {
                    fprintf(stderr, "E-1956 Need comma-separated credentials on stdin\n");
                    freeOnce((void **)&pRequestTemplate);
                    errorCode = 16;
                } else {
                    credentials[countToEol(credentials)] = '\0'; /* Remove \r or EOL */
                    *(strchr(credentials, ',')) = '\0'; /* Split string */

                    signingParameters[iACCESS_KEY_ID] = credentials;
                    signingParameters[iSECRET_ACCESS_KEY] = credentials+strlen(credentials)+1;
                }
                
            }
        }
        
    }
    
    if (!errorCode) { /* Fill template */
        const char *pErrorMessage = 0;

        int iError;
        
        pErrorMessage = librock_fillTemplate(&pFilledRequest, pRequestTemplate,
                                                iEncodeParameters,
                                                ppNamedArguments,
                                                argc-argumentIndex+1, argv+argumentIndex-1,
                                                &iError);
        if (pErrorMessage) {
            int cContext = countToEol(pRequestTemplate + iError);
            if (countToStr(pRequestTemplate+iError,"@") < cContext) {
                if (pRequestTemplate[iError]!='@') {
                    cContext = countToStr(pRequestTemplate + iError,"@");
                }
            }
            fprintf(stderr, "%s at %d:%.*s\n", pErrorMessage, iError, cContext, pRequestTemplate + iError);
            errorCode = 4;
        }
    }
    if (!errorCode) { /* Prepare Content SHA256 and sign */
        char *pSHA256 = 0;
        const char *pErrorMessage = 0;
        if (scanSignature==0) {
            /* Template has SHA256 */
            signingParameters[iSHA256] = (char *) 0;
        } else {
            pErrorMessage = prepareSHA256(&pSHA256, scanSignature, (unsigned long *) &signingParameters[iCONTENT_LENGTH], mdContent32, pFilledRequest);
            if (pErrorMessage) {
                if (atoi(pErrorMessage+2)!= 1741) {
                    fprintf(stderr, "%s\nI-1734 template expanded:\n%s\n", pErrorMessage, pFilledRequest);
                } else {
                    fprintf(stderr, "%s\n", pErrorMessage);
                }
                errorCode = 12;
            }
            signingParameters[iSHA256] = pSHA256;
        }
        if (!errorCode) {
            pErrorMessage = librock_awsFillAndSign(
                               pFilledRequest
                               , (const char **) signingParameters
                               , write_to_FILE, stdout
                               , bVerbose ? write_to_FILE : 0, stderr);
            if (pErrorMessage) {
                fprintf(stderr, "%s\n", pErrorMessage);
                errorCode = 7;
            }
        }
        freeOnce((void **)&pSHA256);
    }
    
    /* Clean up */
    if (ppNamedArguments) {
        int i = 0;
        while(ppNamedArguments[i]) {
             freeOnce( (void * *) & (ppNamedArguments[i]));
             i++;
        }
        freeOnce((void **)&ppNamedArguments);
    }

    freeOnce((void **)&pFilledRequest);
    freeOnce((void **)&pRequestTemplate);
    return errorCode;
}
#endif

/**************************************************************/
//[[Utility Functions in this module.]]

    PRIVATE int qsort_strcasecmp_(const void *pString1, const void *pString2) {
        return strcasecmp(*((char **)pString1), *((char **)pString2));
    }

    PRIVATE int qsort_strcmp_(const void *s1, const void *s2) {
        return strcmp(*((char **)s1), *((char **)s2));
    }

    PRIVATE int countToStr(const char *pRead, const char *sToFind)
    {
        const char *pFind = strstr(pRead, sToFind);
        if (!pFind) {
            return strlen(pRead);
        }
        return pFind - pRead;
    }
    PRIVATE int countToEol(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead && *pRead != '\n' && *pRead != '\r') {
            pRead++;
        }
        return pRead - pStart;
    }
    PRIVATE int countLWS(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead == ' '||*pRead == '\t') {
            pRead++;
        }
        return pRead - pStart;
    }

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
        if (countOptionName(pRead) != length) {
            return 0;
        }
        return strncmp(pRead, pToFind, length) == 0;
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
#if 0 
//This works, but this API requires three corresponding parameters,
//which is an error risk.  It is better to group corresponding parameters
//into a struct. Use librock_safeAppend0().

    PRIVATE char *librock_memappend0_s(char *base, int cb, char **ppWrite, const char *pSource, int cSource)
    { /* Bounds-checked memmove into a buffer. If out of bounds, do nothing and return 0.
         Otherwise: memmove to *ppWrite, follow with a terminating '\0', and update the write position 
       */
        if (cSource == -1) {
            cSource = strlen(pSource);
        }
        if (*ppWrite < base) { /* pWrite is supposed to be within base+cb */
            return 0;
        }
        if (*ppWrite - base + cSource > cb-1) { /* Will not fit with null terminator */
            return 0;
        }
        memmove(*ppWrite, pSource, cSource);
        *ppWrite += cSource;
        *((char *) *ppWrite) = '\0';
        return *ppWrite - cSource;
    }
#endif
    PRIVATE char *librock_appendableSet(struct librock_appendable *pAppendable,
            void *p,
            int cb,
            void *(*realloc)(void *, size_t)
            ) {
         pAppendable->p = p;
         pAppendable->cb = cb;
         pAppendable->iWriting = 0;
         pAppendable->realloc = realloc;
         if (realloc && !p) {
            pAppendable->p = (*(pAppendable->realloc))(0, cb);
         }
         return pAppendable->p;
    }
    
    PRIVATE char *librock_safeAppend0(
                    struct librock_appendable *pAppendable,
                    const char *pSource,
                    int cSource)
    { /* If cSource == -1, set cSource = strlen(pSource).
         If pAppendable->p is 0, increment pAppendable->cb by cSource.
         Otherwise do bounds-checked memmove into a buffer. 
            If would be out of bounds, do as much as possible and return 0.
            Otherwise: memmove into buffer at the write position,
            follow with a terminating '\0', and
            update the write position.
            Return the pointer to first character written into the buffer.
       */
#if defined librock_WANT_ALTERNATE_BRANCHING
        static long iFaultInjection;
        if (librock_triggerAlternateBranch("librock_safeAppend0", &iFaultInjection)) {
            return 0;
        }
#endif
        if (cSource == -1 && pSource) {
            cSource = strlen(pSource);
        }
        if (cSource < 0) {
            return 0; // Invalid parameter.
        }
        if (! (pAppendable->p)) { // Calculate size only
            pAppendable->cb += cSource;
            return (char *) 1;
        }
        if (pAppendable->iWriting < 0) { /* validate */
            return 0; // Caller incorrectly set iWriting.
        }
        if (pAppendable->iWriting + cSource + 1 > pAppendable->cb) {
            /* Will not fit with \0 terminator */

            /* TODO: This implementation does not support realloc,
               so limit what we copy.
            */
            if (pSource && pAppendable->cb - pAppendable->iWriting - 1 > 0) {
                memmove((char *)pAppendable->p + pAppendable->iWriting, pSource, pAppendable->cb - pAppendable->iWriting - 1);
            }
            pAppendable->iWriting = pAppendable->cb; /* Indicate error */
            ((char *) pAppendable->p)[pAppendable->iWriting-1] = '\0';
            return 0;
        }
        if (pSource) {
            memmove((char *)pAppendable->p + pAppendable->iWriting, pSource, cSource);
        } else { /* caller will write */
            *((char *)pAppendable->p + pAppendable->iWriting) = '\0';
        }
        pAppendable->iWriting += cSource;
        ((char *) pAppendable->p)[pAppendable->iWriting] = '\0';
        
        return (char *)pAppendable->p + pAppendable->iWriting - cSource;
    }
#if 0 //Works, but not used.
    PRIVATE char *librock_safeAppendEnv0(struct librock_appendable *pAppendable, const char *pName)
    {
        size_t ret;
#if defined LIBROCK_WANT_GETENV_S_FOR_MSC_VER
        if (GLOBAL_ALTERNATE_BRANCH) {
            return 0;
        }
        if (! (pAppendable->p)) { // Calculate size only
            getenv_s(&ret, 0, 0, pName);
            pAppendable->cb += ret;
            return (char *) 1;
        }
        if (getenv_s(&ret, pAppendable->p + pAppendable->iWriting,
                    pAppendable->cb - pAppendable->iWriting, pName)) {
            // Would overflow
            pAppendable->iWriting = pAppendable->cb - 1;
            return 0;
        }
        if (!ret) {
            /* Environment variable not found */
            ((char *)pAppendable->p)[pAppendable->iWriting] = '\0';
            return pAppendable->p + pAppendable->iWriting;
        }
        pAppendable->iWriting += ret;
        return pAppendable->p + pAppendable->iWriting;
#else
        char *pRead = getenv(pName);
        int length;
        if (GLOBAL_ALTERNATE_BRANCH) {
            return 0;
        }
        if (! (pAppendable->p)) { // Calculate size only
            ret = pRead ? strlen(pRead) : 0;
            pAppendable->cb += ret;
            return (char *) 1;
        }
        if (!pRead) {
            /* Environment variable not found */
            ((char *)pAppendable->p)[pAppendable->iWriting] = '\0';
            return pAppendable->p + pAppendable->iWriting;
        }
        length = strlen(pRead);
        if (! (pAppendable->p)) { // Calculate size only
            pAppendable->cb += length;
            return (char *) 1;
        }
        if (pAppendable->iWriting > pAppendable->cb - length - 1) {
            length = pAppendable->cb - pAppendable->iWriting - 1;
        }
        memmove(pAppendable->p + pAppendable->iWriting, pRead, length);
        pAppendable->iWriting += length;
        ((char *)pAppendable->p)[pAppendable->iWriting] = '\0';
        if (pAppendable->iWriting + 1 >= pAppendable->cb) {
            return 0;
        }
        return pAppendable->p + pAppendable->iWriting;//No Error
#endif
        
    }
#endif

    PRIVATE void freeOnce(void **p)
    {
        if (*p) {
            free(*p);
            *p = 0;
        }
    }
    
    PRIVATE void bToHex0(char *pWrite, unsigned char ch)
    {
        char *digits = "0123456789abcdef";
        *pWrite = digits[(ch>>4)&0x0f];
        pWrite[1] = digits[(ch)&0x0f];
        pWrite[2] = '\0';
    }

    PRIVATE void bToUHex0(char *pWrite, unsigned char ch)
    {
        char *digits = "0123456789ABCDEF";
        *pWrite = digits[(ch>>4)&0x0f];
        pWrite[1] = digits[(ch)&0x0f];
        pWrite[2] = '\0';
    }

PRIVATE char *librock_safeAppendUrlEncoded0(struct librock_appendable *pAppendable, const char *pSource, int cSource)
{  /* If pAppendable->p is 0, increment pAppendable->cb for what is needed
      to URL-encode cSource bytes of pSource.

      Otherwise, bounds-checked URL-encode into a buffer, follow with a terminating
      '\0', update the write position, and return a pointer to the first
      character written. 
      
      If the buffer would overflow, set the write position to the last location, and return 0.
      
    */
    const char *pRead = pSource;
    const char *pStart = pRead;
    int iStart = pAppendable->iWriting;
    if (cSource == -1) {
        cSource = strlen(pSource);
    }
    while(pRead < pSource + cSource) {
        char ch = *pRead;
        if ((ch >= '0' && ch <= '9')
            ||(ch == '.')
            ||(ch >= 'A' && ch <= 'Z')
            ||(ch >= 'a' && ch <= 'z')
            ||(ch == '-')
            ||(ch == '_')
            ||(ch == '~')) {
            /* No URL-encoding required */
            pRead++;
        } else {
            /* Need to encode */
            
            /* First, catch up on non-encoded characters */
            if (pRead > pStart) {
                if (!librock_safeAppend0(pAppendable, pStart, pRead - pStart)) {
                    return 0;
                }
            }
            
            /* Ensure room */
            if (!librock_safeAppend0(pAppendable, "%XX", 3)) {
                return 0;
            }
            if (pAppendable->p) {
                bToUHex0((char *) pAppendable->p + pAppendable->iWriting-2, ch & 0xff);
            }
            pRead++;
            pStart = pRead;
        }
    }
    if (pRead > pStart) {
        if (!librock_safeAppend0(pAppendable, pStart, pRead - pStart)) {
            return 0;
        }
    }
    if (! (pAppendable->p)) { // Calculate size only
        return (char *) 1;
    }

    return (char *)pAppendable->p + iStart;
}

PRIVATE char *librock_safeAppendBase64Encoded0(struct librock_appendable *pAppendable, const char *pSource, int cSource)
{  /* If pAppendable->p is 0, increment pAppendable->cb for what is needed
      to URL-encode cSource bytes of pSource.

      Otherwise, bounds-checked URL-encode into a buffer, follow with a terminating
      '\0', update the write position, and return a pointer to the first
      character written. 
      
      If the buffer would overflow, set the write position to the last location, and return 0.
      
    */
    const char *pRead = pSource;
    long accumulator = 1;
    char *pWrite = 0;
    int cPad = 0;
    int iStart = pAppendable->iWriting;
    const char *vector = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

    if (cSource == -1) {
        cSource = strlen(pSource);
    }
    while(1) {
        if (pRead < pSource + cSource) {
            char ch = *pRead++;
            accumulator <<= 8;
            accumulator |= ch & 0xff;
        } else if (accumulator == 1) {
            break;
        } else {
            while (accumulator < 0x01000000) {
                accumulator <<= 8;
                cPad++;
            }
        }
        if (accumulator & 0x01000000) {
            pWrite = librock_safeAppend0(pAppendable, "----", 4);
            if (!pWrite) {
                return 0;
            }
            if (pAppendable->p) {
                pWrite[3] = vector[accumulator&0x3f];
                accumulator >>= 6;
                pWrite[2] = vector[accumulator&0x3f];
                accumulator >>= 6;
                pWrite[1] = vector[accumulator&0x3f];
                accumulator >>= 6;
                pWrite[0] = vector[accumulator&0x3f];
                pWrite += 4;
            }
            accumulator = 1;
        }
    }
    if (! (pAppendable->p)) { // Calculate size only
        return (char *) 1;
    }
    if (cSource) {
        *pWrite = '\0';
        while(cPad > 0) {
            pWrite[0-cPad] = '=';
            cPad--;
        }
    } else {
        librock_safeAppend0(pAppendable, "", 0);
    }

    return (char *)pAppendable->p + iStart;
} /* librock_safeAppendBase64Encoded0 */

#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
PRIVATE char *mallocNamedValue(const char *pName)
{
    char *pValue = 0;
    int cNeeded = 0;
#if defined LIBROCK_WANT_GETENV_S_FOR_MSC_VER
    getenv_s(&cNeeded, 0, 0, pName);
    if (cNeeded) {
        /* Found */
        pValue = malloc(cNeeded+1);
        if (pValue) {
            if (getenv_s(&cNeeded, pValue, cNeeded+1, pName)) {
                freeOnce((void **) &pValue);
                return 0;
            }
        }
    }
#else
    pValue = getenv(pName);
    if (pValue) {
        cNeeded = strlen(pValue)+1;
        pValue = malloc(cNeeded+1);
        if (!pValue) {
            return 0;
        }
        memmove(pValue, getenv(pName), cNeeded+1);
    }
#endif
    return pValue;
}
#endif
PRIVATE const char *librock_fillTemplateTokenize(
        const char *pTemplate,
        int iOffset,
        int *pCount
        )
{ //DEBUG: reuse
    const char *pRead = pTemplate + iOffset;
    const char *pNext = memchr(pRead,'@',*pCount);
    if (GLOBAL_ALTERNATE_BRANCH) {
        return "999-error";
    }
    if (!pNext) {
        return "0-body";
    }
    if (pNext > pRead) {
        *pCount = pNext - pRead;
        return "0-body";
    }
    /* We start at '@' */
    if (*pCount >=2 && pRead[1] == '/' && pRead[2] == '/') {//  '@//' is comment to end of line.
        pNext = memchr(pRead,'\n',*pCount);
        if (pNext) {
            if (pRead == pTemplate || pRead[-1] == '\n') {
                //Entire line is comment. Remove it.
                *pCount = pNext - pRead + 1;
            } else {
                *pCount = pNext - pRead;
            }
        }
        return "1-comment";
    }
    pNext = memchr(pRead+1,'@',*pCount-1);
    if (!pNext) {
        return "2-Error-non-closing";
    }
    *pCount = pNext+1 - pRead;
    return "3-parameter";
} /* librock_fillTemplateTokenize */

PRIVATE int needEncoding(const char *pValue)
{
    const char *pRead = strchr(pValue,'%');
    const char *valid = "0123456789ABCDEFabcdef";
    if (!pRead) {
        /* Doesn't look URL-encoded already. Encode it */
        return 1;
    }
    if (!strchr(valid,pRead[1])) {
        /* Definitely not URL-encoded already. Encode it. */
        return 1;
    }
    if (!strchr(valid,pRead[2])) {
        /* Definitely not URL-encoded already. Encode it. */
        return 1;
    }
    /* Presume already encoded properly */
    return 0;
}

PRIVATE const char *librock_fillTemplate(
                    char **ppFilled,
                    const char *pTemplate,
                    int iEncodeParameters,
                    char * * namedArguments,
                    int argc, char * const * const argv,int *pErrorIndex)
{ //DEBUG: reuse
    int pass; //two passes.
    struct librock_appendable aFilled;
    int dummy = 0;
    int cTemplate = strlen(pTemplate);
    if (!pErrorIndex) {
        pErrorIndex = &dummy;
    }
    librock_appendableSet(&aFilled, 0, 0, 0); /* First pass: just count */
    for (pass = 0;pass < 2;pass++) {
        /* First pass we determine necessary size */
        const char *pRead = pTemplate;
        const char *pErrorMessage = 0;
        /* if pass==0, aFilled.p is 0, and .cb is being incremented */
        if (pass == 1) { // Second pass.
            aFilled.cb += 1;
            aFilled.p = malloc(aFilled.cb);
            if (!aFilled.p) {
                *pErrorIndex = 0;
                return "E-1144 malloc failed";
            }
        }
        while (*pRead) {
            /* Work in segments */
            int cToken;
            const char *pTokenType;
            int iTokenType;
            cToken = pTemplate + cTemplate - pRead;
            
            pTokenType = librock_fillTemplateTokenize(pTemplate, pRead - pTemplate, &cToken);
            iTokenType = atoi(pTokenType);
            
            if (iTokenType == 0) {
                /* Append rest */
                if (!librock_safeAppend0(&aFilled, pRead, cToken)) {
                    pErrorMessage = "E-2049 would overflow pre-allocated block";
                    break;
                }
            } else if (iTokenType == 1) {
                /* Comment */
            } else if (iTokenType == 2) {
                /* Error: non-closing */
                pErrorMessage = "E-1153 unmatched @ in template";
                break;
            } else if (iTokenType == 3 && cToken==2) { /* Escaped @@ */
                if (!librock_safeAppend0(&aFilled, "@", 1)) {
                    pErrorMessage = "E-2049 would overflow pre-allocated block";
                    break;
                }
            } else if (iTokenType == 3) {
                /* Parameter */
                const char *pValue = 0;
                int iEncodeThis = iEncodeParameters;
                
                if (pRead[1] == 'p') {
                    pRead++;
                    cToken--;
                    iEncodeThis = -1;
                }
                if (pRead[1] == 'e') {// environment variable
                    int iString;
                    pRead += 2;
                    cToken -= 2;

                    /* Copy the name */
                    iString = librock_stringListGetIndex(&namedArguments, 2, pRead, cToken-1);
                    if (GLOBAL_ALTERNATE_BRANCH) {
                        iString = -1;
                    }
                    if (iString < 0) {
                        pErrorMessage = "E-2558 malloc failed";
                    } else if (namedArguments[iString] == 0) {
                        pErrorMessage = "E-2571 named value not found in environment";
                    } else {
                        pValue = namedArguments[iString+1];
                    }
                } else { /* Numeric parameter */
                    int iParameter;
                    pRead++;
                    cToken--;
                    if (!strchr("0123456789", *pRead)) {
                        pErrorMessage = "E-1811 non-numeric replaceable @parameter@ in template";
                    } else {
                        iParameter = atoi(pRead);
                        if (iParameter >= argc) {
                            pErrorMessage = "E-1157 parameter index out of range";
                        } else {
                            pValue = argv[iParameter];
                        }
                    }
                    
                }
                if (!pErrorMessage) {
                    if (iEncodeThis==1 
                        || (iEncodeThis == 0 && needEncoding(pValue)) ) {
                        if (!librock_safeAppendUrlEncoded0(&aFilled, pValue, strlen(pValue))) {
                            pErrorMessage = "E-2049 would overflow pre-allocated block";
                        }
                    } else {
                        if (!librock_safeAppend0(&aFilled, pValue, strlen(pValue))) {
                            pErrorMessage = "E-2049 would overflow pre-allocated block";
                        }
                    }
                }

            } else {
                pErrorMessage = "E-2512 unrecognized template syntax";
            }
            if (pErrorMessage) {
                break;
            }
            pRead += cToken;
        } /* while parse template */
        if (pErrorMessage) {
            *pErrorIndex = pRead - pTemplate;
            freeOnce(&(aFilled.p));
            return pErrorMessage;
        }
    } /* two passes */
    *ppFilled = aFilled.p;
    aFilled.p = 0; // Caller will free
    return 0;
} /* librock_fillTemplate */


#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
    PRIVATE void *librock_fileGetContents(const char *fileName)
    {
        int fd;
        off_t fileLength;
        void *pContents;
        int cRead;
        fd = librock_fdOpenReadOnly(fileName);
        if (fd == -1) {
            return 0;
        }
        fileLength = librock_fdSeek(fd, 0, SEEK_END);
        librock_fdSeek(fd, 0, SEEK_SET);
        pContents = malloc(fileLength+1);
        if (!pContents) {
            librock_fdClose(fd);
            return 0;
        }
        cRead = librock_fdRead(fd, pContents, fileLength);
        if (GLOBAL_ALTERNATE_BRANCH) {
            cRead = 0;
        }
        if (cRead != fileLength) {
            librock_fdClose(fd);
            freeOnce((void **) &pContents);
            return 0;
        }
        ((char *)pContents)[fileLength] = '\0';
        librock_fdClose(fd);
        return pContents;

    } /* librock_fileGetContents */
#endif
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN) || defined(LIBROCK_WANT_fileSha256Contents)
    PRIVATE const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32)
    {
        int fd;
        int cRead;
        char buf[16384];
        void *pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
        if (!pHashInfo) {
            return "E-850 malloc failed";
        }
        fd = librock_fdOpenReadOnly(fname);
        if (fd == -1) {
            freeOnce((void **)&pHashInfo);
            return "E-1821 could not open file";
        }
        librock_sha256Init(pHashInfo);
        while ((cRead = librock_fdRead(fd, buf, sizeof(buf))) > 0) {
            librock_sha256Update(pHashInfo, (unsigned char *) buf,  cRead);
        }
        librock_sha256StoreFinal(mdBuffer32, pHashInfo);
        freeOnce((void **)&pHashInfo);
        librock_fdClose(fd);
        return 0;
    } /* fileSha256Contents */
#endif
    
//gcc -o awsFillAndSign -Dlibrock_WANT_ALTERNATE_BRANCHING -fprofile-arcs -ftest-coverage -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN -Werror -Wall awsFillAndSign.c hmacsha256.c librock_sha256.c
#if defined librock_WANT_ALTERNATE_BRANCHING
#include "tests/awsFillAndSignCoverage.c"
#else
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
int librock_triggerAlternateBranch(const char *name, long *pLong)
{
    /* Cases:
           Coverage support not compiled in.
           Coverage support compiled in, no named test.
           Never reaches named test
           Never reaches numbered test
           Reached and need to repeat
     */
    if (!name) { /* Call once this way, for the whole application */
        remove("librock_armAlternateBranch_next.txt");
        rename("librock_armAlternateBranch.txt", "librock_armAlternateBranch_next.txt");
    }
    if (pLong) {
        *pLong = -1;
    }
    return 0;
}
#endif
#endif

#endif //LIBROCK_UNSTABLE

