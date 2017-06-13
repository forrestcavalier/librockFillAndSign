/* u-librock/mit/appendable.c
    Copyright 2017 MIB SOFTWARE, INC.
   Licensed under the terms of the MIT License (Free/OpenSource, No Warranty)
   (See the LICENSE file in the source.)

 PURPOSE:   Safe string buffer append.

            - Superior API compared to strcat_s, strcpy_s.
            - Two-pass ("counter") mode: counts size.
            - Binary and text compatible.
            - Caller-write mode

 SOURCE:    https://github.com/forrestcavalier/awsFillAndSign/            
 
 QUALITY:   librock_STABLE:   as of 2017-04-18.
            
            librock_PORTABLE: gcc/MSVC/clang/BSD/Linux/Windows.
                              Supports unity build (single compilation unit.)
            
            librock_COVERAGE: https://codecov.io/gh/forrestcavalier/awsFillAndSign
            
            Report bugs at github. See below for help.
            
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

*******************************************************************************/
/*
*******************************************************************************
TABLE OF CONTENTS FOR THE REST OF THIS FILE
*******************************************************************************
    [[Design]]
    [[Typical use]]
    [[Variant use]]
    [[Implementation]]

[[Design and Comparison]]]
    This is a replacement for unsafe (and indeterminant!) <string.h> operations
        strcat(buffer, source);
        strncat(buffer, source, sourceLength);
    
    This is a replacement for safer, but still unsafe (and indeterminant!, and
    risky!) strcat_s and strncat_s, which have undefined behavior if would
    overflow, have no way to determine if an intermediate operation would
    have overflowed, but the last one didn't, and risk mismatches of buffer
    and size parameters.  

    This alternative: safe, convenient, less error-prone, binary-data
    compatible, caller-write mode. Defined error behavior. Always appends \0.
        librock_safeAppend0(buffer, source, source Length);
    
    See variants (below Typical Use), for Base64 and URL encoding.
    
[[Typical use]]

*/
#if defined(librock_TYPICAL_USE)
    #include "librock/appendable.h"
    
    // Declare a struct librock_appendable_s.
    struct librock_appendable_s appendable;

    // Allocate a buffer (or use a fixed buffer.)
    char buffer[100];

    // Associate a pointer and the buffer size.
    librock_appendable_set(&appendable, buffer, sizeof(buffer));
    
    // Make zero or more calls to librock_safeAppend0(). It always
    // stores a terminating \0, and returns 0 if it would overflow.
    librock_safeAppend0(&appendable, "Testing", -1/* -1 will call strlen()*/);
    librock_safeAppend0(&appendable, "\n", 1);

    // Check for overflow errors: (either as you go or at the end before 
    // using the buffer.)
    if (appendable.iWriting == appendable.cb) {
        /* Some operation would have overflowed */
        handler
    } else {
        /* Use buffer or appendable.p */
    }
#endif
/* 
[[Variant use]]
    Binary-data compatible:
        memmove() is used to copy data from source, and a write
        pointer is kept in the structure.  This permits
        source data to contain embedded \0.
    
    Caller-write mode:
        Call librock_safeAppend0() with a null source pointer. if space
        is available, the return value is a non-null pointer into the
        buffer, the caller can write data there.

    Counter mode:
        Set a NULL pointer and 0 buffer size, and then the calls will
        increment the buffer size.  This can be used to implement a
        "two-pass" method, which dynamically calculates on the first
        pass.  Dynamically allocate .cb+1, set the pointer and buffer
        size again, and then do the second pass to modify the buffer.

    Url-encoding:
        char *librock_safeAppendUrlEncoded0(
            struct librock_appendable *pAppendable,
            const char *pSource,
            int cSource);

    Base64 encoding:
        char *librock_safeAppendBase64Encoded0(
            struct librock_appendable *pAppendable, 
            const unsigned char *pSource,
            int cSource);

*/

/*[[Implementation]]*/
#if !defined(PRIVATE)
#define PRIVATE static
#endif

#include "appendable.h"
#if defined librock_WANT_BRANCH_COVERAGE
int librock_triggerAlternateBranch(const char *name, long *pLong);
#endif

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
    char *librock_appendableSet(struct librock_appendable *pAppendable,
            void *p,
            int cb
            ) {
         pAppendable->p = p;
         pAppendable->cb = cb;
         pAppendable->iWriting = 0;
         return pAppendable->p;
    }
    
    char *librock_safeAppend0(
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
#if defined librock_WANT_BRANCH_COVERAGE
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

            if (pSource && pAppendable->cb - pAppendable->iWriting - 1 > 0) {
                memmove((char *)pAppendable->p + pAppendable->iWriting, pSource, pAppendable->cb - pAppendable->iWriting - 1);
            }
            pAppendable->iWriting = pAppendable->cb; /* Indicate error */
            ((char *) pAppendable->p)[pAppendable->iWriting-1] = '\0';
            return 0;
        } else {
            if (pSource) {
                memmove((char *)pAppendable->p + pAppendable->iWriting, pSource, cSource);
            } else { /* caller will write */
                *((char *)pAppendable->p + pAppendable->iWriting) = '\0';
            }
            pAppendable->iWriting += cSource;
            ((char *) pAppendable->p)[pAppendable->iWriting] = '\0';
            
            return (char *)pAppendable->p + pAppendable->iWriting - cSource;
        }
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


char *librock_safeAppendUrlEncoded0(struct librock_appendable *pAppendable, const char *pSource, int cSource)
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
                char *digits = "0123456789ABCDEF";
                char *pWrite = (char *) pAppendable->p + pAppendable->iWriting-2;
                
                *pWrite = digits[(ch>>4)&0x0f];
                pWrite[1] = digits[(ch)&0x0f];
                pWrite[2] = '\0';
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

char *librock_safeAppendBase64Encoded0(struct librock_appendable *pAppendable, const unsigned char *pSource, int cSource)
{  /* If pAppendable->p is 0, increment pAppendable->cb for what is needed
      to URL-encode cSource bytes of pSource.

      Otherwise, bounds-checked URL-encode into a buffer, follow with a terminating
      '\0', update the write position, and return a pointer to the first
      character written. 
      
      If the buffer would overflow, set the write position to the last location, and return 0.
      
    */
    const unsigned char *pRead = pSource;
    long accumulator = 1;
    char *pWrite = 0;
    int cPad = 0;
    int iStart = pAppendable->iWriting;
    const char *vector = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

    if (cSource == -1) {
        cSource = strlen((const char *) pSource);
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
