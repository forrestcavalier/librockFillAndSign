/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Elastic Compute Cloud" (EC2)
   at Amazon Web Services.
   
 Copyright 2016 MIB SOFTWARE, INC. 
	NOTE from the copyright holder: Since templates are
	functional, there is minimal copyrightable matter
	contained in these templates.
   
 LICENSE:   MIT (Free/OpenSource)
 
 STABILITY: UNSTABLE as of 2016-11-22
            Be sure to compile with -DLIBROCK_UNSTABLE.
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign
              
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/
   
   
 */
#if 0 //example only
,"@//aws-ec2-describe-instances.curl"
    "\n""@// Template for an AWS REST API request using CURL."
    "\n""@//"
    "\n""@// TEMPLATE FOR:  AWS EC2 DescribeInstances"
    "\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeInstances.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
	"\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
    "\n""data=\"Action=DescribeInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""
#endif

/*	
	
Scheduled Instances

,"@//aws-ec2-DescribeScheduledInstanceAvailability"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeScheduledInstanceAvailability"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeScheduledInstanceAvailability.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DescribeScheduledInstanceAvailability&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeScheduledInstances"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeScheduledInstances"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeScheduledInstances.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=DescribeScheduledInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-PurchaseScheduledInstances"
"\n""@// TEMPLATE FOR:  AWS EC2 PurchaseScheduledInstances"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_PurchaseScheduledInstances.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=PurchaseScheduledInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-RunScheduledInstances"
"\n""@// TEMPLATE FOR:  AWS EC2 RunScheduledInstances"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_RunScheduledInstances.html"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//"
    "\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL. IMPORTANT: use the -bs command"
    "\n""@// line flag to compute the SHA256 signature of the POST data.)"
    "\n""request = \"POST\""
    "\n""url=\"https://ec2.amazonaws.com/\""
"\n""data="Action=RunScheduledInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

*/
	