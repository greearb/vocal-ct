#!/usr/bin/perl

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

# simple script to manage UDP to TCP for SIP connections

use IO::Socket;
use LWP;
use Socket;

# multiplex based on To:, From:, and Call-Id:

&mksocket(5060, S1);

while(1) {
    $rin = '';
    vec($rin, fileno(S1), 1) = 1;
    
    if(select($rin, undef, undef, undef)) {
	@bits = split(//, unpack("b*", $rin));
	undef($data);
	undef($new_data);
	if($bits[fileno(S1)]) {
	    $data = "";
	    $his_address = recv(S1, $data, 60000, 0);
	    if($data) {
		open(JUNK, ">junk.txt");
		print JUNK $data;
		close(JUNK);
		$new_data = &post_data_tcp($data);
		&post_data_udp(S1, $new_data, $his_address);
	    }
	}
    }
}

sub post_data_udp {
    my($socket, $data, $his_address) = @_;

    ($hisport, $hisiaddr) = sockaddr_in($his_address);
    $hishost = inet_ntoa($hisiaddr);
    print "returns: $hishost:$hisport:\n----$data\n----\n";
    ($sin) = pack_sockaddr_in(5060, $hisiaddr);
    
#		defined(send(S1,$return_data,0,$sin)) || die $!;
    defined(send($socket, $data, 0,$sin)) || die "send: $!";
    $them_S2 = pack($sockaddr,PF_INET, 5060, 192.168);
    print "returns: done\n";
    return;
}

sub post_data_tcp {
    my($request) = $_[0];
    my($content) = "";

    print "sending:\n";
    print "----\n$request\n----\n";
    print "sending: start\n";

    $remote = IO::Socket::INET->new( Proto     => "tcp",
				     PeerAddr  => "newton",
				     PeerPort  => "5060",
				   );

    print $remote $request;
    while(<$remote>) {
	$content .= $_;
    }
    close($remote);
    print "sending: done\n";
    print "----\n$content\n----\n";
    return $content;
}

sub mksocket {
  local($port, $socket) = @_;
  
  ($name,$aliases,$proto) = getprotobyname('udp');
  socket($socket, PF_INET, SOCK_DGRAM, $proto) || die "socket: $!";
  $paddr = sockaddr_in($port, INADDR_ANY); # 0 means let kernel pick
  bind($socket , $paddr) || die "died with port $port $!";
  
}
