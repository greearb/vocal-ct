#!/usr/bin/perl

# generate manpages and html pages from docbook
# we assume input names are "progname.sgml"

$usage = "Usage: generate.pl <docbook file(s)>\n";

die "${usage}" unless $#ARGV > -1;
if( ($ARGV[0] eq "-h") || ($ARGV[0] eq "--help") )
{ die $usage; }

while(<@ARGV>)
{
  # create manpage
  system ("docbook2man", "-o generated_man", $_);

  # create html page
  # get the root name of the program
  ($progname, $sgml) = split (/\./, $_);

  # this step generates "index.html"
  system ("docbook2html", "-o generated_html", $_);

  # rename and move index.html to generated_html/<progname>.html
  system ("mv", "generated_html/index.html", "generated_html/${progname}.html");
}
