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

package Telephony::Headers;

use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter;

@ISA = qw ( Exporter AutoLoader );
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT    = qw ( new add_header );
@EXPORT_OK = qw ( get_value_for_name get_name_for_value );
$VERSION = '0.01';

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
#__END__

sub new
{
    my ($class, %entries) = @_;
    my $self = [ {} ]; #a pseudo-hash reference
    #Construct the pseudo-hash
    foreach my $header_name (keys %entries)
    {
	my $header_value = $entries{$header_name};
	$self->[0]->{$header_name} = @{$self}; #i.e. use the index after the last used one 
	$self->{$header_name} = $header_value; #magical pseudo-hash syntax
    }
    return bless ($self, $class);
}

sub add_header
{
    my ($self, $header_name, $header_value) = @_;
    if (exists $self->{$header_name})
    {
	#For headers like Via that appear more than once, append the new value
	$self->{$header_name} .= ",\n $header_value";
    }
    else
    {
	#Set up the index in the hash part of the pseudo-hash
	$self->[0]->{$header_name} = @{$self};	
	#Insert the value in the array part of the pseudo-hash
	$self->{$header_name} = $header_value; #magical pseudo-hash syntax
    }
    return $self;
}

#! TODO:  make (much more expensive) case-insensitive accessors

sub get_value_for_name
{
    my ($self, $name) = @_;
    return $self->{$name};
}

sub get_name_for_value
{
    my ($self, $value) = @_;
    foreach my $name (keys %$self)
    {
	return $name if (defined $self->{$name} && $self->{$name} eq $value);
    }
}

# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

Telephony::Headers - Perl extension for blah blah blah

=head1 SYNOPSIS

  use Telephony::Headers;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for Telephony::Headers was created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head1 AUTHOR

A. U. Thor, a.u.thor@a.galaxy.far.far.away

=head1 SEE ALSO

perl(1).

=cut
