rm *.gcda
export FIFILE=coverageMisc.err
rm tests/$FIFILE

export AWS_DEFAULT_REGION=
export AWS_ACCESS_KEY_ID=
export AWS_SECRET_ACCESS_KEY=
export AWS_SECURITY_TOKEN=

./awsFillAndSign -Dtest=9 --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
export AWS_DEFAULT_REGION=us-east-1
./awsFillAndSign -Dtest=11 --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
export AWS_ACCESS_KEY_ID=AAAAAAAAAAAAAAAAAAAA
./awsFillAndSign -Dtest=13 --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
export AWS_SECRET_ACCESS_KEY=7666666666666666666666666666666666666666
./awsFillAndSign -Dtest=15 --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
./awsFillAndSign -Dtest=17b --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
export AWS_SECURITY_TOKEN=test_example_security_token
./awsFillAndSign -Dtest=17 --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE

./awsFillAndSign -Dtest=19 --list 2>>tests/$FIFILE
./awsFillAndSign -Dtest=19b --list aws-s3-get.curl 2>>tests/$FIFILE

./awsFillAndSign -Dtest=21 --list aws-ec2-describe-instances.curl >tests/awsFillAndSignTest.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=22 -b tests/awsFillAndSignTest.txt --from-file tests/awsFillAndSignTest.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=23 --list FileDoesNotExist 2>>tests/$FIFILE
./awsFillAndSign -Dtest=24 --help 2>>tests/$FIFILE
./awsFillAndSign -Dtest=25 --coverage 2>>tests/$FIFILE

echo "" | ./awsFillAndSign -Dtest=27 --read-key - --help 2>>tests/$FIFILE
echo "" | ./awsFillAndSign -Dtest=28 --read-key --from-file FileDoesNotExist 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29 --list bad-template.curl 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29b -DAWS_SERVICE_NAME=bad bad-template.curl 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29c --list bad-template2.curl 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29d bad-template3.curl 2>>tests/$FIFILE

echo "x/x,y,zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz" | ./awsFillAndSign --read-key aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
./awsFillAndSign -Dtest=31 --verbose aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE

# Upload file
./awsFillAndSign -Dtest=34 -bFileDoesNotExist aws-s3-put.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=35 -b tests/awsFillAndSignTest.txt aws-s3-put.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=36 --have-sha256 -DCLparam=test --from-file tests/test-raw.txt @param1 2>>tests/$FIFILE
./awsFillAndSign -Dtest=37 aws-s3-put.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=38 aws-s3-list.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE

# unexpected formats of credential scope
echo '""' | ./awsFillAndSign -Dtest=41 --read-key aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x | ./awsFillAndSign -Dtest=42 --read-key aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x/ | ./awsFillAndSign -Dtest=42b --read-key aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x, | ./awsFillAndSign -Dtest=43 --read-key aws-s3-list.curl bucketname prefix 2>>tests/$FIFILE
echo x,y,z | ./awsFillAndSign -Dtest=44 --read-key -DCLParam=%%test --from-file tests/test-raw.txt param1 2>>tests/$FIFILE
echo x,zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz | ./awsFillAndSign --read-key -DCLParam=%%test --from-file tests/test-raw.txt param1 2>>tests/$FIFILE

# escapes and parameter testing.
echo y,z | ./awsFillAndSign 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=45a --verbose --encode 0 --read-key -DCLParam=%20st --from-file tests/test-raw.txt param1 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=45b --verbose --encode 1 --read-key -DCLParam=%20st --from-file tests/test-raw.txt param1 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=45c --verbose --encode -1 --read-key -DCLParam=%20st --from-file tests/test-raw.txt param1 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=45d --encode 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=49 --read-key --verbose 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=50 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=51 --read-key --verbose -D CLParam=%%test --from-file tests/test-curl-escapes.curl 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=52 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=53 --read-key -b tests/test-raw2.txt --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=54 --read-key --verbose -D CLParam=%%test --from-file tests/test-curl-baduri.curl 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=55 --read-key --verbose -D CLParam=%%test --from-file tests/test-missing-request.curl 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=56 --read-key --verbose -D missingCLParam=%%test --from-file tests/test-raw.txt param1@ 2>>tests/$FIFILE
# bad header
echo y,z | ./awsFillAndSign -Dtest=59 --read-key --verbose -D CLParam=%%test --from-file tests/test-curl-bad-header.curl 2>>tests/$FIFILE
# unmatched @
echo y,z | ./awsFillAndSign -Dtest=60 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw4.txt param1@ 2>>tests/$FIFILE
# negative
echo y,z | ./awsFillAndSign -Dtest=62 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw6.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=63 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw7.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=63b --read-key --verbose -DCLParam=%%test --from-file tests/test-raw8.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=63c --read-key --verbose -DCLParam=%%test --from-file tests/test-raw9.txt param1@ 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=64 --read-key --verbose -D CLParam=%%test --from-file tests/test-truncated-data1.curl 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=65 --read-key --verbose -D CLParam=%%test --from-file tests/test-truncated-data2.curl 2>>tests/$FIFILE

# invalid command line parameters
echo y,z | ./awsFillAndSign -Dtest=68 --read-key --verbose -D 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=69 --read-key --verbose -DAWS_SECRET_ACCESS_KEY=notallowed 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=70 --read-key --verbose 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=71 --read-key --verbose -b 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=72 --read-key --unknown --verbose -b 2>>tests/$FIFILE
# E-1157 parameter out of range
echo y,z | ./awsFillAndSign -Dtest=74 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw2.txt 2>>tests/$FIFILE
# Non-numeric replaceable parameter
echo y,z | ./awsFillAndSign -Dtest=76 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw3.txt 2>>tests/$FIFILE
# No headers
echo y,z | ./awsFillAndSign -Dtest=78 --read-key --verbose -DCLParam=%%test --from-file tests/test-raw5.txt 2>>tests/$FIFILE
if [ "$SHOW_MISC" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

echo "librock_safeAppend0 1" >librock_armAlternateBranch.txt
export FIFILE=coverageSA0.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
  do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo y,z | ./awsFillAndSign --read-key --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
  done
if [ "$SHOW_SA" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

echo "librock_safeAppend0 1" >librock_armAlternateBranch.txt
export FIFILE=coverageSA0two.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
  do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo y,z | ./awsFillAndSign --read-key --verbose -D missingCLParam=%%test --from-file tests/test-raw.txt param1@ 2>>tests/$FIFILE
  done
if [ "$SHOW_SA2" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

# upload file with failed malloc
echo 'librock_safeAppend0 1' >librock_armAlternateBranch.txt
export FIFILE=coverageSA0three.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
  done
if [ "$SHOW_SA3" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi



echo 'malloc 1' >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose aws-ec2-describe-instances.curl 2>>tests/$FIFILE
  done
if [ "$SHOW_MALLOC" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

echo "malloc 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc2.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo y,z | ./awsFillAndSign --read-key --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
  done
if [ "$SHOW_MALLOC2" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

# upload file with failed malloc
echo "malloc 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc3.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose aws-s3-put.curl bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
  done
if [ "$SHOW_MALLOC3" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

# stringListGetIndex failure. Uses realloc, not malloc.
echo "global 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc4.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose --from-file tests/test-raw10.txt 2>>tests/$FIFILE
  done
if [ "$SHOW_MALLOC4" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi


echo "global 1" >librock_armAlternateBranch.txt
export FIFILE=coverageGlobal.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose aws-ec2-describe-instances.curl 2>>tests/$FIFILE
  done
if [ "$SHOW_GLOBAL" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

echo "global 1" >librock_armAlternateBranch.txt
export FIFILE=coverageGlobal2.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
  do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo y,z | ./awsFillAndSign --read-key --verbose -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
  done
if [ "$SHOW_GLOBAL2" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

gcov -b awsFillAndSign.c
gcov -b hmacsha256.c
gcov -b librock_sha256.c
