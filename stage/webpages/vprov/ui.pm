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

# $Id: ui.pm,v 1.1 2004/05/01 04:15:31 greear Exp $

package vprov::ui;
use CGI qw/:standard/;

BEGIN {
    use Exporter ();
#    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    # if using RCS/CVS, this may be preferred
    $VERSION = do { my @r = (q$Revision: 1.1 $ =~ /\d+/g); sprintf "%d."."%02d" x $#r, @r }; # must be all one line, for MakeMaker
    @ISA         = qw(Exporter);

    @EXPORT      = qw(  ) ;

    %EXPORT_TAGS = ( ) ;    # eg: TAG => [ qw!name1 name2! ],
    
    # your exported package globals go here,
    # as well as any optionally exported functions
    @EXPORT_OK   = qw();
}
#our @EXPORT_OK;


######################################################################
#                            cgi input / output 
######################################################################


sub box {
    my($title, $contents, $local_width) = @_;
    my($text, $width);

    $width = $local_width;

    $text .= "<table bgcolor=\"#000000\" cellpadding=0 cellspacing=2 border=0 width=$width>\n";
    $text .= Tr(td({-width=>"100%", -bgcolor=>"#c0c0ff", fgcolor=>white, align=>center}, (font({-size => "+2"}, b($title)))));
    $text .= "<tr><td>\n";
    $text .= "<table bgcolor=white cellpadding=0 cellspacing=0 border=0  width=100%>\n";
    $text .= Tr(td({-width=>"100%"}, $contents));
    $text .= "</table></tr></td></table>\n";
    return $text;
}


sub checkbox {
    my($name, $checked) = @_;
    my($result) = "<input type=\"checkbox\" name=$name value=1";
    if($checked) {
	$result .= " checked";
    }
    $result .= "></input>";
    return $result;
}


sub password {
    my($name, $value) = @_;
    my($result) = "<input type=\"password\" name=$name";
    if($value) {
	$result .= " value=\"$value\"";
    }
    $result .= "></input>";
    return $result;
}


sub original_values {
    # store all of the original values for later retrieval
    my($parameters, $output_list) = @_;
    my($result);
    my($i);

    foreach $i (@$output_list) {
	$result .= "<input type=\"hidden\" name=\"original_" . $i->{name} .
	  "\" value=\"";
	if($i->{type} ne "checkbox") {
	    $result .= $$parameters{$i->{name}};
	} else {
	    $result .= $$parameters{$i->{name}} ? $$parameters{$i->{name}} : "";
	}
	$result .= "\">\n";
    }

    return $result;
}


sub output_list {
    my($parameters, $output_list) = @_;
    my($output);
    my($i);
    my(@color) = ("#ffffff", "#c0c0ff");
    my($color_flag) = 0;

    foreach $i (@$output_list) {
	my($input_field);
	if($i->{type} eq "label") {
	    $input_field = $$parameters{$i->{name}};
	}
	if($i->{type} eq "checkbox") {
	    $input_field = &vprov::ui::checkbox($i->{name},
						$$parameters{$i->{name}});
	}
	if($i->{type} eq "password") {
	    $input_field = &vprov::ui::password($i->{name},
						$$parameters{$i->{name}});
	}
	if($i->{type} eq "text") {
	    $input_field = "<input type=\"text\" name=\"" . $i->{name} .
	      "\" value=\"" . $$parameters{$i->{name}} . "\"></input>";
	}
	if($i->{type} eq "filefield") {
	    $input_field = filefield($i->{name});
	}

	$output .= Tr({-bgcolor=>$color[$color_flag++ % 2]},
		      td($i->{desc} . ":"), 
		      td($input_field)) 
	  . "\n";
    }

    return $output;
}


END {
}

return 1;
