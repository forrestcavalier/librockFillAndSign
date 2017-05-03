/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API "SimpleDB" (sdb)
   at Amazon Web Services.
   
 Copyright 2017 MIB SOFTWARE, INC. 
	NOTE from the copyright holder: Since templates are
	functional, there is minimal copyrightable matter
	contained in these templates.
   
 LICENSE:   MIT (Free/OpenSource)
 
 STABLITY:  UNSTABLE as of 2017-02-09
            Be sure to compile with -DLIBROCK_UNSTABLE.
            Check for updates at: https://github.com/forrestcavalier/awsFillAndSign
              
 SUPPORT:   Contact the author for commercial support and consulting at
            http://www.mibsoftware.com/
   
   
 */
,"@//aws-simpledb-create-domain.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB Create Domain"
    "\n""@//   @1_DomainName@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/\""
    "\n""header=\"Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com\""
	"\n""data=\"Action=CreateDomain&DomainName=@1_DomainName@"
	"&Version=2009-04-15&AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@\""

,"@//aws-simpledb-list-domains.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@// TEMPLATE PARAMETERS:  None"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/\""
    "\n""header=\"Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com\""
    "\n""data=\"Version=2009-04-15&AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@"
	"&Action=ListDomains\""
	
,"@//aws-simpledb-put-attributes.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_DomainName@"
    "\n""@//   @2_ItemName@"
    "\n""@//   @3_Name@"
    "\n""@//   @4_Value@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/\""
    "\n""header=\"Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com\""
    "\n""data=\"AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=PutAttributes&DomainName=@1_DomainName@&ItemName=@2_ItemName@"
    "&Attribute.1.Name=@3_Name@&Attribute.1.Replace=true&Attribute.1.Value=@4_Value@\""

	
,"@//aws-simpledb-get-attributes.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_DomainName@"
    "\n""@//   @2_ItemName@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/\""
    "\n""header=\"Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com\""
    "\n""data=\"AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=GetAttributes&DomainName=@1_DomainName@&ItemName=@2_ItemName@\""

,"@//aws-simpledb-delete-attributes.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_DomainName@"
    "\n""@//   @2_ItemName@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/\""
    "\n""header=\"Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com\""
    "\n""data=\"AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=DeleteAttributes&DomainName=@1_DomainName@&ItemName=@2_ItemName@\""

,"@//aws-simpledb-select.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SimpleDB"
    "\n""@//   @1_SelectExpression@"
    "\n""@//.default.AWS_SERVICE_NAME=sdb"
	"\n""url=\"https://sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com/\""
    "\n""header=\"Host:sdb@eAWS_REGION_NOT_USEAST1@.amazonaws.com\""
    "\n""data=\"AWSAccessKeyId=@eAWS_ACCESS_KEY_ID@&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=@eAWS_AMZDATE@&Version=2009-04-15"
    "&Action=Select"
    "&SelectExpression=@1_SelectExpression@\""
    "\n""@//upload-file=\"@p2_body@\""