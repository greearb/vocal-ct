#!/bin/bash
#

MACHINE=$(uname).$(uname -m)

if [ -x ./bin.debug.${MACHINE} ]
then
echo -e " ./bin.debug.${MACHINE}/GetOptTest This is a -a -b -c foo -d bar -0 -1 -2 -x -xa --add 1 --append --close --delete me --verbose --file foo.txt --close=foo.bar test"

    ./bin.debug.${MACHINE}/GetOptTest \
	This is a  \
	-a \
	-b \
	-c foo \
	-d bar \
	-0 \
	-1 \
	-2 \
	-x \
        -xa \
	--add 1 \
        --append \
	--close \
	--delete me \
	--verbose \
	--file foo.txt \
	--close=foo.bar test
fi
