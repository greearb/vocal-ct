Prefix: /tmp/Vocal-Install
Summary: The Vocal classXswitch
Name: Vocal
Version: 1.0
Release: 1
Copyright: BSD
Group: Applications/Communications
Source: Vocal-Sources-1.0.tar
URL: http://www.vovida.com
Vendor: Vovida Networks Inc.

%description
The Vocal classXswitch - Fuelling the Communincations Revolution.
http://www.vovida.com

%prep 

%build

%install
mkdir -p /tmp/Vocal-Install
cp /usr/src/redhat/RPMS/i386/Vocal-Binaries-1.0-1.i386.rpm  /tmp/Vocal-Install
cd -
cp install/deploy /tmp/Vocal-Install
cp install/README /tmp/Vocal-Install
cp install/install.txt /tmp/Vocal-Install
cd -

%files
/tmp/Vocal-Install

%post
 
echo  ''
echo  '  <-----------------------Vovida Networks Inc----------------------->'
echo  '  <                                                                 >'
echo  '  <                                                                 >'
echo  '  <           Thanks for installing the Vocal classXswitch          >'
echo  '  <                                                                 >'
echo  '  <              If you havent specified a prefix, the              >'
echo  '  <               files will be in /tmp/Vocal-Install               >'
echo  '  <                                                                 >'
echo  '  <            Please create a hosts file and run deploy            >'
echo  '  <                      at the prompt as :                         >'
echo  '  <               ./deploy -r <Vocal-Binaries-RPM>                  >'
echo  '  <                                                                 >'
echo  '  <                                                                 >'
echo  '  <-------------Fuelling the Communications Revolution-------------->'
echo  ''

