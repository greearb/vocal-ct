@prefix@/bin/fs
@prefix@/bin/ms
@prefix@/bin/pserver
@prefix@/bin/FileDataStoreWrapper
@prefix@/bin/rs
@prefix@/bin/cdrserv
%dir @prefix@/bin/Tone/
%dir @prefix@/bin/sample-ua-config/
%dir @prefix@/bin/policy/
@prefix@/bin/policy/getKeyReq.tk
@prefix@/bin/policy/getcacert.tk
@prefix@/bin/policy/enrollDevice.tcl
@prefix@/bin/policy/reqOrRet.tcl
@prefix@/bin/policy/openssl.cnf
%dir @prefix@/bin/voicemail/
@prefix@/bin/voicemail/vmserver
@prefix@/bin/voicemail/PromptTable
@prefix@/bin/voicemail/sendMessage
@prefix@/bin/voicemail/sendMime.sh
@prefix@/bin/voicemail/base64encoder
@prefix@/bin/voicemail/voicemail.config
%dir @prefix@/bin/voicemail/Voice/
@prefix@/bin/voicemail/Voice/InvalidUser.wav
@prefix@/bin/voicemail/Voice/OwnerGreeting.wav
@prefix@/bin/voicemail/Voice/beep.wav
@prefix@/bin/voicemail/Voice/greeting.wav
@prefix@/bin/vocalstart
@prefix@/bin/vocald
@prefix@/bin/vocalctl
@prefix@/bin/verifysip
@prefix@/bin/manageusers
@prefix@/bin/vocald-rc.sh
%dir @prefix@/bin/webpages/
@prefix@/bin/webpages/provisioning.cgi
@prefix@/bin/webpages/systemStatus.cgi
@prefix@/bin/webpages/user.cgi
@prefix@/bin/webpages/webconf.cgi
@prefix@/bin/webpages/psTranslate.cgi
@prefix@/bin/webpages/provactions.pm
@prefix@/bin/webpages/nanoxml.pm
%doc @prefix@/bin/webpages/README.txt
%dir @prefix@/bin/webpages/vprov/
@prefix@/bin/webpages/vprov/ui.pm
@prefix@/bin/webpages/vprov/cplxml.pm
@prefix@/bin/webpages/vprov/serverxml.pm
@prefix@/bin/webpages/vprov/userxml.pm
@prefix@/bin/webpages/vprov/provxml.pm
@prefix@/bin/webpages/vprov/vpp.pm
@prefix@/bin/webpages/indexSeed.txt
@prefix@/bin/webpages/provSeed.txt
@prefix@/bin/webpages/snmpSeed.txt
@prefix@/bin/psClient.jar
@prefix@/bin/provgui
@prefix@/bin/SNMPClient.jar
%dir @prefix@/bin/provisioning/
@prefix@/bin/provisioning/DeployServers.java
%dir @prefix@/bin/provisioning/xml/
@prefix@/bin/provisioning/xml/GlobalConfigData.xml
@prefix@/bin/provisioning/xml/ListOfFeatureServers.xml
@prefix@/bin/provisioning/xml/ListOfMarshalServers.xml
@prefix@/bin/provisioning/xml/UserSeed.xml
%dir @prefix@/bin/images/
@prefix@/bin/images/blueball.gif
@prefix@/bin/images/redball.gif
@prefix@/bin/sample-b2bua-cfg.xml/b2bConfig.xml
@prefix@/bin/radiusServer
@prefix@/bin/radiusClient
@prefix@/bin/sample-radius-server.cfg/server.cfg
@prefix@/bin/sample-radius-client.cfg/client.cfg
@prefix@/bin/gua
@prefix@/bin/sipset
@prefix@/bin/sample-gua.cfg
@prefix@/bin/ataOpFlags.txt
@prefix@/bin/ataParams.txt
@prefix@/bin/atatftp.pl
@prefix@/bin/in.tftpd
%dir @prefix@/bin/allinoneconfigure/
@prefix@/bin/allinoneconfigure/allinoneconfigure
@prefix@/bin/allinoneconfigure/deployedServers.dtd
@prefix@/bin/allinoneconfigure/initalprovdata.pl
@prefix@/bin/allinoneconfigure/loadgen_deployedServers.xml
@prefix@/bin/allinoneconfigure/rulestd.sed
@prefix@/bin/allinoneconfigure/sample_deployedServers.xml
@prefix@/bin/allinoneconfigure/sample_vocal.conf
@prefix@/bin/install.cfg
@prefix@/bin/hosts
@prefix@/bin/.rev
@prefix@/bin/hbs
%dir @prefix@/bin/mrtg/
@prefix@/bin/mrtg/mrtg.in
@prefix@/bin/mrtg/serverSeed.in
%doc @prefix@/bin/README_VOCAL.txt
%doc @prefix@/bin/README_SIP.txt
%doc @prefix@/bin/README_RTP.txt
%doc @prefix@/bin/LICENSE.txt
%dir @prefix@/bin/manpages/
%dir @prefix@/bin/manpages/man1/
%dir @prefix@/bin/manpages/man5/
%dir @prefix@/bin/manpages/man8/
@prefix@/bin/manpages/man1/b2bua.1
@prefix@/bin/manpages/man1/fs.1
@prefix@/bin/manpages/man1/ms.1
@prefix@/bin/manpages/man1/pserver.1
@prefix@/bin/manpages/man1/rs.1
@prefix@/bin/manpages/man1/sipset.1
@prefix@/bin/manpages/man1/vmserver.1
@prefix@/bin/manpages/man5/vocalconf.5
@prefix@/bin/manpages/man8/allinoneconfigure.8
@prefix@/bin/manpages/man8/vocal.8
@prefix@/bin/manpages/man8/vocalctl.8
@prefix@/bin/manpages/man8/vocald.8
%dir @prefix@/bin/webpages/man/
@prefix@/bin/webpages/man/b2bua.html
@prefix@/bin/webpages/man/fs.html
@prefix@/bin/webpages/man/ms.html
@prefix@/bin/webpages/man/pserver.html
@prefix@/bin/webpages/man/rs.html
@prefix@/bin/webpages/man/sipset.html
@prefix@/bin/webpages/man/vmserver.html
@prefix@/bin/webpages/man/vocalconf.html
@prefix@/bin/webpages/man/allinoneconfigure.html
@prefix@/bin/webpages/man/vocal.html
@prefix@/bin/webpages/man/vocalctl.html
@prefix@/bin/webpages/man/vocald.html
%dir @prefix@/bin/lib/
@prefix@/bin/lib/libcdr.so
@prefix@/bin/lib/libheartbeat.so
@prefix@/bin/lib/libmail.so
@prefix@/bin/lib/libmedia.so
@prefix@/bin/lib/libproxyBase.so
@prefix@/bin/lib/libps.so
@prefix@/bin/lib/libpsutil.so
@prefix@/bin/lib/librtp.so
@prefix@/bin/lib/libsdp2.so
@prefix@/bin/lib/libsip.so
@prefix@/bin/lib/libsipua.so
@prefix@/bin/lib/libsoundcard.so
@prefix@/bin/lib/libvcpl.so
@prefix@/bin/lib/libvmcp.so
@prefix@/bin/lib/libvme.so
@prefix@/bin/lib/libvutil.so
@prefix@/bin/lib/libdbmanager.so
@prefix@/bin/lib/librecords.so
@prefix@/bin/lib/libsndfile.so.1
@prefix@/bin/lib/libspeex.so.1
