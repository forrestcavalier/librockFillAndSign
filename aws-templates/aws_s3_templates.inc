/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Simple Storage Service" (S3)
   at Amazon Web Services.
   
 Copyright 2016 MIB SOFTWARE, INC. 
	NOTE from the copyright holder: Since templates are
	functional, there is minimal copyrightable matter
	contained in these templates.
   
 LICENSE:   MIT (Free/OpenSource)
 
 STABLITY:  UNSTABLE as of 2016-11-22
            Be sure to compile with -DLIBROCK_UNSTABLE.
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign
              
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/
   
   
 */

,"@//aws-s3-create-bucket;2017-05-08;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS S3"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/"
    "\n""@//   @1_Bucket@"
    "\n""@//.default.AWS_SERVICE_NAME=s3"
    "\n""PUT https://@1_Bucket@.s3.amazonaws.com/ HTTP/1.1"
    "\n""Host:@1_Bucket@.s3.amazonaws.com"
    "\n"
	"\n""@eAWS_DEFAULT_REGION@"

,"@//aws-s3-list-buckets;2017-05-08;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS S3 GET Service (List all buckets)"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTServiceGET.html"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//.default.AWS_SERVICE_NAME=s3"
    "\n""GET https://s3.amazonaws.com/ HTTP/1.1"
    "\n""Host: s3.amazonaws.com"

,"@//aws-s3-list;2017-05-08;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS S3 GET Bucket (List Objects) Version 2, with prefix"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/v2-RESTBucketGET.html"
    "\n""@//   @1_bucket@ - The bucket name"
    "\n""@//   @2_prefix@ - The prefix= query parameter"
    "\n""@//.default.AWS_SERVICE_NAME=s3"
    "\n""GET https://@1_bucket@.s3.amazonaws.com/?prefix=@p2_prefix@&list-type=2@peQueryExtra@ HTTP/1.1"
    "\n""Host: @1_bucket@.s3.amazonaws.com"

,"@//aws-s3-put;2017-05-08;MIB SOFTWARE INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS S3 PUT Object"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectPUT.html"
    "\n""@//   @1_bucket@     - The bucket name"
    "\n""@//   @2_filename@   - The local file name. This goes into the upload-file"
    "\n""@//   	                CURL parameter."
    "\n""@//   @3_objectname@ - The S3 object name."
    "\n""@//.default.AWS_SERVICE_NAME=s3"
    "\n""PUT https://@1_bucket@.s3.amazonaws.com/@p3_objectname@ HTTP/1.1"
    "\n"":curl:upload-file=\"@p2_filename@\""
    "\n""Host: @1_bucket@.s3.amazonaws.com"

,"@//aws-s3-get;2017-05-08;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS S3 GET Object"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectGET.html"
    "\n""@//   @1_bucket@ - bucket name"
    "\n""@//   @2_object@ - object name"
    "\n""@//.default.AWS_SERVICE_NAME=s3"
    "\n""GET https://@1_bucket@.s3.amazonaws.com/@p2_object@@peQueryExtra@ HTTP/1.1"
    "\n""Host: @1_bucket@.s3.amazonaws.com"

,"@//aws-s3-delete;2017-05-08;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS S3 DELETE Object"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectDELETE.html"
    "\n""@//   @1_bucket@ - bucket name"
    "\n""@//   @2_object@ - object name"
    "\n""@//.default.AWS_SERVICE_NAME=s3"
    "\n""DELETE https://@1_bucket@.s3.amazonaws.com/@p2_object@ HTTP/1.1"
    "\n""Host: @1_bucket@.s3.amazonaws.com"
