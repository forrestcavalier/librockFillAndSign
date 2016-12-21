# awsFillAndSign
Fill a CURL config file template with AWS  signature version 4. Cross-platform CLI utility and library in C.
<br>awsFillAndSign Copyright 2016 MIB SOFTWARE, INC.

 PURPOSE:   Sign Amazon Web Services requests with AWS Signature Version 4.

 LICENSE:   MIT (Free/OpenSource)

 STABLITY:  UNSTABLE as of 2016-11-22
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign

 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/

```

 USAGE: awsFillAndSign -e <scope> <template-name.curl> [param1[ param2...]]

   Parameters (param1,param2,...) must already be URI-Encoded as appropriate.

   The output is the filled template with AWS Version 4 signatures added.

 OPTIONS:
  -e <scope>     Set credentials from <scope> and 3 environment variables:
                   AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY, AWS_DEFAULT_REGION
       NOTE: Without -e, credentials are taken from one line on stdin in
       the format of:
          <region>/<scope>,<ID>,<SECRET>
       e.g:
          us-east-1/s3,AXXXXXXXXXXXXXXXXXXX,7777777777777777777777777777777777777777

  --from-file      Load template from file, <template-name.curl>
       NOTE: Without --from-file, name a built-in template.  See --list.

  --verbose        Verbose debugging output on stderr, including generated
                   AWS Canonical Request fields.

  -bs              Calculate the SHA256 body signature for the upload-file or
                   the data CURL options from the filled template.

  -b <file-name>   Calculate SHA256 body signature from file.

  -d <name=value>  Put name=value into the environment.

  -                Marker for end of arguments. (Useful when parameters that
                   follow may start with '-'.)

 INFORMATION commands (output is not a filled and signed template:)
  --help           Show this message.
  --list           List the built-in templates.
  --list <name>    Show the named built-in template along with comments.
```
#Understanding Templates
Templates are small text files which mark replaceable parameters surrounded by '@'.  The purpose of awsFillAndSign is to strip comments, replace the parameters, sign the request, and write the output so that CURL can run it.

If you run awsFillAndSign --write aws-s3-put.curl you will see an example: the built-in template for AWS S3 PUT Object (for file uploads):

```
@//aws-s3-put.curl
@// TEMPLATE FOR:  AWS S3 PUT Object
@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectPUT.html
@//
@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC.
@// TEMPLATE LICENSE:     MIT (Free/Open Source)
@// TEMPLATE PARAMETERS:
@//   @1_bucket@     - The URI-encoded bucket name
@//   @2_filename@   - The local file name. This goes into the upload-file
@//   	                CURL parameter.
@//   @3_objectname@ - The URI-encoded S3 object name.
@//
@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,
@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command
@// line flag to compute the SHA256 signature of the upload-file.)
@//
url="https://@1_bucket@.s3.amazonaws.com/@3_objectname@"
upload-file = "@2_filename@"
header = "Host: @1_bucket@.s3.amazonaws.com"
header = "Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)"
header = "x-amz-content-sha256:will be replaced"
```

This template specifies three parameters. These are in the template text surrounded by '@'.

When you are authoring templates, awsFillAndSign supports the following kinds of parameters:
* Numeric: These start with a number which is the position on the command line after the command line options, starting at 1. For convenient readability, provide a label after the value. (The label part is ignored by awsFillAndSign.)
* Environment variable: These parameters are marked with a 'e' prefix as in '@eENV_VARIABLE_NAME@'.

Some parts of the request will need to be URI-encoded, for example the parts of query strings. If the command line parameters you give to awsFillAndSign are not URI encoded already, the template must specify where it is necessary:
* URI-Encoded Numeric: These are like Numeric parameters with a 'u' prefix, as in '@u3_objectname@'
* URI-Encoded environment variable: These parameters are in the template with a 'eu' prefix, as in '@euENV_VARIABLEVARIABLE@'.

## When there is a request body
When there is an upload-file, as there is for AWS S3 PUT, be sure to give the -bs flag and awsFillAndSign reads the file to compute the SHA256 that is required for the request.
