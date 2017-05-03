rm *.gcda
export FIFILE=coverageMisc.err
rm tests/$FIFILE

export AWS_DEFAULT_REGION=
export AWS_ACCESS_KEY_ID=
export AWS_SECRET_ACCESS_KEY=
export AWS_SECURITY_TOKEN=

./awsFillAndSign -Dtest=9 --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
export AWS_DEFAULT_REGION=us-east-1
./awsFillAndSign -Dtest=11 --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
export AWS_ACCESS_KEY_ID=AAAAAAAAAAAAAAAAAAAA
./awsFillAndSign -Dtest=13 --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
export AWS_SECRET_ACCESS_KEY=7666666666666666666666666666666666666666
./awsFillAndSign -Dtest=15 --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
./awsFillAndSign -Dtest=17b --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
export AWS_SECURITY_TOKEN=test_example_security_token
./awsFillAndSign -Dtest=17 --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE

./awsFillAndSign -Dtest=19 --list 2>>tests/$FIFILE
./awsFillAndSign -Dtest=19b --list aws-s3-get 2>>tests/$FIFILE
./awsFillAndSign -Dtest=19c --list aws-s3-get extra 2>>tests/$FIFILE

./awsFillAndSign -Dtest=21 --list aws-ec2-describe-instances >tests/awsFillAndSignTest.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=22 -b tests/awsFillAndSignTest.txt --from-file tests/awsFillAndSignTest.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=23 --list FileDoesNotExist 2>>tests/$FIFILE
./awsFillAndSign -Dtest=24 --help 2>>tests/$FIFILE
./awsFillAndSign -Dtest=25 --coverage 2>>tests/$FIFILE
./awsFillAndSign -Dtest=26 --verbose --curl aws-simpledb-list-domains 2>>tests/$FIFILE
./awsFillAndSign -Dtest=26b --verbose --from-file tests/test-curl-badv2.curl 2>>tests/$FIFILE
./awsFillAndSign -Dtest=26c --verbose --from-file tests/test-curl-badv2b.curl 2>>tests/$FIFILE
./awsFillAndSign -Dtest=26d --verbose --from-file tests/test-curl-v2.curl 2>>tests/$FIFILE
./awsFillAndSign -Dtest=26e --from-file tests/test-curl-v2.curl 2>>tests/$FIFILE

echo "" | ./awsFillAndSign -Dtest=27 --read-key - --help 2>>tests/$FIFILE
echo "" | ./awsFillAndSign -Dtest=28 --read-key --from-file FileDoesNotExist 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29 --list bad-template 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29b -DAWS_SERVICE_NAME=bad bad-template 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29c --list bad-template2 2>>tests/$FIFILE
./awsFillAndSign -Dtest=29d bad-template3 2>>tests/$FIFILE

echo "x/x,y,zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz" | ./awsFillAndSign -Dtest=30 --read-key aws-s3-list bucketname prefix 2>>tests/$FIFILE
./awsFillAndSign -Dtest=31 --verbose --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE

# Upload file
./awsFillAndSign -Dtest=34 -bFileDoesNotExist --curl aws-s3-put bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=35 -b tests/awsFillAndSignTest.txt --curl aws-s3-put bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=35b --curl aws-s3-put bucket tests/doesnotexist.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=36 --have-sha256 -DCLParam=test --from-file tests/test-raw.txt @param1 2>>tests/$FIFILE
./awsFillAndSign -Dtest=37 aws-s3-put.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=38 aws-s3-list.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=39a aws-s3-PUT-test.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE
./awsFillAndSign -Dtest=39b aws-s3-GET-test.curl bucket tests/awsFillAndSignTest.txt tests/test.txt 2>>tests/$FIFILE

#signature v2
./awsFillAndSign -Dtest=40a aws-v2-PUT-test.curl bucket err object
./awsFillAndSign -Dtest=40b aws-v2-PUT-test expr
./awsFillAndSign -Dtest=40c aws-v2-PUT-test2 expr
./awsFillAndSign -Dtest=40d aws-v2-PUT-test3 expr

# unexpected formats of credential scope
echo '""' | ./awsFillAndSign -Dtest=41 --read-key --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
echo x | ./awsFillAndSign -Dtest=42 --read-key --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
echo x/ | ./awsFillAndSign -Dtest=42b --read-key --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
echo x, | ./awsFillAndSign -Dtest=43 --read-key --curl aws-s3-list bucketname prefix 2>>tests/$FIFILE
echo x,y,z | ./awsFillAndSign -Dtest=44 --read-key -DCLParam=%%test --from-file tests/test-raw.txt param1 2>>tests/$FIFILE
echo x,zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz | ./awsFillAndSign -Dtest=44b --read-key -DCLParam=%%test --from-file tests/test-raw.txt param1 2>>tests/$FIFILE

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
echo y,z | ./awsFillAndSign -Dtest=66 --read-key --verbose --from-file tests/test-raw11.txt 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=66b --read-key --verbose --from-file tests/test-raw12.txt 2>>tests/$FIFILE
echo y,z | ./awsFillAndSign -Dtest=67 --read-key --verbose --from-file tests/test-post1.curl 2>>tests/$FIFILE

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

echo 'librock_safeAppend0 1' >librock_armAlternateBranch.txt
export FIFILE=coverageSA0three.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose --curl aws-s3-put bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
  done
if [ "$SHOW_SA3" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi



echo 'malloc 1' >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose --from-file tests/test-truncated-data2.curl 2>>tests/$FIFILE
  done
if [ "$SHOW_MALLOC" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

echo "malloc 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc2.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    echo y,z | ./awsFillAndSign --read-key --verbose -b tests/test-raw2.txt -DCLParam=%%test --from-file tests/test-raw2.txt param1@ 2>>tests/$FIFILE
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
    ./awsFillAndSign --verbose --curl aws-s3-put bucket tests/awsFillAndSignTest.txt test.txt 2>>tests/$FIFILE
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


# stringListGetIndex failure. Uses realloc, not malloc.
echo "malloc 1" >librock_armAlternateBranch.txt
export FIFILE=coverageMalloc5.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose --curl aws-simpledb-list-domains 2>>tests/$FIFILE
  done
if [ "$SHOW_MALLOC5" = "true" ]; then echo SHOWING tests/$FIFILE ; cat tests/$FIFILE; fi

echo "global 1" >librock_armAlternateBranch.txt
export FIFILE=coverageGlobal.err
rm tests/$FIFILE
while [ -f librock_armAlternateBranch.txt ]
do
    cat librock_armAlternateBranch.txt
    cat librock_armAlternateBranch.txt 2>>tests/$FIFILE
    ./awsFillAndSign --verbose --curl aws-ec2-describe-instances 2>>tests/$FIFILE
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
