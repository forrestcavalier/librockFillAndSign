/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API "Simple Notification Service" (sns)
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
,"@//aws-sns-publish-to-phone;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SNS"
	"\n""@// REST API DOCS: http://docs.aws.amazon.com/sns/latest/dg/sms_publish-to-phone.html"
    "\n""@//   @1_Message@"
    "\n""@//   @2_PhoneNumber@ e.g. +14155552671"
    "\n""@//   @3_SenderID@ 11 alphanumeric chars, at least one letter, no space"
    "\n""@//.default.AWS_SERVICE_NAME=sns"
    "\n""POST https://sns.@eAWS_DEFAULT_REGION@.amazonaws.com/ HTTP/1.1"
    "\n""Host:sns.@eAWS_DEFAULT_REGION@.amazonaws.com"
    "\n"":curl:data=\"Version=2010-03-31"
    "&Action=Publish&Message=@1_Message@&PhoneNumber=@2_PhoneNumber@"
	"&MessageAttributes.entry.1.Name=AWS.SNS.SMS.SenderID"
	"&MessageAttributes.entry.1.Value.DataType=String"
	"&MessageAttributes.entry.1.Value.StringValue=@3_SenderID@\""
	
,"@//aws-sns-publish-to-phone.curl;2017-02-03;MIB SOFTWARE, INC;MIT (Free/Open source, No Warranty)"
    "\n""@// TEMPLATE FOR:  AWS SNS"
	"\n""@// REST API DOCS: http://docs.aws.amazon.com/sns/latest/dg/sms_publish-to-phone.html"
    "\n""@//   @1_Message@"
    "\n""@//   @2_PhoneNumber@ e.g. +14155552671"
    "\n""@//   @3_SenderID@ 11 alphanumeric chars, at least one letter, no space"
    "\n""@//.default.AWS_SERVICE_NAME=sns"
    "\n""request = \"POST\""
    "\n""url=\"https://sns.@eAWS_DEFAULT_REGION@.amazonaws.com/\""
    "\n""header=\"Host:sns.@eAWS_DEFAULT_REGION@.amazonaws.com\""
    "\n""data=\"Version=2010-03-31"
    "&Action=Publish&Message=@1_Message@&PhoneNumber=@2_PhoneNumber@"
	"&MessageAttributes.entry.1.Name=AWS.SNS.SMS.SenderID"
	"&MessageAttributes.entry.1.Value.DataType=String"
	"&MessageAttributes.entry.1.Value.StringValue=@3_SenderID@\""
