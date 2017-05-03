/* awsFillAndSign.c Copyright 2016 MIB SOFTWARE, INC.
   Licensed under the terms of the MIT License (Free/OpenSource, No Warranty)
   (See the LICENSE file in the source.)
   
 PURPOSE:   Create and Sign AWS API requests without the bulky AWS SDK,
            Python, or external libraries.
            
            See more features listed in command.c

            - Very compact. (About 140k bytes compiled for x86.
              Works on the ESP32,too, for example.)
 
 SOURCE:    https://github.com/forrestcavalier/awsFillAndSign/        

            Single Compilation Unit (a.k.a. unity build)
            Compile using:
                gcc awsFillAndSign/command.c
            or
                cl awsFillAndSign/command.c
                
            See below for compiling into a library.
            
 QUALITY:   librock_STABLE:   as of 2017-04-18.
            
            librock_PORTABLE: gcc/MSVC/clang/BSD/Linux/Windows.
            
            librock_COVERAGE: https://codecov.io/gh/forrestcavalier/awsFillAndSign
            
            Report bugs at github. See below for help.
            
 GET HELP:  Visit http://www.mibsoftware.com/awsFillAndSign/
            - Get pre-built binaries.
            
            - Learn about the quality tags (_STABLE,_PORTABLE, etc.)
            
            - Get tips and instructions for compiling into a library.
              (Because of the plethora of compilation environments this
              is more than simply cloning from github and doing a
              unity build. If you know how to build a library, this
              is actually very, very easy.)
              
            - Get pay-as-you-go and contract support and advice for
              getting awsFillAndSign working in your application,
              customization, custom templates, porting to other systems,
              etc.

            - Per-incident support is available.

*********************************************************************/

/********************************************************************/
//[[Predeclarations]]
#if !defined(PRIVATE)
#   define PRIVATE static /* names with module-level scope */
#endif
    
#include "awsFillAndSign.h"
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
                        struct librock_awsFillAndSignParameters_s *signingParameters
                        );
    PRIVATE const char *librock_awsSignature_parseRequest_(
                        struct librock_awsRequest_s *pRequest);
    PRIVATE const char *librock_awsSignature_getStringToSign_(
                        struct librock_awsRequest_s *pRequest,
                        char **ppToSign);
    PRIVATE const char *librock_awsSignature_outputWithAuthorization_(
                        struct librock_awsRequest_s *pRequest,
                        const char *pToSign);
    PRIVATE int countOptionName(const char *pRead); /* count to ' ' or '=', for parsing CURL options */

/**************************************************************/
//[[Implementation]]
//struct for the parsed request Callers should NOT use because it is subject to change.
 struct librock_awsRequest_s {
    const char *pRequest;
    struct librock_awsFillAndSignParameters_s *signingParameters;
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
};


    PRIVATE int qsort_strcasecmp_(const void *pString1, const void *pString2) {
        return strcasecmp(*((char **)pString1), *((char **)pString2));
    }

    PRIVATE int qsort_strcmp_(const void *s1, const void *s2) {
        return strcmp(*((char **)s1), *((char **)s2));
    }
    PRIVATE int countLWS(const char *pRead);
    PRIVATE void bToUHex0(char *pWrite, unsigned char ch)
    {
        char *digits = "0123456789ABCDEF";
        *pWrite = digits[(ch>>4)&0x0f];
        pWrite[1] = digits[(ch)&0x0f];
        pWrite[2] = '\0';
    }

/**************************************************************/
/* The main workhorse function. Call this one */
const char *librock_awsFillAndSign(
        const char *pRequestWithoutBody, //CURL one option per line, or HTTP headers one per line.
        struct librock_awsFillAndSignParameters_s *signingParameters
        )
{
    struct librock_awsRequest_s awsRequest;
    const char *pErrorMessage = 0;
    char *pToSign = 0;

    if (!pRequestWithoutBody) {
        return("E-309 need request");
    }
    if (!signingParameters) {
        return("E-316 need signingParameters.");
    }
    if (!signingParameters->AWS_DEFAULT_REGION) { 
        return("E-316 need complete signingParameters. Missing Service Region.");
    }
    if (!signingParameters->AWS_SERVICE_NAME) {
        return("E-316 need complete signingParameters. Missing Service Name.");
    }
    if (!signingParameters->AWS_ACCESS_KEY_ID) { 
        return("E-316 need complete signingParameters. Missing Access Key Id.");
    }
    if (!signingParameters->AWS_SECRET_ACCESS_KEY) {
        return("E-316 need complete signingParameters. Missing Access Key.");
    }
        
    /* Initialize the structure */
    librock_awsRequest_start_(&awsRequest, pRequestWithoutBody, signingParameters);
    if (!strcmp(signingParameters->AWS_SERVICE_NAME,"sdb")) {
        awsRequest.signatureV2 = 1;
    }

    awsRequest.pListOfHeaders = 0;
    
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
    struct librock_awsFillAndSignParameters_s *signingParameters
    )
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
    pRequest->signingParameters = signingParameters;
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
{//Callers should use librock_awsFillAndSign(), not this.
    const char *pRead = pRequest->pRequest;
    const char *pStartParse;
    int iPass;
    
    while(pRead) {
        if (isCurlOptionName(pRead, "url")) {
            pRequest->bFormatCURL = 1;
        }
        pRead = strchr(pRead,'\n');
        if (pRead) {
            pRead++;
        }
    }
    pRead = pRequest->pRequest;
    if (pRequest->bFormatCURL) {
        /* detected cURL --config format*/
        if (pRequest->signingParameters->fnDebugOutput) {
            const char *pLiteral = "I-171 Detected CURL\n";
            (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
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
            if (!strncmp(pRead,":curl:",6)) {
                goto skip_line;
            }
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
                if (pRequest->signingParameters->AWS_SHA256 == (char *) 0) {
                    pRead += 21;
                    while(*pRead == ' ') {
                        pRead++;
                    }
                    pRequest->signingParameters->AWS_SHA256 = pRead;
                }
            } else if (1/*sign everything*/) {
                if (countToStr(pRead, ":") >= countToEol(pRead)) {
                    return "E-528 malformed header or no Host header";
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
            if (*pRead == '\n') { /* Blank line: end of headers */
                break;
            }
        }
        if (pRequest->amzDate[0]) {
            /* Date was not found in the supplied headers.  Supply it. */
            if (iPass == 1) {
                pRequest->pListOfHeaders[pRequest->cHeaders] = pRequest->amzDate;
            }
            pRequest->cHeaders++;
        }
        if (pRequest->signingParameters->AWS_SECURITY_TOKEN) {
            if (iPass == 1) {
                pRequest->pListOfHeaders[pRequest->cHeaders] = "X-Amz-Security-Token: will_replace";
            }
            pRequest->cHeaders++;
        }
        if (pRequest->signingParameters->CONTENT_TYPE) {
            if (iPass == 1) {
                pRequest->pListOfHeaders[pRequest->cHeaders] = "Content-Type: will_replace";
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
    if (!pRequest->signatureV2) {
        int cValid = 0;
        const char *pRead = pRequest->signingParameters->AWS_SHA256;
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
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, name, strlen(name));
    pLiteral = ">";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));

    while (*pRead) {
        (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pRead, countToEol(pRead));
        (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, "\\n\n", 3);
        pRead += countToEol(pRead);
        if (*pRead) {
            pRead++;
        }
    }

    pLiteral = "</";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, name, strlen(name));
    pLiteral = ">\n";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));

    pLiteral = "<";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, name, strlen(name));
    pLiteral = "Bytes>";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
    pRead = pSource;
    while (*pRead) {
        char buffer[3];
        bToHex0(buffer, *pRead & 0xff);
        (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, buffer, strlen(buffer));
        pRead++;
        if (*pRead) {
            (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, " ", 1);
        }
    }
    pLiteral = "</";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, name, strlen(name));
    pLiteral = "Bytes>\n";
    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
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
        librock_appendableSet(&aCopy, 0, 0);
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
/* For Version 4.
        "@1_VERB@\n"
        "@2_URI@\n"
        "@3_QueryString@\n"
        "@4_Headers@\n"
        "@5_SignedHeaders@\n"
        "@6_PayloadHash@" //No trailing \n
        ;
   For Version 2.
        "@1_VERB@\n"
        "@4_Host@\n"
        "@2_URI@\n"
        "@3_QueryString@" //No trailing \n
        ;
 */
    char *pParameterList[7]; /* pParameterList[0] unused */
    /* Build the Canonical Request. */
    /* We are going to build stringtosignbytes afto digest with hmac-sha256 */

    librock_appendableSet(&aParameter, 0, 0);
    
    if (pRequest->signingParameters->fnDebugOutput) {
        const char *pLiteral = "I-330 ";
        (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
        (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pRequest->pRequest, strlen(pRequest->pRequest));
        (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, "\n", 1);
    }

    /* Overallocate, ensuring room for all the headers, amzDate, payload hash.
    We multiply the strlen(pRequest->pURI) as a worst case for expanding out '/' to %2F
    */
    aParameter.cb = countToEol(pRequest->pUriPath)*3 + strlen(pRequest->pRequest)*2+200+1; 
    //  This buffer is also reused to create stringToSign, which gets parts of the credentials.
    aParameter.cb += 22 + 32*2 + 1; /*room for adding x-amz-content-Sha256: */
    aParameter.cb += strlen(pRequest->signingParameters->AWS_DEFAULT_REGION);
    aParameter.cb += strlen(pRequest->signingParameters->AWS_SERVICE_NAME);
    aParameter.cb += pRequest->signingParameters->AWS_SECURITY_TOKEN ? strlen(pRequest->signingParameters->AWS_SECURITY_TOKEN) : 0;

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
        pRead = strstr(pRead,"\n\n");
        if (!pRead) {
            pErrorMessage = "E-936 missing data for signature Version 2";
        } else {
            pRead += 2;
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
                pReplace = pRequest->signingParameters->AWS_SECURITY_TOKEN;
            }
            if (!strncasecmp(pHeader, "Content-type: will_replace", 26)) {
                pReplace = pRequest->signingParameters->CONTENT_TYPE;
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
        if (!librock_safeAppend0(&aParameter, pRequest->signingParameters->AWS_SHA256, 32*2)) {
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

        }
        if (!pErrorMessage) {
            int i;
            int need = 0;
            char *pWrite;
            for(i = 1;i <= 6;i++) {
                need += strlen(pParameterList[i]) + 2; /* \n\0 */
            }
            pCanonicalRequest = malloc(need);
            if (!pCanonicalRequest) {
                pErrorMessage = "E-1096 malloc failed";
            } else {
                pWrite = pCanonicalRequest;
                if (pRequest->signatureV2) {
                    need = strlen(pParameterList[1]);
                    memmove(pWrite,pParameterList[1],need);
                    pWrite += need;
                    *pWrite++ = '\n';
                    need = strlen(pParameterList[4]);
                    memmove(pWrite,pParameterList[4],need);
                    pWrite += need;
                    *pWrite++ = '\n';
                    need = strlen(pParameterList[2]);
                    memmove(pWrite,pParameterList[2],need);
                    pWrite += need;
                    *pWrite++ = '\n';
                    need = strlen(pParameterList[3]);
                    memmove(pWrite,pParameterList[3],need);
                    pWrite += need;
                    *pWrite = '\0';
                } else {
                    for(i = 1;i <= 6;i++) {
                        need = strlen(pParameterList[i]);
                        memmove(pWrite,pParameterList[i],need);
                        pWrite += need;
                        *pWrite++ = '\n';
                    }
                    pWrite[-1] = '\0'; /* No trailing \n */
                }
                
            }
        }
        freeOnce((void **) &pHost);
        if (pErrorMessage) {
            freeOnce((void **) & pRequest->pSignedHeaders);
            freeOnce((void **) & aParameter.p);
            freeOnce((void **) & pCanonicalRequest);
            return pErrorMessage;
        }

        if (pRequest->signingParameters->fnDebugOutput) {
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
        librock_safeAppend0(&aParameter, pRequest->signingParameters->AWS_DEFAULT_REGION, -1);
        librock_safeAppend0(&aParameter, "/", 1);
        librock_safeAppend0(&aParameter, pRequest->signingParameters->AWS_SERVICE_NAME, -1);
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

        if (pRequest->signingParameters->fnDebugOutput) {
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
        librock_appendableSet(&a,buffer1,sizeof(buffer1));
        
        librock_hmacSha256(resultSha,
                pRequest->signingParameters->AWS_SECRET_ACCESS_KEY,
                strlen(pRequest->signingParameters->AWS_SECRET_ACCESS_KEY),
                pToSign,
                strlen(pToSign));
                
        librock_safeAppendBase64Encoded0(&a, resultSha,32);

        librock_appendableSet(&a,base64Version2,sizeof(base64Version2));
        librock_safeAppendUrlEncoded0(&a,buffer1,strlen(buffer1));
        if (pRequest->signingParameters->fnDebugOutput) {
            pLiteral = "I-1256 ";
            (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
            (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, base64Version2, strlen(base64Version2));
            (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, "\n", 1);
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
            int cbBuf = strlen(pRequest->signingParameters->AWS_SECRET_ACCESS_KEY)+4;
            char *pConcatKey = (char *) malloc(cbBuf+1);
            if (!pConcatKey) {
                return "E-590 malloc failed";
            }
            memmove(pConcatKey, "AWS4", 4);
            memmove(pConcatKey+4, pRequest->signingParameters->AWS_SECRET_ACCESS_KEY, cbBuf-4);
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
            pRead = pRequest->signingParameters->AWS_DEFAULT_REGION;
            librock_hmacSha256(resultSha, (char *) resultSha, 32, pRead, strlen(pRead));
            pRead = strchr(pRead, '/')+1;

            /* kService = HMAC(kRegion, Service) */
            pRead = pRequest->signingParameters->AWS_SERVICE_NAME;
            librock_hmacSha256(resultSha, (char *) resultSha, 32, pRead, strlen(pRead));
            
            /* kSigning = HMAC(kService, "aws4_request")*/
            librock_hmacSha256(resultSha, (char *) resultSha, 32, "aws4_request", 12);

            /* resultSha has the signing key */
            if (pRequest->signingParameters->fnDebugOutput) {
                unsigned int i;
                pLiteral = "I-608 ";
                (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, pLiteral, strlen(pLiteral));
                for (i = 0; i < sizeof(resultSha); i++) {
                    char buf[3];
                    bToHex0(buf, resultSha[i] & 0xff);
                    (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, buf, strlen(buf));
                }
                (*pRequest->signingParameters->fnDebugOutput)(pRequest->signingParameters->debugOutputId, "\n", 1);

            }
        }

        /* Use the signing key which in resultSha, and write the result back to resultSha */
        librock_hmacSha256(resultSha, (char *) resultSha, 32, pToSign, strlen(pToSign));
    }
#define emitCounted(s,c) (*(pRequest->signingParameters->fnOutput))(pRequest->signingParameters->outputId, s, c)
#define emit(s)  (*(pRequest->signingParameters->fnOutput))(pRequest->signingParameters->outputId, s, strlen(s))
    
    /* resultSha has the signature. Output with Authorization */
    {
        const char *pHeader = pRequest->pRequest;
        int didContent = 0;
        int didV2Signature = 0;
        while (*pHeader || !didContent) {
            const char *pStart = pHeader;
            if (*pHeader == '\0' || *pHeader == '\n') {
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
                    emitCounted(pStart,pHeader - pStart);
                    if (isCurlOptionName(pStart, "url") && pRequest->signatureV2) {
                        didContent = 1;
                        if (!strncmp(pRequest->pVerb,"GET",countOptionName(pRequest->pVerb))) {
                            emit("&Signature=");
                            emit(base64Version2);
                            didV2Signature = 1;
                        }
                    }
                    if (*pHeader == '\"') {
                        emitCounted(pHeader,1);
                        pHeader++;
                    }
                    emit("\n");
                    if (*pHeader) pHeader++;
                    continue;
                }
            }
            if (!strncasecmp(pStart, "authorization:", 14)) {
                /* Skip this now. Add it later */
            } else if (!strncasecmp(pStart, "x-amz-content-sha256:", 21)) {
                didContent = 1;
                if (pRequest->bFormatCURL) {
                    emit("header = \x22x-amz-content-sha256:");
                } else {
                    emit("x-amz-content-sha256:");
                }
            
                emitCounted(pRequest->signingParameters->AWS_SHA256,64);
                
                if (pRequest->bFormatCURL) {
                    emit("\x22");
                }
                emit("\n");
                if (*pHeader == '\n') {
                    break;
                }
            } else {
                if (pRequest->signatureV2 && pStart == pRequest->pRequest && strncmp(pRequest->pVerb,"POST",countOptionName(pRequest->pVerb))) {
                    const char *pProtocol = strstr(pStart," HTTP/");
                    if (!pProtocol || pProtocol > pHeader) {
                        pProtocol = pHeader;
                    }
                    emitCounted(pStart, pProtocol-pStart);
                    emit("&Signature=");
                    emit(base64Version2);
                    pStart = pProtocol;
                    didContent = 1;
                } else if (pRequest->bFormatCURL) {
                    emit("header = \x22");
                }
                emitCounted(pStart, pHeader-pStart);
                if (pRequest->bFormatCURL && pHeader[-1] != '\x22') {
                    emit("\x22");
                }
                emit("\n");
            }
            if (*pHeader) {
                pHeader++;
            }
        }
        if (pRequest->signatureV2) {
             if (!didV2Signature && !strncmp(pRequest->pVerb,"POST",countOptionName(pRequest->pVerb))) {
                //                        emit("&Signature="); 11 characters
                pRequest->signingParameters->CONTENT_LENGTH += 11 + strlen(base64Version2);
             }
        } else {
            /* Authorization header */
            if (pRequest->bFormatCURL) {
                emit("header = \x22");
            }
            emit("Authorization:");

            emit("AWS4-HMAC-SHA256 Credential=");
            emit(pRequest->signingParameters->AWS_ACCESS_KEY_ID);
            emit("/");
            emitCounted(pRequest->pXAmzDate, (int) (strchr(pRequest->pXAmzDate, 'T')-pRequest->pXAmzDate));
            emit("/");
            emit(pRequest->signingParameters->AWS_DEFAULT_REGION);
            emit("/");
            emit(pRequest->signingParameters->AWS_SERVICE_NAME);
            emit("/aws4_request");
            
            emit(", SignedHeaders=");
            emit(pRequest->pSignedHeaders);
            emit(", Signature=");
            {
                unsigned int i;
                for (i = 0; i < sizeof(resultSha); i++) {
                    char buf[3];
                    bToHex0(buf, resultSha[i] & 0xff);
                    emitCounted(buf,2);
                }

            }
            if (pRequest->bFormatCURL) {
                emit("\x22");
            }
            emit("\n");

            if (pRequest->signingParameters->AWS_SECURITY_TOKEN) {
                if (pRequest->bFormatCURL) {
                    emit("header = \x22");
                }
                emit("X-Amz-Security-Token:");
                emit(pRequest->signingParameters->AWS_SECURITY_TOKEN);
                if (pRequest->bFormatCURL) {
                    emit("\x22");
                }
                emit("\n");
            }

            /* write an amz-date, if needed */
            if (pRequest->amzDate[0]) {
                if (pRequest->bFormatCURL == 1) {
                    emit("header = \x22");
                    emit(pRequest->amzDate);
                    emit("\x22");
                } else {
                    emit(pRequest->amzDate);
                }
                emit("\n");
            }
        }
        if (pRequest->signingParameters->CONTENT_LENGTH > 0) {
            unsigned long value = pRequest->signingParameters->CONTENT_LENGTH;
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
                emit("header = \x22""Content-Length:");
            } else {
                emit("Content-Length:");
            }
            emit(buf+i);
            
            if (pRequest->bFormatCURL) {
                emit("\x22");
            }
            emit("\n");

        }
        if (pRequest->signingParameters->CONTENT_TYPE) {
            if (pRequest->bFormatCURL) {
                emit("header = \x22""Content-Type:");
            } else {
                emit("Content-Type:");
            }
            emit(pRequest->signingParameters->CONTENT_TYPE);
            
            if (pRequest->bFormatCURL) {
                emit("\x22");
            }
            emit("\n");
        }
        if (*pHeader) { /* Body */
            emit(pHeader);
            if (pRequest->signatureV2 && !strncmp(pRequest->pVerb,"POST",countOptionName(pRequest->pVerb))) {
                emit("&Signature=");
                emit(base64Version2);
            }
        }
        emitCounted(0,0); /* Flag end of output */
    }
    return 0;
} /* librock_awsSignature_outputWithAuthorization */
/**************************************************************/


/**************************************************************/
//[[Utility Functions in this module.]]

    PRIVATE int countLWS(const char *pRead)
    {
        const char *pStart = pRead;
        while (*pRead == ' '||*pRead == '\t') {
            pRead++;
        }
        return pRead - pStart;
    }




