#!/usr/bin/perl

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2002 Vovida Networks, Inc.  All rights reserved.
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

# $Id: userxml.pm,v 1.1 2004/05/01 04:15:31 greear Exp $

package vprov::userxml;
use nanoxml;

BEGIN {
    use Exporter;
#    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    # if using RCS/CVS, this may be preferred
    $VERSION = do { my @r = (q$Revision: 1.1 $ =~ /\d+/g); sprintf "%d."."%02d" x $#r, @r }; # must be all one line, for MakeMaker
    @ISA         = qw(Exporter);
    @EXPORT      = qw(&create_contact_called_xml 
		      &create_contact_calling_xml
		      &create_contact_called_from_user
		      &create_contact_calling_from_user
		      &create_user_xml

		      &parse_parameters_user

		      &modify_user_xml
		      );
    %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],
    
    # your exported package globals go here,
    # as well as any optionally exported functions
    @EXPORT_OK   = qw();
}
#our @EXPORT_OK;

chomp($hostname = `hostname`);
($junk1,$junk2,$junk3,$junk4,@addr) = gethostbyname($hostname);
($a,$b,$c,$d) = unpack('C4', $addr[0]);
$local_ip = "$a.$b.$c.$d";


sub create_contact_node {
    my($tree, $feature_type) = @_;
    my($toutput) = "";
    my($node, $node2);

    $node2 = &find_element($tree, "user", $feature_type);
    if($node2) {
	$node = &find_element($node2, "setfeat");
	my $contents = ${$node->{contents}}[0]->{data};

	if($node && ( $contents eq "ON")) { 
	    my $node3 = &find_element($node2, "featuregroup");
	    # {
	    my $groupname = ${ $node3->{contents} }[0]->{data};

	    
	    $toutput = "  <contact>\n    <host>" . $groupname .
	      "</host>\n    <port>any</port>\n" . "  </contact>\n";
	} else {
	}
    } else {
	print STDERR "could not find $feature_type in tree!\n";
    }
    return $toutput;
}


sub create_contact_called_from_user {
    my($user_xml) = @_;
    my($tree) = &parse_document($user_xml);
    my($output);

    $output = "<?xml version=\"1.0\" ?>\n<calledContactList>\n";
   
    # now, add as appropriate

    $output .= &create_contact_node($tree, "callReturn");
    $output .= &create_contact_node($tree, "cs");
    $output .= &create_contact_node($tree, "cfa");
    $output .= &create_contact_node($tree, "fnab");
#    $output .= &create_contact_node($tree, "cw");
	
    $output .= "</calledContactList>\n";
    return $output;
}


sub create_contact_calling_from_user {
    my($user_xml) = @_;
    my($tree) = &parse_document($user_xml);
    my($output);

    $output = "<?xml version=\"1.0\" ?>\n<callingContactList>\n";
   
    # now, add as appropriate

    $output .= &create_contact_node($tree, "clbl");
    $output .= &create_contact_node($tree, "callerIdBlocking");
	
    $output .= "</callingContactList>\n";
    return $output;
}


sub create_contact_called_xml {
    my($params) = @_;
    my($output);

    if($$params{enable_vm}) {
	$output = <<EOL
<?xml version="1.0" ?>
<calledContactList>
  <contact>
    <host>ForwardNoAnswerBusyGroup</host>
    <port>any</port>
  </contact>
</calledContactList>
EOL
  ;
    } else {
	$output = <<EOL
<?xml version="1.0" ?>
<calledContactList>
</calledContactList>
EOL
  ;
    }
    return $output;
}

sub create_contact_calling_xml {
    my($params) = @_;
    my($output) = <<EOL
<?xml version="1.0" ?>
<callingContactList>
</callingContactList>
EOL
  ;
    return $output;
}


sub parse_parameters_user {
    my ($data) = @_;

    my $parameters = {};

    if($data) {
	
	# scan for xml
	if($data =~ /\<authenticationType value\=\"(\w*)\"/) {
	    $auth_type = $1;
	    if($auth_type eq "None") {
		$$parameters{use_passwd} = 0;
	    } elsif ($auth_type eq "Digest") {
		$$parameters{use_passwd} = 1;
	    } 
	}
	if($data =~ /\<authenticationPassword value\=\"(\w*)\"/) {
	    $$parameters{passwd} = $1;
	}
	$$parameters{enable_vm} = 0;
	if($data =~ /\<fnab enabled\=\"(\w*)\"\>\<setfeat\>(\w*)/) {
	    if($1 eq "true" && $2 eq "ON") {
		$$parameters{enable_vm} = 1;
	    }
	}
	return $parameters;
    } else {
	return undef;
    }
}


######################################################################
#  user_xml
######################################################################

my %user_parameter_mapping = 
  ( use_passwd => \&filter_use_passwd,
    passwd => \&filter_passwd,
    enable_vm => \&filter_enable_vm
  );


sub filter_use_passwd {
    my($tree, $value) = @_;

    my $x = &find_element($tree, "user", "authenticationType");

    if($x) {
	my $auth_type = "None";
	if($value) {
	    $auth_type = "Digest";
	}
	# replace
	$x->{start} =~ s/value=\"[^\"]*\"/value=\"$auth_type\"/;
	return 1;
    } else {
	&psdebug("no good -- undef value because i couldn't find it");
	return undef;
    }
}


sub filter_passwd {
    my($tree, $value) = @_;

    my $x = &find_element($tree, "user", "authenticationType", "authenticationPassword");

    if($x) {
	$x->{start} =~ s/value=\"[^\"]*\"/value=\"$value\"/;
	return 1;
    } else {
	&psdebug("no good -- undef value because i couldn't find it");
	return undef;
    }
}


sub filter_enable_vm {
    my($tree, $value) = @_;

    if($value) {
	# turn on VM

	my $fnab = &find_element($tree, "user", "fnab");

	# set fnab enabled="true"
	if($fnab || return undef) {
	    $fnab->{start} =~ s/enabled=\"[^\"]*\"/enabled=\"true\"/;
	} 

	# set fnab setfeat ON
	my $setfeat = &find_element($fnab, "setfeat");
	if($setfeat || return undef) {
	    $setfeat->{contents} = [ &new_data('ON') ];
	}

	# set fna set="true", and set the forwardto to sip:7000
	my $fna = &find_element($fnab, "fna");
	if($fna || return undef) {
	    $fna->{start} =~ s/set=\"[^\"]*\"/set=\"true\"/;
	} 
	my $fna_forwardto = &find_element($fna, "forwardto");

	if($fna_forwardto || return undef) {
	    $fna_forwardto->{contents} = [ &new_data("sip:7000") ];
	}

	# set cfb set="true", and set the forwardto to sip:7000
	my $cfb = &find_element($fnab, "cfb");
	if($cfb || return undef) {
	    $cfb->{start} =~ s/set=\"[^\"]*\"/set=\"true\"/;
	} 
	my $cfb_forwardto = &find_element($cfb, "forwardto");

	if($cfb_forwardto || return undef) {
	    $cfb_forwardto->{contents} = [ &new_data("sip:7000") ];
	}
    } else {
	# turn off VM

	my $fnab = &find_element($tree, "user", "fnab");

	# set fnab setfeat OFF
	my $setfeat = &find_element($fnab, "setfeat");
	if($setfeat || return undef) {
	    $setfeat->{contents} = [ &new_data('OFF') ];
	}

	# set fna set="false"
	my $fna = &find_element($fnab, "fna");
	if($fna || return undef) {
	    $fna->{start} =~ s/set=\"[^\"]*\"/set=\"false\"/;
	} 

	# set cfb set="false"
	my $cfb = &find_element($fnab, "cfb");
	if($cfb || return undef) {
	    $cfb->{start} =~ s/set=\"[^\"]*\"/set=\"false\"/;
	} 
    }
    return 1;
}


sub modify_user_xml {
    my($params, $changed_list, $document) = @_;
    my($tree) = &parse_document($document);


    # find out which changed here

    my $item;
    foreach $item (@$changed_list) {
	if($user_parameter_mapping{$item}) {
	    $ret = &{$user_parameter_mapping{$item}}($tree, $$params{$item});
	    if(!$ret) {
		return undef;
	    }
	}
    }

    return &output_tree($tree);

    # here are the parameters which we care about

    # do nothing:
    # user (we will assume we got passed the right one)

    # easy
    # passwd (just replace it)
    # use_passwd (just set to Digest or leave alone?)

    # trickier -- we need to do the following:

}


sub create_user_xml {
    my($params) = @_;

    my($authentication_type) = "None";
    if($$params{use_passwd}) {
	$authentication_type = "Digest";
    }

    my($callforward_block) = <<EOL
<fnab enabled="false"><setfeat>OFF</setfeat>
<featuregroup>ForwardNoAnswerBusyGroup</featuregroup>
<fna set="false"><forwardto>none</forwardto>
</fna>
<cfb set="false"><forwardto>none</forwardto>
</cfb>
</fnab>
EOL
    ;
    if($$params{enable_vm}) {
    $callforward_block = <<EOL
<fnab enabled="true"><setfeat>ON</setfeat>
<featuregroup>ForwardNoAnswerBusyGroup</featuregroup>
<fna set="true"><forwardto>sip:7000</forwardto>
</fna>
<cfb set="true"><forwardto>sip:7000</forwardto>
</cfb>
</fnab>
EOL
    ;
    }

    my($output) = <<EOL
<user><name>$$params{user}</name>
<password>6f7d5634f65192a3b9c8479cee3b655</password>
<ipaddr>0.0.0.0</ipaddr>
<marshalgroup>UserAgentGroup</marshalgroup>
<staticRegistration value="false"><terminatingContactList><contact value="UserAgentGroup"></contact>
<contact value="000.000.000.000:0000"></contact>
</terminatingContactList>
</staticRegistration>
<authenticationType value="$authentication_type"><authenticationPassword value="$$params{passwd}"></authenticationPassword>
</authenticationType>
<failurecase>unknown</failurecase>
<cfa enabled="false"><setfeat>OFF</setfeat>
<featuregroup>ForwardAllCallsGroup</featuregroup>
<forwardto>none</forwardto>
</cfa>
$callforward_block<cs enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallScreeningGroup</featuregroup>
</cs>
<clbl enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallBlockingGroup</featuregroup>
<nineHundredBlocked adminSet="false" userSet="false"></nineHundredBlocked>
<longDistanceBlocked adminSet="false" userSet="false"></longDistanceBlocked>
<prefix>not loaded from xml</prefix>
</clbl>
<jtapi enabled="false"><setfeat>OFF</setfeat>
<featuregroup>JtapiGroup</featuregroup>
</jtapi>
<callReturn enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallReturnGroup</featuregroup>
</callReturn>
<callerIdBlocking enabled="false"><setfeat>OFF</setfeat>
<featuregroup>CallerIdBlockingGroup</featuregroup>
</callerIdBlocking>
<voicemail enabled="false"><setfeat>OFF</setfeat>
<featuregroup>VoiceMailGroup</featuregroup>
</voicemail>
<userGroup>Kazoo</userGroup>
<aliases></aliases>
</user>
EOL
    ;
    return $output;
}

return 1;
