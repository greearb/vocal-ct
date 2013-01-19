#!/usr/bin/perl
#
# File: newstage.pl
#
# This is a perl script used to prepare stage area with all the binaries
# built from a checked out CVS source tree.
#
# Usage:  newstage.pl ROOT BIN OS_ARCH DESTDIR PREFIX TARGET_NAME
#
# where:
#    ROOT is the root of the VOCAL tree
#    BIN is the name of the bin directory (e.g. bin.debug.Linux.i686)
#    OS_ARCH is the OS and architecture (e.g. Linux.i686)
#    DESTDIR is the destination directory name (e.g. ../bin.debug.Linux.i686)
#    PREFIX is the directory to be installed to (e.g. /usr/local/vocal)
#    TARGET_NAME is the target (e.g. debug.Linux.i686)

# this requires a file named filelist in the current directory.  That
# file contains all of the files to be copied.

#use Getopt::Long;

$root = $ARGV[0];
$bin = $ARGV[1];
$arch = $ARGV[2];
$destdir = $ARGV[3];
$filelist = $ARGV[4];
$prefix = $ARGV[5];
$target_name = $ARGV[6];

open(FILELIST, $filelist) || die "can't open $filelist: $!";
while(<FILELIST>) {
    $src = $dest = "";
    chomp;
    s/^\#.*$//;
    ($src, $dest, $vars) = split(/\s+/, $_);

    if($vars !~ s/^\(([\w=,]+)\)$/\1/) {
	$vars = "";
    }

    if($src && $dest) {
	$src =~ s/%bin%/$bin/g;
	$src =~ s/%arch%/$arch/g;
        $src =~ s/%target_name%/$target_name/g;
	if($src eq "." || $src eq "/") {
	    $first = "/$dest";
	    $first =~ s/\/$//;
	} else {
	    $first = "$root/$src";
	}
	push(@files, [$first, "$destdir/$dest", $dest, $vars]);
    }
}
close(FILELIST);

$= = 10000000; # big number

if(open(F, "$destdir/../specfilelist.spec")) {
    while(<F>) {
	chomp;
	push(@oldspec, $_);
    }
    close(F);
}
# read the current PREFIXVALUE from the DESTDIR/INSTALLDIR file

if(open(F, "$destdir/INSTALLDIR")) {
    chomp($old_prefix = <F>);
    close(F);
}

if($old_prefix ne $prefix) {
    $rewrite_scripts = 1;
}

# make $destdir if it's not there

if(!-d $destdir) {
    system("mkdir -p $destdir");
    if(!-d $destdir) {
         die "could not make directory $destdir\n";
    }
}


foreach $x (@files) {

    $$x[0] =~ /\/([^\/]*)$/;
    $filename = $1;

    print "X: $$x[3]\n" if $debug;

    my @varlist = split(/,/, $$x[3]);

    my %varlist;
    my $cmd;

    foreach $var (@varlist) {
	if($var =~ /^(\w*)=(\w*)$/) {
	    $varlist{$1} = $2;
	}
    }

    $package = "";

    if($varlist{type} eq "dir") {
	if(!-d "$$x[1]") {
	    # create a directory
	    $ret = mkdir $$x[1], 0755;
	    $status = "written";
	    if(!$ret) {
		$status = "failed";
		$package = $varlist{pkg};
		$anyfailed = 1;
	    }
	} else {
	    $status = "unchanged";
	}
	$filename = "$filename/";
	push(@specfilelist, ["%dir \@prefix\@/bin/$$x[2]", ""]);
    } elsif((!-f "$$x[1]/$filename") || 
       (-M $$x[0] < -M "$$x[1]/$filename") ||
       ($varlist{type} eq "script" && $rewrite_scripts)) {
	print "type: $varlist{type}\n" if $debug;

	if(-e $$x[0]) {
	    if($varlist{type} ne "script") {
	        $cmd = "cp $$x[0] $$x[1] > /dev/null 2>&1";
		$ret = system("$cmd");
	    } else {
		if($$x[1] =~ /\/$/) {
		    if($$x[0] =~ /\/([^\/]*)$/) {
			$$x[1] .= $1;
		    } elsif($$x[0] ne "") {
			$$x[1] .= $$x[0];
		    }
		}
		
		$cmd = "sed -e 's#\@prefix@#$prefix#g;' < $$x[0] > $$x[1]; chmod a+rx $$x[1]";
		print $cmd, "\n" if $debug;
		$ret = system($cmd);
	    }
	    if($ret) {
		$status = "failed";
		$package = $varlist{pkg};
		$anyfailed = 1;
	    } else {
		$status = "written";
		if($varlist{type} eq "doc") {
		    push(@specfilelist, ["%doc \@prefix\@/bin/$$x[2]/$filename"]);
		} else {
		    push(@specfilelist, ["\@prefix\@/bin/$$x[2]/$filename"]);
		}
		
	    }
	} else {
	    # could not find the existing file, so skip
	    $status = "skipped";
	    $package = $varlist{pkg};
	}
    } else {
	$status = "unchanged";
	if($varlist{type} eq "doc") {
	    push(@specfilelist, ["%doc \@prefix\@/bin/$$x[2]/$filename"]);
	} else {
	    push(@specfilelist, ["\@prefix\@/bin/$$x[2]/$filename"]);
	}
    }
    if($filename =~ /.(.{35,35})$/) {
	$filename = "...$1";
    }

    write;
    if($anyfailed) {
	if($varlist{type} eq "dir") {
	    print STDERR "Failed to create directory $$x[1]: $!\n";
	} else {
	    my $cwd = `pwd`;
	    print STDERR "Failed to copy file $$x[0] to $$x[1]: $!\ncmd: $cmd\npwd: $cwd\n";
	}
	exit(-1);
    }
}

if($old_prefix ne $prefix) {
    open(F, ">$destdir/INSTALLDIR") || die "can't open $destdir/INSTALLDIR for writing: $!";
    print F $prefix;
    close(F);
}

$writespec = 0;

$count = scalar(@specfilelist);
$count2 = scalar(@oldspec);

foreach $x (@specfilelist) {
    $$x[0] =~ s!/\./!/!g;
    $$x[0] =~ s!//!/!g;
}

if($count == $count2) {
    for($i = 0; $i < $count; $i++) {
	$a = $specfilelist[$i];
	if($$a[0] ne $oldspec[$i]) {
	    $writespec = 1;
	    print "$$a[0] : $oldspec[$i]\n";

        }
    }
} else {
    $writespec = 1;
}

#print "writespec: $writespec ($count)\n";

if($writespec) {
    open(F, ">$destdir/../specfilelist.spec") || die "can't open $destdir/../specfilelist.spec: $!";
    foreach $x (@specfilelist) {
	print F $$x[0], "\n";
    }
    close(F);
}

format STDOUT_TOP =
Filename                                Result      Needed Package (if failed)
------------------------------------------------------------------------------
.

format STDOUT =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   @<<<<<<<<   @<<<<<<<<<<<<<<<<<
$filename,                              $status,    $package
.
