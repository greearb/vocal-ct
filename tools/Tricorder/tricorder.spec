%define version 0.7
%define release 7
%define prefix /usr

Prefix: %{prefix}
Summary: A graphical tool for generating call-flow diagrams
Name: tricorder
Version: %{version}
Release: %{release}
Packager: Quinn Weaver <qweaver@vovida.com>
Source: ftp://ftp.vovida.org/pub/tools/tricorder/tricorder-%{version}-%{release}.tar.gz
Copyright: Vovida License (BSD-style):  See http://www.vovida.org/license.html
Group: Applications/Communications
Requires: perl >= 5.005
Requires: perl-Tk

%description
tricorder is a graphical call flow analysis tool written in Perl-Tk.
It takes the output from an ipgrab session, parses it, and displays
it graphically in the canonical call flow style used by telecommunications
companies the world around.  You may use the snapshot option to save
these displays to Postscript, allowing you to print them or include
them in your documentation.

Apologies go out to the Star Trek authors, and thanks to Bryan Ogawa,
for the name.

%prep

%setup

%build

%install
mkdir -p %{prefix}/doc/tricorder-%{version}
cp *DESIGN_SPEC* TODO %{prefix}/doc/tricorder-%{version}
SITE_PERL=`rpm -ql perl | grep lib/perl5/site_perl/5.005 | head -1`
mkdir -p $SITE_PERL/Telephony
cp Telephony/SipMessage/SipMessage.pm $SITE_PERL/Telephony
cp Telephony/SipServer/SipServer.pm $SITE_PERL/Telephony
cp Telephony/Headers/Headers.pm $SITE_PERL/Telephony
cp FancyBalloon/FancyBalloon.pm $SITE_PERL
cp CallFlowCanvas/CallFlowCanvas.pm $SITE_PERL

mkdir -p %{prefix}/bin
cp tricorder.pl %{prefix}/bin

%files
%doc
%{prefix}/doc/tricorder-%{version}/DESIGN_SPEC
%{prefix}/doc/tricorder-%{version}/DESIGN_SPEC--brief
%{prefix}/doc/tricorder-%{version}/NEW_DESIGN_SPEC--brief

%attr(0644,root,root) /usr/lib/perl5/site_perl/5.005/Telephony/SipMessage.pm
%attr(0644,root,root) /usr/lib/perl5/site_perl/5.005/Telephony/SipServer.pm
%attr(0644,root,root) /usr/lib/perl5/site_perl/5.005/Telephony/Headers.pm
%attr(0644,root,root) /usr/lib/perl5/site_perl/5.005/FancyBalloon.pm
%attr(0644,root,root) /usr/lib/perl5/site_perl/5.005/CallFlowCanvas.pm
%attr(0755,root,root) %{prefix}/bin/tricorder.pl
