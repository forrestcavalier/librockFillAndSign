 /* u-librock/mit/stringlist.c
    Copyright 2017 MIB SOFTWARE, INC.
   Licensed under the terms of the MIT License (Free/OpenSource, No Warranty)
   (See the LICENSE file in the source.)

 PURPOSE:   Simple expandable string list with linear search.

 SOURCE:    https://github.com/forrestcavalier/awsFillAndSign/            
 
 QUALITY:   librock_STABLE:   as of 2017-04-18.
            
            librock_PORTABLE: gcc/MSVC/clang/BSD/Linux/Windows.
                              Supports unity build (single compilation unit.)
            
            librock_COVERAGE: https://codecov.io/gh/forrestcavalier/awsFillAndSign
            
            Report bugs at github. See below for help.
            
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

*******************************************************************************/
#include <string.h>
#include <stdlib.h>

int librock_stringListGetIndex(char ***ppStrings, int cStep, const char *pName, int cName)
{
    
    int expansion = cStep * 32; /* 32 slots at a time */
    int iString = 0;

    /* Linear search. Is this already set? */
    while(*ppStrings && (*ppStrings)[iString]) {
        if (!strncmp(pName, (*ppStrings)[iString], cName) &&
            (*ppStrings)[iString][cName] == '\0') {
            /* Matched. It is already set. */
            return iString;
        }
        iString += cStep;
    }
    if (!*ppStrings) {
        iString = -1;
    }
    if (iString == -1 || iString % expansion == expansion-cStep) {
        /* Need to expand */
        char **ppExpanded;

        ppExpanded = realloc((void *)*ppStrings, sizeof(char *) * (iString+1+expansion));
        if (!ppExpanded) {
            return -1;
        }
        *ppStrings = ppExpanded;
        memset((void *) (*ppStrings + iString + 1), '\0', sizeof(char *) * expansion);
        if (iString == -1) {
            iString = 0;
        }
    }
    return iString;
} /* librock_stringListGetIndex */
