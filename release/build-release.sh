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

if [ "z$#" != "z2" ]
then
	echo "usage: $0 <mgcp|rtp|sip> <official|practice>"
	echo "where mgcp/rtp/sip is the name of the directory to tag"
	echo "use official if you want to make a real release (this will tag)"
	echo "use practice if you are just practicing (will not tag)"
	exit
fi

. release-versions.sh

if [ "z$2" = "zofficial" ]
then
	RTP_NAME=rtp-${RTP_VERSION}
	MGCP_NAME=mgcp-${MGCP_VERSION}
	SIP_NAME=sip-${SIP_VERSION}
else 
    if [ "z$2" = "zpractice" ]
    then
	RTP_NAME=rtp-${RTP_VERSION}-unofficial
	MGCP_NAME=mgcp-${MGCP_VERSION}-unofficial
	SIP_NAME=sip-${SIP_VERSION}-unofficial
    else
	echo "use official or practice only."
	exit
    fi
fi

rm -r vovida
cvs checkout vovida
( cd vovida ; make docdir ; rm -f doc/freebx )
( cd vovida ; echo `whoami` | make co )

build=0

# should tag here

if [ "zrtp" = "z$1" ]
then
	if [ "zofficial" = "z$2" ]
	then
	release_tag=`echo RTP_${RTP_VERSION}_RELEASE | tr .- __`
	(cd vovida ; cvs tag -F ${release_tag})
	fi
	build=1
fi

if [ "zmgcp" = "z$1" ]
then
	if [ "zofficial" = "z$2" ]
	then
	release_tag=`echo MGCP_${MGCP_VERSION}_RELEASE | tr .- __`
	(cd vovida ; cvs tag -F ${release_tag})
	fi
	build=1
fi

if [ "zsip" = "z$1" ]
then
	if [ "zofficial" = "z$2" ]
	then
	release_tag=`echo SIP_${SIP_VERSION}_RELEASE | tr .- __`
	(cd vovida ; cvs tag -F ${release_tag})
	fi
	build=1
fi

if [ $build = "0" ]
then
	echo "must be one of mgcp, rtp, sip"
	exit
fi
find vovida -name 'CVS' -type d -print | xargs rm -r


# make documentation

( cd vovida ; make doc; make doc )

# get rid of the .d files

find . -name 'obj.debug.*' -type d -print | xargs rm -r

rm -r vovida/private
rm -r vovida/freebx
rm -r vovida/provisioning
rm -r vovida/snmp
rm -r vovida/cdr
rm -r vovida/private-tools
rm -r vovida/tools/vmclient
rm -r vovida/h323
rm -r vovida/contrib/hitachi
rm -r vovida/contrib/hitachi-old
rm -r vovida/release
rm -r vovida/sip/SipProxy
rm -r vovida/old
rm -r vovida/pcontrib
rm -r vovida/policy
rm -r vovida/digimon
rm -r vovida/contrib/equivalence
rm -r vovida/doc
rm -rf mgcp rtp sip


mkdir mgcp rtp sip

if [ "zmgcp" = "z$1" ]
then
	cp -r vovida mgcp 
	(cd mgcp; rm -rf vovida/sip ; rm -rf vovida/snmp)
	(cd mgcp; mv vovida ${MGCP_NAME} ; tar -zcf ${MGCP_NAME}.tar.gz ${MGCP_NAME} ; mv ${MGCP_NAME}.tar.gz ..)
	cp mgcp/${MGCP_NAME}/mgcp/README README-mgcp-${MGCP_VERSION}.txt
fi

if [ "zrtp" = "z$1" ]
then
	cp -r vovida rtp
	(cd rtp ; rm -fr vovida/mgcp vovida/sdp vovida/sip )
	(cd rtp; mv vovida ${RTP_NAME} ;  tar -zcf ${RTP_NAME}.tar.gz ${RTP_NAME} ; mv ${RTP_NAME}.tar.gz ..)
	cp rtp/${RTP_NAME}/rtp/README README-rtp-${RTP_VERSION}.txt
fi

if [ "zsip" = "z$1" ]
then
	cp -r vovida sip
	(cd sip; rm -rf vovida/mgcp)
	(cd sip; mv vovida ${SIP_NAME} ; tar -zcf ${SIP_NAME}.tar.gz ${SIP_NAME} ; mv ${SIP_NAME}.tar.gz ..)
	cp sip/${SIP_NAME}/sip/sipstack/README README-sip-${SIP_VERSION}.txt
fi

