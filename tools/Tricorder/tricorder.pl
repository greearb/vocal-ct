#!/usr/bin/perl -w

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

use strict;
use Getopt::Long;

use Socket;
use Tk;
#! Set tk_strict_Motif to get rid of silly highlighting
use Telephony::SipMessage qw ( _new_sdp_headers get_call_id );
use Telephony::SipServer;
use CallFlowCanvas qw ( _construct_canvas );
use FancyBalloon;

#Making these constant allows Perl to optimize away all the 'if DEBUG'-type
#checks at compile time if debugging is turned off.
use constant DEBUG                 => 0;
use constant DEBUG_SIP_MESSAGES    => 0;
use constant DEBUG_LOGICAL_NAMES   => 0;
use constant DEBUG_TRANSLATIONS    => 0;

if (DEBUG || DEBUG_SIP_MESSAGES || DEBUG_LOGICAL_NAMES)
{
    require Data::Dumper;
    Data::Dumper->import;
}

my $program_name   = 'Tricorder';

my %fonts = (window_default_font      => '-B&H-Lucida-Medium-R-Normal-Sans-14-140-75-75-P-81-ISO8859-1'
		);

my %snapshot_preferences;
my %server_map;
my %translation_table;

my $ipgrab_command = 'ipgrab -t -l -n -f ';

#GUI elements everybody needs to play with
my ($window, $call_menu);
my ($snapshot_button, $snapshot_configure_button, $clear_button, $insert_breaks_button);

#maps (one-to-one) call IDs to CallFlowCanvas objects
my %canvases;
my ($currently_displayed_call_id, $currently_displayed_canvas);
my $canvas_number = 1;

my $canvas;

#Use a hash to optimize membership lookups, since both the source and the
#destination of every SIP message will be checked for membership in the
#set of known servers--with a list, that lookup would become expensive.
my %server_hash;
#The list is there only to preserve the order of the servers.  Its information
#is redundant with that in the hash.  That's O.K.; they hold references, not
#values, so (almost) nothing is lost
#This approach is ever-so-slightly faster (albeit less elegant) than using
#a pseudo-hash, because it saves the array lookup in the implicit
#$pseudo_hash[0]{'key'} done at access time.
my @server_list;

#Process options and, in their absence, set default values.

my ($help, $log_file, $use_ip_numbering_scheme, $silent, $batch_process, $no_cisco, $no_pingtel, $pingtel_canonical_port, $translation_table, $heuristic, $tee, $ipgrab_expression, $paginate_snapshots, $logical_names, $snapshot_filename);
GetOptions ('--help'                     => \$help,
	    '--log-file=s'               => \$log_file,
	    '--silent'                   => \$silent,
	    '--batch-process'            => \$batch_process,
	    '--no-cisco'                 => \$no_cisco,
	    '--no-pingtel'               => \$no_pingtel,
	    '--pingtel-canonical-port=i' => \$pingtel_canonical_port,
	    '--translation-table=s'      => \$translation_table,
	    '--heuristic=s'              => \$heuristic,
	    '--tee=s'                    => \$tee,
	    '--ipgrab-expression=s'      => \$ipgrab_expression,
	    '--paginate-snapshots:s'     => \$paginate_snapshots,
	    '--logical-names=s'          => \$logical_names,
	    '--snapshot-filename=s'      => \$snapshot_filename
	    );

if (defined $help)
{
    &usage();
}
if (defined $batch_process)
{
    if (!defined $log_file)
    {
	&usage ("You must specify a log file on the command line, using the\n"
		. "switch --log-file, for batch processing to work.");
    }
    CallFlowCanvas::batch_mode (1);
}
if (defined $no_pingtel && defined $pingtel_canonical_port)
{
    &usage ('It makes no sense to specify both --no-pingtel and --pingtel_canonical_port');
}

if (defined $log_file)
{
    if (defined $ipgrab_expression)
    {
	&usage ("It makes no sense to specify --log-file and --ipgrab-expression at once,\n"
	     . "since using a pre-made log file implies that ipgrab will not"
	     . "be run interactively.");
    }
    elsif (defined $tee)
    {
	&usage ("It makes no sense to specify --log-file and --tee at once,\n"
	     . "since using a pre-made log file implies that you already have\n"
	     . "a log, hence you don't need to create one with tee.");
    }
}
else #default:  read from a running ipgrab
{
    $ipgrab_command .= (defined $ipgrab_expression) ? $ipgrab_expression : 'port 5060';
    print "\$ipgrab_command is $ipgrab_command\n" if DEBUG;
    $log_file = "$ipgrab_command|";
}
if (defined $heuristic)
{
    unless ($heuristic eq 'assume-server' ||
	    $heuristic eq 'use-vovida-ip-numbering-scheme' ||
	    $heuristic eq 'use-sip-hints')
    {
	&usage ("You entered an invalid argument to --use-heuristic.\n"
	      . "Please use one of the arguments described below.");
    }
}
else # $heuristic is not defined
{
    $heuristic = 'assume-server'; #default:  Treat everything as a server.
}
if (defined $tee)
{
    open (TEE, "<$tee") #!sysopen for security!
	or warn "Could not open $tee to collect ipgrab output:  $!";
}
if (defined $paginate_snapshots)
{
    if (! $paginate_snapshots)
    {
	#the default if no argument to --paginate-snapshots is given
	$paginate_snapshots = 'dumb'; 
    }
    elsif ($paginate_snapshots ne 'smart' && $paginate_snapshots ne 'dumb')
    {
	&usage ("You entered an invalid argument to --paginate-snapshots.\n"
	      . "Please use one of the arguments described below.");
    }
}

#Read preferences from the .tricorderrc file, if it exists.
my $HOME = $ENV{HOME} || '.';
if (-r "$HOME/.tricorderrc")
{
    #Read in and eval the saved value of %snapshot_preferences, which
    #was written by Data::Dumper.
    print "Found a .tricorderrc; reading preferences...\n" if DEBUG;

    my $preferences;

    open RC_FILE, "$HOME/.tricorderrc"
	or die "Race condition:  $HOME/.tricorderrc was readable a moment ago, but now it is not:  $!";
    {
	local $/ = undef;          #Prepare to slurp.
	$preferences = <RC_FILE>;  #Slurp!
    }
    eval $preferences
	or die "Malformed preferences file $HOME/.tricorderrc:  $!\n"
	     . "If you don't know how to fix it, just remove it and\n"
	     . "start over, specifying new preferences.";

    close RC_FILE
	or warn "Could not close $HOME/.tricorderrc:  $!";

    print "Done reading preferences.\n" if DEBUG;
}

#Override the value of file if the appropriate command-line switch was given.
if (defined $snapshot_filename)
{
    $snapshot_preferences{file} = $snapshot_filename;
}

#Read in the mapping of IP:port combinations to logical names if
#--logical-names was specified
if ($logical_names)
{
    my $server_map_code;
    open SERVER_MAP, $logical_names
	or die "Could not open server map $logical_names:  $!";
    {
	local $/ = undef;                 #Prepare to slurp.
	$server_map_code = <SERVER_MAP>;  #Slurp!
    }
    eval $server_map_code
	or die "Malformed server map (from file $logical_names):  $!"; #! confess?
    print "Got server map:\n" . Data::Dumper->Dump ([ \%server_map ], 'server_map') if DEBUG_LOGICAL_NAMES;
}

#Read in the table of additional translations, if one was specified.
if (defined $translation_table)
{
    my $translation_table_code;
    open TRANSLATION_TABLE, $translation_table
	or die "Could not open translation table $translation_table:  $!";
    {
	local $/ = undef;
	$translation_table_code = <TRANSLATION_TABLE>;
    }
    eval $translation_table_code
	or die "Malformed translation table (from file $translation_table):  $!"; #! confess?
}

print "Opening log file $log_file...\n" unless $silent;
open LOG_FILE, $log_file #!Ack!  Insecure!  Use sysopen() instead.
    or die "Could not open ipgrab log $log_file:  $!";
print "Starting graphical user interface...\n" unless $silent;
&create_gui();

#Start the Tk event loop.
#Invoke the parsing callback whenever there is input to read.
$window->fileevent (\*LOG_FILE, 'readable', \&parse);
#The rest of the time, process GUI events
MainLoop();

my $i = 0;    
my @sip_message;
my @sdp_message;
my $starting_new_sip_message = 0;
my $starting_new_sdp_message = 0;
my $in_sip_message = 0;
my $in_sdp_message = 0;

#Parse a single line and accumulate lines into messages.  Once the end of a
#message is reached, parse it as a whole and construct a new SipMessage,
#then draw an arrow for it on the canvas.  Discover and draw new servers
#in a similarly dynamic fashion.
#! Uses blocking <>; later, replace with non-blocking calls?
my $last_sip_message;
sub parse
{
    if (! ($_ = <LOG_FILE>))
    {
	#Try to construct a SIP message from the latest batch of
	#accumulated output.  This may fail, in which case the
	#constructor will return undef, but it will succeed in
        #two important cases:
	#1.  The SIP message is complete except for the trailing line
	#of dashes.  ipgrab seems to omit that line in most cases,
	#so it is important to make this attempt to make sure the
	#last message appears.
	#2.  ipgrab output was cut off (e.g. by the user pressing
	#Ctrl-C) while listing SIP headers.  As a result, the pop-up header
	#list will be incomplete, but at least the request itself
	#and the extant headers will be displayed correctly.
	close LOG_FILE;
	my $message = Telephony::SipMessage->new (\@sip_message);
	&dispatch ($message) if defined $message;
	print "Reached end of SIP messages.\n"
	    . "Got final SIP message:\n" . Data::Dumper->Dump ([ $message ]) if DEBUG_SIP_MESSAGES;
	#! my $relevant_canvas = $canvases{$message->get_call_id()};
	if (CallFlowCanvas::batch_mode())
	{
	    print "In batch mode; preparing to take snapshots:\n" if DEBUG;
	    &take_snapshots();
	}
    }
    else
    {
	print "Got a line of available input:\n\t$_" if DEBUG_SIP_MESSAGES;
	print TEE if $tee;
	if (m/^-+$/) #In ipgrab output, lines of dashes separate messages
	{
	    if ($in_sip_message)
	    {
		my $message = Telephony::SipMessage->new (\@sip_message);
		$in_sip_message = 0; @sip_message = qw();
		$i++;
		#An unblessed reference means the message was incomplete,
		#which means we hit the end of the file.
		return if (ref $message ne 'Telephony::SipMessage');
		&do_the_cisco_massage   ($message) unless defined $no_cisco;
		&do_the_pingtel_massage ($message) unless defined $no_pingtel;
		&massage_some_more ($message) if defined $translation_table;
		if (DEBUG_SIP_MESSAGES)
		{
		    print "Constructed SipMessage\n"
			. Data::Dumper->Dump ([ $message ])
			. "\n\n";
		}
		&dispatch ($message);
	    }
	    elsif ($in_sdp_message)
	    {
		#Attach to last SIP message
		$last_sip_message->{sdp_headers} = Telephony::SipMessage->_new_sdp_headers (\@sdp_message);
		$in_sdp_message = 0; @sdp_message = [];
	    }
	    elsif ($starting_new_sip_message)
	    {
		$starting_new_sip_message = 0;
		$in_sip_message = 1;
	    }
	    elsif ($starting_new_sdp_message)
	    {
		$starting_new_sdp_message = 0;
		$in_sdp_message =1;
	    }
	}
	elsif (m/SIP Headers/)
	{
	    $starting_new_sip_message = 1;
	}
	elsif (m/SDP Headers/)
	{
	    $starting_new_sdp_message = 1;
	}
	else #It's part of a message proper.
	{
	    if ($in_sip_message)
	    {
		push @sip_message, $_;
	    }
	    elsif ($in_sdp_message)
	    {
		push @sdp_message, $_;
	    }
	    else
	    {
		warn "Parser found unexpected data between messages:\n$_" if DEBUG_SIP_MESSAGES;
	    }
	}
    }
}

sub dispatch
{
    ($last_sip_message) = @_;
    my $call_id = $last_sip_message->get_call_id();
    $call_id
	or warn "Found a null call ID for message:\n" . Data::Dumper->Dump ([ $last_sip_message ]);

    if (!exists $canvases{$call_id})
    {
	&add_canvas ($call_id, $last_sip_message);
    }
    if (! defined $currently_displayed_call_id)
    {
	$currently_displayed_call_id = $call_id;
	$currently_displayed_canvas = $canvases{$call_id};
    }

    $canvases{$call_id}->add_sip_message ($last_sip_message,
					  ($call_id eq $currently_displayed_call_id));
}

sub add_canvas
{
    my ($call_id, $last_sip_message) = @_;

    $canvases{$call_id} = CallFlowCanvas->new ($window,
					       $canvas,
					       \%snapshot_preferences,
					       $paginate_snapshots,
					       {
						   heuristic => $heuristic,
						   logical_names => $logical_names
					       },
					       $last_sip_message
					       );
    $call_menu->add ('radiobutton',
		     label         => $call_id,
		     variable      => \$currently_displayed_call_id,
		     value         => $call_id,
		     command       => \&switch_canvases,
		     accelerator   => $canvas_number++
		     );
}

sub pack_buttons
{
    $snapshot_button->pack();
    $snapshot_configure_button->pack();
    $clear_button->pack();
    $insert_breaks_button->pack();
}

sub switch_canvases
{
    $currently_displayed_canvas->hide();
    $currently_displayed_canvas = $canvases{$currently_displayed_call_id};
    $currently_displayed_canvas->show();
}

# Massage the source and destination IP to account for
# the fact that Cisco IP phones use high local ports,
# rather than port 5060.  Otherwise the phone appears as
# two different boxes, since a box is determined by
# the combination of IP and port.  (This is necessary
# to allow for the discrimination of multiple servers
# running on a single machine.)
#! This foreach biz could be done more efficiently with references...
sub do_the_cisco_massage
{
    my $cutoff = 9999;
    my $message = $_[0];

    foreach my $fieldname ('source_port', 'destination_port')
    {
	if ($message->{$fieldname} > $cutoff)
	{
	    $message->{$fieldname} = 5060;
	    print "\&do_the_cisco_massage changed port $message->{$fieldname} to 5060\n" 
		. "for message " . Data::Dumper->Dump ([ $message ]) if DEBUG_SIP_MESSAGES;
	}
    }
}

# Massage the source and destination IP to account for
# the fact that Pingtel IP phones use several local ports
# in the 1000 range in addition to port 5060 (and, occassionally,
# port 6060).  Otherwise the phone appears as several different boxes,
# since a box is determined by the combination of IP and port.
#(This is necessary to allow for the discrimination of multiple servers
# running on a single machine.)
sub do_the_pingtel_massage
{
    my $message = $_[0];

    my $lower_bound = 1001;
    my $upper_bound = 1299;
    my $port = $message->{source_port}; #Yep, only (local) source ports
                                        #display this behavior.

    if ($port >= $lower_bound && $port <= $upper_bound || $port == 6060)
    {
	$message->{source_port} = $pingtel_canonical_port || 5060;
	print "\&do_the_pingtel_massage changed port $port to 5060\n"
	    . "for message " . Data::Dumper->Dump ([ $message ]) if DEBUG_SIP_MESSAGES;
    }
}

#Make the further IP:port substitutions specified in the translation table,
#overcoming quirky port-changing behavior on the part of servers or endpoints.
#! This foreach biz could be done more efficiently with references...
sub massage_some_more
{
    my $message = $_[0];
    
    foreach my $ip_and_port_to_replace (keys %translation_table)
    {
	unless ($ip_and_port_to_replace =~ /(.*):(.*)/)
	{
	    die "Malformed IP and port to replace $ip_and_port_to_replace in translation table $translation_table";
	}
	my $ip_to_replace   = $1;
	my $port_to_replace = $2;
	foreach my $fieldname ('source', 'destination')
	{
	    if ($message->{"${fieldname}_ip"}   eq $ip_to_replace &&
		$message->{"${fieldname}_port"} eq $port_to_replace)
	    {
		print "Translating $fieldname IP and port " . $message->{"${fieldname}_ip"} . ' and ' . $message->{"${fieldname}_port"} . "\n" if DEBUG_TRANSLATIONS;
		my $replacement_ip_and_port = $translation_table{$ip_and_port_to_replace};
		unless ($replacement_ip_and_port =~ /(.*):(.*)/)
		{
		    die "Malformed replacement IP and port $replacement_ip_and_port in translation table $translation_table";
		}
		my $replacement_ip   = $1;
		my $replacement_port = $2;
		print "\tto $1 and $2, respectively\n" if DEBUG_TRANSLATIONS;
		$message->{"${fieldname}_ip"}   = $replacement_ip;
		$message->{"${fieldname}_port"} = $replacement_port;
	    }
	}
    }
}

sub read_message_text
{
    my @message_text; my $i = 0;
    while (<LOG_FILE>)
    {
	return \@message_text if m/^-+$/;
	$message_text[$i++] = $_;
    }
    return \@message_text; #Return incomplete text at end of log;
                           #Maybe someone can make something of it.
}

sub create_gui
{
    print "Entered create_gui\n" if DEBUG;
    $window = MainWindow->new ( #-font       => $fonts{window_default_font}
				);
    $window->title ('Tricorder                  '  . &get_quip());
    my $menubar = $window->Menu();

    $menubar->cascade (-label => 'Calls');
    $call_menu = $menubar->Menu (type    => 'cascade',
				 -title   => 'Calls',
				 tearoff => 1
				 #!font  => $fonts{menu_font},
				 );
    $menubar->entryconfigure ('Calls',
			      menu => $call_menu);
    $window->configure (menu => $menubar);

    $canvas = CallFlowCanvas::_construct_canvas ($window);
    $canvas->pack (fill => 'both');

    $snapshot_button = $window->Button (-text    => 'Take a Snapshot!',
					-command => sub
					{
					    #!&remove_server_boxes();
					    &take_snapshots();
					    #!&restore_server_boxes();
					    #!&draw_lines_under_server_boxes();
					}
					);
    $snapshot_button->pack();
    $snapshot_configure_button = $window->Button (-text    => 'Snapshot Properties...',
						  -command => \&configure_snapshot_properties
						  );
    $clear_button = $window->Button (-text    => 'Clear Screen',
				     -command => \&clear_screen);
    $clear_button->pack();
    $insert_breaks_button = $window->Button (-text    => 'Insert Break',
					     -command => \&draw_insert_breaks_dialog);
    $insert_breaks_button->pack();
}

sub take_snapshots
{
    local $_;
    map { $_->take_snapshot(); } values %canvases;
}

sub show_error
{
    #! Replace this with a Tk Dialog object?  It seems to fit the task...
    my $pop_up = MainWindow->Toplevel (takefocus  => 1);
    my $warning   = $pop_up->Label ($_[0]);
    my $OK_button = $pop_up->Button (text    => 'OK',
				     command => sub { $warning->withdraw(); });
    $warning->pack();
    $OK_button->pack();
}

#Create a dialog that lets the user change some snapshot properties
#--specifically, the target filename, the color mode, and the horizontal
#and vertical scaling.
sub configure_snapshot_properties
{
    my ($file, $colormode, $scaling_factor, $page_width, $page_length)
	= @snapshot_preferences{'file', 'colormode', 'scaling_factor', 'page_length', 'page_width'};

    my $left_justify = 30; #pixels    #!This is bad.  These values should
    my $maximum_widget_width   = 600; #!appear in a global %settings instead.

    my $dialog = $window->Toplevel (width      => $left_justify * 2
				                + $maximum_widget_width,
				    takefocus  => 1);
    $dialog->title ('Configure');

    my $frame1 = $dialog->Frame (width => $maximum_widget_width);
    my $frame2 = $dialog->Frame (width => $maximum_widget_width);
    my $frame3 = $dialog->Frame (width => $maximum_widget_width);
    my $button_frame = $dialog->Frame (width => $maximum_widget_width);

    my $file_entry = $frame1->Entry();
    $file_entry->insert (0, $file);
    my $file_entry_label = $frame1->Label (text => 'Output to file');

    $colormode = &canonical_to_verbose ($snapshot_preferences{colormode});
    print "Going into snapshot configuration, color mode is $colormode\n" if DEBUG;
    my $color = $frame2->Radiobutton (variable => \$colormode,
				      value    => 'Color',
				      text     => 'Color');
    my $greyscale = $frame2->Radiobutton (variable => \$colormode,
					  value    => 'Greyscale',
					  text     => 'Greyscale');
    my $mono = $frame2->Radiobutton (variable => \$colormode,
				     value    => 'Black and White',
				     text     => 'Black and White');
    my $scaling_factor_scale = $frame3->Scale (orient       => 'horizontal',
					       from         => .50,
					       to           => 5.00,
					       tickinterval => .50,
					       label        => 'Scaling Factor',
					       length       => $maximum_widget_width,
					       variable     => \$scaling_factor);
    my $page_width_scale = $frame3->Scale (orient       => 'horizontal',
					   from         => 500,
					   to           => 5000,
					   tickinterval => 500,
					   label        => 'Page Width (Pixels)',
					   length       => $maximum_widget_width,
					   variable     => \$page_width);

    my $page_length_scale = $frame3->Scale (orient       => 'horizontal',
					    from         => 500,
					    to           => 10000,
					    tickinterval => 1000,
					    label        => 'Page Height (Pixels)',
					    length       => $maximum_widget_width,
					    variable     => \$page_length);

    my $OK_button = $button_frame->Button (text    => 'OK',
				     command => sub
				                {
						    @snapshot_preferences{'file', 'colormode', 'scaling_factor', 'page_width', 'page_length'} = ($file_entry->get(), &verbose_to_canonical ($colormode), $scaling_factor, $page_width, $page_length);
						    print "New snapshot prefernces:\n" . Data::Dumper->Dump ([ \%snapshot_preferences ]) if DEBUG;
						    $dialog->withdraw();
						    &save_snapshot_preferences();
						}
				     );
    my $Cancel_button = $button_frame->Button (text    => 'Cancel',
					 command => sub
					            {
							$dialog->withdraw();
						    }
					 );
    $frame1->pack();
    $frame2->pack();
    $frame3->pack();
    $button_frame->pack();
    
    #! Later replace this with a Tk::FileSelect
    $file_entry_label->pack (side => 'left');
    $file_entry->pack       (side => 'left', fill => 'x');

    $color->pack            (side => 'left');
    $greyscale->pack        (side => 'left');
    $mono->pack             (side => 'left');

    $scaling_factor_scale->pack (fill => 'x');
    $page_width_scale->pack     (fill => 'x');
    $page_length_scale->pack    (fill => 'x');
    
    $OK_button->pack        (side => 'left');
    $Cancel_button->pack    (side => 'right');
}

sub draw_insert_breaks_dialog
{
    my $maximum_widget_width = 600;
    my $left_justify = 30;

    my $dialog = $window->Toplevel (width      => $left_justify * 2
				                + $maximum_widget_width,
				    takefocus  => 1);
    $dialog->title ('Insert a Visual Break');
    my $comment_entry = $dialog->Entry();
    my $comment_entry_label = $dialog->Label (text => 'Comment');
    my $OK_button = $dialog->Button (text => 'OK',
				     command => sub 
				                {
						    &insert_breaks ($comment_entry->get());
						    $dialog->withdraw();
						}
				     );
    my $Cancel_button = $dialog->Button (text => 'Cancel',
					 command => sub 
					            {
							$dialog->withdraw();
						    }
					 );
    $comment_entry_label->grid ($comment_entry);
    $OK_button->grid ($Cancel_button);
}

sub insert_breaks
{
    my $comment;

    local $_;
    map { $_->insert_break ($comment); } values %canvases;
}

#Convert Tk's description of a color mode to something more suitable for
#printing in a user interface.
sub canonical_to_verbose
{
    my $colormode = $_[0];

    if ($colormode eq 'color')
    {
	return 'Color';
    }
    elsif ($colormode eq 'gray')
    {
	return 'Greyscale';
    }
    elsif ($colormode eq 'mono')
    {
	return 'Black and White';
    }
    else
    {
	die "canonical_to_verbose was invoked with $colormode, which is not a valid colormode specification.";
    }
}

#Convert my user interface's description of a color mode to the canonical
#representation used by the Tk Canvas->postscript method.
sub verbose_to_canonical
{
    my $color_string = $_[0];
    
    if ($color_string eq 'Color')
    {
	return 'color';
    }
    elsif ($color_string eq 'Greyscale')
    {
	return 'gray';
    }
    elsif ($color_string eq 'Black and White')
    {
	return 'mono';
    }
    else
    {
	die "verbose_to_canonical was invoked with $color_string, which is not a valid color specification as displayed on the radiobuttons.";
    }
}

sub save_snapshot_preferences
{
    print "Saving snapshot preferences...\n" if DEBUG;

    open RC_FILE, ">$HOME/.tricorderrc"
	or die "Could not open $HOME/.tricorderrc for writing:  $!";
    print RC_FILE "#This is your personal configuration file for Tricorder,\n"
	         ."#the graphical call-flow diagram tool.  You probably shouldn't\n"
		 ."#edit it by hand; instead, run tricorder.pl and use the\n"
		 ."#'Snapshot Properties...' button to change your preferences graphically.\n\n";
    print RC_FILE Data::Dumper->Dump ([ \%snapshot_preferences ], [ '*snapshot_preferences' ]);
    close RC_FILE
	or &show_error ("Tricorder could not close $HOME/.tricorderrc; your preferences will not be saved.");

    print "Saved.\n" if DEBUG;
}

sub get_quip
#Return a random Star Trek quotation for the user's enjoyment
{
    my @quips = ("Cap'n, I'm giving 'er all I've got!",
		 "It's life, Jim, but not as we know it.",
		 "He's dead, Jim.",
		 "Beam me up, Scotty.",
		 "Live long and prosper.",
		 );
    srand();
    return $quips[rand ($#quips + 1)];
}

#Lecture the user on proper syntax.
sub usage
{
    my ($message, $exit_value) = @_;
    print STDERR $message if defined $message;

    print STDERR <<fnord;
usage:  $program_name --help
        $program_name [--log-file log_file_name [--batch-process]] OPTIONS
	$program_name [--tee output_file_path] [--ipgrab-expression expr] OPTIONS
	where options are of the form:

        [--silent]
	[--no-cisco] [--no-pingtel | --pingtel_canonical_port port]
	[--translation-table file]
	[--heuristic assume-server
                   | use-vovida-ip-numbering-scheme
                   | use-sip-hints]
	[--paginate-snapshots [dumb | smart]]
        [--logical-names map_file]
        [--snapshot_filename]

        and
        
        --help prints this message,

	--log-file specifies an ipgrab log to examine in graphical mode.
	  If --log-file is not specified, $program_name starts an ipgrab
	  process of its own and reads from it, dynamically updating the
          display as input becomes available.

	--batch-process tells $program_name to read a log file, take a
          snapshot, and exit immediately, without further user interaction.
          (Hence --batch-process must be specified in conjunction with
          --log-file).

        --snapshot-filename overrides the snapshot name specified in
          the .tricorderrc (although it can, in turn, be overriden by
          the "Snapshot Properties..." dialog in the GUI).  This option
          is useful primarily in conjunction with --batch-process.

        --tee tells $program_name to write the output from such an
          ipgrab subprocess to the file named output_file_path,
	  so that the raw header information is not lost.  (To tee to
          standard output or standard error, use /dev/stdout or /dev/stderr
	  respectively.)

	--ipgrab-expression specifies the expression to run with ipgrab,
          e.g. port 5060 or port 5070 or port 5074.  See the ipgrab(8)
	  man page for more details.

	--no-cisco tells $program_name not to massage each messsage's
          source and destination IP to account for the fact that Cisco IP phones
          use high local ports, rather than 5060.  (If --no-cisco is
          specified, the phone will appear as two different boxes, since
          a box is determined by the combination of IP and port.)

        --no-pingtel works like --no-cisco, but the massaging alogrithm
          is a different one designed to work for Pingtel SIP phones
          (which use several different ports within a single call
          scenario).

        --pingtel-canonical-port tells the Pingtel massaging algorithm
          to change deviant local ports to port, rather than to 5060
          (the default).  Naturally, --pingtel-canonical port may not
          be specified in conjunction with --no-pingtel.

        --translation-table specifies a file from which to take
          further translations of IP:port combinations, as another
          hack to correct nasty port-changing behavior on the part of
          certain servers.  The file contains a hash mapping IP:port
          combinations that should be replaced (keys) to their
          replacement IP:port combinations (values).  The hash
          must be named %translation_table.

        --silent tells $program_name not to print messages about
          its activity to standard output.  (The default is to print
          some messages).

	--heuristic tells $program_name to discriminate between servers and
          endpoints using one of the following heuristics:

               assume-server:  Treat everything as a server.  This produces
               perfectly usable diagrams for scenarios with a small number
               of endpoints, but it breaks down at some point, since each
               server is represented as a separate box in the diagram.

               use-vovida-ip-numbering-scheme:  Examine the last octet
               of the message sender and recipients' IPs to decide if they
	       are servers or endpoints.  In the Vovida scheme, .180,
	       .200, .110, and .210 are reserved for servers.  This method
	       is fast, but, of course, it relies on the local setup using
	       those numbers.

               use-sip-hints:  Examine the actual SIP messages.  This is
               the most costly method, but also the most reliable.

	  The default is assume-server.

	  --paginate-snapshots tells $program_name to separate its Postscript
            snapshots (taken when you press the "Take a Snapshot!" button)
	    into files of a certain size.  The argument dumb means to
	    separate strictly based on page size, without concern for
	    orphaned or dangling messages or servers; smart means to
            take message and server boundaries into account as well.
            Hence smart is liable to increase the number of pages,
            as trailing data that do not fit semantically on a given
            page are relegated to its successor.  Note also that smart
	    scales the snapshot horizontally so that it occupies only
            one page horizontally.  Otherwise, there is no way to guarantee
            that long message arrows will fit in the horizontal page length
            guaranteed by the user.

            If no argument is given, $program_name assumes dumb.

            The size of pages is configurable through the GUI, via
            the "Snapshot Properties..." button.

          --snapshot-filename sets the name of the file to which to save
            Postscript output (or, in the case of paginated output
            as invoked by --paginate-snapshots, the name on which
            to base the various output filenames).  --snapshot-filename
            overrides the filename value stored in the user's .tricorderrc.

          --logical_names tells $program_name to use logical names for
            servers, rather than labeling them by IP or hostname and port.
            The mandatory argument map_file is the name of a file defining
            a Perl hash %server_map that maps strings of the form IP:port to
            logical names.

fnord

    exit $exit_value || 1;
}
