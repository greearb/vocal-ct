Prefix: /tmp/vocalbinaries/bin
Summary: The Vocal classXswitch binaries
Name: Vocal-Binaries
Version: 1.0
Release: 1
Copyright: BSD
Group: Applications/Communications
Source: Vocal-Sources-1.0.tar
URL: http://www.vovida.com
Vendor: Vovida Networks Inc.

%description
The Vocal binaries rpm contains all the binary files that are required to run the vocal classXswitch. To be used in conjunction with deploy to install the application.

%prep


%build

%install
mkdir -p /tmp/vocalbinaries
cd -
cp -dR stage/bin /tmp/vocalbinaries
cd -

%files
/tmp/vocalbinaries/bin
