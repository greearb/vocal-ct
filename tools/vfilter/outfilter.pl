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

use Fcntl;
#chdir; # go home

$pipeName = "sip-outpipe";

$IN_FIFO = "$pipeName.out";
$IN_CTL_FIFO = "$pipeName.out.ctl";
$OUT_FIFO = "$pipeName.in";
$OUT_CTL_FIFO = "$pipeName.in.ctl";


unless (-p $IN_FIFO) {
    unlink $IN_FIFO;
    system('mknod', $IN_FIFO, 'p')
      && die "can't mknod $IN_FIFO: $!";
}

unless (-p $IN_CTL_FIFO) {
    unlink $IN_CTL_FIFO;
    system('mknod', $IN_CTL_FIFO, 'p')
      && die "can't mknod $IN_FIFO: $!";
}

unless (-p $OUT_FIFO) {
    unlink $OUT_FIFO;
    system('mknod', $OUT_FIFO, 'p')
      && die "can't mknod $OUT_FIFO: $!";
}

unless (-p $OUT_CTL_FIFO) {
    unlink $OUT_CTL_FIFO;
    system('mknod', $OUT_CTL_FIFO, 'p')
      && die "can't mknod $OUT_FIFO: $!";
}

sysopen (IN_FIFO, $IN_FIFO, O_RDWR) || die "can't read $IN_FIFO: $!";
sysopen (OUT_FIFO, $OUT_FIFO, O_RDWR) || die "can't write $OUT_FIFO: $!";
sysopen (IN_CTL_FIFO, $IN_CTL_FIFO, O_RDWR) || die "can't read $IN_CTL_FIFO: $!";
sysopen (OUT_CTL_FIFO, $OUT_CTL_FIFO, O_RDWR) || die "can't write $OUT_CTL_FIFO: $!";

sub readOk {

    $rin = $win = $ein = '';
    vec($rin,fileno(IN_FIFO),1) = 1;
    vec($rin,fileno(IN_CTL_FIFO),1) = 1;

    ($nfound,$timeleft) =
      select($rout=$rin, $wout=$win, $eout=$ein, $timeout);

    @bits = split(//, unpack("b*", $rout));

    if($bits[fileno(IN_CTL_FIFO)]) {
	# this is a reset of some sort, so trash everything and punt
	
    }

    

}

sub recMsg {
    local($msg);

    # use select?


    # next line blocks until there's a reader
    print "reading header\n";

    # check for reset messages in case something gets restarted
    $total_bytes_read = 0;
    $msg = "";

    while($total_bytes_read < 15) {
	$total_bytes_read += sysread(IN_FIFO, $b, 15 - $total_bytes_read);
	if($total_bytes_read == 0) {
	    # do something here 
#	    print "nothing!\n";
	}
	$msg .= $b;
    }

    if ($total_bytes_read < 15) {
#	print "foo!\n";
    }


    # parse for number of bytes to read
    chomp($msg);
    ($cmd, $bytes_to_read) = split(/ /, $msg, 2);
#    print "done reading header\n";
#    print "reading ($bytes_to_read bytes)\n";
    $total_bytes_read = 0;
    $msg = "";
    while($total_bytes_read  < $bytes_to_read) {
	$bytes = sysread(IN_FIFO, $b, 
			 $bytes_to_read - $total_bytes_read);
	if($bytes == 0) {
	    # do something here 
#	    print "nothing!\n";
	}
	$msg .= $b;
	$total_bytes_read += $bytes;
    }

    if ($total_bytes_read < $bytes_to_read) {
#	print "foo!\n";
    }

#    print "done reading ($total_bytes_read bytes)\n";

#    print "got: $msg\n";
    return $msg;
}


sub sendMsg {
    local($msg) = $_[0];
    local($encodedMsg);
    local($total_bytes_written) = 0;
    local($header);
    local($bytes_to_write);
    local($bytes);

#    print "write buf: $header\n";

    $header = sprintf("msg %10.10d\n", length($msg));
    $encodedMsg = $header . $msg;

    $bytes_to_write = length($encodedMsg);

    while($total_bytes_written < $bytes_to_write) {
	$bytes  = syswrite(OUT_FIFO, $encodedMsg, 
			   length($encodedMsg) - $total_bytes_written, 
			   $total_bytes_written);
	if(!$bytes) {
	    die "error writing $OUT_FIFO: $!";
	}
	$total_bytes_written += $bytes;
    }
}


######################################################################

#  everything above this line should not need to be changed

######################################################################

# doing this resets the filter software and stops any waiting by the
# other side

$msg = "a";
syswrite(OUT_CTL_FIFO, $msg, length($msg));


# here is the infinite loop in question

while (1) {
    print "waiting for message\n";
    $buf = recMsg();
    print "got msg... parsing and retransmitting\n";
    print "buf:\n$buf\n----";  # uncomment this to print out buffers before they
                            # are modified
    $buf = &filter($buf);
    print "newbuf:\n$buf\n!!!!";  # uncomment this to print out buffers before they
                            # are modified
    sendMsg($buf);
}


# below this is the filter

sub filter {
    local($_) = $_[0];

    # put filter commands here
    # for more information about perl regular expressions, do 
    #
    #     man perlre
    # here are some examples
    # s/hello/goodbye/; # replace hello with goodbye at most once per message
    # s/true/false/g; # replace true with false as many times as possible
    # s/upper/lower/i; # replace upper with lower without checking case
    # s/^From/To/; # replace a with b at the start of the line only
    # y/\r//d; # this should kill carriage returns  (y means use tr syntax)

    #s/aaln_([0-1])\@galileo.private.vovida.com/aaln_\1\@192.168.5.1/g;

    #s/aaln_0\@galileo.private.vovida.com/6396\@192.168.5.1/g;
    s/aaln_0\@galileo.private.vovida.com/6249\@192.168.5.1/g;

    s/([0-9])([0-9])([0-9])([0-9])\@galileo.private.vovida.com/\1\2\3\4\@192.168.5.1/g;
    s/UDP galileo.private.vovida.com/UDP 192.168.5.1/g;

    #s/([0-9])([0-9])([0-9])([0-9])\@yoyo.private.vovida.com/\1\2\3\4\@192.168.5.1/g;
    #s/UDP yoyo.private.vovida.com/UDP 192.168.5.1/g;
    #s/([0-9])([0-9])([0-9])([0-9])\@frisbee.private.vovida.com/\1\2\3\4\@192.168.5.1/g;
    #s/UDP frisbee.private.vovida.com/UDP 192.168.5.1/g;


    # Since our gateway puts dns name into the IN IP4 address field,
    # convert it to an IP addresses for now until our gateway is fixed.
    # Order is IMPORTANT !!! 
    s/IN IP4 galileo.private.vovida.com/IN IP4 192.168.5.1/g;
    #s/IN IP4 yoyo.private.vovida.com/IN IP4 192.168.20.15/g;
    #s/IN IP4 frisbee.private.vovida.com/IN IP4 192.168.20.16/g;

    {   
        if (/^ACK /)
        {
            # We changed this line's IP address when the previous msg came in,
            # since our stack incorrectly used this line instead of the c= line.
            # Changed this back for the next message for consistency.
            s/o=Pingtel 5 5 IN IP4 192\.168\.5\.11[0-3]/o=Pingtel 5 5 IN IP4 127.0.0.1/;
        }
        else
        {
            # If this is any 1xx response, add a Content-Length: 0 line.
            # This may no longer be needed
            if (/^SIP\/2.0 1/)
            {
                s/CSeq: 1 INVITE\r\n\r\n/CSeq: 1 INVITE\r\nContent-Length: 0\r\n\r\n/;
            }
        }
    }

    if(/v=0/)
    {
        # this is an SDP
        s/\r\r\n/\r\n/g;        
#	print "this message has an SDP component!\n";

    }


    # recalculate the content length

    if(/(v=0(.|\n)*)/)
    {
        print "xxx gotcha!\n";
        $len = length($1);
        s/Content-Length: [0-9]*/Content-Length: $len/;
    }
    # don't change this line

    return $_;
}
