awsFillAndSign: awsFillAndSign.o hmacsha256.o librock_sha256.o

INCLUDES = librock_fdio.h \
	librock_postinclude.h \
	librock_preinclude.h \
	aws_templates.inc \
	aws-curl-templates/aws_apigateway_templates.inc \
	aws-curl-templates/aws_ec2_ami_templates.inc \
	aws-curl-templates/aws_ec2_ebs_templates.inc \
	aws-curl-templates/aws_ec2_hosts_templates.inc \
	aws-curl-templates/aws_ec2_net_templates.inc \
	aws-curl-templates/aws_ec2_reserved_templates.inc \
	aws-curl-templates/aws_ec2_scheduled_templates.inc \
	aws-curl-templates/aws_ec2_spot_templates.inc \
	aws-curl-templates/aws_ec2_templates.inc \
	aws-curl-templates/aws_ec2_vm_templates.inc \
	aws-curl-templates/aws_ec2_vpc_templates.inc \
	aws-curl-templates/aws_iam_templates.inc \
	aws-curl-templates/aws_iot_templates.inc \
	aws-curl-templates/aws_mobileanalytics_templates.inc \
	aws-curl-templates/aws_rds_templates.inc \
	aws-curl-templates/aws_s3_templates.inc \
	aws-curl-templates/aws_sqs_templates.inc

awsFillAndSign.o: awsFillAndSign.c ${INCLUDES}
	${CC} -c -DLIBROCK_UNSTABLE -DLIBROCK_AWSFILLANDSIGN_MAIN awsFillAndSign.c