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
	
Reserved Instances


,"@//aws-ec2-AcceptReservedInstancesExchangeQuote"
"\n""@// TEMPLATE FOR:  AWS EC2 AcceptReservedInstancesExchangeQuote"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_AcceptReservedInstancesExchangeQuote.html"
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
"\n""data="Action=AcceptReservedInstancesExchangeQuote&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-CancelReservedInstancesListing"
"\n""@// TEMPLATE FOR:  AWS EC2 CancelReservedInstancesListing"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_CancelReservedInstancesListing.html"
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
"\n""data="Action=CancelReservedInstancesListing&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-CreateReservedInstancesListing"
"\n""@// TEMPLATE FOR:  AWS EC2 CreateReservedInstancesListing"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_CreateReservedInstancesListing.html"
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
"\n""data="Action=CreateReservedInstancesListing&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeReservedInstances"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeReservedInstances"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeReservedInstances.html"
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
"\n""data="Action=DescribeReservedInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeReservedInstancesListings"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeReservedInstancesListings"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeReservedInstancesListings.html"
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
"\n""data="Action=DescribeReservedInstancesListings&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeReservedInstancesModifications"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeReservedInstancesModifications"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeReservedInstancesModifications.html"
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
"\n""data="Action=DescribeReservedInstancesModifications&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeReservedInstancesOfferings"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeReservedInstancesOfferings"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeReservedInstancesOfferings.html"
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
"\n""data="Action=DescribeReservedInstancesOfferings&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-GetReservedInstancesExchangeQuote"
"\n""@// TEMPLATE FOR:  AWS EC2 GetReservedInstancesExchangeQuote"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_GetReservedInstancesExchangeQuote.html"
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
"\n""data="Action=GetReservedInstancesExchangeQuote&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-ModifyReservedInstances"
"\n""@// TEMPLATE FOR:  AWS EC2 ModifyReservedInstances"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_ModifyReservedInstances.html"
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
"\n""data="Action=ModifyReservedInstances&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-PurchaseReservedInstancesOffering"
"\n""@// TEMPLATE FOR:  AWS EC2 PurchaseReservedInstancesOffering"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_PurchaseReservedInstancesOffering.html"
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
"\n""data="Action=PurchaseReservedInstancesOffering&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""


*/
