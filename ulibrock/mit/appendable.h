#if !defined(librock_appendable_H)
#define librock_appendable_H 1
#include <sys/types.h>
#include <string.h>


	struct librock_appendable {
        void *p;
        int cb;
        int iWriting;
        void *(*reallocfn)(void *, size_t);
    };
    char *librock_appendableSet(struct librock_appendable *pAppendable,
            void *p,
            int cb
//            void *(*reallocfn)(void *, size_t)
            );
    char *librock_safeAppend0(
                        struct librock_appendable *pAppendable,
                        const char *pSource,
                        int cSource);
#if 0 //Works but not used
    PRIVATE char *librock_safeAppendEnv0(
                        struct librock_appendable *pAppendable,
                        const char *pName);
#endif
    char *librock_safeAppendUrlEncoded0(
                        struct librock_appendable *pAppendable,
                        const char *pSource,
                        int cSource);
    char *librock_safeAppendBase64Encoded0(
                        struct librock_appendable *pAppendable,
                        const unsigned char *pSource,
                        int cSource);
#endif