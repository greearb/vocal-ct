#!/usr/bin/perl

# convert an options list to nroff source

while(<>) {
    if(/^Usage:\s*([^ ]+)\s+(.*)$/) {
	# this is the usage line
	print ".B $1\n";
	print ".I $2\n";
	print ".SH DESCRIPTION\n";
    }

    if(/option/i) {
	# the options have started here
	$options = 1;
	print "<refsect1><title>Options</title>\n";
    }

    if($options && /^\s*(\-\w)( ((\w+\s?)*))?\s\s+(\w.*)$/) {
	print "<varlistentry>\n";
	print "<term>\n";
	print "$1";
	if($3) {
	    print " <filename>$3</filename>";
	}
	print "\n</term>\n";
	print "<listitem><para>\n";
	print "$4\n";
	print "</para></listitem>\n";
	print "</varlistentry>\n";
    }
}
