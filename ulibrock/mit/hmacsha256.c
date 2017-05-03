/* hmacsha256.c Copyright 2016 MIB SOFTWARE, INC.
Part of libROCK by MIB SOFTWARE:
- MIT License
- High-Quality. Highly portable. Compiles on gcc/MSVC/Clang/Windows/Linux/BSD/more.
- Global names start "librock_", for compatibility.

*******************************************************************************
[[The MIT License]]
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

*******************************************************************************
TABLE OF CONTENTS FOR THE REST OF THIS FILE
*******************************************************************************
    [[Header include and compatibility]]
        #include "librock_preinclude.h"
        #include <stdlib.h>, <string.h>, <stdint.h>
        #if defined LIBROCK_WANT_INCLUDE_PTHREAD: <pthread.h>
        #if defined(LIBROCK_AWSREQUEST4_MAIN): <stdio.h>, <fcntl.h>, "librock_fdio.h"
        #include "librock_postinclude.h"
        
    [[Predeclarations of functions from librock_sha256.c]]
        librock_sha256Init, librock_sha256Update, librock_sha256StoreFinal
    
    [[Implementation]]
    
        function librock_awsSignatureVersion4  The Workhorse
        
    [[PRIVATE implementation functions]]
        
    [[PRIVATE Utility Functions]]
        

*/

#include "librock_preinclude.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "librock_postinclude.h"

//[[Predeclarations of functions from librock_sha256.c]]
struct librock_SHA256_CTX;
 	int librock_sha256Init(struct librock_SHA256_CTX *c);
	int librock_sha256Update(struct librock_SHA256_CTX *c, const void *data_, int len);
	int librock_sha256StoreFinal(unsigned char *md, struct librock_SHA256_CTX *c);

//[[Implementation]]

void librock_hmacSha256(unsigned char *digest,const char *key,int cbKey,const char *toSign,int cbToSign)
{
    char keyBuffer[64];
    char padBuffer[64];
    char context[13*8+64]; /* includes extra room */
    unsigned int i = 0;

//    void *pContext = malloc(librock_sha256Init(0));
    void *pContext = &context[0]; /* avoid malloc, so we never have an error to return */
    librock_sha256Init(pContext);

    memset(keyBuffer,'\0',sizeof(keyBuffer));
    if (cbKey > 64) {
        librock_sha256Update(pContext,(unsigned char *) key, cbKey);
        librock_sha256StoreFinal((unsigned char *)keyBuffer, pContext);
    } else {
        memcpy(keyBuffer, key, cbKey);
    }
    librock_sha256Init(pContext);
    for(i = 0;i < sizeof(padBuffer);i++) {
        padBuffer[i] = keyBuffer[i] ^ 0x36; /* XOR ipad */
    }

    librock_sha256Update(pContext,(const unsigned char *)padBuffer,sizeof(padBuffer));
#if (0) //Debugging
    printf("ipad:");
    for(i = 0; i < 64;i++) {
        printf("%02x",padBuffer[i] & 0xff);
    }
    printf("\n");
#endif
    librock_sha256Update(pContext,(const unsigned char *)toSign,cbToSign);
    librock_sha256StoreFinal((unsigned char *)digest,pContext);
#if (0) //Debugging
    for(i = 0; i < 20;i++) {
        printf("%02x",digest[i] & 0xff);
    }
    printf("\n");
#endif
    for(i = 0;i < sizeof(keyBuffer);i++) {
        keyBuffer[i] = keyBuffer[i] ^ 0x5c; /* XOR opad */
    }
    librock_sha256Init(pContext);
    librock_sha256Update(pContext,(const unsigned char *)keyBuffer,sizeof(keyBuffer));
    librock_sha256Update(pContext,(const unsigned char *)digest,32); /* SHA256 */
    librock_sha256StoreFinal((unsigned char *)digest,pContext);
//	free(pContext);

} /* librock_hmacSha256 */

#if (0) /*typical use */
#include <stdio.h>
int main()
{
	unsigned char digest[32];
	int i;
	const char *tosign = "The quick brown fox jumps over the lazy dog";
//	librock_hmacSha256(digest,"",0,"",0); /* Test. Expect 0xb613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad */
	librock_hmacSha256(digest,"key",3,tosign,strlen(tosign)); /* Test. Expect 0xf7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8 */

    printf("hmac-sha256:");
    for(i = 0; i < 32;i++) {
        printf("%02x",digest[i] & 0xff);
    }
    printf("\n");
	return 0;
}
#endif
