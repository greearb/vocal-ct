#!/usr/bin/perl

# program usage and help information
if ($#ARGV == 1  || $#ARGV == 2 || $#ARGV == 3) {
    # assign arguments
    $dotag = 1;
    &Usage if ( (! $ARGV[0] =~ m/^mgcp|rtp|sip$/) || 
                ( $#ARGV >= 2 && $ARGV[2] ne '-notag' ) ); 
    $release = $ARGV[0];
    $version = $ARGV[1];
    $checkoutversion = $ARGV[3];
    $dirname = $release . '-' . $version;
    $tarname = $dirname . '.tar.gz';
    $fileext = '-' . $release . '-' . $version . '.txt'; 
    $release_tag = 'R_' . $release . '_' . ${version};
    $release_tag =~ tr/a-z/A-Z/;
    $release_tag =~ s/[.-]/_/g;
    $dotag = 0 if ( $#ARGV >= 2 ); 
} else {
    &Usage();
}

%files_to_remove =
    (
        mgcp   => [ 'sip' ],
        rtp    => [ 'mgcp', 'sdp', 'sip' ],
        sip    => [ 'mgcp' , 'sip/base/BasicProxy.cxx', 
                    'sip/base/BasicProxy.hxx', 
                    'sip/base/BaseProxyTestMain.cxx',
                    'sip/sua' ],
        common => [ 'provisioning', 'snmp', 'cdr',
                    'h323', 'release',
                    'policy', 'doc', 'vm', 'vmclient',
                    'snmp', 'stage', 'config', 'apache-rs', 'snmp++' ,
                    'BRANCHES', 'heartbeat', 'proxies', 'translators', 
		    'gw', 'install', 
		    'contrib/Jindent21', 'contrib/acculab',
		    'contrib/audiocodes', 'rtsp', 'snmp', 'snmp++',
		    'contrib/libxml', 'contrib/xerces-1_0_3','tools/Tricorder'
		  ],
        required => [ 'do-not-distribute', 'private', 'tripstack' ],
    );

%files_to_copy =
    (
        mgcp   => [ 'mgcp/README' ],
        rtp    => [ 'rtp/README' ],
        sip    => [ 'sip/sipstack/README.first' ]
    );

# clean up if necessary (checkout etc)
print "--> cleaning up existing checkout if any\n";
if ( -d "$dirname" ) {
    $syscmd = '\rm -rf ' . $dirname;
    print "    -> detected checkout, removing it\n"; 
    $stat = system( $syscmd );
    &SysFailed( $syscmd ) if ( $stat != 0 ); 
}

if ( $dotag ) {
    # tag to mark the release point
    print "--> tagging head branch with $release_tag\n";
    $syscmd = 'cvs rtag -F -b ' . $release_tag . ' vocal';
    print "    -> syscmd: $syscmd\n";
    print "    -> output: ./tag.log\n";
    $syscmd = $syscmd . ' > ./tag.log 2>&1';
    $stat = system( $syscmd );
    &SysFailed( $syscmd ) if ( $stat != 0 ); 
}

# checkout for sources
if ( $dotag == 0) {
	$release_tag = 'HEAD';
	$release_tag = $checkoutversion if ( $checkoutversion );
}
print "--> cvs exporting source with tag=$release_tag\n";
$syscmd = 'cvs export -r ' . $release_tag . ' -d ' . $dirname . ' vocal';
print "    -> syscmd: $syscmd\n";
print "    -> output: ./export.log\n";
$syscmd = $syscmd . ' > ./export.log 2>&1';
$stat = system( $syscmd );
&SysFailed( $syscmd ) if ( $stat != 0 ); 

# package (rm/cp/tar)
print "--> removing unnecessary dirs\n";
foreach $target (common, required, $release) {
    foreach $i ( 0 .. $#{ $files_to_remove{$target} } ) {
        $dir = $files_to_remove{$target}[$i];
        $dirsrc = './' . $dirname . '/' . $dir;
        $syscmd = '/bin/rm -rf ' . $dirsrc;
        $stat = system( $syscmd );
        print "    -> $syscmd\n";
        if ( ! -e $dirsrc ) {
            print "    -> skipping $dirsrc as it does not exist\n";
        } else {
            $stat = system( $syscmd );
            &SysFailed( $syscmd ) if ( $stat != 0 );
        }
    }
}                                          

print "--> cp readme file\n";
foreach $i ( 0 .. $#{ $files_to_copy{$release} } ) {
    $file = $files_to_copy{$release}[$i];
    $filesrc = './' . $dirname . '/' . $file;
    $syscmd = '\cp ' . $filesrc . ' ./' . $dirname .
        '/README';
    $syscmd .= $fileext if ( $release ne 'sip');
    print "    -> $syscmd\n";
    if ( ! -e $filesrc ) {
        print "    -> skipping $filesrc as it does not exist\n";
    } else {
        $stat = system( $syscmd );
        &SysFailed( $syscmd ) if ( $stat != 0 );
    }
}                                      
 
print "--> making doc (make doc)\n";
$syscmd = "(cd $dirname; echo USE_BUILDDIR=no > ./.make_prefs; make doc)";
print "    -> syscmd: $syscmd\n";
print "    -> output: ./mkdoc.log\n";
$syscmd = $syscmd . ' > ./mkdoc.log 2>&1';
$stat = system( "$syscmd" );
&SysFailed( $syscmd ) if ( $stat != 0 );

print "--> last check to remove unnceessary files\n";
$syscmd = 'find . -name \'obj.debug.*\' -type d -exec rm -rf {} \; > /dev/null';
print "    -> syscmd: $syscmd\n";
$syscmd = $syscmd . ' > /dev/null';
$stat = system( "$syscmd" );
#&SysFailed( $syscmd ) if ( $stat != 0 );

print "--> tarring up the final tarbomb\n";
$syscmd = 'tar zcvpf ' . $tarname . " $dirname";
print "    -> syscmd: $syscmd\n";
print "    -> output: ./tar.log\n"; 
$syscmd = $syscmd . ' > ./tar.log 2>&1';
$stat = system( $syscmd );
&SysFailed( $syscmd ) if ( $stat != 0 );

# build
print "--> untarring the tarbomb in /tmp and attemp to build\n";
if ( -d "/tmp/$dirname" ) {
    print "    -> removing /tmp/$dirname\n";
    system( '\rm -rf /tmp/' . $dirname );
}
if ( -e "/tmp/$tarname" ) {
    print "    -> removing /tmp/$tarname\n";
    system( '\rm /tmp/' . $tarname );
}
$syscmd = '(cp ' . './' . $tarname . ' /tmp;' .
          'cd /tmp; ' . 'tar zxvpf ' . "./$tarname" . ')'; 
print "    -> syscmd: $syscmd\n";
print "    -> output: ./untar.log\n";
$syscmd = $syscmd . ' > ./untar.log 2>&1';
$stat = system( $syscmd );
&SysFailed( $syscmd ) if ( $stat != 0 );

# if build fails, the remove the tag
print "--> checking the build results\n";
$syscmd = '(cd /tmp/' . $dirname . 
          "; echo USE_BUILDDIR=no > ./.make_prefs; make $release)"; 
print "    -> $syscmd\n";
$syscmd = $syscmd . ' > ./make.log 2>&1';
$stat = system( $syscmd );
if ( $stat != 0 && $ARGV[2] ne '-notag' ) {
    print "--> removing tag $release_tag as make failed\n";
    $syscmd = "cvs rtag -d $release_tag vocal";
    print "    -> syscmd: $syscmd\n";
    print "    -> output: ./dtag.log\n";
    $syscmd = $syscmd . ' > ./dtag.log 2>&1';
    $stat = system( $syscmd );
    &SysFailed( $syscmd ) if ( $stat != 0 );
}
    
&SysFailed( $syscmd ) if ( $stat != 0 );

sub Usage
{
    print STDERR<<EOF
usage: $0 <mgcp|rtp|sip> <version> [-notag [branch] ] 
EOF
    ;
    exit( 1 );
}

sub SysFailed
{
    my( $command ) = @_;

    print "*** error: failed to execute $command\n";
    exit( 1 );
}

# end of the program
