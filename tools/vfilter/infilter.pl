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

$pipeName = "sip-inpipe";

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
#    print "reading header\n";

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
    print "buf:\n$buf\n----\n";  # uncomment this to print out buffers before they
                            # are modified
    $buf = &filter($buf);
    print "newbuf:\n$buf\n!!!!\n";  # uncomment this to print out buffers before they
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
    # s/hello/goodbye/; # replace hello with goodbye at most once per line
    # s/^From/To/; # replace a with b at the start of the line only
    # y/\r//d; # this should kill carriage returns  (y means use tr syntax)

    if(/v\=0/) {
        # has an SDP

        # Our stack should not be looking at the o= line.
        if (/c=IN IP4 192.168.5.110/)
        {
            s/o=Pingtel 5 5 IN IP4 127\.0\.0\.1/o=Pin 5 5 IN IP4 192.168.5.110/;
        }
        elsif (/c=IN IP4 192.168.5.111/)
        {
            s/o=Pingtel 5 5 IN IP4 127\.0\.0\.1/o=Pin 5 5 IN IP4 192.168.5.111/;
        }
        elsif (/c=IN IP4 192.168.5.112/)
        {
            s/o=Pingtel 5 5 IN IP4 127\.0\.0\.1/o=Pin 5 5 IN IP4 192.168.5.112/;
        }
        elsif (/c=IN IP4 192.168.5.113/)
        {
            s/o=Pingtel 5 5 IN IP4 127\.0\.0\.1/o=Pin 5 5 IN IP4 192.168.5.113/;
        }

    }

    # s/sip\:6396\@192.168.5.1/sip\:6396\@yoyo.private.vovida.com/g;
    # s/sip\:6396\@192.168.26.180/sip\:aaln_0\@galileo.private.vovida.com/g;
    # s/sip\:6396\@192.168.16.180/sip\:aaln_0\@galileo.private.vovida.com/g;
    # s/sip\:6396\@192.168.6.180/sip\:aaln_0\@galileo.private.vovida.com/g;
    s/sip\:6396\@192.168.5.1/sip\:aaln_0\@galileo.private.vovida.com/g;
    s/sip\:6391\@192.168.5.1/sip\:6391\@galileo.private.vovida.com/g;
    s/sip\:6248\@192.168.5.1/sip\:6248\@galileo.private.vovida.com/g;
    s/sip\:6249\@192.168.5.1/sip\:aaln_0\@galileo.private.vovida.com/g;
    # s/sip\:6391\@192.168.5.1/sip\:6391\@frisbee.private.vovida.com/g;
    # s/aaln_([0-1])\@192.168.5.1/aaln_\1\@galileo.private.vovida.com/g;
    # s/sip\:([0-9])([0-9])([0-9])([0-9])\@192.168.26.180/sip\:\1\2\3\4\@galileo.private.vovida.com/g;
    # s/sip\:([0-9])([0-9])([0-9])([0-9])\@192.168.16.180/sip\:\1\2\3\4\@galileo.private.vovida.com/g;
    # s/sip\:([0-9])([0-9])([0-9])([0-9])\@192.168.6.180/sip\:\1\2\3\4\@galileo.private.vovida.com/g;
    # s/sip\:([0-9])([0-9])([0-9])([0-9])\@192.168.5.1/sip\:\1\2\3\4\@galileo.private.vovida.com/g;
    # s/UDP 192.168.5.1/UDP galileo.private.vovida.com/g;
    # s/UDP 192.168.20.15/UDP yoyo.private.vovida.com/g;
    # s/UDP 192.168.20.16/UDP frisbee.private.vovida.com/g;

    # don't change this line

    return $_;
}
