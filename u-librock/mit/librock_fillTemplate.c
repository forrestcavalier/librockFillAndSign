/* u-librock/mit/librock_fillTemplate.c
    Copyright 2017 MIB SOFTWARE, INC.
   Licensed under the terms of the MIT License (Free/OpenSource, No Warranty)
   (See the LICENSE file in the source.)

 PURPOSE:   Build a string from a template.

 SOURCE:    https://github.com/forrestcavalier/awsFillAndSign/            
 
 QUALITY:   librock_STABLE:   as of 2017-04-18.
            
            librock_PORTABLE: gcc/MSVC/clang/BSD/Linux/Windows.
                              Supports unity build (single compilation unit.)
            
            librock_COVERAGE: https://codecov.io/gh/forrestcavalier/awsFillAndSign
            
            Report bugs at github. See below for help.
            
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

*******************************************************************************/

#if !defined(PRIVATE)
#define PRIVATE static
int librock_stringListGetIndex(char ***ppStrings, int cStep, const char *pName, int cName);

#endif
#if defined(librock_WANT_BRANCH_COVERAGE)
extern long librock_global_iAlternateBranch;
int librock_triggerAlternateBranch(const char *name, long *pLong);
#define GLOBAL_ALTERNATE_BRANCH librock_triggerAlternateBranch("global", &librock_global_iAlternateBranch)
#endif

#include "appendable.h"
#include <stdlib.h>

PRIVATE int needEncoding(const char *pValue);

const char *librock_fillTemplateTokenize(
        const char *pTemplate,
        int iOffset,
        int *pCount
        )
{ 
    const char *pRead = pTemplate + iOffset;
    const char *pNext = memchr(pRead, '@', *pCount);
 #if defined(librock_WANT_BRANCH_COVERAGE)
    if (GLOBAL_ALTERNATE_BRANCH) {
        needEncoding("%");
        needEncoding("%0");
        needEncoding("%00");
        return "999-error";
    }
#endif
    if (!pNext) {
        return "0-body";
    }
    if (pNext > pRead) {
        *pCount = pNext - pRead;
        return "0-body";
    }
    /* We start at '@' */
    if (*pCount >=2 && pRead[1] == '/' && pRead[2] == '/') {//  '@//' is comment to end of line.
        pNext = memchr(pRead, '\n', *pCount);
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
    pNext = memchr(pRead+1, '@', *pCount-1);
    if (!pNext) {
        return "2-Error-non-closing";
    }
    *pCount = pNext+1 - pRead;
    return "3-parameter";
} /* librock_fillTemplateTokenize */

PRIVATE int needEncoding(const char *pValue)
{
    const char *pRead = strchr(pValue, '%');
    const char *valid = "0123456789ABCDEFabcdef";
    if (!pRead) {
        /* Doesn't look URL-encoded already. Encode it */
        return 1;
    }
    if (!strchr(valid, pRead[1])) {
        /* Definitely not URL-encoded already. Encode it. */
        return 1;
    }
    if (!strchr(valid, pRead[2])) {
        /* Definitely not URL-encoded already. Encode it. */
        return 1;
    }
    /* Presume already encoded properly */
    return 0;
}

const char *librock_fillTemplate(
                    char **ppFilled,
                    const char *pTemplate,
                    int iEncodeParameters,
                    char * * namedArguments,
                    int argc, char * const * const argv, int *pErrorIndex)
{ 
    int pass; //two passes.
    struct librock_appendable aFilled;
    int dummy = 0;
    int cTemplate = strlen(pTemplate);
    if (!pErrorIndex) {
        pErrorIndex = &dummy;
    }
    librock_appendableSet(&aFilled, 0, 0); /* First pass: just count */
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

                    iString = librock_stringListGetIndex(&namedArguments, 2, pRead, cToken-1);
 #if defined(librock_WANT_BRANCH_COVERAGE)
                    if (GLOBAL_ALTERNATE_BRANCH) {
                        iString = -1;
                    }
 #endif
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
            if (aFilled.p) {
                free(aFilled.p);
                aFilled.p = 0;
            }
            return pErrorMessage;
        }
    } /* two passes */
    *ppFilled = aFilled.p;
    aFilled.p = 0; // Caller will free
    return 0;
} /* librock_fillTemplate */
