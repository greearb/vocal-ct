#!/usr/bin/perl

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

# $Id: vipgrab.pl,v 1.1 2004/05/01 04:15:31 greear Exp $

# simple wrapper for ipgrab.

# you should be able to use it using the same command line you would
# use for ipgrab, e.g.

#            ipgrab port 5060 udp

# should be the same as

#            ./vipgrab.pl port 5060 udp

# This program requires perl-Tk .

use Tk;
use Tk::Balloon;
use Data::Dumper;



my $pref_file = "$ENV{HOME}/.vipgrabrc";

my $mw = MainWindow->new;


&main;

sub set_relative_position {
    local($old_toplevel, $new_toplevel, $delta_x, $delta_y) = @_;
    my $old_geometry, $new_geometry, $x_pos, $y_pos;

    $old_geometry = $old_toplevel->geometry();
    if($old_geometry =~ /^=?((\d+)x(\d+))?(([+-]\d+)([+-]\d+))?$/) {
	$x_pos = $5;
	$y_pos = $6;
	if($x_pos =~ /([+-])(\d+)/) {
	    $x_sign = $1;
	    $real_x_pos = $2;
	}
	if($y_pos =~ /([+-])(\d+)/) {
	    $y_sign = $1;
	    $real_y_pos = $2;
	}

	$x_pos = $x_sign . ($delta_x + $real_x_pos);
	$y_pos = $y_sign . ($delta_y + $real_y_pos);
	
	$new_toplevel->geometry($x_pos . $y_pos);
    } else {
	print $old_geometry, "\n";
    }
}

sub preference_dialog {
    if(!Exists($prefs)) {
	$prefs = $mw->Toplevel();
	&set_relative_position($mw, $prefs, 50, 50);
	# using transient means that smart window managers won't bury this
	# window under others, which is usually a good thing

	$prefs->transient($mw); 

	$prefs->title("vipgrab Preferences");
	$scrollback_frame = $prefs->Frame()->pack();
	$scrollback_frame->Label(-text=>"Number of scrollback lines: ")
	  ->pack(side=>'left');
	$scrollback_frame->Entry(-textvariable=>\$preferences{"scrollback"})
	  ->pack(side=>'left');

	$log_frame = $prefs->Frame()->pack();
	$log_frame->Label(-text=>"Log filename: ")
	  ->pack(side=>'left');
	$log_frame->Entry(-textvariable=>\$preferences{"logfile"})
	  ->pack(side=>'left');
	$log_frame->Checkbutton(-text=>"Log to File", 
				-variable=>\$preferences{use_logfile})
	  ->pack();

	$command_frame = $prefs->Frame()->pack();
	$command_frame->Label(-text=>"ipgrab command line: ")
	  ->pack(side=>'left');
	$command_frame->Entry(-textvariable=>\$preferences{"ipgrab_command"})
	  ->pack(side=>'left');

	$prefs->Checkbutton(-text=>"Move To End", 
			    -variable=>\$move_to_end)
	  ->pack();

	$prefs->Checkbutton(-text=>"SIP Tooltips", 
			    -variable=>\$preferences{use_sip_tooltips},
			    -command=>\&update_tooltips)
	  ->pack();

	$prefs->Button(-text=>"Close", 
		       -command=> sub { $prefs->withdraw })
	  ->pack();

    } else {
	$prefs->deiconify();
	$prefs->raise();
    }
}



sub update_tooltips {
    if($preferences{use_sip_tooltips}) {
	$sipBalloon->attach($listbox, 
			    -statusmsg => "Use Button 1 to view the message in the main window, or Button 2 to open a new window",
			    -balloonmsg => \$list_balloonmsg,
			    -balloonposition => 'mouse',
			    -postcommand => sub { 
				$list_balloonmsg = 
				  $summaryList[$listbox->index("\@$data")];
			    },
			    -motioncommand => sub { 
				my $x = $listbox->pointerx - $listbox->rootx;
				my $y = $listbox->pointery - $listbox->rooty;
				$data = "$x,$y";
			    }
			   );
    } else {
	$sipBalloon->detach($listbox);
    }
}




sub quit {
    &write_preferences;
    exit;
}



sub start {
    if(!$ipgrab_running) {
	my $ipgrab = $preferences{"ipgrab_command"};
	foreach(@ARGV) {
	    $ipgrab .= " ". $_;
	}
	
	open(IPGRAB, "$ipgrab |") || die "can't run $ipgrab!\n";
	open(LOGFILE, ">>$preferences{logfile}") || die "can't open logfile $preferences{logfile}\n";
	$mw->fileevent(IPGRAB, 'readable', [\&insert_text]);
	$b->attach($start_button, 
		   -balloonmsg => "Start ipgrab", 
		   -statusmsg => "Start ipgrab (currently running)");
	$ipgrab_running = 1;
    }
}

sub stop {
    if($ipgrab_running) {
	close(IPGRAB);
	close(LOGFILE);
	$b->attach($start_button, 
		   -balloonmsg => "Start ipgrab", 
		   -statusmsg => "Start ipgrab (currently NOT running)");
	$ipgrab_running = 0;
    }
}


sub update_if_diff {
    if($old_packet ne $packet) {
	&update_messages(0);
	$old_packet = $packet;
    }
}

sub update_messages {
    local($finished) = @_;
    if($source) {
	$seq = sprintf("%5.5d", $sequence_number);
	$x = "$seq: $source:$source_port -> $dest:$dest_port";
	$packetList[$seq - $first_number] = $x . "\n" . $packet;
	$summaryList[$seq - $first_number] = $summary;
	if($have_inserted) {
	    $listbox->delete('end');
	}
	$listbox->insert('end', $x);
	$have_inserted = 1;
	if($move_to_end) {
	    $listbox->yview('moveto', $listbox->index('end'));
	}
    }
    if($finished) {
	$have_inserted = 0;
	$packet = "";
	$summary = "";
	undef(%parseState);
	$sequence_number++;
	while(($sequence_number - $first_number) 
	      > $preferences{"scrollback"}) {
	    shift(@packetList);
	    shift(@summaryList);
	    $first_number++;
	    $listbox->delete(0);
	}
    }
}



sub insert_text {

    if($_ = <IPGRAB>) {
	# start parsing

	if($preferences{use_logfile}) {
	    print LOGFILE $_;
	}
	$packet .= $_;
	chomp;

	if(/^=================================================================$/) {
	    # start of a packet
	    &update_messages(1) if ($seq);

	    $state = $StartOfPacket;
	} elsif(/^\-\-\-\-\-/) {
	    # this is buggy, but I'm not entirely sure why.
	    # move forward to next state
	    if($state == $StartOfPacket) {
#		print "sop->soen\n";
		$state = $StartOfEnet;
	    } elsif($state == $StartOfEnet) {
#		print "soen->eoen\n";
		$state = $EndOfEnet;
	    } elsif($state == $EndOfEnet) {
#		print "eoen->soip\n";
		$state = $StartOfIp;
	    } elsif($state == $StartOfIp) {
#		print "soip->eoip\n";
		$state = $EndOfIp;
	    } elsif($state == $EndOfIp) {
#		print "eoip->sou\n";

		$state = $StartOfUdp;
	    }
	} else {

# because the above stuff seems buggy, skip it.  it works fine without
# it anyway.

# elsif($state == $StartOfIp) {
#		print "xxx\n";
	    if(/Source address/) {
		s/Source address\s+//;
		$source = $_;
	    } elsif(/Destination address/) {
		s/Destination address\s+//;
		$dest = $_;
	    }
	    if(/SIP Headers/) {
		$parseState{sip_header} = 1;
	    }
	    if($parseState{sip_header}) {
		$a = $_;
		if($a =~ s/^Header:\s+//) {
		    $b = sprintf("%-80.80s\n", $a);
		    $summary .= $b;
		}
	    }

#	} elsif($state == $StartOfUdp) {
#	    print "aaa\n";
	    if(/Source port:/) {
#		print "sp: $_\n";
		s/Source port:\s+//;
		/^([0-9]+)/;
		$source_port = $_;
#		$source_port = $1;
	    } elsif(/Destination port/) {
#		print "dp: $_\n";
		s/Destination port:\s+//;
		/^([0-9]+)/;
		$dest_port = $_;
#		$dest_port = $1;
	    }
	}
    } else {
	$mw->fileevent(IPGRAB, 'readable', "");
    }
}




sub open_new_message {
    my $message = $_[0];
    $message =~ /^(\d+)/;
    my $msgNumber = $1;
    my $new_message = $mw->Toplevel();

    $new_message->title("Message #$msgNumber");

    $temp_text = $new_message->Scrolled("Text", 
					-width => 80, 
					-height =>25)
      ->pack(-expand => 1);
    $new_message->Button(-text=> "Close", 
			 -command => [ sub { $_[0]->destroy;}, $new_message ])
      ->pack();
    $temp_text->insert('1.0', $message);
}

sub write_preferences { 
    open(PREFS, ">$pref_file")
      or die "can't write preferences file $pref_file";

    print PREFS Data::Dumper->Dump([\%preferences], [qw(*preferences)]);
    close(PREFS);
}

# read the preferences, or if there are none, create a default
# preferences file in $ENV{HOME}/.vipgrabrc

sub read_preferences { 
    my $pref_data;
    my $prefs_ok;

    if(-f $pref_file) {
	open(PREFS, $pref_file) 
	  or die "can't read preferences file $pref_file";
	while(<PREFS>) {
	    $pref_data .= $_;
	}
	close(PREFS);

	$prefs_ok = eval $pref_data;
    } 

    if(!defined($prefs_ok)) {
	$initial_status = "unable to read $pref_file, using default values";

	# use default values
	$preferences{"ipgrab_command"} = "sudo /sbin/ipgrab";
	$preferences{"logfile"} = "vipgrab.log";
	$preferences{"scrollback"} = 100; # no more than 100 items in queue
	$preferences{use_logfile} = 1;
	$preferences{use_sip_tooltips} = 1;
    }
}



sub main {
    $StartOfPacket = 1;
    $StartOfEnet = 2;
    $EndOfEnet = 3;
    $StartOfIp = 4;
    $EndOfIp = 5;
    $StartOfUdp = 6;

    &read_preferences();
    
    
    if(($< != 0) && ($> != 0)) {
	# need root password to run
	print STDERR "warning: you are not root!\n";
	#    exit;
    }
    
    $first_number = 0;
    
    
    print $ipgrab, "\n";
    
    $mw->title( "vipgrab");
    

    $menu_frame = $mw->Frame(-relief=>'raised', 
			     -borderwidth=>2)
      ->pack(-expand=> 1, -fill => x);
    $menu_frame->Menubutton(-text => "File", 
			    -menuitems=> [ [ "command" => "Quit", 
					     -command => \&quit ]]
			   )
      ->pack(-side=> 'left');
    $speedbar_frame = $mw->Frame()
      ->pack(-expand=> 1, -fill=>'x');
    $quit_button = $speedbar_frame->Button(-text=>"Quit", 
					   -command=>\&quit)
      ->pack(side=>'left');
    $start_button = $speedbar_frame->Button(-text=>"Start", 
					    -command=>\&start)
      ->pack(side=>'left');
    $stop_button = $speedbar_frame->Button(-text=>"Stop", 
					   -command=>\&stop)
      ->pack(side=>'left');
    $prefs_button = $speedbar_frame->Button(-text=>"Prefs", 
					    -command=>\&preference_dialog)
      ->pack(side=>'left');
    
    $listbox = $mw->Scrolled("Listbox", 
			     -scrollbars=>"e", 
			     -width=>60, 
			     -selectmode=>"single")->pack();
    
    $text = $mw->Scrolled("Text", 
			  -width => 80, 
			  -height =>25)
      ->pack(-expand => 1);

    $status_font = $mw->fontCreate(-size=>8);

    $status_bar_widget = $mw->Label(-anchor=>'w', 
				    -width=>90, 
				    -font=>"helvetica 10 normal",
				    -justify=>"left", 
				    -relief=>"sunken", 
				    -text => $initial_status)
      ->pack(-expand=>1, -anchor=>'w');
    

    # balloon fun!
    $b = $mw->Balloon(-statusbar => $status_bar_widget, 
		      -background=>"#ffff80");
    
    $sipBalloon = $mw->Balloon(-font=>'courier',  
			       -statusbar => $status_bar_widget, 
			       -background=>"#e0e0ff");
    
    $b->attach($quit_button, 
	       -balloonmsg => "Quit program", 
	       -statusmsg => "Quit the program");

    $b->attach($stop_button, 
	       -msg => "Stop ipgrab");

    $b->attach($prefs_button, 
	       -msg => "Change preferences for vipgrab.pl");
    
#     $b->attach($listbox, 
# 	       -balloonposition => 'mouse',
# 	       -msg => "Use Button 1 to view the message in the main window, or Button 2 to open a new window",
# 	      );
    

    $listbox->bind('<Button-1>', 
		   sub {
		       $text->delete("1.0", 'end');
		       $text->insert('1.0', 
				     $packetList[$listbox->curselection()]);
		   }
		  );

    $listbox->bind('<Button-2>', 
		   [ sub {
		       $data = "$_[1],$_[2]";
		       print $data, "\n";
		       $myIndex = $listbox->index("\@$data");
		       if($myIndex >=0 ) {
			   &open_new_message($packetList[$myIndex]);
		       }
		   }, Ev('x'), Ev('y') ]
		  );

    &start();
    $mw->repeat(100, \&update_if_diff);
    &update_tooltips;
    MainLoop;
}





__END__







