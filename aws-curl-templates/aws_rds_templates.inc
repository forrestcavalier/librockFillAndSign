/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Relational Database Service" (RDS)
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

,"@//aws-rds-describe-db-engine-versions.curl"
    "\n""@// TEMPLATE FOR:  AWS EC2 DescribeDBEngineVersions"
    "\n""@// REST API DOCS: http://http://docs.aws.amazon.com/AmazonRDS/latest/APIReference/API_DescribeDBEngineVersions.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
	"\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://rds.amazonaws.com/\""
    "\n""data=\"Action=DescribeDBEngineVersions&Version=2014-09-01\""
    "\n""header = \"host:rds.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""
