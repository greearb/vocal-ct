#!/usr/bin/perl

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2001-2003 Vovida Networks, Inc.  All rights reserved.
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

# $Id: vpp.pm,v 1.1 2004/05/01 04:15:31 greear Exp $

package vprov::vpp;
use IO::Socket;
use Socket;

BEGIN {
#    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    # if using RCS/CVS, this may be preferred
    $VERSION = do { my @r = (q$Revision: 1.1 $ =~ /\d+/g); sprintf "%d."."%02d" x $#r, @r }; # must be all one line, for MakeMaker
    @ISA         = qw(Exporter);
    @EXPORT      = qw(&get_data 
		      &get_data2 
		      &put_data 
                      &remove_data
		      &list_data 
		      &delete_user 
		      &login 
		      &psdebug
		      &psdebug_get
		      &psdebug_enable
		     );
    %EXPORT_TAGS = ( 'debug' => [ qw!psdebug psdebug_get psdebug_enable!] );
    # eg: TAG => [ qw!name1 name2! ],
    
    # your exported package globals go here,
    # as well as any optionally exported functions
    @EXPORT_OK   = qw();
}
#our @EXPORT_OK;

my $use_ssl = 1;

eval {
    require Net::SSLeay; 
    import Net::SSLeay qw(die_now die_if_ssl_error);
#    use Net::SSLeay qw(die_now die_if_ssl_error);
};
if($@) {
    &psdebug3("TLS modules not found, disabling TLS");
    $use_ssl = 0;
}

if($use_ssl) {
    &psdebug3("TLS modules found");
    Net::SSLeay::load_error_strings();
    die_if_ssl_error("failed to load tls error strings");

    Net::SSLeay::SSLeay_add_ssl_algorithms();   # Important!
    die_if_ssl_error("failed to add tls algorithms");
    Net::SSLeay::randomize();
    die_if_ssl_error("failed to randomize");
}


&psdebug3("started loading psinslib.pm: ".time() . "\n");

my($VPP_VERSION) = "VPP/1.1";

sub create_connection {
    my($addr, $port) = @_;

    my($remote);
    $remote = IO::Socket::INET->new( Proto     => "tcp",
                                     PeerAddr  => $addr,
                                     PeerPort  => $port,
                                   );
    if(defined($remote) && $remote) {
	$remote->autoflush(1);
        return {fh => $remote};
    } else {
	return undef;
    }
}

sub look_for_returncode_length_old {
    my($data) = @_;

    if($data =~ /\n[^\n]*\n/) {
	return 1;
    } else {
	return 0;
    }
}


sub look_for_returncode_length {
    my($data) = @_;
    my $len, $retcode;

    if($data =~ s/^([0-9]+) (\w+)\nContent-Length: ([0-9]*)\n//) {
	$len = $3;
	$retcode = $1;

	if(length($data) >= $len) {
	    return 1;
	}
    } else {
	return 0;
    }
}


sub look_for_returncode {
    my($data) = @_;

    if($data =~ /\n/) {
	return 1;
    } else {
	return 0;
    }
}


sub hack_tls {
    my($socket, $user, $pass) = @_;
    my($buf);
    my($l);
    my(@bits);

    &psdebug("starting tls");
    &putsock($socket, "TLS . $VPP_VERSION\n");

    $buf = &read_until($socket, \&look_for_returncode_length);

    if($buf !~ /200 OK/) {
	&psdebug("TLS disabled");
	# no good, just go on
	return $socket;
    }

    &psdebug("TLS OK, enabling TLS");
    # convert to TLS
    $socket = &enable_tls($socket);

    return $socket;
}


sub hack_pass {
    my($socket, $user, $pass) = @_;
    my($buf);
    my($l);
    my(@bits);

    &psdebug("beginning AUTH");
    &putsock($socket, "AUTH $user:$pass $VPP_VERSION\n");
    &psdebug("put AUTH");

    $buf = &read_until($socket, \&look_for_returncode_length);

    if($buf !~ /200 OK/) {
	&psdebug("AUTH failed");
	# no good
	&closesock($socket);
	return undef;
    }
    &psdebug("AUTH OK");
    return $socket;
}


sub read_until {
    my ($socket, $ref_func) = @_;
    my $buf;
    my $flag;

    while(!$flag) {
	my($rin);
	$rin = '';

	vec($rin, &sockfileno($socket), 1) = 1;
	$count = select($rin, undef, undef, 5.0);
	if($count > 0) {
	    @bits = split(//, unpack("b*", $rin));
	    if($bits[&sockfileno($socket)]) {
		$l = &getsock($socket, \$tmpdata);

		if(!defined($l) || ($l == 0)) {
		    # this is an error
		    &closesock($socket);
#		    die "deadbeef\n";

		    return undef;
		}

		$buf .= $tmpdata;

#		&psdebug2("buf: $buf");

		if(&$ref_func($buf)) {
		    $flag = 1;
		}
	    } else {
		# this is a problem
		&closesock($socket);
		return undef;
	    }
	} elsif ($count == 0) {
	    # this is a timeout
	    &closesock($socket);
	    return undef;
	} else {
	    &closesock($socket);
	    return undef;
	}
    }
    return $buf;
}

sub sockfileno {
    my ($sock) = @_;

    return fileno($sock->{fh});
}

sub getsock {
    my ($sock, $data) = @_;
    my $tmpdata;

    if(defined($sock->{tls})) {
	&psdebug("TLS read");

	$$data = Net::SSLeay::read($sock->{tls}) or die_if_ssl_error("ssl read");
	return length($$data);
    } else {
	return sysread($sock->{fh}, $$data, 1);
    }
}

sub putsock {
    my ($sock, $data) = @_;

    if(defined($sock->{tls})) {
	&psdebug("TLS write");

	$rc = Net::SSLeay::write($sock->{tls}, $data) or die_if_ssl_error("ssl write: $!");
        return length($data);
    } else {
	return syswrite($sock->{fh}, $data);
    }
}

sub closesock {
    my ($sock) = @_;

    if(defined($sock->{tls})) {
	Net::SSLeay::shutdown($sock->{tls}) or die_if_ssl_error("ssl shutdown: $!");
	return 1;
    } else {
	return close($sock->{fh});
    }
}

sub enable_tls {
    my($sock) = @_;
    my $cts, $ssl;


    # first, we need to negotiate TLS


    $ctx = Net::SSLeay::CTX_tlsv1_new() or die_now("Failed to create SSL_CTX $!");
    Net::SSLeay::CTX_set_options($ctx, &Net::SSLeay::OP_ALL);
    die_if_ssl_error("ssl ctx set options");

    $ssl = Net::SSLeay::new($ctx) or die_now("Failed to create SSL $!");
    Net::SSLeay::set_fd($ssl, &sockfileno($sock));   # Must use fileno
    die_if_ssl_error("ssl set fd");

    &psdebug("connecting as TLS: ", &sockfileno($sock), "!\n");
    $res = Net::SSLeay::connect($ssl);
    die_if_ssl_error("connect");
    if($res <= 0) {
	die_if_ssl_error("ssl connect");
    }

    &psdebug("TLS connection succeeded");

    $sock->{tls} = $ssl;

    return $sock;
}


sub create_sock {
    my($hostport) = @_;
    my($port) = 6005;
    my($sock);

#    print "y\n";
    &psdebug("creating socket");

    if($hostport =~ s/^([^\@]+)\@//) {
#	print STDERR $1, "\n" if $debug;
	($user, $pass) = split(/:/, $1, 2);
    }

    if($hostport =~ s/\:([0-9]+)$//) {
	$port = $1;
    }

    $sock = &create_connection($hostport, $port);

    if(defined($sock)) {
	my($oldfh);
	
	$oldfh = select($sock->{fh}); $| = 1; select($oldfh);
	# do TLS if needed

	if($use_ssl) {
#	    print "yyyy\n";
	    $sock = &hack_tls($sock);
	    if(!defined($sock)) {
		return undef;
	    }
	}

	# do AUTH
	if($user || $pass) {
#	    print "yyyyy\n";
	    $sock = &hack_pass($sock, $user, $pass);
#	    print "yyyyyy\n";
	} else {
	    $sock = &hack_pass($sock, "", "");
#	    &closesock($sock);
#	    $sock =  undef;
	}
    }

    return $sock;
}

sub remove_data()
{
    my($hostport, $dirname, $filename) = @_;
    my($socket) = &create_sock($hostport);
    my($retval);
    my($flag) = 0;
    my($count);
    my($buf);
    my($tmpdata);
    my($oldfh);
    my($rin);

    $oldfh = select($socket); $| = 1; select($oldfh);


    if(!defined($socket)) {
	return -1;
    }

    if (! (print $socket "REMOVE $dirname $filename\n")) {
	# error
	return -1;
    }

    while(!$flag) {
	my($rin);
	$rin = '';

	vec($rin, fileno($socket), 1) = 1;
	$count = select($rin, undef, undef, undef);
	if($count > 0) {
	    @bits = split(//, unpack("b*", $rin));
	    if($bits[fileno($socket)]) {
		$l = sysread($socket, $tmpdata, 1);
		$buf .= $tmpdata;
		if($buf =~ /\n/) {
		    $flag = 1;
		}
	    } else {
		# this is a problem
		close($socket);
		return -1;
	    }
	} elsif ($count == 0) {
	    # this is a timeout
	    close($socket);
	    return -1;
	} else {
	    close($socket);
	    return -1;
	}
    }

    if($buf !~ /200 OK/) {
	# no good
	close($socket);
	return -1;
    }


    if(!close($socket)) {
	# error
	return -1;
    }

    return 0;
}


sub login {
    my($hostport) = @_;

    my($socket) = &create_sock($hostport);
    if(!defined($socket)) {
	return undef;
    }
    &closesock($socket);
    return 1;
}


sub put_data {
    my($hostport, $dirname, $filename, $data) = @_;
    my($socket) = &create_sock($hostport);
    my($buf);

    if(!defined($socket)) {
	return -1;
    }

    $len = length($data);
    if (! (&putsock($socket, "PUT $dirname $filename\n") && 
	   &putsock($socket, "Content-Length: $len\n") &&
	   &putsock($socket, $data) &&
	   &putsock($socket, "\n"))) {
	# error
	return -1;
    }

    $buf = &read_until($socket, \&look_for_returncode);

    if($buf !~ /200 OK/) {
	# no good
	&closesock($socket);
	return -1;
    }


    if(!&closesock($socket)) {
	# error
	return -1;
    }

    return 0;
}


sub list_data {
    my($hostport, $listname) = @_;
    my($socket) = &create_sock($hostport);
    my($flag, $len, $buf, $data, $rin);
    my($gotheader);

    if(!defined($socket)) {
	return undef;
    }

    &putsock($socket, "LIST $listname $VPP_VERSION\n\n");

    $buf = &read_until($socket, \&look_for_returncode);

    &closesock($socket);

    return $buf;
}


sub get_data {
    my($hostport, $dirname, $filename) = @_;

#    print "x\n";

    my($socket) = &create_sock($hostport);
    my($buf, $retcode);
    my($gotheader, $l);

    if (!defined($socket)) {
	return undef;
    }

    &psdebug("getting data...");

    &putsock($socket, "GET $dirname $filename $VPP_VERSION\n\n");

    $buf = &read_until($socket, \&look_for_returncode_length);
    &closesock($socket);

    if($buf =~ s/^([0-9]+) (\w+)\nContent-Length: ([0-9]*)\n//) {
	$retcode = $1;
    }

    if($retcode != 200) {
	return undef;
    }
    return $buf;
}


sub get_data2 {
    my($hostport, $dirname, $filename) = @_;

#    print "x\n";

    my($socket) = &create_sock($hostport);
    my($buf, $retcode);
    my($gotheader, $l);

    if (!defined($socket)) {
        &psdebug("Could not create socket to hostport: $hostport");
	return ("", "Could not create socket to hostport: $hostport");
    }

    &psdebug("getting data...");

    &putsock($socket, "GET $dirname $filename $VPP_VERSION\n\n");

    $buf = &read_until($socket, \&look_for_returncode_length);
    &closesock($socket);

    if($buf =~ s/^([0-9]+) (\w+)\nContent-Length: ([0-9]*)\n//) {
	$retcode = $1;
    }

    if($retcode != 200) {
        &psdebug("Ret-code is not correct: $retcode\n");
	return ("", "Return code not 200, was: $retcode, buf: $buf\n");
    }
    return ($buf, "");
}


sub delete_user {
    my($hostport, $username) = @_;
    my($socket) = &create_sock($hostport);
    my($flag, $len, $buf, $data);
    my($gotheader);

    if (!defined($socket)) {
	return undef;
    }

    &putsock($socket, "DELETEUSER $username \n\n");

    $buf = &read_until($socket, \&look_for_returncode_length);

    if($buf =~ /200 OK/) {
	return 0;
    } else {
	return undef;
    }

    &closesock($socket);

    if($flag == 1) {
	return 0;
    } else {
	return undef;
    }
}

sub psdebug3 {
    $psdebug_string .= join("\n", @_) . "\n";
}

sub psdebug {
    my(@y) = @_;
    my $s;
    if($debug_enable & 2) {
	$s = join('\n', @y);
	print $s;
	print "\n";
    } elsif ($debug_enable == 1) {
	$psdebug_string .= join(" ", @_) . "\n";
    }
}

sub psdebug2 {
    my(@y) = @_;
    my $s;
    if($debug_enable & 4) {
	$s = join(" ", @y);
	print $s;
	print "\n";
    }
}


sub psdebug_get {
    my $ret = $psdebug_string;
    $psdebug_string = "";
    return $ret;
}

sub psdebug_enable {
    ($debug_enable) = @_;
    if($debug_enable & 2) {
	print "enabling psdebug...\n";
	# flush queued messages
	print $psdebug_string;
	$psdebug_string = "";
    }
}

END {
}


1;
