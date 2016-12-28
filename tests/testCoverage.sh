rm *.gcda
export FIFILE=coverageMisc.err
export AWS_DEFAULT_REGION=us-east-1
export AWS_ACCESS_KEY_ID=AAAAAAAAAAAAAAAAAAAA
export AWS_SECRET_ACCESS_KEY=7666666666666666666666666666666666666666

rm tests/$FIFILE
./awsFillAndSign --list 2>>tests/$FIFILE

./awsFillAndSign --list aws-ec2-describe-instances.curl >tests/awsFillAndSignTest.txt 2>>tests/$FIFILE
./awsFillAndSign -e ec2 -b tests/awsFillAndSignTest.txt --from-file tests/awsFillAndSignTest.txt 2>>tests/$FIFILE
./awsFillAndSign --list FileDoesNotExist 2>>tests/$FIFILE
./awsFillAndSign --help 2>>tests/$FIFILE
./awsFillAndSign --coverage 2>>tests/$FIFILE

echo "" | ./awsFillAndSign - --help 2>>tests/$FIFILE
echo "" | ./awsFillAndSign --from-file FileDoesNotExist 2>>tests/$FIFILE

echo "x/x,y,zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz" | ./awsFillAndSign aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
./awsFillAndSign --verbose -e s3 aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE

# Upload file
./awsFillAndSign -es3 -bFileDoesNotExist aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
./awsFillAndSign -es3 -b tests/awsFillAndSignTest.txt aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
./awsFillAndSign -es3 -bs aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
./awsFillAndSign -es3 -bs aws-s3-list.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE

# unexpected formats of credential scope
echo '""' | ./awsFillAndSign aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x | ./awsFillAndSign aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x, | ./awsFillAndSign aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x,y,z | ./awsFillAndSign -DCLParam=%%test --from-file tests/test-raw.txt param1 2>>tests/$FIFILE
echo x,y,zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz | ./awsFillAndSign -DCLParam=%%test --from-file tests/test-raw.txt param1 2>>tests/$FIFILE

# escapes and parameter testing.
echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw.txt param1@ 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -D CLParam=%%test --from-file tests/test-curl-escapes.curl 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -b tests/test-raw2.txt --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -D CLParam=%%test --from-file tests/test-curl-baduri.curl 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -D missingCLParam=%%test --from-file tests/test-raw.txt param1@ 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw4.txt param1@ 2>>tests/$FIFILE

# invalid command line parameters
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -D 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -DAWS_SECRET_ACCESS_KEY=notallowed 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -e 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign -bs --verbose -b 2>>tests/$FIFILE
echo /x/y,y,z | ./awsFillAndSign --unknown --verbose -b 2>>tests/$FIFILE
# E-1157 parameter out of range
echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw2.txt 2>>tests/$FIFILE
# Non-numeric replaceable parameter
echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw3.txt 2>>tests/$FIFILE
# No headers
echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw5.txt 2>>tests/$FIFILE

echo "librock_safeAppend0 1" >librock_armAlternateBranch.txt
export FIFILE=coverageSA0.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
  do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo /x/y,y,z | ./awsFillAndSign --verbose -e s3 -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
  done

echo "librock_safeAppend0 1" >librock_armAlternateBranch.txt
export FIFILE=coverageSA0two.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
  do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo /x/y,y,z | ./awsFillAndSign -bs --verbose -D missingCLParam=%%test --from-file tests/test-raw.txt param1@ 2>>tests/$FIFILE
  done

# -bs upload file with failed malloc
echo 'librock_safeAppend0 1' >librock_armAlternateBranch.txt
export FIFILE=coverageSA0three.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose -e ec2 -bs aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
  done



echo 'malloc 1' >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose -e ec2 -bs aws-ec2-describe-instances.curl 2>>tests/$FIFILE
  done

echo "malloc 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc2.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
  done

# TODO: (2) -bs upload file with failed malloc
echo "malloc 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc3.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose -e ec2 -bs aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
  done

echo "global 1" >librock_armAlternateBranch.txt
export FIFILE=coverageGlobal.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose -e ec2 -bs aws-ec2-describe-instances.curl 2>>tests/$FIFILE
  done

echo "global 1" >librock_armAlternateBranch.txt
export FIFILE=coverageGlobal2.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
  do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo /x/y,y,z | ./awsFillAndSign --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
  done

gcov awsFillAndSign.c
gcov hmacsha256.c
gcov librock_sha256.c
exit 0