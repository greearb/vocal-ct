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

package Telephony::SipServer;

use strict;
use Socket;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

use Socket;
use Carp;

require Exporter;

use constant USE_HOSTNAMES => 0;

#@ISA = qw(Exporter AutoLoader); #! Uncomment after debugging to allow auto-loading
@ISA= qw ( Exporter );
@EXPORT    = qw (   );
@EXPORT_OK = qw ( make_server_hash get_ip get_hostname get_logical_name );
$VERSION = '0.01';

1;
#__END__

sub new
{
    my %self;

    my ($class, $ip, $port, $logical_name) = @_;
    $port = 5060 unless defined $port;
    if (USE_HOSTNAMES)
    {
	my $hostname = gethostbyaddr (inet_aton ($ip), AF_INET);
	defined $hostname
	    or carp "Could not find the hostname for $ip:  $!";
	$self{hostname} = $hostname if defined $hostname;
    }
    $self{ip} = $ip;
    $self{port} = $port;
    $self{logical_name} = $logical_name if defined $logical_name;
    bless (\%self, $class);
}

sub get_ip
{
    my $self = shift();
    return $self->{ip};
}

sub get_hostname
{
    my $self = shift();
    if (exists $self->{hostname})
    {
	return $self->{hostname};
    }
    else
    {
	return $self->{ip};
    }
}

sub get_logical_name
{
    my $self = shift();
    return $self->{logical_name};
}

# Below is stub documentation for your module. You better edit it!

=head1 NAME

Telephony::SipServer - Perl extension for blah blah blah

=head1 SYNOPSIS

  use Telephony::SipServer;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Telephony::SipServer, created by h2xs. It looks like the
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

Telephony::SipServer - Perl extension for blah blah blah

=head1 SYNOPSIS

  use Telephony::SipServer;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Telephony::SipServer was created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head1 AUTHOR

A. U. Thor, a.u.thor@a.galaxy.far.far.away

=head1 SEE ALSO

perl(1).

=cut
