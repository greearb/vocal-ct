package FancyBalloon;

use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);
use Tk;
use Data::Dumper;

use Telephony::Headers qw ( get_name_for_value );
use constant DEBUG => 0;

require Exporter;

@ISA = qw (Exporter AutoLoader Tk::Toplevel);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT    = qw (   );
@EXPORT_OK = qw (   );

$VERSION = '0.01';

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
#__END__

sub new
{
    my ($class, $headers, $header_name_color, $header_value_color, $canvas) = @_;
    return undef if !defined $headers;
    
    my $balloon = $canvas->Toplevel (takefocus => 0);
    defined $balloon
	or croak "The balloon came out empty!  How can that be?";

    print "Constructed a balloon:\n" . Data::Dumper->Dump ([ $balloon ]) if DEBUG;
    
    my $width_of_widest_name  = 0;
    my $width_of_widest_value = 0;
    
    #Caveat hacor:  Funky array-hash duality mojo follows...

    print "Header pseudo-hash:\n" . Data::Dumper->Dump ([ $headers ]) if DEBUG;
    
    my @name_value_pairs;

    my ($i, $header_name, $header_value, $header_name_label, $header_value_label, $header_name_width, $header_value_width);
    for ($i = 1; $i <= @$headers; $i++) #Treat the pseudo-hash as an array.
    {
	$header_value = $headers->[$i];
	$header_name = $headers->get_name_for_value ($header_value);
	$header_value_label = $balloon->Label (text       => $header_value,
					       foreground => $header_value_color,
					       anchor     => 'w');
	$header_name_label  = $balloon->Label (text       => $header_name,
					       foreground => $header_name_color,
					       anchor     => 'w');
	print "Made a header name label with text $header_name;\n"
	    . "Made a header value label with text $header_value\n" if DEBUG;
	$header_name_width  = $header_name_label->reqwidth();
	$header_value_width = $header_value_label->reqwidth();
	print "\$header_name_width  is $header_name_width;\n"
	    . "\$header_value_width is $header_value_width.\n" if DEBUG;
	$width_of_widest_name  = $header_name_width  if ($header_name_width > $width_of_widest_name);
	$width_of_widest_value = $header_value_width if ($header_value_width > $width_of_widest_value);
	push @name_value_pairs, [ $header_name_label, $header_value_label ];
    }

    print "Widest name  is $width_of_widest_name pixels wide;\n"
	. "Widest value is $width_of_widest_value pixels wide.\n" if DEBUG;

    my $x = 5;
    my $y = 5;
    map
    {
	my $header_name_label  = $_->[0];
	my $header_value_label = $_->[1];
	$header_name_label->grid ($header_value_label, -sticky => 'w');

#!	$header_name_label->place  (-x      => $x,
#!				    -y      => $y,
#!				    -width  => $width_of_widest_name,
#!				    -anchor => 'nw');
#!	$header_value_label->place (-x      => $x + $width_of_widest_name +2,
#!				    -y      => $y,
#!				    -width  => $width_of_widest_value,
#!				    -anchor => 'nw');
#!	$y += $header_value_label-> reqheight() + 2;
    } @name_value_pairs;

#!  return bless ($balloon, $class);
    return $balloon;
}

# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

FancyBalloon - Perl extension for blah blah blah

=head1 SYNOPSIS

  use FancyBalloon;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for FancyBalloon was created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head1 AUTHOR

A. U. Thor, a.u.thor@a.galaxy.far.far.away

=head1 SEE ALSO

perl(1).

=cut
