Summary: A SIP call control system.
Name: vocal
Version: 1.3.0
Release: 1
URL: http://www.vovida.org/
Source0: %{name}-%{version}.tar.gz
License: Vovida Software License
Group: Applications/System
#BuildRoot: %{_tmppath}/%{name}-root

%description

VOCAL is a complete open source SIP call control system.  Includes
proxy server, redirect server, registrar, user agent, simple voice
mail, feature servers implementing CPL.

%prep
%setup -q

%build
cd vocal
make CODE_OPTIMIZE=1 all

%install
rm -rf $RPM_BUILD_ROOT

# produce staging directory
cd vocal
make CODE_OPTIMIZE=1 install-bin

# install the binaries


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%dir /usr/local/vocal/bin
%dir /usr/local/vocal/bin/voicemail
%dir /usr/local/vocal/bin/voicemail/Voice
/usr/local/vocal/bin/voicemail/Voice/InvalidUser.wav
/usr/local/vocal/bin/voicemail/Voice/OwnerGreeting.wav
/usr/local/vocal/bin/voicemail/Voice/beep.wav
/usr/local/vocal/bin/voicemail/Voice/greeting.wav
/usr/local/vocal/bin/voicemail/base64encoder
/usr/local/vocal/bin/voicemail/vmserver
/usr/local/vocal/bin/voicemail/fsvm
%config /usr/local/vocal/bin/voicemail/voicemail.config
/usr/local/vocal/bin/voicemail/sendMime.sh
/usr/local/vocal/bin/voicemail/sendMessage
/usr/local/vocal/bin/voicemail/ua.cfg
/usr/local/vocal/bin/voicemail/uavm_cfg.txt
%dir /usr/local/vocal/bin/provisioning
%dir /usr/local/vocal/bin/provisioning/xml
/usr/local/vocal/bin/provisioning/xml/GlobalConfigData.xml
/usr/local/vocal/bin/provisioning/xml/UserSeed.xml
/usr/local/vocal/bin/provisioning/xml/ListOfMarshalServers.xml
/usr/local/vocal/bin/provisioning/xml/ListOfFeatureServers.xml
/usr/local/vocal/bin/provisioning/DeployServers.java
%dir /usr/local/vocal/bin/Tone
/usr/local/vocal/bin/Tone/3beeps
/usr/local/vocal/bin/Tone/cwbeep
/usr/local/vocal/bin/Tone/ringback
/usr/local/vocal/bin/Tone/dialtone
%dir /usr/local/vocal/bin/webpages
/usr/local/vocal/bin/webpages/indexSeed.txt
/usr/local/vocal/bin/webpages/provSeed.txt
/usr/local/vocal/bin/webpages/systemStatus.cgi
/usr/local/vocal/bin/webpages/snmpSeed.txt
/usr/local/vocal/bin/webpages/provisioning.cgi
/usr/local/vocal/bin/webpages/user.cgi
%dir /usr/local/vocal/bin/policy
/usr/local/vocal/bin/policy/getcacert.tk
/usr/local/vocal/bin/policy/reqOrRet.tcl
/usr/local/vocal/bin/policy/enrollDevice.tcl
/usr/local/vocal/bin/policy/openssl.cnf
/usr/local/vocal/bin/policy/getKeyReq.tk
%dir /usr/local/vocal/bin/sample-ua-config
/usr/local/vocal/bin/sample-ua-config/ua1000.cfg
/usr/local/vocal/bin/sample-ua-config/auto1000.cfg
/usr/local/vocal/bin/sample-ua-config/ua1001.cfg
/usr/local/vocal/bin/sample-ua-config/auto1001.cfg
%dir /usr/local/vocal/bin/mrtg
/usr/local/vocal/bin/mrtg/mrtg.in
/usr/local/vocal/bin/mrtg/serverSeed.in
%dir /usr/local/vocal/bin/images
/usr/local/vocal/bin/images/blueball.gif
/usr/local/vocal/bin/images/redball.gif
%dir /usr/local/vocal/bin/callAgentNCS
/usr/local/vocal/bin/.rev
/usr/local/vocal/bin/FileDataStoreWrapper
/usr/local/vocal/bin/fs
/usr/local/vocal/bin/cdrserv
/usr/local/vocal/bin/js
/usr/local/vocal/bin/callAgent
/usr/local/vocal/bin/rs
/usr/local/vocal/bin/vocalstart
/usr/local/vocal/bin/hbs
/usr/local/vocal/bin/psClient.jar
/usr/local/vocal/bin/deploy
/usr/local/vocal/bin/hosts
/usr/local/vocal/bin/SNMPClient.jar
/usr/local/vocal/bin/ms
/usr/local/vocal/bin/README
/usr/local/vocal/bin/ua
/usr/local/vocal/bin/install.cfg
/usr/local/vocal/bin/pserver

%doc README


#postinstall
%post




%changelog
* Wed Jan 30 2002 BRYAN OGAWA <bko@cisco.com>
- Initial build.
