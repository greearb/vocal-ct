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
#use LWP;
use Socket;
require "getopts.pl";

&Getopts("p");

if($opt_p) {
    $print = 1;
}
# multiplex based on To:, From:, and Call-Id:

&mkudpsocket(5060, S1);
#$tcp_socket = &create_connection("", 5060) || die "can't create connection to host: $!";

print "next\n";

while(1) {
    $rin = '';
    $max_fh = -1;
    vec($rin, fileno(S1), 1) = 1;
    vec($rin, fileno($tcp_socket), 1) = 1;

    print "wheel!\n" if ($print);

    if(select($rin, undef, undef, undef)) {
	@bits = split(//, unpack("b*", $rin));
	undef($data);
	undef($new_data);
	print "in\n" if ($print);
	if($bits[fileno(S1)]) {
	    $his_address = &handle_udp(S1, $tcp_socket);
	}
	if($bits[fileno($tcp_socket)]) {
	    &handle_tcp($tcp_socket, S1, $his_address);
	}
    }
}

sub handle_udp {
    my($udp_socket, $tcp_socket) = @_;
    my($his_address);
    my($data);

    print "handling udp\n" if ($print);

    $his_address = recv($udp_socket, $data, 60000, 0);
    if($data) {
        if($print) {
	print "---------- UDP ----------\n";
	print $data;
	print "---------- --- ----------\n";
        }
	open(JUNK, ">junk.txt");
	print JUNK $data;
	close(JUNK);
    }
    &post_data_tcp($tcp_socket, $data);
    return $his_address;
}


sub handle_tcp {
    my($tcp_socket, $udp_socket, $udp_address) = @_;
    my($data);
    print "handling tcp\n" if ($print);

    $data = &recv_data_tcp($tcp_socket);
    if($data) {
        if($print) {
	print "++++++++++ TCP ++++++++++\n";
	print $data;
	print "---------- --- ----------\n";
        }
	open(JUNK, ">junk.txt");
	print JUNK $data;
	close(JUNK);
    }
#    &post_data_udp($udp_socket, $data, $udp_address);
}


sub post_data_udp {
    my($socket, $data, $his_address) = @_;

    ($hisport, $hisiaddr) = sockaddr_in($his_address);
    $hishost = inet_ntoa($hisiaddr);
    print "returns: $hishost:$hisport:\n----$data\n----\n" if ($print);
    ($sin) = pack_sockaddr_in(5060, $hisiaddr);
    
#		defined(send(S1,$return_data,0,$sin)) || die $!;
    defined(send($socket, $data, 0,$sin)) || die "send: $!";
    $them_S2 = pack($sockaddr,PF_INET, 5060, 192.168);
    print "returns: done\n" if ($print);
    return;
}

sub recv_data_tcp {
    my($socket) = @_;

    my($headers) = "";
    my($body) = "";
    my($got_full_msg);
    my($msg);

    # wait for the appropriate line


    # message states:
    # 0 -- headers
    # 1 -- got all headers

    my($msg_state) = 0; 

    my($buf);

    while(($msg_state == 0)  && ($_ = <$socket>)) {
	$headers .= $_;
	if(/^\s*$/) {
	    $msg_state = 1;
	}
    }

    if(!$msg_state) {
	print "at eol: error!\n";
    } else {
	if($print) {
	print "got headers!\n";
	print "----------\n";
	print $headers;
	print "----------\n";
	}
    }

    # get the body if there's still data

    # get content-length
    my($body_length) = 0;
    if($headers =~ /(^|[\r\n])content\-length\:\s*([0-9]+)/i) {
	print $2, "\n" if ($print);
	$body_length = $2;
    } elsif ($headers =~ /(^|[\r\n])l:\s*([0-9]+)/i) {
	print $2, "\n" if($print);
	$body_length = $2;
    }

    print "TCP: getting $body_length bytes of body\n" if($print);

    my($bytes_to_read) = $body_length;
    my($actual_bytes_read);

    while($bytes_to_read) {
	$actual_bytes_read = read($socket, $data, $bytes_to_read);
	if(!$actual_bytes_read) {
	    print "no bytes in body: error!\n";
	    $bytes_to_read = 0;
	} else {
	    $bytes_to_read = $bytes_to_read - $actual_bytes_read;
	}
    }

    $body = $data;
    if($print) {
    print "got body!\n";

    print "----------\n";
    print $body;
    print "----------\n";
    }

    $msg = $headers . $body;
    return $msg;
}

# usage $socket = create_connection("hostname", port);

sub create_connection {
    my($addr, $port) = @_;

    my($remote);
    $remote = IO::Socket::INET->new( Proto     => "tcp",
				     PeerAddr  => $addr,
				     PeerPort  => $port,
				   );
    return ($remote);
}

sub post_data_tcp {
    my($socket, $request) = @_;

    if($print) {
    print "TCP:\n";
    print "----\n$request\n----\n";
    print "sending: start\n";
    }
    if($socket) {
        my($bytes) = print $socket $request;
    }
    if(!$bytes) {
#	print "write error: $!\n";
    } else {
	print "bytes written: $bytes\n" if ($print);
    }

    print "sending: done\n" if($print);
}

sub mkudpsocket {
  local($port, $socket) = @_;
  
  ($name,$aliases,$proto) = getprotobyname('udp');
  socket($socket, PF_INET, SOCK_DGRAM, $proto) || die "socket: $!";
  $paddr = sockaddr_in($port, INADDR_ANY); # 0 means let kernel pick
  bind($socket , $paddr) || die "died with port $port $!";
  
}
