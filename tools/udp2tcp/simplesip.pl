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

# $Id: simplesip.pl,v 1.1 2004/05/01 04:15:31 greear Exp $

# simple script to manage UDP to TCP for SIP connections

use Test2;
use IO::Socket;
use Socket;
use Data::Dumper;

&mkudpsocket(5060, S1);

$local_addr = "192.168.5.31";
$code_text{"180"} = "Ringing";
$code_text{"200"} = "OK";

sub start_rtp {
    local($rtp) = @_;
    open($rtp, "| ./rtpSimple > rtpsimple.log") 
      || die "can't start rtpSimple: $!";

    # unbuffer the filehandle
    $a = select($rtp);
    $| = 1;
    select($a);
}

$rtp = "RTP";

&start_rtp($rtp);

while(1) {
    $rin = '';
    $max_fh = -1;
    vec($rin, fileno(S1), 1) = 1;

#    print "wheel!\n";

    if(select($rin, undef, undef, 10)) {
	@bits = split(//, unpack("b*", $rin));
	undef($data);
	undef($new_data);
#	print "in\n";
	if($bits[fileno(S1)]) {
	    $his_address = &handle_udp(S1, $tcp_socket);
	}
    } else {
	# nothing, so bye!
#	&bye_udp(S1);
    }
}

sub bye_udp {
    if(!$last_invite) {
	return;
    }
    local($bye) = &generate_bye($last_invite);

#    print "bye:\n";
#    print $bye;
#    print "\n";
    if(length($bye)) {
	&post_data_udp(S1, $bye, $his_address);
    }
    $last_invite = "";
}

sub handle_udp {
    my($udp_socket, $tcp_socket) = @_;
    my($his_address);
    my($data);

#    print "handling udp\n";

    $his_address = recv($udp_socket, $data, 60000, 0);
    if($data) {
#	print "---------- UDP ----------\n";
#	print $data;
#	print "---------- --- ----------\n";
	open(JUNK, ">junk.txt");
	print JUNK $data;
	close(JUNK);
    }

    $data = &filter_udp2tcp($data);

    if(length($data)) {
	&post_data_udp(S1, $data, $his_address);
    }
    return $his_address;
}


sub post_data_udp {
    my($socket, $data, $his_address) = @_;

    ($hisport, $hisiaddr) = sockaddr_in($his_address);
    $hishost = inet_ntoa($hisiaddr);
#    print "returns: $hishost:$hisport:\n----$data\n----\n";
    ($sin) = pack_sockaddr_in(5060, $hisiaddr);
    
#		defined(send(S1,$return_data,0,$sin)) || die $!;
    defined(send($socket, $data, 0,$sin)) || die "send: $!";
    $them_S2 = pack($sockaddr,PF_INET, 5060, 192.168);
#    print "returns: done\n";
    return;
}


sub mkudpsocket {
  local($port, $socket) = @_;
  
  ($name,$aliases,$proto) = getprotobyname('udp');
  socket($socket, PF_INET, SOCK_DGRAM, $proto) || die "socket: $!";
  $paddr = sockaddr_in($port, INADDR_ANY); # 0 means let kernel pick
  bind($socket , $paddr) || die "died with port $port $!";
  
}


sub filter_udp2tcp {
    local($_) = @_;

    if(/^invite/i) {
#	$_ = &respond($_, 180, "", "");
	# respond w/ 200, natch!
	$last_invite = $_;
	$hostport = &process_sdp($_);
	($remote_host, $remote_port) = split(/:/, $hostport);
	$_ = &respond($_, 200, &make_sdp($local_addr, "3466"), "application/sdp");
	print $rtp "n 3466 $remote_host $remote_port\n";

#    local($hostip) = "192.168.5.3";
#    local($port) = "3466";
    } else {
	if(/^sip/i) {

	} elsif(/^ack/i) {
	    $_ = "";
#	    print RTP "p sample.raw\n";
#	    print RTP "r record.raw\n";
	    print $rtp "v p:sample.raw p:beep.raw r:record.raw\n";
	} elsif(/^bye/i) {
	    $_ = &respond($_, 200, "", "");
	    $last_invite = "";
	    print $rtp "d\n";
	    system("sleep 1; sox -r 8000 -U -b record.raw record.au &");
	} else {
	    $_ = &respond($_, 200, "", "");
	}
    }

#    print Dumper(&turntohash($_));

#    print $_;

    return $_;
}

sub respond {
    local($old_msg, $code, $content, $content_type) = @_;
    local($msg);

    # take an incoming message, and form1 an outgoing message from it
    $msg = "SIP/2.0 $code $code_text{$code}\r\n";
    $msg .= &strip_extraneous($old_msg);
    $msg = &add_content($msg, $content, $content_type);
}

sub generate_bye {
    # byes are a special case -- they need a different CSeq, and the
    # From/To may need to be reversed as appropriate

    local($invite_msg) = @_;

    local($bye_msg);

    $bye_msg = &inc_cseq($invite_msg, "BYE");

    local($msg_hash) = &turntohash($bye_msg);

    local($old_from) = $msg_hash->{"from"};
    local($old_to) = $msg_hash->{"to"};

    $bye_msg = "BYE $msg_hash->{MSG_URL} SIP/2.0\r\n" 
      . &strip_extraneous($bye_msg);

#    print $bye_msg;

    if($old_from =~ /$local_addr[^0-9]/) {
    } else {
	$bye_msg = &replace_header($bye_msg, "from", $old_to);
	$bye_msg = &replace_header($bye_msg, "to", $old_from);
    }

    print $rtp "d\n";

    # do something here

    return $bye_msg;
}

sub replace_header {
    local($msg, $header, $new_contents) = @_;

    @split_msg = split(/\r?\n/, $msg);
    $linecount = 0;
    local($new_msg) = "";
    $past_headers = 0;

    $header =~ y/A-Z/a-z/;

    foreach(@split_msg) {
	if($linecount == 0) {
	    $new_msg .= $_ . "\r\n";
	} else {
	    if(/^[\s\r\n]*$/) {
		$past_headers = 1;
	    }
	    if (!$past_headers) {
		($header_name, $header_body) = split(/\:/, $_, 2);
		$header_name =~ y/A-Z/a-z/;
		if($header_name eq $header) {
		    # do something!
		    $header_body = $new_contents;
		}
		$new_msg .= $header_name . ":" . $header_body . "\r\n" ;
	    } else {
		# gobble everything else and put it into a thingy
		$new_msg .= $_ . "\r\n";
	    }
	}
	$linecount++;
    }
    return $new_msg;
}


sub turntohash {
    local($msg) = @_;
    local($hash);

    $hash = {};

    @split_msg = split(/\r?\n/, $msg);
    $linecount = 0;
    $newmsg = "";
    $past_headers = 0;

    $header =~ y/A-Z/a-z/;

    foreach(@split_msg) {
	if($linecount == 0) {
	    $hash->{"FIRST_LINE"} = $_;
	    # figure out if this is a request or a response
	    @stuff = split(/\s+/);
	    if($stuff[0] !~ /^SIP/) {
		$stuff[0] =~ y/A-Z/a-z/;
		$stuff[2] =~ y/A-Z/a-z/;
		$hash->{"MSG_TYPE"} = $stuff[0];
		$hash->{"MSG_URL"} = $stuff[1];
		$hash->{"SIP_VERSION"} = $stuff[2];
	    } else {
		$stuff[0] =~ y/A-Z/a-z/;
		$stuff[2] =~ y/A-Z/a-z/;
		$hash->{"SIP_VERSION"} = $stuff[0];
		$hash->{"MSG_TYPE"} = "response";
		$hash->{"RESPONSE_CODE"} = $stuff[1];
		$hash->{"RESPONSE_TEXT"} = $stuff[2];
	    }
	} else {
	    if(/^[\s\r\n]*$/) {
		$past_headers = 1;
		next;
	    }
	    if (!$past_headers) {
		($header_name, $header_body) = split(/\:/, $_, 2);
		$header_name =~ y/A-Z/a-z/;
		$hash->{$header_name} = $header_body;
	    } else {
		# gobble everything else and put it into a thingy
		$hash->{"CONTENTS"} .= $_ . "\r\n";
	    }
	}
	$linecount++;
    }
    return $hash;
}


sub turntomsg {
    local($hash) = @_;
    local($msg);

    if($hash->{"MSG_TYPE"} eq "response") {
	# this is a response
	
    } else {


    }
}



sub inc_cseq {
    local($msg, $type) = @_;

    local($stuff, $num);

    $msg =~ s/cseq:\s*(.*)\r?\n/CSeq: \r\n/i;
    $stuff = $1;
    $stuff =~ /^([0-9]+)/;
    $num = $1; 
    $num++;
    $msg =~ s/CSeq\: \r\n/CSeq: $num $type\r\n/;
    return $msg;
}


sub process_sdp {
    local($sdp) = @_;

    local($host, $port);

    # chomp out the appropriate stuff
    if(/o\=.*IP4\s+([0-9\.]+)/) {
	$host = $1;
    }
    if(/m\=.*audio\s+([0-9]+)/) {
	$port = $1;
    }
    return "$host:$port";
}



sub make_sdp {
    local($hostip, $port) = @_;
#    local($hostip) = "192.168.5.3";
#    local($port) = "3466";
    local($sdp_info);

    $sdp_info  = "v=0\r\n";
    $sdp_info .= "o=- 305743370 305743370 IN IP4 $hostip\r\n";
    $sdp_info .= "s=VOVIDA Session\r\n";
    $sdp_info .= "c=IN IP4 $hostip\r\n";
    $sdp_info .= "t=957844468 0\r\n";
    $sdp_info .= "m=audio $port RTP/AVP 0\r\n";
    $sdp_info .= "a=rtpmap:0 PCMU/8000\r\n";
    $sdp_info .= "a=ptime:40\r\n";

    return $sdp_info;
}

sub add_content {
    local($msg, $content, $content_type) = @_;

#    print "content: \n", $content, "\n ---------- \n";

    if(!$content) {
	$msg .= "Content-Length: 0\r\n\r\n";
    } else {
	$msg .= "Content-Type: $content_type\r\n";
	$msg .= "Content-Length: " . length($content) . "\r\n";
	$msg .= "\r\n";
	$msg .= $content;
    }
    return $msg;
}


sub strip_extraneous {
    local($msg) = $_[0];
    # take an incoming message, and form an outgoing message from it

    @split_msg = split(/\r?\n/, $msg);
    $linecount = 0;
    $newmsg = "";
    $past_headers = 0;

    foreach(@split_msg) {
	if($linecount == 0) {
	    if(/^SIP\/2\.0/ || /^INVITE/i || /^BYE/i || /^ACK/i || /^CANCEL/i) {
	    # get rid of this
	    } else {
		$newmsg .= $_ . "\r\n";
	    }
	} else {
	    if(/^[\s\r\n]*$/) {
#		print "past headers!\n\n";
		$past_headers = 1;
	    } else {
#		print "ok:$linecount:$_:\n";
	    }
	    if(!/^content-length:/i 
	       && !/^content-type:/i 
	       && !($past_headers)) {
#		print "got $_\n";
		$newmsg .= $_ . "\r\n";
	    }
	}

	$linecount++;
    }
    return $newmsg;
}



__END__

