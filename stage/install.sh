#! /bin/sh

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


echo -n "Creating vocal software directory /usr/local/vocal ..."
mkdir -p /usr/local/vocal
echo "done"
echo -n "Cleaning out old Vovida software, if it exists ..."
rm -rf /usr/local/vocal/bin
echo "done"
echo -n "Copying Vovida software to /usr/local/vocal ..."
cp -r bin /usr/local/vocal/bin
mkdir -p /usr/local/vocal/etc
cp -r etc/* /usr/local/vocal/etc
echo "done"

echo -n "Making log directory /usr/local/vocal/log ..."
mkdir -p /usr/local/vocal/log
echo "done"

echo -n "Copying system startup script vocalstart to /etc/rc.d/init.d ..."
cp /usr/local/vocal/bin/vocalstart /etc/rc.d/init.d
echo "done"
if [ ! -f  /etc/rc.d/rc3.d/S90vocalstart ]
then
    echo -n "Setting system to automatically start Vovida software on boot (rc3.d) ..."
    ln -s /etc/rc.d/init.d/vocalstart /etc/rc.d/rc3.d/S90vocalstart
    echo "done"
fi
if [ ! -f  /etc/rc.d/rc5.d/S90vocalstart ]
then
    echo -n "Setting system to automatically start Vovida software on boot (rc5.d) ..."
    ln -s /etc/rc.d/init.d/vocalstart /etc/rc.d/rc5.d/S90vocalstart
    echo "done"
fi

if [ ! -f /usr/local/vocal/etc/vocal.conf ]
then
    if [ -f /usr/local/vocal/etc/vocal.conf.`hostname` ]
    then
	echo "Copying config file for `hostname` to"
	echo -n "    /usr/local/vocal/etc/vocal.conf ..."
	cp /usr/local/vocal/etc/vocal.conf.`hostname` /usr/local/vocal/etc/vocal.conf
	echo "done"
    fi
fi
