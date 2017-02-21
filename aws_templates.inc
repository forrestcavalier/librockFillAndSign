/* Templates for MIB SOFTWARE's awsFillAndSign utility
   to use the REST API of the "Identity and Access Management" (iot)
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
 
//AWS Services supporting Signature Version 4:

//API Gateway
#include "aws-curl-templates/aws_apigateway_templates.inc"

//EC2
#include "aws-curl-templates/aws_ec2_ami_templates.inc"
#include "aws-curl-templates/aws_ec2_ebs_templates.inc"
#include "aws-curl-templates/aws_ec2_hosts_templates.inc"
#include "aws-curl-templates/aws_ec2_net_templates.inc"
#include "aws-curl-templates/aws_ec2_reserved_templates.inc"
#include "aws-curl-templates/aws_ec2_scheduled_templates.inc"
#include "aws-curl-templates/aws_ec2_spot_templates.inc"
#include "aws-curl-templates/aws_ec2_vm_templates.inc"
#include "aws-curl-templates/aws_ec2_vpc_templates.inc"
#include "aws-curl-templates/aws_ec2_templates.inc"

//S3
#include "aws-curl-templates/aws_s3_templates.inc"

//Mobile Analytics
#include "aws-curl-templates/aws_mobileanalytics_templates.inc"

//AWS IoT
#include "aws-curl-templates/aws_iot_templates.inc"

//Amazon Relational Database Service 
#include "aws-curl-templates/aws_rds_templates.inc"

//IAM
#include "aws-curl-templates/aws_iam_templates.inc"

//Amazon Simple Queue Service 
#include "aws-curl-templates/aws_sqs_templates.inc"

//Amazon Simple Notification Service
#include "aws-curl-templates/aws_sns_templates.inc"

//SimpleDB
#include "aws-curl-templates/aws_simpledb_templates.inc"
