#!/usr/bin/perl

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2000,2003 Vovida Networks, Inc.  All rights reserved.
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

# $Id: psTranslate.cgi,v 1.1 2004/05/01 04:15:31 greear Exp $

#use psinslib;
use vprov::vpp;

sub handleRemoveRequest()
{
    my($hostPort, $dir, $fileName) = @_;
    print "Removing file:$fileName from dir:$dir on: $hostPort\n";

    &remove_data($hostPort, $dir, $fileName);
}


sub handleServers()
{
    my($hostPort, $psDir, $oldIp, $newIp, $serverContext) = @_;
    print "ProvisionServer: $hostPort\n";
    print "Old Ip: $oldIp\n";
    print "New Ip: $newIp\n";
    open (FH, "find $psDir | grep $serverContext |");
    while(<FH>)
    {
        chomp;
        $fullPath = $_;
        $st = $_;
        $st=~s/(^.*)\/($serverContext.*)/X/;
        $_ = $2;
        if(/$serverContext\/.*/)
        {
           $st = $_;
           $st=~s/($serverContext)\/(.*)/X/;
           $dir = $1;
           $fileName = $2;
           $newName = $fileName;
           $newName =~s/$oldIp/$newIp/;
           print "Going to replace:$fullPath\n";
           #If dir name contains the OldIp ref, change it to the new Ip
           $dir =~s/$oldIp/$newIp/;
           print "Dir:$dir\n";
           print "FileName:$fileName\n";
           $_ = $newName;
           $newName =~s/\.xml//;
           print "New FileName:$newName\n";
           $cData = `cat $fullPath`;
           #Substitute the OLD IP with new one
           $cData =~s/$oldIp/$newIp/g;
#           print $cData, "\n";
           #if dir is either Marshals, Redirects, Heartbeats, Features, remove the old data first
           if(($dir eq "Marshals") ||
              ($dir eq "Features") ||
              ($dir eq "Heartbeats") ||
              ($dir eq "Redirects") )
           {
               $remFileName = $fileName;
               $_ = $remFileName;
               $remFileName=~s/\.xml//;
               &handleRemoveRequest($hostPort, $dir, $remFileName);
           }
           &put_data($hostPort, $dir, $newName, $cData);
        }
        close FH if eof;
    }
}


sub main()
{
  ($hostPort, $psDir, $oldIp, $newIp) = @ARGV;
  if(($#ARGV+1) lt 4)
  {
    print "Uage: $0 <pserverHost:port> < provisionDir> <oldIp> <newIp>\n";
    return -1;
  }
  print $hostPort,"\n";
  print $psDir,"\n";
  $_ = $psDir;
  /provisioning_data/ || die "Invalid provision_data dir\n";
  -d $psDir || die "$psDir is not a valid dir.\n";
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Marshals");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Redirects");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Features");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Contact_Lists_Calling");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Contact_Lists_Called");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Features_.*_Called");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Features_.*_Calling");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Accounts");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "SystemConfiguration");
  &handleServers($hostPort, $psDir, $oldIp, $newIp, "Heartbeats");
}

&main();
