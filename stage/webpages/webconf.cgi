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

# $Id: webconf.cgi,v 1.1 2004/05/01 04:15:31 greear Exp $

use CGI qw/:standard/;
use CGI ':cgi-lib';
use provactions;
use vprov::vpp qw/:debug/;
use vprov::ui;

&psdebug("started: " . time());
#&psdebug_enable(1);
#$debug = 1;


$query = new CGI;


# figure out local host IP address
# NOTE:  Be able to handle machines with multiple interfaces if this
#  ever is un-commented and used somehow. --Ben
#chomp($hostname = `hostname`);
#($junk1,$junk2,$junk3,$junk4,@addr) = gethostbyname($hostname);
#($a,$b,$c,$d) = unpack('C4', $addr[0]);
#$local_ip = "$a.$b.$c.$d";


$script_url = $ENV{SCRIPT_NAME};


# todo:

# need to fix

# SystemConfiguration/GlobalConfigData.xml

# set <redirectReasonInSIP value="true"></redirectReasonInSIP>

# to true not false

# should the software make sure audio files are in the right format
# via sox ?  I think it should.  the right command to fix files which
# are wav files is

# sox voicemail/vmhome/2000/custom.wav -U voicemail/vmhome/2000/a.wav

# need to clean up code -- use more of CGI and be happier

# need to fix actual allinoneconfigure code to not need java.

# 



sub show_one_user {
    my($userid) = @_;

    my($output);
    my($auth_type);
    my($parameters) = &get_data_one_user($userid);

    print STDERR $user_parameter_list;

    if($parameters) {
#	$upload_custom_greeting_txt = filefield('uploaded_greeting');
	
	
	$output = start_multipart_form(-method=>"post",
				       -action=>"$script_url");
	$output .= "\n";
	$output .= "<input type=hidden name=\"cmd\" value=\"write\">\n";
	$output .= "<input type=hidden name=\"user\" value=\"$userid\">\n";
	$output .= &vprov::ui::original_values($parameters, \@user_parameter_list);
	$output .= "\n";
	$output .= "<table width=100% border=\"0\" cellpadding=\"2\" cellspacing=\"0\">\n";

	$output .= &vprov::ui::output_list($parameters, \@user_parameter_list);

 	$output .= Tr(td("<input type=reset name=\"_reset\" value=\"Revert\">"),
 		      td("<input type=submit name=\"_submit\" value=\"Save\">"));

	$output .= "</table>\n";
	$output .= "</form>\n";
	
	$output .= "<table width=100% bgcolor=\"#c0c0ff\">\n";
	$output .= Tr(td({-align=>"right", -colspan=>2},
			 "<a href=\"$script_url\">Return to main menu</a>"));
	$output .= "</table>\n";
    } else {
	$output = "Unable to find user $userid<p>";
    }
    return $output;
}


sub show_users {
    my($users) = &get_account_list();
    my $user;
    my $text;
    my $count = 0;
    my (@color) = ("#ffffff", "#e0e0ff");

    
    $text .= "<table cellspacing=0 border=0 width=\"100%\">";

    foreach $user (sort @$users) {
	$text .= Tr({-bgcolor=>$color[$count % 2]}, 
		    td( "<a href=\"$script_url?cmd=show&user=$user\">$user</a>"), 
		   );
	$count = !$count;
    }
#    $text .= Tr({-bgcolor=>$color[$count % 2]}, 
#		    td({-colspan=>2, -align=>"right"}, "<a href=\"$script_url?cmd=adduser\">Add user</a>"));
    $text .= "</table>";
    $text .= "\n";
    return $text;
}


sub select_change_user {
    my($users) = &get_account_list();
    my $user;
    my $text;
    my $count = 0;
    my (@color) = ("#ffffff", "#e0e0ff");

    
    $text .= "<table cellspacing=0 border=0 width=\"100%\">";

    foreach $user (sort @$users) {
	$text .= Tr({-bgcolor=>$color[$count % 2]}, 
		    td( "<a href=\"$script_url?cmd=show&user=$user\">$user</a>"), 
		   );
	$count = !$count;
    }
#    $text .= Tr({-bgcolor=>$color[$count % 2]}, 
#		    td({-colspan=>2, -align=>"right"}, "<a href=\"$script_url?cmd=adduser\">Add user</a>"));
    $text .= "</table>";
    $text .= "\n";
    return $text;
}


sub select_delete_user {
    my($users) = &get_account_list();
    my $user;
    my $text;
    my $count = 0;
    my (@color) = ("#ffffff", "#e0e0ff");

    
    $text .= "<table cellspacing=0 border=0 width=\"100%\">";

    foreach $user (sort @$users) {
	$text .= Tr({-bgcolor=>$color[$count % 2]}, 
		    td("<a href=\"$script_url?cmd=delete&user=$user\">Delete $user</a>") 
		   );
	$count = !$count;
    }
    $text .= "</table>";
    $text .= "\n";
    return $text;
}



sub show_gw {
    my($param) = &get_gw_data();

    # get the value from somewhere

    $output = "<form method=post action=\"$script_url\">";
    $output .= "<input type=hidden name=\"cmd\" value=\"writegw\">";

    $output .= "<table width=100%>";

    $output .= Tr(td("Gateway (in form host:port):"), 
		  td("<input type=text name=\"gw_location\" value=\"$$param{gw_location}\"></input>"));

    $output .= Tr(td("<input type=reset name=\"_reset\" value=\"Revert\">"),
		  td("<input type=submit name=\"_submit\" value=\"Save\">"));
    $output .= "</table>";

    $output .= "</form>";

    $output .= "<table width=100% bgcolor=\"#c0c0ff\">";
    $output .= Tr(td({-align=>"right", -colspan=>2},
		     "<a href=\"$script_url\">Return to main menu</a>"));
    $output .= "</table>";
    return $output;
}


sub html_header {
    my ($cookie_user, $cookie_pass) = @_;
    my($output);
    my($hostname);

    if($cookie_user && $cookie_pass) {
	$output = $query->header(-expires=> '-1y', -cookie=>[$cookie_user, $cookie_pass], -pragma => 'nocache');
    } else {
	$output = $query->header(-expires=> '-1y', -pragma => 'nocache');
    }

    chomp($hostname = `hostname`);
    $output .= "<html><head><title>VOCAL Configuration for $hostname</title></head><body bgcolor=ffffff>";
    $output .= "<TABLE width=\"100%\" border=0 cellspacing=0 cellpadding=0>";
    $output .= Tr(td({-bgcolor=>"#ffd0ff", -align=>center}, "<font size=+4>VOCAL Configuration for $hostname</font>"));
    $output .= Tr(td({-align=>right, -bgcolor=>"#ffd0ff"},
		     &show_actions()
		    ));
    $output .= "<TR><TD>";
    return $output;
}


sub html_footer {
    my($output);

    $output .= "</TD></TR></TABLE>\n";
    $output .= "</BODY></HTML>";
    return $output;
}


sub get_actions {
    my($single_cmd);
    my($cmdarray) = [];

    foreach $single_cmd (sort keys %$cmd) {
	if($$cmd{$single_cmd}{menu}) {
	    $$cmdarray[$$cmd{$single_cmd}{menu} - 1]
	      = "<a href=\"$script_url?cmd=$single_cmd\">$$cmd{$single_cmd}{text}</a>";
	}
    }

    return $cmdarray;
}


sub show_actions {
    my($cmdarray) = &get_actions();
    my($single_output);
    my($output);

    foreach $single_output (@$cmdarray) {
	if($output) {
	    $output .= " | ";
	}
	$output .= $single_output;
    }

    return $output;
}


sub show_actions_table {
    my($cmdarray) = &get_actions();
    my($single_output);
    my($text) = "<table bgcolor=white width=\"100%\">";
    my (@color) = ("#ffffff", "#e0e0ff");
    my($count);

    shift(@$cmdarray);

    foreach $single_output (@$cmdarray) {
	$text .= Tr({-bgcolor=>$color[$count % 2]}, 
		    td( $single_output), 
		   );
	$count = !$count;
    }

    $text .= "</table>";

    return $text;
}


sub inactive_box {
    return &vprov::ui::box("Provisioning not running", 
		     "VOCAL is not running now.<P>" .
		     "Please make sure that VOCAL is running ". 
		     "before trying to run this system." .
		     "<P><a href=\"$script_url\">" .
		     "Return to main menu</a>", 
		     300);
}


sub log_out {
    return "thank you for logging out.";
}


sub log_in {
    my $output;

    $output = start_multipart_form(-method=>"post",
				   -action=>"$script_url");
    $output .= "<input type=hidden name=\"cmd\" value=\"cookie\">";
    $output .= "<table width=100%>";
    $output .= Tr(td("Username"), 
		  td("<input type=text name=\"username\"></input>" ));
    $output .= Tr(td("Password"), 
		  td("<input type=password name=\"password\"></input>" ));

    $output .= Tr(td("<input type=reset name=\"_reset\" value=\"Revert\">"),
		  td("<input type=submit name=\"_submit\" value=\"Log in\">"));
    $output .= "</table>";
    $output .= "</form>";

    return $output;
}


sub bad_log_in {
    my $output;

    $output = "Your login was incorrect or expired.  Please log in again.<p>";
    $output .= &log_in();

    return $output;
}

sub Vars {
    my(@list) = $query->param();
    my($params) = {};
    foreach $item (@list) {
	$$params{$item} = $query->param($item);
    }
    return $params;
}

######################################################################

sub command_login {
    print &vprov::ui::box("Login", &log_in(), 200);
}

sub command_bad_login {
    print &vprov::ui::box("Bad Login", &bad_log_in(), 200);
}


sub command_logout {
    print &vprov::ui::box("Thank you for logging out", &log_out(), 200);
}

sub command_main {
    print &vprov::ui::box("Choose an action", &show_actions_table(), 200);
}

sub command_changeuser {
    print &vprov::ui::box("Choose user to change", &select_change_user(), 200);
}


sub command_deleteuser {
    print &vprov::ui::box("Choose user to delete", &select_delete_user(), 200);
}


sub command_show {
    print &vprov::ui::box("User " . param('user'), 
		    &show_one_user(param('user')), 
		    400);
}


sub command_adduser {
    my($output);

    $output = "<form method=post action=\"$script_url\">";
    $output .= "<input type=hidden name=\"cmd\" value=\"create\">";
    $output .= "Enter new user id: <input type=text name=\"user\" value=\"$userid\">\n";
    $output .= "<p><input type=submit name=\"_submit\" value=\"Create User\"></input>";
    $output .= "</form>";
    
    print &vprov::ui::box("Add user",
		    $output,
		    400);
}


sub command_create {
    $params = Vars;
    
    my($output) = &create_new_user($params);
    
    if($output !~ /failed/i) {
	$output .= "<p>";
	$output .= "<a href=\"$script_url?cmd=show&user=$$params{user}\">";
	$output .= "Go to to user page</a>";
	$output .= "<br>";
    }
	
    $output .= "<a href=\"$script_url\">";
    $output .= "Return to main menu</a>";
	
    print &vprov::ui::box("Trying to create user \"" . param('user') .'"',
		    $output,
		    400);
}


sub command_write {
    $params = Vars;
    my($fh);

    if ($$params{'uploaded_greeting'}) {
	$fh = $query->upload('uploaded_greeting');
    }
    my($output) = &update_one_user($params, $fh);
    
    $output .= "<p>";
    $output .= "<a href=\"$script_url?cmd=show&user=$$params{user}\">";
    $output .= "Return to user page</a>";
    
    $output .= "<br><a href=\"$script_url\">";
    $output .= "Return to main menu</a>";
    
    print &vprov::ui::box("Trying to save user \"" . param('user') .'"',
		    $output,
		    400);
}


sub command_delete {
    $params = Vars;
    
    my($output) = &delete_one_user($params);
    
    $output .= "<p>";
    $output .= "<a href=\"$script_url\">";
    $output .= "Return to main page</a>";
    
    print &vprov::ui::box("Trying to delete user \"" . param('user') .'"',
		    $output,
		    400);
}


sub command_setvmdialplan {
    $params = Vars;
    my($output) = &set_vm_dialplan();
    
    $output .= "<p>";
    $output .= "<a href=\"$script_url\">";
    $output .= "Return to main page</a>";

    print &vprov::ui::box("Trying to create VM dialplan",
		    $output,
		    400);
}


sub command_writegw {
    $params = Vars;
    
    my($output) = &update_gw($params);
    
    $output .= "<p><a href=\"$script_url\">";
    $output .= "Return to main menu</a>";
    
    print &vprov::ui::box("Trying to write Gateway",
		    $output,
		    400);
}


sub command_showgw {
    $params = Vars;
    print &vprov::ui::box("Change Gateway",
		    &show_gw(),
		    400);
}

sub command_showstatus {
    my($output)="<table width=\"100%\" border=\"0\" cellspacing=\"0\">";
    my (@color) = ("#ffffff", "#e0e0ff");
    my $count = 1;

    $output .= Tr({ -align=>LEFT}, th(['Process', 'Port', 'PID']));

    open STATUS, "../bin/vocalctl status |" or die "can't fork";
    while (<STATUS>) {
      ($proc, $port, $pid) = split;
      if ( ($proc eq "ps") || ($proc eq "vmserver") )
      {
        $pid = $port;
        $port = "";
      }
      $output .= Tr({ -align=>LEFT, -bgcolor=>$color[$count % 2]  }, td([$proc, $port, $pid]));
      $count++;
    }
    $output .= "</table>";

    print &vprov::ui::box("System Status",
		    $output,
		    400);
}


$cmd = {
	main => { code => \&command_main, text => "Main menu", menu => 1 },
	adduser => { code => \&command_adduser, text => "Add user", menu => 2 },
	changeuser => { code => \&command_changeuser, text => "Change user", menu => 3 },
	deleteuser => { code => \&command_deleteuser, text => "Delete user", menu => 4 },
#	setvmdialplan => { code => \&command_setvmdialplan, text => "Enable VM dialplan", menu => 5 },
	showgw => {  code => \&command_showgw, text => "Change gateway", menu => 5 },

	showstatus => {  code => \&command_showstatus, text => "System Status", menu => 6 },
	show => { code => \&command_show, text => "Show" },
	write => { code => \&command_write, text => "Write" },
	create => { code => \&command_create, text => "Create" },
	delete => { code => \&command_delete, text => "Delete" },
	writegw => { code => \&command_writegw, text => "Writegw" },
	login => { code => \&command_login, text => "Log in", menu => 7 },
	login_again => { code => \&command_login, text => "Log in again" },
	bad_login => { code => \&command_bad_login, text => "Bad login" },
	logout => { code => \&command_logout, text => "Log out", menu => 8 },
       };

if($query->param()) {
    $command = $query->param('cmd');
} else {
    $command = "main";
}

if($command eq "logout") {
    $cookie_user = cookie(-name => "user",
			  -value => "",
			  -path => "/vocalconf/",
			  -expires => "");
    $cookie_pass = cookie(-name => "pass",
			  -value => "",
			  -path => "/vocalconf/",
			  -expires => "");
} elsif($command eq "cookie") {
    # form a cookie
    my $rv;
    my $err;
    ($rv, $err) = &set_user_pass2($query->param("username"), $query->param("password"));
    if (length($err)) {
      &psdebug("error from set_user_pass2: $err");
    }

    if ($rv) {
	$cookie_user = cookie(-name => "user",
			      -value => param('username'),
			      -path => "/vocalconf/",
			      -expires => "");
	$cookie_pass = cookie(-name => "pass",
			      -value => param('password'),
			      -path => "/vocalconf/",
			      -expires => "");
	$command = "main";
	$verify_ok = &verify_active();
	&psdebug("verify: $verify_ok\n");

    } else {
	$command = "bad_login";
    }
} elsif($query->param('debugmode')) {
    print STDERR "\n\nDebug mode\n\n";

    &set_user_pass($query->param("username"), $query->param("password"));
    # debug-mode -- just go forward

    $verify_ok = &verify_active();
    &psdebug("verify: $verify_ok\n");
} elsif( !($query->cookie("user") && $query->cookie("pass")) || 
	 !&set_user_pass($query->cookie("user"), $query->cookie("pass"))) {
    # not logged in.
    $command = "login";
} else {
    # $verify_ok is true if the PSERVER is active
    $verify_ok = &verify_active();
    &psdebug("verify: $verify_ok\n");
}

&psdebug("command: $command\n");

$cmd_to_exec = $$cmd{$command}{code};

if($cookie_user && $cookie_pass) {
    print &html_header($cookie_user, $cookie_pass);
} else {
    print &html_header();
}

if($command eq "login" || $command eq "logout" || $command eq "login_again" || $command eq "bad_login" || $verify_ok) {
    if(!$cmd_to_exec) {
	$cmd_to_exec = \&command_main;
    }
    
    &{$cmd_to_exec}();
} else {
    print &inactive_box();
}

&psdebug("ended: " . time());

print "<PRE>" . &psdebug_get() . "</PRE>\n" if $debug;

print &html_footer();


sub debug {
    $debug_string .= join('\n', @_);
}
