#!/usr/bin/perl -w

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
# Usage: genPutUsers begin-userid num-users
#
# Note: This script requires the executable PutMultipleFiles
#       to be located in the current directory.
#
# Script to generate x number of user accounts in xml format
# and place them in the correct Provisioning directories.
#
# The starting user name must be an integer and the subseqent
# user names will increment upwards from there.
#
# A file called tplate will be used as the source for
# the contents of the generated files. Tplate should be a valid
# xml user file. The user name tag will be replaced with the
# correct tag for each user.
#
# This script will generate all files first in the current directory
# and then place them into the provisioning directories.
#
#
# $Id: genPutUsers.pl,v 1.1 2004/05/01 04:15:31 greear Exp $
#
#
###############################################################################
# display usage message
#
if (@ARGV != 2 ) {
  die "Usage:  $0  begin-userid num-users\n";
}

# verbose mode
$verbose = 0;

# parse arguments
$idbase = $ARGV[0];   # begin userid
$count = $ARGV[1];    # generate $count users

# set template file:
$template = "./tplate";      # this is the template file we want to clone

$id = $idbase;

# using $template file, generate $idbase+n.xml file in current directory
sub cloneAccount {

   # assume <TEMPLATE> and <OUTFILE> are already opened:
   while (<TEMPLATE>)
   {
       $line = $_;
       chomp($line);
       if (/(^<user><name>)/) {
          $newline = "<user><name>$id</name>";
          print OUTFILE "$newline\n"; 
          # printf( "newline = $newline\n" );
          next;
       }
       # ignore the rest of xml data, just output to OUTFILE:
       print OUTFILE "$line\n";
   }
}

# set up file handles to generate new user xml files:
sub prepareData {
   $outfile = "$id";    # file name = userAccount#
   if (-e $outfile )
   {
      rmdir $outfile;
   }

   # first, fix up the file content   
   open( TEMPLATE, "<" . $template) || die "Could not open $template\n";
   open(OUTFILE, ">$outfile");
      print "generating $outfile...\n" if $verbose;
      cloneAccount();
   close( OUTFILE );
   close( TEMPLATE );
}

# main prog
sub main {
   # loop $count times
   $i = $count;
   while ($i) {
       prepareData();     
       $id++;
       $i--;
   }   
   # call FileDataStoreWrapper to put the files in the hash bins:-
   # usage: <#users -> this determines the # of hash bin buckets, 1000==> 6>
   #        <provisioning root directory, e.g. /usr/local/vocal/prov....>
   #        <target directory name, e.g. Accounts>
   #        <begin User number, e.g. 5000>
   #        <# Users to generate: do it in batches, or else it'll take forever!>
   @args = ("./PutMultipleFiles", "1000", "/usr/local/vocal/provisioning_data", "Accounts", $idbase, $count);
   system( @args ) == 0 
      or die "system @args failed: $?";

   $id = $idbase;
   $i = $count;
   while ($i) {
       $outfile = "$id";    # file name = userAccount#
       unlink ${outfile};
       $id++;
       $i--;
   }

}

main();

## end of program
