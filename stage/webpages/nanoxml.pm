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

# $Id: nanoxml.pm,v 1.1 2004/05/01 04:15:31 greear Exp $

package nanoxml;

#use strict;

BEGIN {
    use Exporter ();
#    our ($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

    # if using RCS/CVS, this may be preferred
    $VERSION = do { my @r = (q$Revision: 1.1 $ =~ /\d+/g); sprintf "%d."."%02d" x $#r, @r }; # must be all one line, for MakeMaker
    @ISA         = qw(Exporter);
    @EXPORT      = qw(&parse_document
		      &print_tree
		      &output_tree
		      &find_element
		      &new_element
		      &new_data
		      &replace_element
		     );

    %EXPORT_TAGS = ( ) ;    # eg: TAG => [ qw!name1 name2! ],
    
    # your exported package globals go here,
    # as well as any optionally exported functions
    @EXPORT_OK   = qw();
}
#our @EXPORT_OK;

sub parse_document {
    my($document) = @_;
    my($root) = &new_tree();
    my($i) = 0;
    my($doc_len) = length($document);
    my($x);

    while($i < $doc_len) {
	$x = &scan_next_tag($document, \$i);
	&insert_tree($x, $root);
    }

    $root->{done} = 1;
    undef($root->{stack}); # break cycle if need be

    return $root;
}


sub find_element {
    my($tree, @text) = @_;

    if(!$tree->{contents}) {
	return undef;
    }

    if(@text) {
	my $top = shift(@text);
	my $x;
	foreach $x (@{$tree->{contents}}) {
	    if($x->{start} =~ /^<$top/) {
		return &find_element($x, @text);
	    }
	}
	return undef;
    } else {
	return $tree;
    }
}

sub replace_element {
    my($old, $new) = @_;

    if(defined($old)) {
	%$old = %$new;
    }
}


sub output_element_data {
    my($data, $indent, $canonical) = @_;
    my($output);
    my $space_string = "                                                                                                    ";


    if($canonical) {
	
	$data =~ s/^\s*\n$//;
	$data =~ s/^[\s]+/ /;
	$data =~ s/[\s]+$/ /;
	if($data) {
	    $output .= substr($space_string, 0, $indent);
	    $output .= $data . "\n";
	}
    } else {
	$output .= $data;
    }
    return $output;
}

sub output_element_tag {
    my($data, $indent, $canonical) = @_;
    my($output);
    my $space_string = "                                                                                                    ";

    if($canonical) {
	$data =~ s/^\s+//;
	$data =~ s/\s+$//;
	$output .= substr($space_string, 0, $indent);
	$output .= $data . "\n";
    } else {
	$output .= $data;
    }
    return $output;
}

sub output_element_header {
    my($data, $indent, $canonical) = @_;
    return $data;
}

sub print_tree {
    my($output) = &output_tree(@_);

    print $output;
}


sub output_tree {
    my($element, $indent, $indent_incr, $canonical) = @_;
    my($output);

    if($element->{type} eq "root") {
	my($iter);
	foreach $iter (@{$element->{contents}}) {
	    $output .= &output_tree($iter, 
				    $indent + $indent_incr, 
				    $indent_incr, 
				    $canonical);
	}
    } elsif($element->{type} eq "data") {
	$output .= &output_element_data($element->{data}, 
					$indent - $indent_incr, 
					$canonical);
    } elsif($element->{type} eq "header") {
	$output .= &output_element_header($element->{data}, 
					  $indent - $indent_incr, 
					  $canonical);
    } elsif($element->{type} eq "element") {
	$output .= &output_element_tag($element->{start}, 
				       $indent - $indent_incr, 
				       $canonical);
	my($iter);
	foreach $iter (@{$element->{contents}}) {
	    $output .= &output_tree($iter, 
				    $indent + $indent_incr, 
				    $indent_incr, 
				    $canonical);
	}
	$output .= &output_element_tag($element->{end}, $indent - $indent_incr, $canonical);
    }
    return $output;
}


sub new_tree {
    my($type, $data);
    my($element);

    $element = { type => "root",
		 contents => [],
		 done => 0,
		 stack => []
	       };
    push(@{$element->{stack}}, $element);
    return $element;
}


sub new_element {
    my($value) = @_;
    my($element);

    return { type=> "element", 
	     start => $value, 
	     end => "", 
	     contents => [],
	     done => 0
	   };
}


sub new_data {
    my($value) = @_;
    my($element);

    return  { type => "data",
	      data => $value,
	      done => 1
	    };
}

sub new_header {
    my($value) = @_;
    my($element);

    return  { type => "header",
	      data => $value,
	      done => 1
	    };
}

sub insert_tree {
    my($data, $tree) = @_;
    my($element);

    if($data =~ /^<\?/) {
	# this is a header

	$element = &new_header($data);
	push(@{${$tree->{stack}}[$#stack]->{contents}}, $element);
	
    } elsif($data =~ /^<\//) {
	$element = pop(@{$tree->{stack}});
	$element->{end} = $data;
	$element->{done} = 1;
    } elsif($data =~ /^</) {
	$element = &new_element($data);

	if($data =~ /\/>$/) {
	    $element->{done} = 1;
	}
	push(@{${$tree->{stack}}[$#stack]->{contents}}, $element);
	if(!$element->{done}) {
	    push(@{$tree->{stack}}, $element);
	}
    } else {
	$element = &new_data($data);
	push(@{${$tree->{stack}}[$#stack]->{contents}}, $element);
    }
    return $element;
}



sub scan_next_tag {
    my($str, $pos) = @_;
    my($len) = length($str);
    my($result);
    my($c);

    if($$pos >= $len) {
	print "bad: $len $pos\n";
	return undef ;
    }

    # do this character by character, yes...
    if(substr($str, $$pos, 1) eq "<") {
	while(($$pos < $len) && ($c = substr($str, $$pos, 1)) ne ">") {

	    if($$pos >= $len) {
		print "bad: $len $pos\n";
		return undef ;
	    }
	    return undef if($$pos >= $len);
	    $result .= $c;
	    $$pos++;
	}
	$result .= $c;
	$$pos++;
    } else {
	# not in a tag
	while(($$pos < $len) && ($c = substr($str, $$pos, 1)) ne "<") {

	    if($$pos >= $len) {
		print "bad: $len $pos\n";
		return undef ;
	    }

	    return undef if($$pos >= $len);
	    $result .= $c;
	    $$pos++;
	}
    }
    return $result;
}

END {
}

1;
return 1;
