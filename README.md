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
 USAGE: awsFillAndSign [-e][-v][-bs] template-name.curl [param1[ param2...]]

   Without -e, credentials are taken from one line on stdin in the format of:
      <region>/<service>,<ID>,<SECRET>
   e.g:
      us-east-1/s3,AXXXXXXXXXXXXXXXXXXX,7777777777777777777777777777777777777777

   Parameters (param1,param2,...) must already be URI-Encoded as appropriate.

   The output is the filled template with AWS Version 4 signatures added.

 OPTIONS:
  -e <service>     Use service (e.g 's3') and the environment variables
                   AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY, AWS_DEFAULT_REGION
  -t <file-name>   Load template from file.
  -v               Verbose debugging output on stderr, including generated
                   AWS Canonical Request fields.
  -bs              Calculate the SHA256 body signature for the upload-file or
                   the data CURL options from the filled template.
  -b <file-name>   Calculate SHA256 body signature from file.
  -                Marker for end of arguments. (Useful when parameters that
                   follow may start with '-'.)

 INFORMATION commands (output is not a filled and signed template:)
  --help           Show this message.
  --list           List the standard templates.
  --write <name>   Show the named standard template along with comments.
```
