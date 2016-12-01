/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Elastic Compute Cloud" (EC2)
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
 
 
#if 0
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
Elastic IP Addresses

,"@//aws-ec2-AllocateAddress"
"\n""@// TEMPLATE FOR:  AWS EC2 AllocateAddress"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_AllocateAddress.html"
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
"\n""data="Action=AllocateAddress&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-AssociateAddress"
"\n""@// TEMPLATE FOR:  AWS EC2 AssociateAddress"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_AssociateAddress.html"
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
"\n""data="Action=AssociateAddress&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeAddresses"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeAddresses"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeAddresses.html"
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
"\n""data="Action=DescribeAddresses&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DescribeMovingAddresses"
"\n""@// TEMPLATE FOR:  AWS EC2 DescribeMovingAddresses"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DescribeMovingAddresses.html"
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
"\n""data="Action=DescribeMovingAddresses&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-DisassociateAddress"
"\n""@// TEMPLATE FOR:  AWS EC2 DisassociateAddress"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_DisassociateAddress.html"
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
"\n""data="Action=DisassociateAddress&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-MoveAddressToVpc"
"\n""@// TEMPLATE FOR:  AWS EC2 MoveAddressToVpc"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_MoveAddressToVpc.html"
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
"\n""data="Action=MoveAddressToVpc&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-ReleaseAddress"
"\n""@// TEMPLATE FOR:  AWS EC2 ReleaseAddress"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_ReleaseAddress.html"
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
"\n""data="Action=ReleaseAddress&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

,"@//aws-ec2-RestoreAddressToClassic"
"\n""@// TEMPLATE FOR:  AWS EC2 RestoreAddressToClassic"
"\n""@// REST API DOCS: http://docs.aws.amazon.com/AWSEC2/latest/APIReference/API_RestoreAddressToClassic.html"
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
"\n""data="Action=RestoreAddressToClassic&Version=2016-09-15\""
    "\n""header = \"host:ec2.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""

*/

