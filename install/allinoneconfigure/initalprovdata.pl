#! /bin/sh

# the following is magic to allow perl to run with multiple locations of
# perl.  It only works with perls that have the -x switch, however!
exec `which perl` -x $0 ${1+"$@"}
    if $running_under_some_shell;
exit
#!perl

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

# $Id: initalprovdata.pl,v 1.1 2004/05/01 04:15:16 greear Exp $

require "getopts.pl";
    
&Getopts("L:I:");

if($opt_L) {
    push(@INC, $opt_L);
}

use lib "../../provisioning/web";
use lib "../webpages";
require vprov::vpp;
import vprov::vpp;

if (length($opt_I)) {
  $local_ip = $opt_I;
}
else {
  chomp($hostname = `hostname`);
  ($junk1,$junk2,$junk3,$junk4,@addr) = gethostbyname($hostname);
  ($a,$b,$c,$d) = unpack('C4', $addr[0]);
  $local_ip = "$a.$b.$c.$d";
}

print "initialprovdata.pl:  Using local_ip: $local_ip\n";

while(<STDIN>) {
    chomp;
    ($key, $value) = split(/\s+/, $_, 2);
    $config{$key} = $value;
}

$prov_host = "$config{authentication}\@$local_ip";

#print STDERR "$config{authentication}\n";


######################################################################
#       section to generate SystemConfiguration ListOfFeatureServers
######################################################################

$var = <<ENDSEC
<featureServers>
  <serverType
      value="ForwardAllCalls">
    <serverGroup
        value="ForwardAllCallsGroup">
      <server
          host="${local_ip}"
          port="5085">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="ForwardNoAnswerBusy">
    <serverGroup
        value="ForwardNoAnswerBusyGroup">
      <server
          host="${local_ip}"
          port="5105">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="CallBlocking">
    <serverGroup
        value="CallBlockingGroup">
      <server
          host="${local_ip}"
          port="5080">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="CallScreening">
    <serverGroup
        value="CallScreeningGroup">
      <server
          host="${local_ip}"
          port="5100">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="Voicemail">
    <serverGroup
        value="VoicemailGroup">
      <server
          host="${local_ip}"
          port="5110">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="CallReturn">
    <serverGroup
        value="CallReturnGroup">
      <server
          host="${local_ip}"
          port="5095">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="CallerIdBlocking">
    <serverGroup
        value="CallerIdBlockingGroup">
      <server
          host="${local_ip}"
          port="5090">
      </server>
    </serverGroup>
  </serverType>
</featureServers>
ENDSEC
    ;
die "could not set ListOfFeatureServers" 
  if(&put_data($prov_host, "SystemConfiguration", "ListOfFeatureServers", $var) < 0);

######################################################################
#       section to generate SystemConfiguration ListOfRedirectServers
######################################################################

$var = <<ENDSEC
<redirectServers>
  <serverGroup
      value="RedirectGroup">
    <server
        host="${local_ip}"
        port="5070">
    </server>
  </serverGroup>
</redirectServers>
ENDSEC
    ;
die "could not set ListOfRedirectServers"
if(&put_data($prov_host, "SystemConfiguration", "ListOfRedirectServers", $var) < 0);

######################################################################
#       section to generate SystemConfiguration ListOfPdpServers
######################################################################

$var = <<ENDSEC
<pdpServers>
  <serverGroup
      value="PdpGroup">
  </serverGroup>
</pdpServers>
ENDSEC
    ;
die "could not set ListOfPdpServers"
if(&put_data($prov_host, "SystemConfiguration", "ListOfPdpServers", $var) < 0);

######################################################################
#       section to generate SystemConfiguration ListOfHeartbeatServers
######################################################################

$var = <<ENDSEC
<heartbeatServers>
  <serverGroup
      value="HeartbeatGroup">
    <server
        host="${local_ip}"
        port="0">
    </server>
  </serverGroup>
</heartbeatServers>
ENDSEC
    ;
die "could not set ListOfHeartbeatServers"
 if(&put_data($prov_host, "SystemConfiguration", "ListOfHeartbeatServers", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5085
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5085"><type value="ForwardAllCalls"></type>
<group value="ForwardAllCallsGroup"></group>
</featureServer>
ENDSEC
    ;
die "could not set feature 5085"
 if(&put_data($prov_host, "Features", "${local_ip}:5085", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5095
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5095"><type value="CallReturn"></type>
<group value="CallReturnGroup"></group>
</featureServer>
ENDSEC
    ;
die "could not set fs 5095"
  if(&put_data($prov_host, "Features", "${local_ip}:5095", $var) < 0);

######################################################################
#       section to generate Marshals ${local_ip}:5065
######################################################################

$var = <<ENDSEC
<marshalServer host="${local_ip}" port="5065"><type value="Gateway"></type>
<group value="GatewayGroup"></group>
<gateway host="10.0.0.5" port="5060"></gateway>
<noResponseTimer value="100"></noResponseTimer>
<allowUnbillableCalls value="true"></allowUnbillableCalls>
<allowUnknownCallers value="false"></allowUnknownCallers>
<conferenceBridge><bridgeNumber value="none"></bridgeNumber>
<accessList></accessList>
</conferenceBridge>
</marshalServer>
ENDSEC
    ;
die "could not set ms 5065" 
 if(&put_data($prov_host, "Marshals", "${local_ip}:5065", $var) < 0);

######################################################################
#       section to generate SystemConfiguration GlobalConfigData
######################################################################

$var = <<ENDSEC
<globalConfiguration><expiryTimer value="3600"></expiryTimer>
<multicastHost host="$config{multicast_host}" port="$config{multicast_port}"></multicastHost>
<maxMissedHeartbeats value="8"></maxMissedHeartbeats>
<heartbeatInterval value="250"></heartbeatInterval>
<proxyAuthKey value="VovidaClassXSwitch"></proxyAuthKey>
<redirectReasonInSIP value="true"></redirectReasonInSIP>
<startRange value="1000"/>
<endRange value="2000"/>
</globalConfiguration>
ENDSEC
    ;
die "could not set GlobalConfigData"
  if(&put_data($prov_host, "SystemConfiguration", "GlobalConfigData", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5100
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5100"><type value="CallScreening"></type>
<group value="CallScreeningGroup"></group>
</featureServer>
ENDSEC
    ;
die "could not set fs 5100"
  if(&put_data($prov_host, "Features", "${local_ip}:5100", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5110
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5110"><type value="Voicemail"></type>
<group value="VoicemailGroup"></group>
<uaVMServers firstPort="5170" host="${local_ip}" lastPort="5174" vmType="vmail"></uaVMServers>
</featureServer>
ENDSEC
    ;
die "could not set fs 5110"
  if(&put_data($prov_host, "Features", "${local_ip}:5110", $var) < 0);

######################################################################
#       section to generate Marshals ${local_ip}:5064
######################################################################

# $var = <<ENDSEC
# <marshalServer host="${local_ip}" port="5064"><type value="Internetwork"></type>
# <group value="InternetworkGroup"></group>
# <noResponseTimer value="100"></noResponseTimer>
# <allowUnbillableCalls value="true"></allowUnbillableCalls>
# <allowUnknownCallers value="false"></allowUnknownCallers>
# <conferenceBridge><bridgeNumber value=""></bridgeNumber>
# <accessList></accessList>
# </conferenceBridge>
# </marshalServer>
# ENDSEC
#     ;
# die "could not set ms 5064"
#   if(&put_data($prov_host, "Marshals", "${local_ip}:5064", $var) < 0);

######################################################################
#       section to generate Heartbeats ${local_ip}:0
######################################################################

$var = <<ENDSEC
<heartbeatServer host="${local_ip}" port="0"><group value="HeartbeatGroup"></group>
</heartbeatServer>
ENDSEC
    ;
die "could not set heartbeats"
  if(&put_data($prov_host, "Heartbeats", "${local_ip}:0", $var) < 0);

######################################################################
#       section to generate  Features_${local_ip}_5095_Called *69@vovida.com.cpl
######################################################################

$var = <<ENDSEC
<?xml version="1.0" ?>
<!DOCTYPE cpl SYSTEM "cpl.dtd">
<cpl>
  <incoming>
    <location clear="yes" url="callreturn">
      <redirect/>
    </location>    </incoming>
</cpl>
ENDSEC
    ;
die "could not set fscpl"
  if(&put_data($prov_host, "Features_${local_ip}_5095_Called", "*69\@vovida.com.cpl", $var) < 0);

######################################################################
#       section to generate SystemConfiguration ospServer
######################################################################

$var = <<ENDSEC
<ospServer><servicePoints></servicePoints>
<localValidation value="1"></localValidation>
<sslLifetime value="4"></sslLifetime>
<http><maxConnections value="8"></maxConnections>
<persistence value="50000"></persistence>
<retryDelay value="100"></retryDelay>
<retryLimit value="3"></retryLimit>
<timeout value="7000"></timeout>
</http>
<ospExtension value="1"></ospExtension>
<customerID value="10"></customerID>
<deviceID value="1000"></deviceID>
<auditURL value="http://localhost:8888/"></auditURL>
</ospServer>
ENDSEC
    ;
die "could not set ospserver"
  if(&put_data($prov_host, "SystemConfiguration", "ospServer", $var) < 0);

######################################################################
#       section to generate SystemConfiguration defaultIpPlan
######################################################################

$var = <<ENDSEC
<ipplan>
<key index="0" value="^sip:7000"><contact value="sip:7000\@${local_ip}:5110;user=phone"></contact>
</key>
<key index="1" value="^sip:9.*"><contact value="sip:\$USER\@${local_ip}:5065;user=phone"></contact>
</key>
<key index="2" value="^sip:(.{7})@"><contact value="sip:9\$1\@${local_ip}:5065;user=phone"></contact>
</key>
<key index="4" value="^sip:(011.*)"><contact value="sip:9\$1\@${local_ip}:5065;user=phone"></contact>
</key>
</ipplan>
ENDSEC
    ;
die "could not set defaultIpPlan"
  if(&put_data($prov_host, "SystemConfiguration", "defaultIpPlan", $var) < 0);

######################################################################
#       section to generate SystemConfiguration defaultPhonePlan
######################################################################

$var = <<ENDSEC
<digitalplan>
<key index="0" value="^sip:7000"><contact value="sip:7000\@${local_ip}:5110;user=phone"></contact>
</key>
<key index="1" value="^sip:9.*"><contact value="sip:\$USER\@${local_ip}:5065;user=phone"></contact>
</key>
<key index="2" value="^sip:(.{7})@"><contact value="sip:9\$1\@${local_ip}:5065;user=phone"></contact>
</key>
</digitalplan>
ENDSEC
    ;
die "could not set defaultPhonePlan"
  if(&put_data($prov_host, "SystemConfiguration", "defaultPhonePlan", $var) < 0);

######################################################################
#       section to generate SystemConfiguration ListOfMarshalServers
######################################################################

$var = <<ENDSEC
<marshalServers>
  <serverType
      value="UserAgent">
    <serverGroup
        value="UserAgentGroup">
      <server
          host="${local_ip}"
          port="5060">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="Gateway">
    <serverGroup
        value="GatewayGroup">
      <server
          host="${local_ip}"
          port="5065">
      </server>
    </serverGroup>
  </serverType>
  <serverType
      value="Internetwork">
    <serverGroup
        value="InternetworkGroup">
    </serverGroup>
  </serverType>

</marshalServers>
ENDSEC
    ;
die "could not set listofmarshalservers"
  if(&put_data($prov_host, "SystemConfiguration", "ListOfMarshalServers", $var) < 0);

######################################################################
#       section to generate SystemConfiguration ListOfCdrServers
######################################################################

$var = <<ENDSEC
<cdrServers>
  <serverGroup
      value="CdrGroup">
  </serverGroup>
</cdrServers>
ENDSEC
    ;
die "could not set listofcdrservers"
  if(&put_data($prov_host, "SystemConfiguration", "ListOfCdrServers", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5105
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5105"><type value="ForwardNoAnswerBusy"></type>
<group value="ForwardNoAnswerBusyGroup"></group>
</featureServer>
ENDSEC
    ;
die "could not set fs 5105"
  if(&put_data($prov_host, "Features", "${local_ip}:5105", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5080
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5080"><type value="CallBlocking"></type>
<group value="CallBlockingGroup"></group>
</featureServer>
ENDSEC
    ;
die "could not set fs 5080"
  if(&put_data($prov_host, "Features", "${local_ip}:5080", $var) < 0);

######################################################################
#       section to generate Features ${local_ip}:5090
######################################################################

$var = <<ENDSEC
<featureServer host="${local_ip}" port="5090"><type value="CallerIdBlocking"></type>
<group value="CallerIdBlockingGroup"></group>
</featureServer>
ENDSEC
    ;
die "could not set fs 5090"
  if(&put_data($prov_host, "Features", "${local_ip}:5090", $var) < 0);

######################################################################
#       section to generate Marshals ${local_ip}:5060
######################################################################

$var = <<ENDSEC
<marshalServer host="${local_ip}" port="5060"><type value="UserAgent"></type>
<group value="UserAgentGroup"></group>
<gateway host="none" port="0"></gateway>
<noResponseTimer value="100"></noResponseTimer>
<allowUnbillableCalls value="true"></allowUnbillableCalls>
<allowUnknownCallers value="false"></allowUnknownCallers>
<conferenceBridge><bridgeNumber value="none"></bridgeNumber>
<accessList></accessList>
</conferenceBridge>
</marshalServer>
ENDSEC
    ;
die "could not set ms 5060" 
  if(&put_data($prov_host, "Marshals", "${local_ip}:5060", $var) < 0);

######################################################################
#       section to generate Redirects ${local_ip}:5070
######################################################################

$var = <<ENDSEC
<redirectServer host="${local_ip}" port="5070"><group value="RedirectGroup"></group>
<syncPort value="22002"></syncPort>
</redirectServer>
ENDSEC
    ;
die "Could not set rs 5070" 
  if(&put_data($prov_host, "Redirects", "${local_ip}:5070", $var) < 0);


### Local Variables: ###
### mode: cperl ###
### End: ###
