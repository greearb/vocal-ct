#! /usr/bin/perl

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

# $Id: parse-by-call.pl,v 1.1 2004/05/01 04:15:31 greear Exp $

# code to parse an ipgrab log by call

require "getopts.pl";

&Getopts("lb:h");

if($opt_h) {
    &usage();
}

foreach(@ARGV) {
    $start_seen = 0;
    open(FILE, $_) || die "can't open $_: $!";
    if($opt_b) {
	if($opt_b < 0) {
	    seek(FILE, $opt_b, 2);
	} elsif ($opt_b > 0) {
	    seek(FILE, $opt_b, 1);
	}
    }
    while(<FILE>) {
	if(/^sip-req:/ || /^sip-res:/) {
	    if($start_seen) {
		if(!defined($call{$call_id})) {
		    $call{$call_id} = [];
		}
		push(@{$call{$call_id}}, @msg);
	    } else {
		$start_seen = 1;
	    }		
  	    undef(@msg);
	    push(@msg, $_);
	    # start of a new one
	    $call_id = "";
	} elsif($start_seen && /^Header:/) {
	    # continues
	    push(@msg, $_);
	    if(/Call\-ID: (.+)$/) {
		$call_id = $1;
		#	    print "got $call_id\n";
	    }
	} else {
	    # all else is ignored
	}
    }
    if(!defined($call{$call_id})) {
	$call{$call_id} = [];
    }
    push(@{$call{$call_id}}, @msg);
    
    undef(@msg);
    push(@msg, $_);
    # start of a new one
    $call_id = "";
    close(FILE);
}

foreach(sort keys %call) {
    print "Call-ID: $_\n";
    if($opt_l) {
	foreach(@{$call{$_}}) {
	    print;
	}
    } else {
	print grep(/^sip\-r/, @{$call{$_}});
    }
}


sub usage {
    print <<EOF
Usage: $0 [-b byte-offset] [-l] IPGRAB-FILE...

Analyze IPGRAB-FILE(s) by sorting the SIP messages according to
Call-ID.  Output is of the form:

  Call-ID: <call-id>
  sip-req:    INVITE ...
  sip-res:    SIP/2.0 100 Trying ...
    <rest of SIP messages with matching Call-ID>
  Call-ID: <call-id>
    ...

Options:
    -b byte-offset          if byte-offset is positive, seek byte-offset bytes
                            forward in each IPGRAB-FILE before starting to 
			    parse.
                            if byte-offset is negative, seek byte-offset bytes
                            backward from the end of each IPGRAB-FILE before
			    starting to parse.
    -l                      Print the entire SIP message instead of just the
                            sip-req / sip-res line.

BUGS

    ipgrab must be modified to put the timestamp in more places
    (e.g. in the SIP header line).  Then $0 can be modified to look at
    the timestamps in multiple files and reorder them correctly.

EOF
  ;
    exit -1;
}
