#!/bin/sh

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 
# 3. The names "VOCAL", "Vovida Open Communication Application Library",
#    and "Vovida Open Communication Application Library (VOCAL)" must
#    not be used to endorse or promote products derived from this
#    software without prior written permission. For written
#    permission, please contact vocal\@vovida.org.
# 
# 4. Products derived from this software may not be called "VOCAL", nor
#    may "VOCAL" appear in their name, without prior written
#    permission of Vovida Networks, Inc.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
# NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
# IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
# 
# ====================================================================
# 
# This software consists of voluntary contributions made by Vovida
# Networks, Inc. and many individuals on behalf of Vovida Networks,
# Inc.  For more information on Vovida Networks, Inc., please see
# <http://www.vovida.org/>.

set -x

usage ()
{
    if [ $1 ]; then
	echo $1
    fi
    echo <<EOF
Usage:  $0 [ -d | -s ], where
    -d causes $0 to print debugging output, and
    -s asks $0 to suppress all messages
        (including output from the tar and rpm commands)
The default is to print output from tar and rpm, but not to print
debugging messages.

EOF
    if [ $2 ]; then
	exit $2
    else
	exit 1
    fi
}

DEBUG=1      #variable
SILENT=''    #defaults...

#GETOPT_COMPATIBLE=1
#OPTIONS=`getopt ds`

#eval set -- "$OPTIONS"
#while true; do
#    case $1 in
#	-d) DEBUG=1;  shift ;;
#	-s) SILENT=1; shift ;;
#	--) break;;
#    esac
#done

if [ [ $DEBUG ] && [ $SILENT ] ]; then
    usage 'It is contradictory to specify both -d and -s.'
fi

if [ $1 && $1 eq '--debug' ]; then
    DEBUG=1
    set -x
fi

VERSION=0.7
RELEASE=7

if [ $1 ]; then
    LOCAL_CVS=$1/vocal/tools
    shift
else
    LOCAL_CVS=..
fi

if [ ! `rpm -qa | fgrep perl-Tk` ]; then
    rpm -i /home/install/perl-Tk-800.015-1.i386.rpm
fi

TAR_COMMAND="tar czf /usr/src/redhat/SOURCES/tricorder-$VERSION-$RELEASE.tar.gz tricorder-$VERSION"
RPM_BUILD_COMMAND='rpm -ba tricorder.spec'
RPM_INSTALL_COMMAND="rpm -U --force ../RPMS/i386/tricorder-$VERSION-$RELEASE.i386.rpm"

if [ $SILENT ]; then
    TAR_COMMAND="$TAR_COMMAND > /dev/null"
    RPM_BUILD_COMMAND="$RPM_BUILD_COMMAND > /dev/null"
    RPM_INSTALL_COMMAND="$RPM_INSTALL_COMMAND > /dev/null"
fi

cd $LOCAL_CVS
 if [ $DEBUG ]; then echo "The version is $VERSION."; fi
mv Tricorder tricorder-$VERSION
$TAR_COMMAND
cp -f tricorder-$VERSION/tricorder.spec /usr/src/redhat/SPECS
mv tricorder-$VERSION Tricorder
cd /usr/src/redhat/SPECS
$RPM_BUILD_COMMAND
$RPM_INSTALL_COMMAND

if [ $DEBUG ]; then
    set +x
fi
