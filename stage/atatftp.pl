#!/usr/bin/perl -w
require 5.002;

# This script takes a request from a TFTP server to provision an ATA.
# It gets the provisioning data and converts it to a form usable by
# the ATA, then returns the data to the TFTP server.

use strict;
use sigtrap;
use IO::Socket;
use CGI;
use XML::Parser;
use Socket;

# =======================================================================
# The Vovida Software License, Version 1.0 
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
# 
# 
# All third party licenses and copyright notices and other required
# legends also need to be complied with as well.
# 
# ====================================================================

# uncomment for debugging
 my $debug_is_on = 1;
#my $debug_is_on = 0;

# logfile 
my $templog = "/tmp/.atalog";
# tempfile for writing text provisioning data
my $temptext = "/tmp/.atatxt";
# tempfile for writing binary provisioning data
my $tempbin = "/tmp/.atabin";
# template for changing OpFlags in first pass
my $template1 = "../../etc/ataOpFlags.txt";
# template for setting provisioning data in second pass
my $template2 = "../../etc/ataParams.txt";
# lock file
my $lockfile = "/tmp/.atatftp.lock";

# conversion program for ATA
my $cfgfmt = "../../bin/cfgfmt";

# data file used by cfgfmt
my $ptag = "../../bin/ptag.dat";

# string to set up new user
my $default_provisioning;

# name of MAC address table
my $mac_table_file = "../../var/mac_addresses";

# low end of range of MAC addresses (get from pserver)
my $low_range = 1000;;

# high end of range of MAC addresses (get from pserver)
my $high_range = 2000;;

# MAC address coming from tftp server query
my $path_info;

# userid assigned to above MAC address
my $userid;

# CGI object
my $tftp_query = new CGI;

# Array of 2 values for returning last pserver result.
# First value is the status (ie 200)
# Second value is content, if there is any.
my @pserver_result;

# global provisioning data (from pserver)
my @global_provisioning;

# hash of values to write out in tftp response
my %tftp_hash;

# name of the User Agent Marshal group
my $marshalgroup = "UserAgentGroup";

# address of marshal --> Outbound Proxy
# defaults to the current host ip.
my $marshal_address = `hostname -i`;

# make chomp get trailing spaces and newlines
$/ = " \n";
chomp $marshal_address;
# set variable back so socket read will work
$/ = "\n";

# used for parsing ListOfMarshalServers xml
my $inside_server_group = 0;

###################################################
# BEGIN
###################################################
# Only one process may use this script at a time.
# We use a lock file to enforce this.
&lock();


close STDERR;
if (-f ${templog})
{
  open STDERR, ">>${templog}";
}
else
{
  open STDERR, ">${templog}";
}

if ($#ARGV > -1)
{
  $path_info = $ARGV[0];
}
else
{
  $path_info = $tftp_query->path_info();
}

#remove initial /
$path_info =~ s/^\/*//;
die "blank MAC address given" unless $path_info;

# decide whether the path_info is a zup request
if($path_info =~ /.zup/)
{
  print STDERR "got a zup request\n";
  open ZUP, "../../etc/${path_info}" or die "failed to open ${path_info}";
  print "Content-type: tftp\n\n";
  print while <ZUP>;
  close ZUP;
  exit(0);
}
# decide whether the path_info is a mac address
elsif (is_mac_address($path_info))
{
# continue
}
# this must be some sort of bootstrap request
# try to get the ATA to send something else
else
{
    print STDERR "got a bootstrap request: ${path_info}\n";
    open ATATEXT, ">$temptext" or die "failed to open ${temptext} for writing";
    open CHANGE_OP_FLAGS, $template1
      or die "failed to open ${template1} for reading";
    my $hostname = `hostname -i`;
    chomp $hostname;

    while(<CHANGE_OP_FLAGS>)
    {
      print ATATEXT;
    }
    close ATATEXT;
    close CHANGE_OP_FLAGS;
    &send_text_cfg();
    &unlock();
    exit(0);
}
print STDERR "MAC address is ${path_info}\n";

# we will respond with the provisioning data, once we have it.

# Connect to pserver
&get_pserver_connection();

# get Global Provisioning Data
send_to_pserver("GET", "SystemConfiguration", "GlobalConfigData");
die "Pserver failed to return global provisioning"
  if $pserver_result[0] == 400;
die "Pserver returned empty global provisioning result"
  if (scalar @pserver_result) < 2;

my $global_provisioning = $pserver_result[1];

# use the XML Parser to parse the response from pserver
my $p = new XML::Parser(ErrorContext => 2,
                        Handlers     => {Start => \&start_global_element
                                        }
                       );

$p->parse($global_provisioning);

# open the data file and search for MAC address.
# if the MAC address exists, get provisioning data from pserver
# otherwise, create a new user and write it to pserver
# convert the data to ATA format and return in to tftp server

my %mac_hash;

if (-f $mac_table_file)
{
  open MACTABLE, $mac_table_file;
  while (<MACTABLE>)
  {
    my ($mac, $id) = split;
    if ($mac && $id)
    {
      $mac_hash{$mac} = $id;
    }
  }
  close MACTABLE;
}

# get the list of userids currently in Provisioning
# the list will be put into pserver_result[1]
send_to_pserver("NLIST", "Accounts", "Accounts");
chomp $pserver_result[1];

# sort alphabetically into an array
my @nlist = split(/,/, $pserver_result[1]);

$userid = $mac_hash{$path_info};
if ($userid)
{
  # this user already exists -- make sure it is still in Provisioning.
  if (&is_in_provisioning($userid))
  {
    # parse the Account XML to get the Group name for User Agent Marshal
    send_to_pserver("GET", "Accounts", $userid);

    die "Pserver failed to return user provisioning for ${userid}"
      if $pserver_result[0] == 400;

    die "Pserver returned empty user provisioning for ${userid}"
    if (scalar @pserver_result) < 2;

    my $user_provisioning = $pserver_result[1];

    my $p = new XML::Parser(ErrorContext => 2,
                Handlers     => {Char => \&account_char_handler
                                        }
                       );
    $p->parse($user_provisioning);
  }
  else
  {
    &create_new_user($userid);
  }
}
else
{
  # this user does not exist -- make a new one.
  # use one greater than the last one seen.

  my @mac_values = values %mac_hash;

  #check to see if there are any mac addresses
  if (scalar @mac_values > 0)
  {
    #sort them in reverse order
    my @userids = sort {$b cmp $a} (@mac_values);
    my $highest_userid = $userids[0];
    $userid = $highest_userid + 1;
  }
  else
  {
    $userid = $low_range;
  }

  # if userid is out of range but it already exists in the file
  # then use it anyway.

  # make sure this user does not already exist in Provisioning
  # if it does, increment userid and try again
  # we only use $attempts if we need to loop back to low end of range.
  my $attempts = -1;

  while(&is_in_provisioning($userid))
  {
    if ($userid++ >= $high_range)
    {
      #start from the beginning in case there are any holes to fill.
      $userid = $low_range;
      $attempts = 0;
    }
    if ($attempts > -1)
    {
      if ($attempts++ >= ($high_range - $low_range))
      {
        die "No userid available in given range";
      }
    }
  }

  &create_new_user($userid);

# Append the new MAC address to the table of MAC addresses

if (-f $mac_table_file)
{
  # append to existing file
  open MACTABLE, ">>$mac_table_file" or die "failed to open $mac_table_file for writing";
}
else
{
  # create new file
  open MACTABLE, ">$mac_table_file" or die "failed to open $mac_table_file for writing";
}
  print MACTABLE "${path_info} ${userid}\n";

  close MACTABLE;
}

&set_up_tftp_params();
&send_text_cfg();

&unlock();

###############
# END OF MAIN #
###############

sub send_text_cfg
{
  die "ptag.dat is missing" unless -f ${ptag};
  my $cfg_result = system("${cfgfmt} -t${ptag} ${temptext} ${tempbin}");
  die "failed to format configuration data for ata" if $cfg_result < 0;
  open ATABIN, "$tempbin" or die "failed to open ${tempbin} for reading";
  print "Content-type: tftp\n\n";
  print while <ATABIN>;
  close ATABIN;
  if (!$debug_is_on)
  {
    unlink ${temptext};
    unlink ${templog};
  }
  unlink ${tempbin};
}

sub lock()
{
  # LOCK_EX is 2
  open LOCKFILE, ">$lockfile" or die "unable to get the lock";
  flock LOCKFILE, 2;
  my $pid = getppid();
  print LOCKFILE "$pid";
}

sub unlock()
{
  # LOCK_UN is 8
  flock LOCKFILE, 8;
  unlink $lockfile;
}

# checks a string to see if it is an ATA mac address
sub is_mac_address
{
  my $candidate = shift;

  # check that the first 3 characters are "ata"
  my $ata_identifier = substr $candidate, 0, 3;
  if ($ata_identifier ne "ata")
  {
    print STDERR "wrong ata identifier: ${ata_identifier}\n";
    return 0;
  }

  # at this point I have confidence that it is an ATA mac address
  # strip off the first 3 characters
  my $mac_address_string = substr $candidate, 3;

  my @chars = split //, $mac_address_string;

  # there should be 6 octets (12 digits) in a mac address
  my $num_chars = scalar(@chars);
  if ($num_chars != 12)
  {
    print STDERR "wrong number of characters: $num_chars\n";
    return 0;
  }

  return 0 unless $mac_address_string =~ /[0-9|a-f|A-F]{12}/;

  # everything checked out
  return 1;
}

# checks nlist to see if a given number is in it
sub is_in_provisioning
{
  my $userid = shift;
  return 0 unless $userid;

  my $item;

  # If the nlist is null, then userid is not in it
  return 0 unless @nlist;

  foreach $item (@nlist)
  {
    if ($item eq $userid)
    {
      return 1;
    }
  }
  return 0;
}


#argument should be a string like "Content-Length: 200"
sub get_content_length
{
  my $line = shift;
  my ($content_length_string, $content_length) = split(/\s+/, $line);

  return $content_length;
}


# Note that connection to pserver must already
# be established.
sub send_to_pserver
{
  my ($line, $content_length, $status, $content, $junk);
  my $pserver_command = shift;
  my $domain = shift;
  my $filename = shift;


  my $output = ${pserver_command} . " " . ${domain} . " " . ${filename} . "\n";
  if ($pserver_command eq "PUT")
  {
    my $data = shift;
    my $content_length = length($data) + 1;
    $output .= "Content-Length: ${content_length}\n";
    $output .= "${data}\n";
  }

  print SOCK "${output}";

# should be 200 OK or 400 FAILED
  $line = <SOCK>;
  die "no response from pserver" unless $line;
  ($status) = split(/\s+/, $line);
  $pserver_result[0] = $status;

  $line = <SOCK>;

   ($junk, $content_length) = split(/\s+/, $line);
   if ($content_length == 0)
   {
     # set array-length to 1
     $#pserver_result=0;
     return;
   }
   my $bytes_read = read(SOCK, $content, $content_length);

   die "read wrong number of bytes: ${bytes_read}\n"
     unless $bytes_read == $content_length;

   $pserver_result[1] = $content;
   return;

}

# used for parsing XML from Global Configuration
sub start_global_element
{
  my $xp = shift;
  my $elem = shift;

  # the autoProvisionATA element looks like
  # <autoProvisionATA startRange="1000" endRange="2000"/>
  if ($elem eq "autoProvisionATA")
  {
    my $attr1 = shift;
    my $val1 = shift;
    my $attr2 = shift;
    my $val2 = shift;

    $low_range = $val1 if $val1;
    $high_range = $val2 if $val2;
    die "Bad userid range: ${high_range} must be greater than ${low_range}"
      unless ${high_range} > ${low_range};
  }
}

# used for parsing XML from ListOfMarshalServers
sub start_marshals_element
{
  my $xp = shift;
  my $elem = shift;

  # the serverGroup element looks like
  #  <serverGroup value="UserAgentGroup">
  #    <server host="ipaddress" port="5060"/>
  #  </serverGroup>
  # but it is theoretically possible for the user's provisioning
  # to specify a different group, so check for that.

  if ($elem eq "serverGroup")
  {
    my $attr1 = shift;
    my $val1 = shift;
    if ($val1 eq $marshalgroup)
    {
      $inside_server_group = 1;
    }
    else
    {
      $inside_server_group = 0;
    }
  }
  elsif (($elem eq "server") && $inside_server_group)
  {
    # we always take the first server in the group.

    my $hostAttr = shift;
    my $hostValue = shift;
    my $portAttr = shift;
    my $portValue = shift;

    $marshal_address = $hostValue if $hostValue;
    $inside_server_group = 0;
  }
}

sub account_char_handler
{
  my $p = shift;
  my $data = shift;

  if ($p->current_element eq "marshalgroup")
  {
    $marshalgroup = $data;
  }
}

# Establish initial connection to Pserver
# Requires password authentication
sub get_pserver_connection
{
  my ($port, $iaddr, $paddr, $proto, $line, $result);
  my ($status, $content_length);
  $port = 6005;
  $iaddr = inet_aton("localhost");
  $paddr = pack_sockaddr_in($port, $iaddr);
  $proto = getprotobyname('tcp');
  socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "socket: $!";

  connect(SOCK, $paddr)         or die "connect: $!";

  select (SOCK); $|=1; select STDOUT;

# need to get the password for this.
  my $password;
  if (-f "../../etc/vocal.secret.in")
  {
    $password = `cat ../../etc/vocal.secret.in`;
  }
  else
  {
    $password = "hello";
  }
  print SOCK "AUTH vocal:${password} VPP/1.1\n"; 

# should be 200 OK
  $line = <SOCK>;
  ($status) = split(/\s+/, $line);
  die "Connection not authorized\n" unless $status == 200;

# should be Content_Length: 0
  $line = <SOCK>;
  $content_length = &get_content_length($line);
  if ($content_length)
  {
    die "Unexpected Content-Length from Auth: ${content_length}\n";
  }
}

# Send the XML strings to Pserver that make up user data
sub create_new_user
{
  my $userid = shift;
  die "attempting to create blank user" unless $userid;

########################################################
#  Calling and Called Contact list
########################################################

  send_to_pserver ("PUT", "Contact_Lists_Calling", ${userid},
                   "<callingContactList/\>");
  die "Pserver failed to PUT ${userid} Calling Contact List"
    unless $pserver_result[0] == 200;

  send_to_pserver ("PUT", "Contact_Lists_Called", ${userid},
                   "<calledContactList/\>");
  die "Pserver failed to PUT ${userid} Called Contact List"
    unless $pserver_result[0] == 200;

########################################################
# default xml string to create new user
########################################################
  $default_provisioning = <<EOF;
<user><name>${userid}</name>
<password>6f7d5634f65192a3b9c8479cee3b655</password>
<ipaddr>0.0.0.0</ipaddr>
<marshalgroup>${marshalgroup}</marshalgroup>
<staticRegistration value="false"><terminatingContactList><contact value="UserAgentGroup"></contact>
<contact value="000.000.000.000:0000"></contact>
</terminatingContactList>
</staticRegistration>
<authenticationType value="None"><authenticationPassword value="password"></authenticationPassword>
</authenticationType>
<failurecase>unknown</failurecase>
<cfa enabled="false"><setfeat>OFF</setfeat>
<featuregroup>ForwardAllCallsGroup</featuregroup>
<forwardto>none</forwardto>
</cfa>
<fnab enabled="false"><setfeat>OFF</setfeat>
<featuregroup>ForwardNoAnswerBusyGroup</featuregroup>
<fna set="false"><forwardto>none</forwardto>
</fna>
<cfb set="false"><forwardto>none</forwardto>
</cfb>
</fnab>
<cs enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallScreeningGroup</featuregroup>
</cs>
<clbl enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallBlockingGroup</featuregroup>
<nineHundredBlocked adminSet="false" userSet="false"></nineHundredBlocked>
<longDistanceBlocked adminSet="false" userSet="false"></longDistanceBlocked>
</clbl>
<callReturn enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallReturnGroup</featuregroup>
</callReturn>
<callerIdBlocking enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallerIdBlockingGroup</featuregroup>
</callerIdBlocking>
<voicemail enabled="false"><setfeat>OFF</setfeat>
<featuregroup>VoiceMailGroup</featuregroup>
</voicemail>
<userGroup>ATA</userGroup>
<aliases></aliases>
</user>
EOF
########################################################

  send_to_pserver ("PUT", "Accounts", ${userid}, $default_provisioning);
  die "Pserver failed to PUT ${userid} Account"
    unless $pserver_result[0] == 200;

}

sub set_up_tftp_params
{
  open ATATEXT, ">$temptext" or die "failed to open ${temptext} for writing";
  open PARAMS, $template2 or die "failed to open ${template2} for reading";

  #set up marshal_address
  send_to_pserver("GET", "SystemConfiguration", "ListOfMarshalServers");

  die "Pserver failed to return list of marshal servers"
    if $pserver_result[0] == 400;

  die "Pserver returned empty list of marshal servers result"
    if (scalar @pserver_result) < 2;

  my $list_of_marshal_servers = $pserver_result[1];
  my $p = new XML::Parser(ErrorContext => 2,
                Handlers     => {Start => \&start_marshals_element
                                        }
                       );
  $p->parse($list_of_marshal_servers);

  my $hostname = `hostname -i`;
  chomp $hostname;

  while(<PARAMS>)
  {
    if (/^upgradecode:/)
    {
      s/{TFTP_HOST}/$hostname/;
      print ATATEXT;
    }
    elsif (/^cfgInterval:/)
    {
      # Note that random interval does not work because the
      # ATA will schedule an immediate update every time
      # cfgInterval changes, so if cfgInterval is randomized
      # it will loop endlessly.
      # One solution would be to assign a random number once
      # and store it in Provisioning.
      # make cfgInterval be a random number between 3000 and 4000
      # my $random_interval = int(rand 1000) + 3000;
      # print ATATEXT "cfgInterval:${random_interval}\n";
      print ATATEXT "cfgInterval:3600\n";
    }
    elsif (/^UID0:/)
    {
      print STDERR "setting UID0 to ${userid}\n";
      print ATATEXT "UID0:${userid}\n";
    }
    elsif (/^SipOutBoundProxy:/)
    {
      print STDERR "setting SipOutBoundProxy to ${marshal_address}\n";
      print ATATEXT "SipOutBoundProxy:${marshal_address}\n";
    }
    elsif (/^GkOrProxy:/)
    {
      print STDERR "setting GkOrProxy to ${marshal_address}\n";
      print ATATEXT "GkOrProxy:${marshal_address}\n";
    }
    else
    {
      print ATATEXT;
    }
  }
  close ATATEXT;
}

