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

package Telephony::SipMessage;

use strict;
use vars qw ($VERSION @ISA @EXPORT @EXPORT_OK);
use Carp;
use Data::Dumper;
use Telephony::Headers;

use constant DEBUG_SIP_MESSAGES => 0;

require Exporter;

#@ISA = qw(Exporter AutoLoader); #! Uncomment after debugging to allow auto-loading
@ISA =       qw ( Exporter );
@EXPORT    = qw (   );
@EXPORT_OK = qw ( _new_sdp_headers get_reqres get_request get_response get_sip_uri get_sip_version get_source_ip get_destination_ipget_sip_headers get_sdp_headers get_sip_header get_sdp_header get_call_id);
$VERSION = '0.01';

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
#__END__

sub new
{
    my ($class, $sip_message_text, $sdp_message_text) = @_;

    my %self;
    my $sip_headers = Telephony::Headers->new(); $self{sip_headers} = $sip_headers;

    my $request_line;
    my @sip_message;
    if (ref ($sip_message_text) eq 'ARRAY')
    {
	#Multiple lines were passed in.  This is the only case used by
        #Tricorder.
	@sip_message = @$sip_message_text;
	$request_line = $sip_message[0];
    }
    elsif (ref ($sip_message_text) eq 'SCALAR')
    {
	#Only one line was passed in.
	$request_line = $$sip_message_text;
	#@sip_message remains undefined.
    }
    else
    {
	#A reference to the wrong kind of thingy was passed in!
	carp "Got a reference of type " . ref ($sip_message_text) ."\n"
	    .", which is not a valid argument; returning an undefined\n"
	    ."object.  This probably means you reached the last SIP\n"
	    ."message and it was incomplete, which is normal for ipgrab.\n\n";
	return undef;
    }
    if (DEBUG_SIP_MESSAGES)
    {
	print "new sip_message constructed from a ref of type "
	    . ref ($sip_message_text) . "\n\n";
	if (defined $request_line)
	{
	    print "my \$request_line = $request_line;\n\n";
	}
	if (defined @sip_message)
	{
	    print "\@sip_message is\n" . Data::Dumper->Dump ([ \@sip_message ]) . "\n\n";
	}
    }
    
    #Parse the ipgrab output corresponding to one SIP message and set the
    #data accordingly.

    my @request_fields;
    if (defined $request_line)
    {
	@request_fields = split /\s+/, $request_line;
	
	print "\@request_fields is\n" . Data::Dumper->Dump ([ \@request_fields ]) . "\n\n" if DEBUG_SIP_MESSAGES;
    }

    if ($request_fields[0] eq 'sip-req:')
    {
	#Parse a SIP request, e.g.
	#@request_fields = ('sip-req:', 'INVITE', 'sip:93831073@192.168.36.180', 'SIP/2.0', '[192.168.6.21:50623->192.168.36.180:5060]');

	#! All this "if defined" business is necessary to catch the
	#! end of the file, in case it ends with a partial SIP message
	#! (which is quite possible if the user terminated ipgrab before
	#! it finished logging).
	#! Is there a better way to check for this condition?

	#Convert to lowercase for case-insensitive comparison later on.
	$self{reqres}  = uc ($request_fields[1]) if defined ($request_fields[1]);
	$self{sip_uri} = $request_fields[2] if defined ($request_fields[2]);
	if (defined $request_fields[3])
	{
	    if ($request_fields[3] =~ m|SIP/(\S+)$|i) #! Record only the version number
	    {
		$self{sip_version} = $1;
	    }
	}
	if (defined $request_fields[4])
	{
	    if ($request_fields[4] =~ m{\[(.*?):(\d+)->(.*?):(\d+)\]})
	    {
		print "Matched request source and destination info:\n"
		    . "\tsource IP:  $1\n"
  		    . "\tsource port:  $2\n"
		    . "\tdestination IP:  $3\n"
		    . "\tdestination port:  $4\n"
		    if DEBUG_SIP_MESSAGES;
		$self{source_ip}        = $1;
		$self{source_port}      = $2;
		$self{destination_ip}   = $3;
		$self{destination_port} = $4;
	    }
	    else
	    {
		warn "Found a message with no source/destination IP.\n"
	    }
	}
    }
    elsif ($request_fields[0] eq 'sip-res:')
    {
	#Parse a SIP response, e.g.
        #@request_fields = ('sip-res:', 'SIP/2.0', '200', 'OK', '[192.168.16.210:5060->192.168.36.110:5060]')
	
	$self{sip_uri} = undef; #SIP responses don't have URIs.

	#All this "if defined" business is necessary to catch the
	#end of the file, in case it ends with a partial SIP message
	#(which is quite possible if the user terminated ipgrab before
	#it finished logging).
	#! Is there a better way to check for this condition?
	if (defined $request_fields[1])
	{
	    if ($request_fields[1] =~ m|SIP/(\S+)$|i) #! Record only the number
	    {
		$self{sip_version} = $1;
	    }
	}
	
	$self{reqres} =  $request_fields[2] if (defined $request_fields[2]);

	#In a SIP response, the numerical "return" code (or the extension-code,
	#as it is called in the RFC) is followed by a verbal explanation of
	#the response.  This explanation may be any number of words long,
	#and we have split the request-line on whitespace, so we must continue
	#to iterate through the fields adding them to the return code till we
	#encounter ipgrab's special extra info, of the form
	#[source_ip:source_port->destination_ip:destination_port]

	my ($i, $field);
  	for ($i = 3; $i <= $#request_fields; $i++)
  	{
	    #In a well-formed SIP message, this will never happen.
	    #However, aborted ipgrab sessions can contain partial SIP messages.
	    #The convention is to return undef in this case.
	    return undef if (! defined $request_fields[$i]);
  	    $field = $request_fields[$i];
	    #Add to the "verbal explanation of the response" described above.
  	    $self{explanation} .= $field;
  	    if ($field =~ m{\[(.*?):(\d+)->(.*?):(\d+)\]})
  	    {
		print "Matched response source and destination info:\n"
		    . "\tsource IP:  $1\n"
		    . "\tsource port:  $2\n"
  		    . "\tdestination IP:  $3\n"
  		    . "\tdestination port:  $4\n"
		    if DEBUG_SIP_MESSAGES;
  		$self{source_ip}        = $1;
  		$self{source_port}      = $2;
  		$self{destination_ip}   = $3;
  		$self{destination_port} = $4;
  		last;
  	    }
  	    else
  	    {
  		print "Failed to match source and destination info\n" if DEBUG_SIP_MESSAGES;
  	    }
  	}
    }
    else
    {
	warn "Unrecognized message type $request_fields[0]; skipping the message and returning the undefined value...\n";
	return undef;
    }

#!    shift (@sip_message); #Discard the request-line, which we just parsed
    foreach (@sip_message)
    {
	if (m/^Header:\s+(\S+?):\s*(.*)$/)
	{
	    print "Got a header match:  $_\n\n" if DEBUG_SIP_MESSAGES;
	    $sip_headers->add_header ($1, $2);
	}
    }

    my $self_reference = bless (\%self, $class);

    if (defined $sdp_message_text && (ref $sdp_message_text eq 'ARRAY'))
    {
	print "Got SDP headers as a constructor argument\n" if DEBUG_SIP_MESSAGES;
	$self_reference->{sdp_headers} = _new_sdp_headers ($sdp_message_text);
    }
    
    return $self_reference; #Well, I'll be, it's a self-referencing class! :P
}

sub _new_sdp_headers
{
    my $self = shift();
    my @sdp_lines = @{shift()};
    my $headers = Telephony::Headers->new();

    foreach (@sdp_lines)
    {
	if (m/Header:\s+(\S)=(.*)$/)
	{
	    $headers->add_header ($1, $2);
	}
    }
    return $headers;
}

sub get_call_id
{
    my ($self) = @_;
    return $self->{sip_headers}->get_value_for_name ('Call-ID');
}

sub get_reqres
#! return the SIP request or response (e.g. INVITE, ACK, 200)
{
    my $self = shift();
    return $self->{reqres};
}

#Aliases for the unwary
*get_request  = *get_reqres;
*get_response = *get_reqres;

sub get_sip_uri
{
    my $self = shift();
    return $self->{sip_uri};
}

sub get_sip_version
{
    my $self = shift();
    return $self->{sip_version};
}

sub get_source_ip
{
    my $self = shift();
    return $self->{source_ip};
}

sub get_source_port
{
    my $self = shift();
    return $self->{source_port};
}

sub get_destination_ip
{
    my $self = shift();
    return $self->{destination_ip};
}

sub get_destination_port
{
    my $self = shift();
    return $self->{destination_ip};
}

sub get_sip_headers()
#! returns headers by value--more overhead, but better encapsulation
{
    my $self = shift();
    return %{$self->{sip_headers}};
}

sub get_sdp_headers()
#! returns headers by value--more overhead, but better encapsulation
{
    my $self = shift();
    return %{$self->{sdp_headers}};
}

sub get_sip_header
#! returns a header by value--more overhead, but better encapsulation
{
    my ($self, $header_name) = @_;
    return $self->{sip_headers}->{$header_name};
}

sub get_sdp_header()
#! returns a header by value--more overhead, but better encapsulation
{
    my $self = shift();
    my $header_name = shift();
    # SDP headers are case-sensitive, so don't upcase.
    return $self->{sdp_headers}->{header_name};
}

# Below is stub documentation for your module. You better edit it!

=head1 NAME

Telephony::SipMessage - Perl extension for blah blah blah

=head1 SYNOPSIS

  use Telephony::SipMessage;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Telephony::SipMessage, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.


=head1 AUTHOR

A. U. Thor, a.u.thor@a.galaxy.far.far.away

=head1 SEE ALSO

perl(1).

=cut


# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Telephony::SipMessage - Perl extension for blah blah blah

=head1 SYNOPSIS

  use Telephony::SipMessage;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Telephony::SipMessage was created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head1 AUTHOR

A. U. Thor, a.u.thor@a.galaxy.far.far.away

=head1 SEE ALSO

perl(1).

=cut
