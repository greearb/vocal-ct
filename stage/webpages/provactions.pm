#!/usr/bin/perl

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2002-2003 Vovida Networks, Inc.  All rights reserved.
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

# $Id: provactions.pm,v 1.1 2004/05/01 04:15:31 greear Exp $

package provactions;

use Exporter;
use vprov::vpp;

use vprov::cplxml;
use vprov::serverxml;
use vprov::userxml;

BEGIN {
#    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    # if using RCS/CVS, this may be preferred
    $VERSION = do { my @r = (q$Revision: 1.1 $ =~ /\d+/g); sprintf "%d."."%02d" x $#r, @r }; # must be all one line, for MakeMaker
    @ISA         = qw(Exporter);
    @EXPORT      = qw(
		      &set_user_pass
		      &set_user_pass2
		      &get_account_list
		      &get_vm_box
		      &get_data_one_user
		      &create_new_user
		      &validate_input
		      &print_input

		      &update_one_user
		      &get_gw_data
		      &update_gw

		      &delete_one_user
		      &set_vm_dialplan
		      &verify_active

		      &u_create_user
		      &u_delete_user
		      &u_login
		      &u_print_user
		      
		      @user_parameter_list
		     );


    %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],
    
    # your exported package globals go here,
    # as well as any optionally exported functions
    @EXPORT_OK   = qw();
}
#our @EXPORT_OK;

@user_parameter_list = 
  (
   { name => "user", type => "label", desc => "User ID" },
   { name => "use_passwd", type => "checkbox", desc => "Use Password" },
   { name => "passwd", type => "password", desc => "Password" },
#   { name => "allow_local", type => "checkbox", desc => "Allow local calls" },
#   { name => "allow_gw", type => "checkbox", desc => "Allow gateway calls" },
   { name => "enable_vm", type => "checkbox", desc => "Enable Voicemail" },
   { name => "voicemail_addr", type => "text", 
     desc => "Voicemail Email Address (voicemail sent to this address)" },
   { name => "use_custom", type => "checkbox", desc => "Use custom greeting" },
   { name => "uploaded_greeting", type => "filefield", 
     desc => "Upload custom greeting" }
);

my $errmsg;

&psdebug("started loading provactions.pm: ".time() . "\n");

$VOCAL_BASE = "/usr/local/vocal";  # VOCAL_BASE
$VOCAL_BASE_CONFIGURED = '/usr/local/vocal';
if($VOCAL_BASE_CONFIGURED ne '@'.'prefix@') {
    $VOCAL_BASE = $VOCAL_BASE_CONFIGURED;
}


$vmhome_dir = "$VOCAL_BASE/var/vmhome";

chomp($hostname = `hostname`);
($junk1,$junk2,$junk3,$junk4,@addr) = gethostbyname($hostname);
($a,$b,$c,$d) = unpack('C4', $addr[0]);
$local_ip = "$a.$b.$c.$d";


$local_port = 6005;

# Get the IP from the config file if it exists there...
if (open(CFGFILE, "</usr/local/vocal/etc/vocal.conf")) {
  while (<CFGFILE>) {
    my $ln = $_;
    if ($ln =~ /^\s*PSERVER\s+(.*):(\d*)/) {
      $local_ip = $1;
      $local_port = $2;
    }
  }
}

$pserver_hostname;

sub set_user_pass2 {
    my ($username, $password) = @_;
    my ($data);

    if(defined(&u_login($username, $password, $local_ip, $local_port))) {
	return (1, "");
    } else {
	return (0, "u_login failed, username: $username  host: $host");
    }
}


sub set_user_pass {
    my ($username, $password) = @_;
    my ($data);

    if(defined(&u_login($username, $password, "$local_ip", $local_port))) {
	return 1;
    } else {
	return 0;
    }
}

sub verify_active {
    # verify that provisioning system is active
    my $data = &get_data($pserver_hostname, "SystemConfiguration", "GlobalConfigData");
    if(defined($data)) {
	return 1;
    } else {
	return 0;
    }
}

sub get_account_list {
    my($users) = [];
    @$users = split(/,/, &list_data($pserver_hostname, "Accounts"));
    pop(@$users);

    return $users;
}


sub get_vm_box {
    my($params) = @_;

    open(CONFIG, "$vmhome_dir/$$params{user}/config") || return -1;
    while(<CONFIG>) {
	chomp;
	if(s/^SendTo\s+string\s+//) {
	    $$params{voicemail_addr} = $_;
	}
	if(s/^Greeting\s+string\s+//) {
	    if(/custom\.wav/ && -f "$vmhome_dir/$$params{user}/custom.wav") {
		$$params{use_custom} = 1;
	    }
	}
    }
    close(CONFIG) || return -1;

    return;
}


sub get_data_one_user {
    my($userid) = @_;
    my($data) = &get_data($pserver_hostname, "Accounts", $userid);
    my($parameters);

    
    if($data) {
	$parameters = &parse_parameters_user($data);
	$$parameters{user} = $userid;
	# get vm data
	&get_vm_box($parameters);
	
	return $parameters;
    } else {
	return undef;
    }
}


sub put_data_one_user {
    my($userid, $account, $called, $calling, $fna) = @_;

    my $ret;
    my $totalret = 1;
    my $errmsg;

    $ret = &put_data($pserver_hostname, "Accounts", $userid, $account);
    if($ret) {
	$errmsg .= "\ncould not write Account data\n";
	$totalret = undef;
    }

    $ret = &put_data($pserver_hostname, "Contact_Lists_Called", $userid, $called);

    if($ret) {
	$errmsg .= "\ncould not write Contact_Lists_Called data\n";
	$totalret = undef;
    }



    $ret = &put_data($pserver_hostname, "Contact_Lists_Calling", $userid, $calling);

    if($ret) {
	$errmsg .= "\ncould not write Contact_Lists_Calling data\n";
	$totalret = undef;
    }

    $ret = &put_data($pserver_hostname, "Features_${local_ip}_5105_Called", "$userid.cpl", $fna);

    if($ret) {
	$errmsg .= "\ncould not write Features 5105 data\n";
	$totalret = undef;
    }

    return ($totalret, $errmsg);
}


sub validate_user {
    my($user) = @_;

    if(($user =~ /^[0-9]{4,16}$/ || $user =~ /^test[0-9]{4,16}/) && $user ne "7000" && $user ne "8000" ) {
	return 1;
    } else {
	return 0;
    }
}


sub u_create_user {
    my($userid, $novm) = @_;

    my($params) = {};

    $params->{user} = $userid;

    return &i_create_user($params, $novm);
}


sub u_delete_user {
    my($userid) = @_;

    my($params) = {};

    $params->{user} = $userid;

    return &i_delete_user($params);
}

sub u_login {
    my($username, $password, $host, $port) = @_;
    my($retval, $output);

    $retval = undef;

    if ($#_ != 3) {
	$output = "wrong number of arguments";
	$retval = undef;
    } else {
	$pserver_hostname = "$username:$password\@$host:$port";
	my ($data);
	$data = &login($pserver_hostname);

	&psdebug("Username: $username\nPassword: $password\nHost: $host\nPort: $port\n");
	
	if(defined($data)) {
	    &psdebug("Username: $username\nPassword: $password login succeeded\n");
	    $retval = 1;
	    $output = "login succeeded";
	} else {
	    &psdebug("Username: $username\nPassword: $password\n login FAILED\n");
	    $retval = undef;
	    $output = "login failed";
	}
    }

    return wantarray ? ($retval, $output) : $retval;
}

sub u_print_user {
    my($userid) = @_;

    my($params) = &get_data_one_user($userid);

    return &i_print_user($params);
}


sub i_print_user {
    my($params) = @_;

    my($retval);
    my($output);
    my($single);

    if($params) {
	# now, print based on this
	foreach $single (sort keys %$params) {
	    $output .= "$single:  $$params{$single}\n";
	}
	$retval = 1;
    } else {
	$retval = undef;
	$output = "could not get data for user";
    }
    return wantarray ? ($retval, $output) : $retval;
}


sub i_create_user {
    my($params, $novm) = @_;
    my($output);
    my($params_clean) = {};
    my($retval);

    #    print "xxx: $$params{user} xxx\n\n";
    # need to verify that user does not already exist
    # need to verify that user has "safe" userid
    
    if(&validate_user($$params{user})) {
	$$params_clean{user} = $$params{user};
	$$params_clean{passwd} = "password";
	my($user, $user_errmsg) = &update_one_user_entry($params_clean);
	my($vm, $vm_errmsg);

	if($user) {
	    ($vm, $vm_errmsg) = &create_vm_box($params_clean);
	    if($vm || $novm) {
		$output .= "Created user \"$$params{user}\" successfully";
		$retval = 1;
	    } else {
		$output .= "Failed to create user \"$$params{user}\": $!";
		$output .= "<br>voice mailbox creation failed<p>";
		$output .= "vm reason: $vm_errmsg";
		$retval = undef;
	    }
	} else {
	    $output .= "Failed to create user \"$$params{user}\": $!";
	    $output .= "<br>user creation failed<P>";
	    $output .= "reason: $user_errmsg";
	    $retval = undef;
	}
    } else {
	# invalid user
	if($$params{user} ne "7000") {
	    $output = "Failed: Users may have 4-16 digit user IDs only<p>";
	    $retval = undef;
	} else {
	    $output = "Failed: User 7000 is reserved for voice mail<p>";
	    $retval = undef;
	}
    }

    return wantarray ? ($retval, $output) : $retval;
}


sub create_new_user {
    my ($retval, $text) = &i_create_user(@_);
    return $text;
}


sub validate_input {
    my($params) = @_;

    # validate input here, return 1 if ok.

    if(
       # validate the user
       &validate_user($$params{user})  &&

       # validate that passwords can only contain "reasonable" characters

       $$params{passwd} =~ /^([0-9A-Za-z\!\@\#\$\%\^\&\*\(\)\[\]\{\}\|\\\,\.\<\>\/\?]*)$/
      ) {
	return 1;
    } else {
	return 0;
    }
}


sub print_input {
    my($params) = @_;
    my($output, $tmp);

    foreach $tmp (sort keys %$params) {
        $output .= $tmp . " : " . $$params{$tmp} . "<p>";
    }
    print $output;
}


sub get_original_params {
    my($params) = @_;
    my($original_params) = {};
    my($key);

    foreach $key (sort keys %$params) {
	my($trimmed_key) = $key;
	if($trimmed_key =~ s/^original_//) {
	    # remove from params, put in original_params
	    $$original_params{$trimmed_key} = $$params{$key};
#	    undef($$params{$key});
	}
    }

    return $original_params;
}


sub params_changed {
    my($params) = @_;
    my($key);
    my($original_params) = &get_original_params($params);
    my(@changed_params);

    foreach $key (@user_parameter_list) {
	if($$params{$key->{name}} ne $$original_params{$key->{name}}) {
	    &psdebug($key->{name} . " has been changed");
	    push(@changed_params, $key->{name});
	}
    }
    return @changed_params;
}


# returns:
# -1 on error, 
# 0 if ok, 
# 1 if unchanged

sub update_one_user_entry {
    my($params) = @_;
    my(@changed_params) = &params_changed($params);

    if(&validate_input($params)) { 
	my($new_user_xml);

	my($data) = &get_data($pserver_hostname, "Accounts", $$params{user});
	if($data) {
	    &psdebug("got data");
	    $new_user_xml = &modify_user_xml($params, \@changed_params, $data);
	    &psdebug("output", $new_user_xml);
	} else {
	    &psdebug("from scratch");
	    $new_user_xml = &create_user_xml($params);
	}

	my($new_called_xml) = &create_contact_called_from_user($new_user_xml);
	my($new_calling_xml) = &create_contact_calling_from_user($new_user_xml);
	my($new_fna_xml) = &create_fna_feature_xml($params);
	
	my($ret, $err) = &put_data_one_user($$params{user}, 
					    $new_user_xml, 
					    $new_called_xml,
					    $new_calling_xml,
					    $new_fna_xml);
	
	return ($ret, $err);
    } else {
	return (undef, "invalid parameters");
    }
}


# returns -1 on error or 0 if ok
sub update_one_user {
    my($params, $fh) = @_;
    my($output);

    if(&params_changed($params)) {
	my($r, $r_err) = &update_one_user_entry($params);
	my($r2, $r2_err) = &update_vm_box($params, $fh);
	if($r && $r2) { 
	    $output = "Saved user \"$$params{user}\" successfully";
	} else {
	    $output .= "Failed to save user \"$$params{user}\" successfully";
	    $output .= "\n<p>\n";
	    if(!$r) {
		$output .= $r_err;
	    }
	    if(!$r2) {
		$output .= $r2_err;
	    }
	}
    } else {
	$output = "No data changed.";
    }

    return $output;
}


sub get_gw_data {
    my($data) = &get_data($pserver_hostname, "Marshals", "${local_ip}:5065");
    my($param) = {};

    if($data =~ /gateway host=\"((\w|\d|[\.\-])*)\" port=\"([0-9]*)\"/) {
	$$param{gw_location} = "$1:$3";
    }
    return $param;
}


sub update_gw {
    my($params) = @_;
    my($output);

    my($retcode) = 0;
    my($reason);
    # validate here

    if($$params{gw_location} =~ /^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}.[0-9]{1,3}(:[0-9]{1,5})?$/) {
	$retcode = 0;
    } else {
	$retcode = "Illegal gateway location (must be in the form host:port, where host is an IP address)";
    }

    if(!$retcode) {
	my($gw_xml) = &create_gw_marshal_xml($params);
	$retcode = &put_data($pserver_hostname, "Marshals", "${local_ip}:5065", $gw_xml);
	if($retcode) {
	    $retcode = "Unable to write data to provisioning server"; 
	}
    }

    if(!$retcode) {
        $output = "Wrote new gateway successfully";
    } else {
        $output = "Failed to write new gateway successfully.<p>Reason: $retcode";
    }

    return $output;
}


sub update_vm_box {
    my($params, $fh) = @_;
    my($greeting) = "greeting.wav";
    my($vm, $errmsg);

    if(!$$params{enable_vm}) {
	# nothing to do if there's no VM enabled
#	print "no vm enabled\n";
	return 1, "did nothing";
    }

    if(!-d "$vmhome_dir/$$params{user}") {
	($vm, $errmsg)  = &create_vm_box($params);
	if(!$vm) {
	    $output .= "Failed to add voicemail box for user \"$$params{user}\": $!";
	    $output .= "<br>voice mailbox creation failed<p>";
	    $output .= "vm reason: $errmsg";
	    return undef, $output;
	}
    }

    if($$params{use_custom}) {
	# use a custom greeting
	$greeting = "custom.wav";
    }

    # modify the config file with the appropriate bits
    
    open(CONFIG, ">$vmhome_dir/$$params{user}/config") || return undef, "could not write file: $!";
    print CONFIG "Name      string  User\n";
    print CONFIG "SendTo    string  $$params{voicemail_addr}\n";
    print CONFIG "Greeting  string  $greeting\n";
    close(CONFIG) || return undef, "could not write file: $!";

    # if the wav file has been uploaded, upload it.

    if($fh) {
	#	    print "I received an uploaded_greeting!\n";
	open(CUSTOM, ">$vmhome_dir/$$params{user}/custom.wav") || return undef, "could not write file: $!";
	while(<$fh>) {
	    print CUSTOM;
	}
	close(CUSTOM) || return undef, "could not write file: $!";
    }

    return 1, "successfully created voice mailbox";
}


# returns -1 on error or 0 if ok
sub create_vm_box {
    my($params) = @_;

    # test to make sure that I can write in vmhome
    if(!-w $vmhome_dir) {
	return undef, "cannot write directory $vmhome_dir";
    }

    my ($ret) = mkdir "$vmhome_dir/$$params{user}", 0757;
    if(!$ret) {
	return undef, "could not make directory $vmhome_dir/$$params{user}: $!";
    }

    return 1, "created vm box successfully";
}


sub delete_vm_box {
    my($user) = @_;

    if($user !~ /\.\./ && $user !~ /\//) {
	my($userdir) = "$vmhome_dir/$user";
	my($x);
	# allow deletions if things don't have 2 . in them
	# simultaneously or a / 
	opendir(DIR, $userdir) || return undef, "could not read $userdir: $!";
	@files = grep(!/^\.\.?$/, readdir(DIR));
	closedir(DIR);

	foreach $x (@files) {
	    $x = "$userdir/$x";
#	    print "deleting $x...<br>\n";
	    unlink $x || return undef, "could not remove $userdir/$x: $!";
	}

#	unlink @files || return -1;
	rmdir $userdir || return undef, "could not remove userdir: $!";
    } 
    return true, "delete succeeded";
}


sub i_delete_user {
    my($params) = @_;
    my($output);
    my($retval) = undef;

    if(!defined(&delete_user($pserver_hostname, $$params{user}))) {
	# an error
	$retval = undef;
        $output = "Failed to delete user \"$$params{user}\" successfully:<br>";
	$output .= "could not delete user XML data";
    } else {
	my ($vm_r, $vm_err) = &delete_vm_box($$params{user}) || $retcode;
	
	if(!defined($vm_r)) {
	    $output = "Failed to delete user \"$$params{user}\" successfully:<br>";
	    $output .= "could not delete voicemail box";
	    $retval = undef;
	} else {
	    $output = "Deleted user \"$$params{user}\" successfully";
	    $retval = 1;
	}
    }

    return wantarray ? ($retval, $output) : $retval;
}


sub delete_one_user {
    my($retval, $output) = &i_delete_user(@_);
    return $output;
}


sub set_vm_dialplan {
    my($output);
    my($retcode) = &put_data($pserver_hostname,
			     "SystemConfiguration", 
			     "defaultPhonePlan", 
			     &create_digitalplan_xml());
    $retcode = &put_data($pserver_hostname,
			 "SystemConfiguration",
			 "defaultIpPlan", 
			 &create_ipplan_xml()) || $retcode;

    if(!$retcode) {
	$output = "Created VM dialplan successfully";
    } else {
	$output = "Failed to create VM dialplan successfully: $!";
    }

    return $output;
}

return 1;


