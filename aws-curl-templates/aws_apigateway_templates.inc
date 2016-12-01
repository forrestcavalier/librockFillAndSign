/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Amazon API Gateway" (apigateway)
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
//https://docs.aws.amazon.com/apigateway/api-reference/resource/

,"@//aws-apigateway-account.curl"
    "\n""@// TEMPLATE FOR:  AWS API Gateway Account"
    "\n""@// REST API DOCS: https://docs.aws.amazon.com/apigateway/api-reference/resource/account/"
    "\n""@//"
    "\n""@// TEMPLATE REVISION:    2016-11-26 by MIB SOFTWARE, INC."
    "\n""@// TEMPLATE LICENSE:     MIT (Free/Open Source)"
    "\n""@// TEMPLATE PARAMETERS:"
    "\n""@//   @1_region@ - The URI-encoded region"
    "\n""@//"
	"\n""@// (Use awsFillAndSign by MIB SOFTWARE to fill the template, strip comments,"
    "\n""@// and add AWS Signatures before using CURL.)"
    "\n""@//"
    "\n""url=\"https://apigateway.@1_region@.amazonaws.com/account"
    "\n""header = \"Host: apigateway.@1_region@.amazonaws.com\""
    "\n""header = \"Authorization: AWS4-HMAC-SHA256 Credential=...,SignedHeaders=...,Signature=... (all will be replaced)\""
    "\n""header = \"x-amz-content-sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\""

/*

    Account
    ApiGateway
    ApiKey
    ApiKeyIds
    ApiKeys
    Authorizer
    Authorizers
    BasePathMapping
    BasePathMappings
    ClientCertificate
    ClientCertificates
    Deployment
    Deployments
    DomainName
    DomainNames
    Integration
    IntegrationResponse
    Method
    MethodResponse
    Model
    Models
    Resource
    Resources
    RestApi
    RestApis
    Stage
    Stages
    Template
    TestInvokeAuthorizerResponse
    TestInvokeMethodResponse
    Usage
    UsagePlan
    UsagePlanKey
    UsagePlanKeys
    UsagePlans
*/
