#!/bin/bash
#

MACHINE=$(uname).$(uname -m)

if [ -x ./bin.debug.${MACHINE} ]
then
    echo -e " ./bin.debug.${MACHINE}/ParsePairTest"

    ./bin.debug.${MACHINE}/ParsePairTest

    if [ $? -eq 0 ]; then
        exit 1;
    fi
    
    echo "
<configuration>
    <a/>
    <b> 
        This is b
    </b>
    <c>   </c>
    <foo>        bar              </foo>
</configuration>
" > test.xml    
     
    cat test.xml
     
    echo -e " ./bin.debug.${MACHINE}/ParsePairTest test.xml"

    ./bin.debug.${MACHINE}/ParsePairTest test.xml

    rm test.xml
    
    if [ ! $? -eq 0 ]; then
        exit 1;
    fi
    
    echo "
a
b: This is b # Yes
c
foo  = bar
A B C D
" > test.txt

    cat test.txt
    
    echo -e " ./bin.debug.${MACHINE}/ParsePairTest test.txt txt"

    ./bin.debug.${MACHINE}/ParsePairTest test.txt txt
    
    rm test.txt

    echo "---------------------------"
    echo -e " ./bin.debug.${MACHINE}/ParsePairStringTest"

    ./bin.debug.${MACHINE}/ParsePairStringTest
fi
