#!/usr/bin/perl -w
#

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

###############################################################################
#
# script to randomly select a calling user and a called user,
#        fix up their respectively ua.cfg files, launch respective
#        ua's, let ua make x calls, then select another pair of
#        user accounts and repeat the whole process....
#
# Usage: ranGenLoad rand-offset rand-limit calls-per-run
#
#
# $Id: ranLoadGen.pl,v 1.1 2004/05/01 04:15:31 greear Exp $
#
#
###############################################################################
# display usage message
#
if (@ARGV != 4 ) {
  die "Usage:  $0  rand-base rand-limit calls-per-run times-to-run\n";
}

# verbose mode
$verbose = 1;

# parse arguments
$randbase = $ARGV[0];
$randlimit = $ARGV[1];
$callsprun = $ARGV[2];
$times2run = $ARGV[3];

# set default calling/ called user ids:
$calling = 10000;
$called  = 10001;

# set default paths where calling/ called ua to be invoked:
$callingpath = "/root/ldgen/call/ua.call.cfg";
$calledpath  = "/root/ldgen/rec/ua.rec.cfg";

# setup path to ua:
$uapath = "/root/ldgen/ua"; 

# seed the random number generator
srand( time() ^ ($$ + ($$ << 15)) );

# generate calling/ called users a random
sub genUsers {
   $range = $randlimit - $randbase;  # 0..n
   $calling = int(rand $range) + $randbase;
   $called  = int(rand $range) + $randbase;
   printf( "calling = %d, called = %d\n", $calling, $called ) if $verbose;
   
   if ($calling eq $called) {
      $called = $called - 1;
      printf( "calling = %d, called = %d\n", $calling, $called ) if $verbose;
   }
}

# fix up the ua.cfg files, called by prepareData()
sub fixUaCfg {
   $clg = $_[0];    # calling number
   $cld = $_[1];    # called number

   # assume <UA> and <OUTFILE> are already opened:
   while (<UA>)
   {
       $line = $_;
       chomp($line);
       if (/(^User_Name)/) {
          @fields = split(/\s+/, $line);
          printf( "f1=%s f2=%s f3=%s\n", $fields[0], $fields[1], $fields[2] ) if $verbose;
          $newline = "$fields[0]     $fields[1]     $clg";
          print OUTFILE "$newline\n"; 
          printf( "newline = $newline\n" );
          next;
       }
       if (/(^CallUrl)/) {
          @fields = split(/\s+/, $line);
          printf( "f1=%s f2=%s f3=%s\n", $fields[0], $fields[1], $fields[2] ) if $verbose;
          @sipurl = split(/@/, $fields[2]);
          $newline = "$fields[0]     $fields[1]     sip:$cld@".$sipurl[1];
          print OUTFILE "$newline\n"; 
          printf( "newline = $newline\n" );
          next;
       }
       if (/(^NumOfCalls)/) {
          @fields = split(/\s+/, $line);
          printf( "f1=%s f2=%s f3=%s\n", $fields[0], $fields[1], $fields[2] ) if $verbose;
          $newline = "$fields[0]     $fields[1]     $callsprun";
          print OUTFILE "$newline\n"; 
          printf( "newline = $newline\n\n" );
          next;
       }
       # ignore the rest of cfg data, just output to OUTFILE:
       print OUTFILE "$line\n";
   }
}

# set up data used to fix up the ua config files:
sub prepareData {
   $temp = "temp.out";
   if (-e $temp )
   {
      rmdir $temp;
   }

   # first, fix up the calling ua.cfg
   open( UA, "<" . $callingpath) || die "Could not open $callingpath\n";
   open(OUTFILE, ">$temp");
      fixUaCfg( $calling, $called );
   close( UA );
   close( OUTFILE );
   rmdir  "$callingpath".".bak";
   rename $callingpath, "$callingpath".".bak";
   rename $temp, $callingpath;

   # now, fix up the called ua.cfg
   rmdir $temp;
   open(UA, "<" . $calledpath ) || die "Could not open $calledpath\n";
   open(OUTFILE, ">$temp");
       fixUaCfg( $called, $calling );
   close(UA);
   close( OUTFILE );
   rmdir  "$calledpath".".bak";
   rename $calledpath, "$calledpath".".bak";
   rename $temp, $calledpath;
}

# launch ua -f ua.cfg respectively
sub startUas {
   system("./start-uaxterms startupUa.sxt");
   print "done calling start-xterms\n";
   sleep $callsprun*(rand(20)+1);
   print "done sleeping\n";
}

# main prog
sub main {
   # loop $times2run
   $i = $times2run;
   while ($i) {
       genUsers();
       prepareData();     
       startUas();
       $i--;           # comment this out if run forever
   }   
}

main();

## end of program
