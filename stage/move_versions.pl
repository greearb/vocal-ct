#!/usr/bin/perl

# script to move versions -- right now, it modifies files ending in
# .txt in the main directory and in the doc dir, and change
# version.conf as well.

open(F, "version.conf") || die "cannot find version.conf: $!";
chomp($old_version = <F>);
close(F);

print "Old version: $old_version\n";
print "\n";

print "Enter new version: ";
chomp($new_version = <STDIN>);

print "New version: $new_version\n";
print "Is this correct?  Enter y to continue, any other response to quit: ";
chomp($is_ok = <STDIN>);
if($is_ok eq "y") {
    # do the work
    system("perl -pi.bak -e 's/$old_version/$new_version/g;' version.conf ../*.txt ../doc/*.txt");
} else {
    print "aborted.\n";
}
