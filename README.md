# librockFillAndSign
Fill a CURL config file template with AWS signature version 4. Cross-platform CLI utility and library in C.
<br>librockFillAndSign Copyright 2016-2017 MIB SOFTWARE, INC.

 PURPOSE:   Sign Amazon Web Services requests with AWS Signature Version 4.

 LICENSE:   MIT (Free/OpenSource, NO WARRANTY)

 STABLITY:  STABLE as of 2017-05-09
            <br>Check for updates at: https://github.com/forrestcavalier/librockFillAndSign
            <br>Travis-CI status: [![Build Status](https://travis-ci.org/forrestcavalier/librockFillAndSign.svg?branch=master)](https://travis-ci.org/forrestcavalier/librockFillAndSign)
            <br>CodeCov status: [![codecov](https://codecov.io/gh/forrestcavalier/librockFillAndSign/branch/master/graph/badge.svg)](https://codecov.io/gh/forrestcavalier/librockFillAndSign)

 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

```
librockFillAndSign Copyright 2016-2017 MIB SOFTWARE, INC.

 USAGE: librockFillAndSign [OPTIONS] <template-name> [param1[ param2...]]

   The output is the filled template with AWS Version 4 signatures.
   Credentials come from the environment variables AWS_ACCESS_KEY_ID
   and AWS_SECRET_ACCESS_KEY (unless using the --read-key option.)
   AWS_SECURITY_TOKEN is included if it is defined.


 OPTIONS:
  --from-file      Treat the template-name as a file, not a built-in template.
                   (Use --list to see built-in templates.)

  --have-sha256    The template has a SHA256 body signature.

  -b <file-name>   Calculate SHA256 body signature from specified file.
                   (Only needed if the template does not give accurate
                   upload-file or data CURL options.)

  --read-key       Read credentials from one line on stdin in the format of:
                      <ID>,<SECRET>

  -D <name=value>  Put name=value in the environment, for template variables.

  --encode <flag>  Control percent-encoding. 0 - (default) auto-detect;
                   1 - always percent-encode; -1 - never percent-encode.

  --curl           Output in curl config format.

  --wget           Output a command line for wget.

  --verbose        Verbose debugging output on stderr, including generated
                   AWS Canonical Request fields.

  --               Marker for end of arguments. (Useful when parameters that
                   follow may start with '-'.)

 INFORMATION commands (output is not a filled and signed template:)
  --help           Show this message.
  --list           List the built-in templates.
  --list <name>    Show the named built-in template along with comments.
```
#Understanding Templates
Templates are small text files which mark replaceable parameters surrounded by '@'.  The purpose of librockFillAndSign is to strip comments, replace the parameters, sign the request, and write the output so that CURL can run it.

If you run librockFillAndSign --list aws-s3-put you will see an example: the built-in template for AWS S3 PUT Object (for file uploads):

```
@//aws-s3-put
@// TEMPLATE FOR:  AWS S3 PUT Object
@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectPUT.html
@//
@// TEMPLATE REVISION:    2017-05-08 by MIB SOFTWARE INC
@// TEMPLATE LICENSE:     MIT (Free/Open source, No Warranty)
@// TEMPLATE PARAMETERS:
@//   @1_bucket@     - The bucket name
@//   @2_filename@   - The local file name. This goes into the upload-file
@//   	                CURL parameter.
@//   @3_objectname@ - The S3 object name.
@//
@// (Before using CURL, use librockFillAndSign by MIB SOFTWARE to fill the
@// template, strip comments, and add headers for AWS Signature version 4.)
@//
@//.default.AWS_SERVICE_NAME=s3
PUT https://@1_bucket@.s3.amazonaws.com/@p3_objectname@ HTTP/1.1
:curl:upload-file="@p2_filename@"
Host: @1_bucket@.s3.amazonaws.com
```

This template specifies three parameters. These are in the template text surrounded by '@'.

When you are authoring templates, librockFillAndSign supports the following kinds of parameters:
* Numeric: These start with a number which is the position on the command line after the command line options, starting at 1. For convenient readability, provide a label after the value. (The label part is ignored by librockFillAndSign.)
* Environment variable: These parameters are marked with a 'e' prefix as in '@eENV_VARIABLE_NAME@'.

If the AWS_SERVICE_NAME is not given by the template, it must be in the environment.

Some parts of the request will be URL-encoded, controlled by the --encode option. By default, this is auto-detected for each parameter. If a parameter value has a percent character (%) followed by two hexadecimal ascii digits, it is considered already encoded. The template can start with @p<numeric> or @pe to never encode (which is appropriate for CURL upload-file, for example.)

## When there is a request body
When there is an upload-file, as there is for AWS S3 PUT, librockFillAndSign reads the file to compute the SHA256 that is required for the request. If there is a data= field, that will be used to compute the body. You can also use -b, or supply the SHA256 in the template and give the --have-sha256 option.
