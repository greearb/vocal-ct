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
#    permission, please contact vocal@vovida.org.
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

# $Id: vocald-rc.sh,v 1.1 2004/05/01 04:15:41 greear Exp $

# this is a simple redhat-style RC script for VOCAL.  it uses vocalctl
# to do all of its dirty work, so it should not need to be too smart.

# redhat magic
# chkconfig: - 85 15
# processname: /usr/local/vocal/bin/vocald
# config: /usr/local/vocal/etc/vocal.conf
# description: VOCAL (Vovida Open Communication Application Library) \
#              is a voice over IP call control system based on the \
#              SIP protocol.  It controls and routes VoIP calls between \
#              multiple users and gateways.

. /etc/init.d/functions

VOCAL_BASE=/usr/local/vocal # VOCAL_BASE
VOCAL_BASE_CONFIGURED=/usr/local/vocal
if [ "x${VOCAL_BASE_CONFIGURED}" != "x@""prefix@" ]
then
    VOCAL_BASE=${VOCAL_BASE_CONFIGURED}
fi

prog="vocald"

case "$1" in
  start)
	echo -n "Starting $prog:"
	${VOCAL_BASE}/bin/vocalctl -n start
	RETVAL=$?
	[ "$RETVAL" -eq 0 ] && success "$prog startup" || failure "$prog startup"
	echo
        ;;
  stop)
	echo -n "Stopping $prog:"
	${VOCAL_BASE}/bin/vocalctl -n stop
	RETVAL=$?
	[ "$RETVAL" -eq 0 ] && success "$prog shutdown" || failure "$prog shutdown"
	echo
        ;;
  status)
	${VOCAL_BASE}/bin/vocalctl status
	RETVAL=0
        ;;
  restart)
	echo -n "Stopping $prog:"
	${VOCAL_BASE}/bin/vocalctl -n stop
	RETVAL=$?
	[ "$RETVAL" -eq 0 ] && success "$prog shutdown" || failure "$prog shutdown"
	echo
	echo -n "Starting $prog:"
	${VOCAL_BASE}/bin/vocalctl -n start
	RETVAL=$?
	[ "$RETVAL" -eq 0 ] && success "$prog startup" || failure "$prog startup"
	echo
        ;;
  *)
        echo $"Usage: $prog {start|stop|status|restart}"
        exit 1
esac

exit $RETVAL
