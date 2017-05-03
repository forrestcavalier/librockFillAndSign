/* awsFillAndSign/command.c Copyright 2017 MIB SOFTWARE, INC.
   Licensed under the terms of the MIT License (Free/OpenSource, No Warranty)
   (See the LICENSE file in the source.)
   
 PURPOSE:   Create and Sign AWS API requests from the command line without
            the bulky AWS SDK, Python, or external libraries.

            - Includes built-in templates for S3, EC2, etc. Templates have
              helpful comments with URLs for more info. (Easy to extend
              and customize.)

            - Outputs CURL --config and raw formats (to use CURL, wget,
              and other HTTPs clients.)

            - Very compact. (About 140k bytes compiled for x86.
              Works on the ESP32,too, for example.)
 
 SOURCE:    https://github.com/forrestcavalier/awsFillAndSign/        

            Single Compilation Unit (a.k.a. unity build)
            Compile using:
                gcc awsFillAndSign/command.c
            or
                cl awsFillAndSign/command.c
                
            See below for compiling into a library.
            
            Try running with --help for synopsis.
            
 QUALITY:   librock_STABLE:   as of 2017-04-18.
            
            librock_PORTABLE: gcc/MSVC/clang/BSD/Linux/Windows.
            
            librock_COVERAGE: https://codecov.io/gh/forrestcavalier/awsFillAndSign
            
            Report bugs at github. See below for help.
            
 GET HELP:  Visit http://www.mibsoftware.com/awsFillAndSign/
            - Get pre-built binaries.
            
            - Learn about the quality tags (_STABLE,_PORTABLE, etc.)
            
            - Usage examples
            
            - Get tips and instructions for modifying and authoring
              new templates.

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

#if !defined(PRIVATE)
#define PRIVATE static
#endif
#include "awsFillAndSign.h"

#   include <stdio.h> /* fwrite, fprintf */
#   include <fcntl.h>
#   include "ulibrock/mit/librock_fdio.h" /* portable _open, read, close */
    PRIVATE char *mallocNamedValue(const char *pName);
    int main(int argc, char **argv);
    PRIVATE int write_to_FILE(void *id, const char *pSource, int count); /* helper function */
    const char *librock_fileSha256Contents(const char *fname, unsigned char *mdBuffer32, unsigned long *contentLength); /* store 32 bytes, returns error explanation or 0 */
    void *librock_fileGetContents(const char *fname); /* allocate memory, read entire file */
    
    const char *librock_fillTemplateTokenize(
                        const char *pTemplate,
                        int iOffset,
                        int *pCount
                        );
    const char *librock_fillTemplate(
                        char **ppFilled,
                        const char *pTemplate,
                        int iEncodeParameters,
                        char ** namedArguments,
                        int argc,
                        char * const * const argv,
                        int *pErrorIndex);

/**************************************************************/
//[[built-in templates]]

const char *librock_awsBuiltInTemplates[] = {
    "@//awsFillAndSign_write_templates_" //Placeholder

#include "aws_templates.inc"

#if defined librock_WANT_BRANCH_COVERAGE
/* For branch and line coverage */
,"@//bad-template;\n@//;"
    "\n@//.default.bad"
    "\n@//="

,"@//bad-template2;2016-11-26;MIB SOFTWARE, INC"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""url=\"https://s3.amazonaws.com/"
    "\n""header = \"Host: s3.amazonaws.com\""

,"@//bad-template3;"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@"

,"@//aws-simpledb-GET-test;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_SelectExpression@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/?AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=Select"
    "&SelectExpression=@1_SelectExpression@\""

,"@//aws-v2-PUT-test.curl;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_SelectExpression@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
    "\n""request=PUT"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/?AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=Select"
    "&SelectExpression=@1_SelectExpression@\""

,"@//aws-v2-PUT-test;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_SelectExpression@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""PUT https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/?AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=Select"
    "&SelectExpression=@1_SelectExpression@ HTTP/1.1"
    "\n""Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com"
    "\n\nStuff"
,"@//aws-v2-PUT-test2;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_SelectExpression@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""PUT https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/?AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=Select"
    "&SelectExpression=@1_SelectExpression@"
    "\n""Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com"
    "\n\nStuff"
,"@//aws-v2-PUT-test3;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_SelectExpression@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""PUT https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/?AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=Select"
    "&SelectExpression=@1_SelectExpression@"
    "\n""Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com"
    "\n\nStuff HTTP/1.1"

,"@//aws-s3-GET-test.curl;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
   "\n""@//.default.AWS_SERVICE_NAME=s3"
   "\n""request=\"GET\""
   "\n""url=\"https://@1_bucket@.s3.amazonaws.com/@p3_objectname@\""
   "\n""header = \"Host: @1_bucket@.s3.amazonaws.com\""

,"@//aws-s3-PUT-test.curl;2017-04-20;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
   "\n""@//.default.AWS_SERVICE_NAME=s3"
   "\n""upload-file=\"@p2_filename@\""
   "\n""url=\"https://@1_bucket@.s3.amazonaws.com/@p3_objectname@\""
   "\n""header = \"Host: @1_bucket@.s3.amazonaws.com\""
   
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

struct write_to_CURL_s {
    FILE *f;
    int cTotalWritten;
    int atEOL;
    int needclose;
    int body;
};

PRIVATE int write_to_CURL(void *id, const char *pRead, int len)
{ /* Helper function */
int cWritten = 0;
struct write_to_CURL_s *s = (struct write_to_CURL_s *) id;
    if (s->cTotalWritten == 0) {
        const char *pRest = memchr(pRead,' ',len);
        if (!pRest) {
            pRest = pRead + strlen(pRead);
        }
        cWritten += fwrite("request=",1,8,s->f);
        cWritten += fwrite(pRead,1,pRest - pRead,s->f);
        cWritten += fwrite("\n",1,1,s->f);
        if (*pRest) {
            pRest++;
        }
        len -= pRest - pRead;
        pRead = pRest;
        pRest = memchr(pRead,' ',len);
        if (!pRest) {
            pRest = pRead + len;
        }
        cWritten += fwrite("url=\x22",1,5,s->f);
        cWritten += fwrite(pRead,1,pRest - pRead,s->f);
        s->needclose = 1;
        len = 0;
    } else if (len == 0 && pRead == 0) {
        if (s->needclose) {
            cWritten += fwrite("\x22",1,1,s->f);
        }
    } else {
        while(len > 0) {
            const char *pRest = memchr(pRead,'\n',len);
            if (pRest == pRead) {
                pRest++;
                if (s->needclose) {
                    cWritten += fwrite("\x22",1,1,s->f);
                    s->needclose = 0;
                }
                if (s->atEOL) {
                    s->body = 1;
                    pRead++;
                    len--;
                }
                s->atEOL = 1;
            } else {
                if (s->atEOL) {
                    if (s->body) {
                        cWritten += fwrite("data=\x22",1,6,s->f);
                        s->needclose = 1;
                    } else if (len >= 6 && !strncmp(":curl:",pRead,6)) {
                        pRead += 6;
                        len -= 6;
                    } else {
                        cWritten += fwrite("header=\x22",1,8,s->f);
                        s->needclose = 1;
                    }
                }
                if (!pRest) {
                    pRest = pRead + len;
                }
                s->atEOL = 0;
            }
            cWritten += fwrite(pRead, 1, pRest - pRead, s->f);
            len -= pRest - pRead;
            pRead = pRest;
        }
    }
    fflush(s->f);
    s->cTotalWritten += cWritten;
    return cWritten;
} /* write_to_CURL */


PRIVATE int write_to_FILE(void *id, const char *pRead, int len)
{ /* Helper function */
    int cWritten = fwrite(pRead, 1, len, (FILE *)id);
    fflush((FILE *)id);
    return cWritten;
} /* write_to_FILE */

struct write_to_raw_s {
    FILE *f;
    int skipn;
};

PRIVATE int write_to_raw(void *id, const char *pRead, int len)
{ /* Helper function */
int cWritten = 0;
struct write_to_raw_s *s = (struct write_to_raw_s *) id;

/* DEBUG: Content-Type: application/x-www-form-urlencoded, and Content-Length
 */
    while(len > 0) {
        const char *pRest = memchr(pRead,'\n',len);
        if (!pRest) {
            pRest = pRead + len;
        }
        if (pRest == pRead) {
            pRest++;
        } else {
            s->skipn = 0;
        }
        if (!strncmp(pRead,":curl:",6)) {
            s->skipn = 1;
        } else if (s->skipn) {
            s->skipn = 0;
        } else {
            cWritten += fwrite(pRead, 1, pRest - pRead, s->f);
            s->skipn = 0;
        }
        len -= pRest - pRead;
        pRead = pRest;
    }
    fflush(s->f);
    return cWritten;
} /* write_to_raw */

int librock_stringListGetIndex(char ***ppStrings,int cStep, const char *pName, int cName);

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
                char **ppString = &((*ppNamedArguments)[iString+1]);
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
                    *ppString = pValue;
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
        *contentLength = 0;
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

                pErrorMessage = librock_fileSha256Contents(fileName, &mdContent32[0], contentLength);
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
            if (pRead && *pRead == '\n') {
                void *pHashInfo;
                pRead++;
                /* Compute hash */
                pHashInfo = malloc(librock_sha256Init(0)/*Get size */);
                if (!pHashInfo) {
                    return "E-1741 malloc failed";
                }
                *contentLength = strlen(pRead);
                librock_sha256Init(pHashInfo);
                librock_sha256Update(pHashInfo, 
                   (unsigned char *) pRead, *contentLength);
                librock_sha256StoreFinal(mdContent32, pHashInfo);
                freeOnce((void **)&pHashInfo);
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
    "\n"" USAGE: awsFillAndSign [OPTIONS] <template-name> [param1[ param2...]]"
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
    "\n""  --curl           Output in curl config format."
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
    int outCurl = 0;
    int iEncodeParameters = 0;
    int credentialsFromEnv = 1;
    char credentials[200];
    struct librock_awsFillAndSignParameters_s signingParameters;
    unsigned char mdContent32[] = { /* SHA256 of empty string */
        0xe3,0xb0,0xc4,0x42,0x98,0xfc,0x1c,0x14,0x9a,0xfb,0xf4,0xc8,0x99,0x6f,0xb9,0x24,0x27,0xae,0x41,0xe4,0x64,0x9b,0x93,0x4c,0xa4,0x95,0x99,0x1b,0x78,0x52,0xb8,0x55
    };
    char *pRequestTemplate = 0;
    char **ppNamedArguments = 0;
    char *pFilledRequest = 0;
    int errorCode = 0;

#if defined librock_WANT_BRANCH_COVERAGE
    argumentIndex = 1;
    fprintf(stderr,"I-1474 %s(COVERAGE TEST)", "[[awsFillAndSign]]");
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
            } else if (!strcmp(argv[argumentIndex], "--curl")) {
                outCurl = 1;
            } else if (!strcmp(argv[argumentIndex], "--from-file")) {
                /* Load template from file */
                fromFile = 1;
#if defined librock_WANT_BRANCH_COVERAGE
            } else if (!strcmp(argv[argumentIndex], "--coverage")) {
                {
                    char *filled;
                    librock_fillTemplate(
                                &filled,
                                "Template",
                                0,
                                0,
                                0, 0, 0);
                     freeOnce((void **) &filled);
                }
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
                unsigned long contentLength;
                
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
                pErrorMessage = librock_fileSha256Contents(pBody, &mdContent32[0], &contentLength);
                if (pErrorMessage) {
                    fprintf(stderr, "%s for file '%s'\n", pErrorMessage, pBody);
                    return 7;
                }
                signingParameters.CONTENT_LENGTH = contentLength;
            } else {
                fprintf(stderr, "E-1457 Unrecognized option: %s\nTry --help.\n",argv[argumentIndex]);
                return 8;
            }
            argumentIndex++;
        }
    }

    if (argumentIndex >= argc) {
        fprintf(stderr, "Usage: awsFillAndSign <template-name> [param1[ param2...]]\nTry --help.\n");
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

        if (!pErrorMessage) {
            signingParameters.CONTENT_TYPE = 0;
            signingParameters.AWS_SHA256 = 0; //Set below
            pName = "AWS_DEFAULT_REGION";
            signingParameters.AWS_DEFAULT_REGION = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];

            if (!signingParameters.AWS_DEFAULT_REGION) {
                pErrorMessage = "E-2173 need complete signingParameters. Missing Service Region.";
            }

            pName = "AWS_SERVICE_NAME";
            signingParameters.AWS_SERVICE_NAME = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
            
            pName = "AWS_SECURITY_TOKEN";
            signingParameters.AWS_SECURITY_TOKEN = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
        }
        if (pErrorMessage) {
            fprintf(stderr, "%s\n", pErrorMessage);
            errorCode = 14;
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
                ppNamedArguments[iString+1] = malloc(strlen(signingParameters.AWS_DEFAULT_REGION)+2);
                if (!ppNamedArguments[iString+1]) {
                    pErrorMessage = "E-2558 malloc failed";
                }
                if (!pErrorMessage) {
                    memmove(ppNamedArguments[iString], pName,strlen(pName)+1);
                    pLiteral = signingParameters.AWS_DEFAULT_REGION;
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
                    signingParameters.AWS_ACCESS_KEY_ID = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
                    pName = "AWS_SECRET_ACCESS_KEY";
                    signingParameters.AWS_SECRET_ACCESS_KEY = ppNamedArguments[librock_stringListGetIndex(&ppNamedArguments, 2,pName, strlen(pName) )+1];
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
                    errorCode = 6;
                } else if (!strchr(credentials,',')) {
                    fprintf(stderr, "E-1956 Need comma-separated credentials on stdin\n");
                    errorCode = 16;
                } else {
                    credentials[countToEol(credentials)] = '\0'; /* Remove \r or EOL */
                    *(strchr(credentials, ',')) = '\0'; /* Split string */

                    signingParameters.AWS_ACCESS_KEY_ID = credentials;
                    signingParameters.AWS_SECRET_ACCESS_KEY = credentials+strlen(credentials)+1;
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
            signingParameters.AWS_SHA256 = (char *) 0;
        } else {
            pErrorMessage = prepareSHA256(&pSHA256, scanSignature, (unsigned long *) &signingParameters.CONTENT_LENGTH, mdContent32, pFilledRequest);
            if (pErrorMessage) {
                if (atoi(pErrorMessage+2)!= 1741) {
                    fprintf(stderr, "%s\nI-1734 template expanded:\n%s\n", pErrorMessage, pFilledRequest);
                } else {
                    fprintf(stderr, "%s\n", pErrorMessage);
                }
                errorCode = 12;
            } else if (signingParameters.CONTENT_LENGTH > 0) {
                if (!outCurl) {
                    signingParameters.CONTENT_TYPE = "application/x-www-form-urlencoded";
                }
                signingParameters.AWS_SHA256 = pSHA256;
            } else {
                /* No content */
                signingParameters.AWS_SHA256 = pSHA256;
            }
        }
        if (!errorCode) {
            if (outCurl) {
                struct write_to_CURL_s s;
                memset(&s, '\0', sizeof(s));
                s.f = stdout;
                signingParameters.outputId = (void *) &s;
                signingParameters.fnOutput = write_to_CURL;
            } else {
                struct write_to_raw_s s;
                memset(&s, '\0', sizeof(s));
                s.f = stdout;
                signingParameters.outputId = (void *) &s;
                signingParameters.fnOutput = write_to_raw;
            }
            signingParameters.debugOutputId = (void *) stderr;
            signingParameters.fnDebugOutput = bVerbose ? write_to_FILE : 0;
            
            pErrorMessage = librock_awsFillAndSign(
                                    pFilledRequest, &signingParameters);
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


//gcc -o awsFillAndSign -Dlibrock_WANT_BRANCH_COVERAGE -fprofile-arcs -ftest-coverage -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN -Werror -Wall awsFillAndSign.c hmacsha256.c librock_sha256.c
#if defined librock_WANT_BRANCH_COVERAGE
#else
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

#define LIBROCK_AWSFILLANDSIGN_MAIN 1
#if defined(LIBROCK_AWSFILLANDSIGN_MAIN)
//#   define LIBROCK_WANT_fillTemplate
#   include "ulibrock/mit/stringlist.c"
#   include "ulibrock/mit/librock_fillTemplate.c"
#   include "ulibrock/mit/appendable.c"
#   include "awsFillAndSign.c"
#   include "ulibrock/mit/hmacsha256.c"
#   include "ulibrock/mit/librock_sha256.c"

#   define LIBROCK_WANT_fileGetContents
#   define LIBROCK_WANT_fileSha256Contents
#   include "ulibrock/mit/librock_fdio.c"

#   if defined librock_WANT_BRANCH_COVERAGE
//Must be last in the single compilation unit, because it #undefs malloc
#   include "tests/awsFillAndSignCoverage.c"
#   endif

#endif //defined(LIBROCK_AWSFILLANDSIGN_MAIN)
