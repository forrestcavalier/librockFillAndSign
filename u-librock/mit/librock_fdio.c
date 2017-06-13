/* u-librock/mit/librock_fdio.c
    Copyright 2017 MIB SOFTWARE, INC.
   Licensed under the terms of the MIT License (Free/OpenSource, No Warranty)
   (See the LICENSE file in the source.)

 PURPOSE:   Compatibility and portability shim for fd file io on Windows / *nix.

 SOURCE:    https://github.com/forrestcavalier/awsFillAndSign/            
 
 QUALITY:   librock_STABLE:   as of 2017-04-18.
            
            librock_PORTABLE: gcc/MSVC/clang/BSD/Linux/Windows.
                              Supports unity build (single compilation unit.)
            
            librock_COVERAGE: https://codecov.io/gh/forrestcavalier/awsFillAndSign
            
            Report bugs at github. See below for help.
            
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

*******************************************************************************/


#if !defined LIBROCK_FDIO_DEFINED
#define LIBROCK_FDIO_DEFINED 1
#  if defined __MINGW32__  && defined __MSVCRT__
#	include <io.h>		// _sopen_s
#   include <stdio.h> //Seek constants
#   include <fcntl.h>

#	define librock_fdOpenReadOnly(fname) open(fname, O_RDONLY|O_BINARY)
#	define librock_fdSeek _lseek
#	define librock_fdRead _read
#	define librock_fdClose _close
#  elif defined _MSC_VER
#	include <io.h>		// _sopen_s
#	include <share.h>		// SH_DENYNO
	int librock_fdOpenReadOnly(const char *fname)
	{
			int fd;
			_sopen_s(&fd, fname, _O_RDONLY|_O_BINARY, _SH_DENYNO, 0/*ignored when not O_CREAT */);
			return fd;	
	}
#	define librock_fdSeek _lseek
#	define librock_fdRead _read
#	define librock_fdClose _close
#  else
#	include <unistd.h> //open, etc.
#	define librock_fdOpenReadOnly(fname) open(fname, O_RDONLY)
#	define librock_fdRead read
#	define librock_fdClose close
#	define librock_fdSeek lseek
#  endif
#endif
#if defined(librock_WANT_BRANCH_COVERAGE)
extern long librock_global_iAlternateBranch;
int librock_triggerAlternateBranch(const char *name, long *pLong);
#define GLOBAL_ALTERNATE_BRANCH librock_triggerAlternateBranch("global", &librock_global_iAlternateBranch)
#endif

#if defined(LIBROCK_WANT_fileGetContents)
#include <stdlib.h>

    void *librock_fileGetContents(const char *fileName, off_t *returnLength)
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
#if defined(librock_WANT_BRANCH_COVERAGE)
        if (GLOBAL_ALTERNATE_BRANCH) {
            cRead = 0;
        }
#endif
        if (cRead != fileLength) {
            librock_fdClose(fd);
            free(pContents);
            pContents = 0;
            return 0;
        }
        ((char *)pContents)[fileLength] = '\0';
        librock_fdClose(fd);
        if (returnLength) {
            *returnLength = fileLength;
        }
        return pContents;

    } /* librock_fileGetContents */
#endif
#if defined(LIBROCK_WANT_fileSha256Contents)
struct librock_SHA256_CTX;
    extern int librock_sha256Init(struct librock_SHA256_CTX *c);
    extern int librock_sha256Update(struct librock_SHA256_CTX *c, const void *data_, int len);
    extern int librock_sha256StoreFinal(unsigned char *md, struct librock_SHA256_CTX *c);

    const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32, unsigned long *contentLength)
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
            free(pHashInfo);
            return "E-1821 could not open file";
        }
        librock_sha256Init(pHashInfo);
        *contentLength = 0;
        while ((cRead = librock_fdRead(fd, buf, sizeof(buf))) > 0) {
            librock_sha256Update(pHashInfo, (unsigned char *) buf,  cRead);
            *contentLength += cRead;
        }
        librock_sha256StoreFinal(mdBuffer32, pHashInfo);
        free(pHashInfo);
        librock_fdClose(fd);
        return 0;
    } /* fileSha256Contents */
#endif
