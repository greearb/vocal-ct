#!/usr/bin/perl

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2001-2002 Vovida Networks, Inc.  All rights reserved.
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

# $Id: psins.pl,v 1.1 2004/05/01 04:15:31 greear Exp $

require "getopts.pl";
use lib "../../provisioning/web";
use vprov::vpp;

&Getopts("tDgpldV");

if($opt_D) {
    &psdebug_enable(2);
}

$argument_count = $#ARGV + 1;

if($opt_g && $argument_count == 4) {
    $data = &get_data($ARGV[0], $ARGV[1], $ARGV[2]);
    if(!defined($data)) {
	die "cannot get data from provisioning server";
    }
    if($ARGV[3] eq '-') {
	# use stdin
	open(DATA, ">&1") || die "can't open stdout: $!";
    } else {
        open(DATA, ">$ARGV[3]") || die "can't open $ARGV[3]: $!";
    }
    print DATA $data;
    close(DATA);
} elsif($opt_p && $argument_count == 4) { 
    $data = "";
    if($ARGV[3] eq '-') {
	# use stdin
	open(DATA, "<&0") || die "can't open stdin: $!";
    } else {
        open(DATA, $ARGV[3]) || die "can't open $ARGV[3]: $!";
    }
    while(<DATA>) {
	$data .= $_;
    }
    close(DATA);
    $retval = &put_data($ARGV[0], $ARGV[1], $ARGV[2], $data);
    if($retval == -1) {
	die "cannot write to $ARGV[0]";
    }
} elsif($opt_l && $argument_count == 3) {
    $data = &list_data($ARGV[0], $ARGV[1]);
    if($ARGV[2] eq '-') {
	# use stdin
	open(DATA, ">&1") || die "can't open stdout: $!";
    } else {
        open(DATA, ">$ARGV[2]") || die "can't open $ARGV[2]: $!";
    }
    print DATA $data;
    close(DATA);
} elsif($opt_d && $argument_count == 2) {
    $data = &delete_user($ARGV[0], $ARGV[1]);
} elsif($opt_V && $argument_count == 1) {
    $data = &login($ARGV[0]);
    if(defined($data)) {
	print "Login succeeded.\n";
    } else {
	print "Login failed.\n";
    }
} else {
    &usage();
}


sub usage {
    print "Usage: $0 <options>\n";
    print "    Get:      -g <hostname:port> <directory-name> <dest-filename> <file>\n";
    print "    Put:      -p <hostname:port> <directory-name> <dest-filename> <file>\n";
    print "    List:     -l <hostname:port> <listname> <file>\n";
    print "    Delete:   -d <hostname:port> <user>\n";
    print "    Login:    -V <hostname:port>\n";
    print "\n";
    print "Global Options:\n";
    print "    -D  turn on debugging mode\n";
    print "    -t  require TLS\n";

    print "\n";
    exit;
}
