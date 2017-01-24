#if defined librock_WANT_ALTERNATE_BRANCHING
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
        return 0;
    }
    
    FILE *f;
    if (*pLong == -1) { /* Not injecting */
        return 0;
    } else if (*pLong == 0) {
        /* Initialize */
        char buf[200];
        int length = strlen(name);
        f = fopen("librock_armAlternateBranch.txt","rb");
        if (!f && *pLong == 0) {
            /* First time hit will need to get it from _next */
            f = fopen("librock_armAlternateBranch_next.txt","rb");
        }
        if (!f) {
            *pLong = -1;
            return 0;
        }
        if (!fgets(buf, sizeof(buf), f)) {fclose(f);*pLong = -1;return 0;} //One line, to keep gcov counts accurate
        fclose(f);
        if (!strncmp(buf, name, length) && buf[length] == ' ') {
            /* Matched */
            if (*pLong == 0) {
                fprintf(stderr, "I-2339 read librock_armAlternateBranch.txt: %s\n", buf);
            }
            *pLong = atol(buf + length+1);
            remove("librock_armAlternateBranch.txt");
            f = fopen("librock_armAlternateBranch_next.txt","wb");
            if (!f) {perror("I-2316 librock_armAlternateBranch_next.txt");exit(-1);} //One line, to keep gcov counts accurate
            fputs(name, f);
            fputs(" ", f);
            snprintf(buf,sizeof(buf),"%ld",*pLong+1);
            fputs(buf, f);
            fclose(f);
        } else {
            *pLong = -1;
            return 0; /* Not injecting */
        }
    }
    if (*pLong == 1) {
        *pLong -= 3;
        rename("librock_armAlternateBranch_next.txt", "librock_armAlternateBranch.txt");
    } else {
        *pLong -= 1;
    }
    if (*pLong <= 0) {
        return 1;
    } else {
        return 0;
    }
    return 0;
    
} /* librock_triggerAlternateBranch */
time_t time( time_t *arg )
{
    time_t fixed = 0;
    if (arg) {
        *arg = fixed;
    }
    return fixed;
}
	
int librock_coverage_main()
{
    /* Compute hash */
    void *pHashInfo;
    const char *pString;
    unsigned char mdCanonicalRequest[32];
    int i;
    struct librock_appendable aTest;
    librock_appendableSet(&aTest, mdCanonicalRequest, sizeof(mdCanonicalRequest), 0);
    librock_safeAppend0(&aTest,"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 33);

    librock_appendableSet(&aTest, 0, 200, realloc);
    librock_appendableSet(&aTest, aTest.p, 200, realloc);
    
    pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
    if (pHashInfo) {
        librock_sha256Init(pHashInfo);
        librock_sha256Update(pHashInfo, (unsigned char *) "hello", -1);
        librock_sha256Update(pHashInfo, (unsigned char *) "hello", 5);
        pString = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        librock_sha256Update(pHashInfo, (unsigned char *) pString, strlen(pString));
        pString = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        librock_sha256Update(pHashInfo, (unsigned char *) pString, strlen(pString));
        librock_sha256StoreFinal(mdCanonicalRequest, pHashInfo);

        /* More than 55 bytes, less than 64 in last block */
        librock_sha256Init(pHashInfo);
        pString = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        librock_sha256Update(pHashInfo, (unsigned char *) pString, strlen(pString));
        librock_sha256StoreFinal(mdCanonicalRequest, pHashInfo);
        
        // go over 65536 byte length
        librock_sha256Init(pHashInfo);
        pString = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        for(i = 70000 / strlen(pString);i > 0;i--) {
            librock_sha256Update(pHashInfo, (unsigned char *) pString, strlen(pString));
        }
        librock_sha256StoreFinal(mdCanonicalRequest, pHashInfo);

        free(pHashInfo);
    }
    {
        const char *pSigningParameters[6];
        pSigningParameters[0] = 0;
        pSigningParameters[1] = 0;
        pSigningParameters[2] = 0;
        pSigningParameters[3] = 0;
        pSigningParameters[4] = 0;
        pSigningParameters[5] = 0;
        pString = librock_awsFillAndSign(
            0
            ,0
            ,0
            ,0
            ,0
            ,0
            );
        fprintf(stderr,"I-2393 %s\n", pString ? pString : "");

        pString = librock_awsFillAndSign(
            "request"
            ,0
            ,0
            ,0
            ,0
            ,0
            );
        fprintf(stderr,"I-2402 %s\n", pString ? pString : "");

        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            ,0
            ,0
            ,0
            ,0
            );
        fprintf(stderr,"I-2402 %s\n", pString ? pString : "");

        pSigningParameters[1] = "test1";
        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            ,0
            ,0
            ,0
            ,0
            );
        fprintf(stderr,"I-2402 %s\n", pString ? pString : "");
        pSigningParameters[2] = "test2";
        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            ,0
            ,0
            ,0
            ,0
            );
        fprintf(stderr,"I-2402 %s\n", pString ? pString : "");
        pSigningParameters[4] = "test4";
        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            ,0
            ,0
            ,0
            ,0
            );
        fprintf(stderr,"I-185 %s\n", pString ? pString : "");
        pSigningParameters[5] = "test5";
        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            , write_to_FILE, stdout
            , write_to_FILE, stderr
            );
        fprintf(stderr,"I-195 %s\n", pString ? pString : "");
        pSigningParameters[0] = "test0";
        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            , write_to_FILE, stdout
            , write_to_FILE, stderr
            );
        fprintf(stderr,"I-196 %s\n", pString ? pString : "");
        pSigningParameters[0] = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
        pString = librock_awsFillAndSign(
            "request"
            ,pSigningParameters
            , write_to_FILE, stdout
            , write_to_FILE, stderr
            );
        fprintf(stderr,"I-210 %s\n", pString ? pString : "");
 
    }
    fprintf(stderr,"I-2404 %d\n", countToValue("Up to newline\n"));
    {
        int iString;
        char **ppStringList = 0;
        for(iString = 0;iString < 33;iString++) {
            iString = librock_stringListGetIndex(&ppStringList, 1, "test", 4);
            ppStringList[iString] = "test2";
        }
        iString = librock_stringListGetIndex(&ppStringList, 1, "test2", 4);
    }
    fprintf(stderr,"I-206\n");
    if (1) {
        struct librock_appendable aBuffer;
        char credentials[200];
        const char *pRead;

        librock_appendableSet(&aBuffer, 0, 0, 0);

        putenv("awsFILLFAULT=this is a test");
//out20170117        librock_safeAppendEnv0(&aBuffer,"awsFILLFAULT"); /* Calculate size only */

        librock_appendableSet(&aBuffer, credentials, sizeof(credentials), 0);

        librock_safeAppend0(&aBuffer,0,-1); //-1 length
//out20170117        librock_safeAppendEnv0(&aBuffer,"  "); //environment variable not found
        aBuffer.iWriting = -1; // Set invalid position
        librock_safeAppend0(&aBuffer,"",1);

        aBuffer.iWriting = aBuffer.cb-2; // Set position near end
        librock_safeAppend0(&aBuffer,"FILL",4);
        librock_safeAppend0(&aBuffer,"FILL",4);
        aBuffer.iWriting = aBuffer.cb-2; // Set position near end
        librock_safeAppend0(&aBuffer,0,4);
        librock_safeAppend0(&aBuffer,0,4);

        aBuffer.iWriting = aBuffer.cb-2; // Set position near end
//out20170117        librock_safeAppendEnv0(&aBuffer,"awsFILLFAULT");

        aBuffer.iWriting = 0;
        librock_safeAppendUrlEncoded0(&aBuffer,"%",-1); //-1 length
        needEncoding("%");
        needEncoding("%0");
        needEncoding("%00");
        pRead = "%44";
        librock_awsSignature_canonicalQueryString_(&aBuffer, &pRead, 0);
        pRead = "%AA";
        librock_awsSignature_canonicalQueryString_(&aBuffer, &pRead, 0);
        pRead = "%";
        librock_awsSignature_canonicalQueryString_(&aBuffer, &pRead, 0);
        pRead = "%Z";
        librock_awsSignature_canonicalQueryString_(&aBuffer, &pRead, 0);
        pRead = "%AZ";
        librock_awsSignature_canonicalQueryString_(&aBuffer, &pRead, 0);
        pRead = "%30%3A%2e%40%41%61%60%7D%7E%5F%2D0*.@Aa`}~_-";
        librock_awsSignature_canonicalQueryString_(&aBuffer, &pRead, 0);
 
    }
    fprintf(stderr,"I-253\n");
    countToEol("\r");
    countToValue("");
    countLWS(" \t");
    fprintf(stderr,"I-257\n");
    time(0);
    return 0;
} /* librock_coverage_main */
#undef malloc
#undef realloc
long librock_iFaultInjection_malloc;
void *librock_FaultInjection_malloc(size_t size)
{
        if (librock_triggerAlternateBranch("malloc", &librock_iFaultInjection_malloc)) {
            return 0;
        }
        return malloc(size);
}
void *librock_FaultInjection_realloc(void *p, size_t size)
{
        if (librock_triggerAlternateBranch("malloc", &librock_iFaultInjection_malloc)) {
            return 0;
        }
        return realloc(p, size);
}
#endif
