# $Id: Makefile,v 1.3 2004/06/20 21:55:42 greear Exp $


BUILD = build

ifdef RELEASE 

STAGING_ARGS = -R 

endif

# NOTE:  Removing b2bua from the list below, not porting it
#     to non-threaded model at this time. --Ben

.PHONY : netMgnt proxy_agent snmp support mgcp proxy_base rtp libsoundcard\
	 subdirs cleanall clean doc cppdoc contrib contrib_equiv mgcp_demo\
	 rtp_demo releasedir util sdp sdp2 sip siptest co login usage\
	 mgcp_test rtp_test sdp_test sip_test util_test tdiff_correct.log\
	 docdir ps cdrlib pslib libpep ps cdr osp libOSP common libpdp\
	 libpep policy ua ua_test release staging staging_notar heartbeat\
	 hbs snmpplusplus vme vmcp mail media_server fsvm java rpm siph323csgw\
	 install radius radius_test loadgen vocal_gk all kitchen_sink mrtg \
	 extras extras_broken usage_note default_target install_note \
	 loadbalance libmedia

SUBDIRS = netMgnt snmpplusplus proxy_agent snmp rtp mgcp proxy_base\
	 private sip sdp heartbeat hbs util contrib radius

DOCPPSUBDIRS = \
	rtp \
	sip \
	sdp/sdp2 \
	heartbeat \
	util \
	radius \
        libmedia \
        libsoundcard


NO_DISTRO_SUBDIRS = apache-rs do-not-distribute doc gw private\
	 release sipvm tripstack

VERIFY_SUBDIRS = util/unit_test sip/test/unit_test rtp/unit_test sdp/sdp2/unit_test


EXTERNAL_SUBDIRS = rtp mgcp sdp util sip

######################################################################

# this code is the start of modularity for the makefile process.  we
# won't be using it for the moment, however.

#MAKE_INCLUDES := $(foreach dir,$(SUBDIRS),$(dir)/Makefile.data)

#-include $(MAKE_INCLUDES)

# do not move this down!
default_target: usage_note all

usage_note:
	@echo "**********************************************************"
	@echo "    'make usage' will show you other targets for make"
	@echo "**********************************************************"

usage:
	@echo "The make file no longer supports a default target."
	@echo "Please choose one of the following:"
	@echo ""
	@echo "contrib        support items for other stacks"
	@echo "snmp           C SNMP Library, no Management Station"
	@echo "proxy_agent    C++ SNMP Library and trap agent"
	@echo "netMgnt        Network Management Station"
	@echo "libmgcp        MGCP stack only"
	@echo "mgcp           MGCP stack and examples"
	@echo "mgcp_test      MGCP stack + test cases"
	@echo "proxy_base     Proxy Base"
	@echo "sip            SIP stack"
	@echo "sip_test       SIP stack + test cases"
	@echo "ua             SIP User Agent"
	@echo "ua_test        SIP User Agent + test"
	@echo "gua            SIP User Agent"
	@echo "uagui          graphical front-end for SIP User Agent"
	@echo "rtp            RTP stack and examples"
	@echo "rtp_test       RTP stack and examples + test cases"
	@echo "rtspstack      RTSP stack"
	@echo "rtspif         RTSP inteface"
	@echo "rtspserver     RTSP server"
	@echo "rtspclient     RTSP client"
	@echo "sdp            SDP stack and test program"
	@echo "sdp2           SDP stack (version 2) and test program"
	@echo "sdp_test       SDP stack and test program + test cases"
	@echo "util           Vovida utility library"
	@echo "util_test      Vovida utility library + test cases"
	@echo "cdrlib         CDR Library"
	@echo "pslib          Provisioning Library"
	@echo "libpep         Policy Library"
	@echo "policy         Policy Server"
	@echo "rtp_demo       RTP stack and examples (copied to release)"
	@echo "fs             SIP Feature Server"
	@echo "cpl            CPL Building Blocks"
	@echo "cplfs          CPL Feature Server"
	@echo "fsvm           VM Feature Server"
	@echo "js             JTAPI Feature Server"
	@echo "rs             SIP Redirect Server"
	@echo "ls             TRIP Location Server"
	@echo "ms             SIP Marshal Server"
	@echo "ps             Provisioning Server"
	@echo "prov           Provisioning Server (Mascarpone)"
	@echo "loadgen        Load Generator"
	@echo "siph323csgw    SIP-H.323 Call Signaling Gateway"
	@echo "vocal_gk       Basic H.323 Gatekeeper"
	@echo "heartbeat      Heartbeat Library"
	@echo "hbs            Heartbeat Server"
	@echo "delopy         deploy program"
	@echo "vme            vme"
	@echo "vmcp           vmcp"
	@echo "mail           mail"
	@echo "osptnep        osp TransNexus enrollment client"
	@echo "java           SNMPClient.jar and psClient.jar"
	@echo "rpm            build a binary RPM"
	@echo "deploy         Installation and remote deployment tool"
	@echo "radius         RADIUS stack"
	@echo "radius_test    RADIUS stack and test program + test cases"
	@echo ""
	@echo "oldtest           run all old test cases -- does not work"
	@echo ""
	@echo "all            all of the above targets"
	@echo "icc	       Makes everything under above target all except callAgent for Intel C++ compiler "
	@echo "clean          clean all code"
	@echo "cleanall       clean all code thoroughly"
	@echo "veryclean      clean debug.*, bin.*, *.d, *.o"

#
# this include is here because our default target overrides everyone
# else's default target

include $(BUILD)/Makefile.pre


MAKE_NOJ = $(patsubst %-j4,%, $(patsubst %-j2,%, $(MAKE)))

cleantest: 
	-rm testall.log

test: verify

oldtest: rtp_test util_test mgcp_test sip_test sdp_test radius_test

tdiff_correct.log:
	find . -name 'T_RESULTS.bin*' -print | xargs -icat_results cat cat_results >> T_RESULTS.all
	tools/test-analyzer -d T_RESULTS.all > tdiff_correct.log

test_report_manual: tdiff_correct.log
	echo "Generating test report from current testall.log"
	tools/test-analyzer -d testall.log > tdiff.log
	echo "Test report for " `date` > TEST_REPORT.txt
	echo "If there are changes, they will appear below this line:" >> TEST_REPORT.txt
	echo "----------------------------------------------------------------------" >> TEST_REPORT.txt
	diff -c tdiff_correct.log tdiff.log >> TEST_REPORT.txt

test_report:
	-$(MAKE) -k test
	$(MAKE) test_report_manual

contrib: releasedir
	cd contrib; $(MAKE)

contrib_xerces_c:
	cd contrib; $(MAKE) xerces-c

contrib_equiv:
	cd contrib; $(MAKE) Equivalence

contrib_imap:
	cd contrib; $(MAKE) imap

snmp: proxy_agent
	cd snmp; $(MAKE)

mrtg: proxy_agent
	cd snmp; $(MAKE) mrtg

proxy_agent:  proxy_base
	cd proxies/agent; $(MAKE)

snmpplusplus:  snmp
	cd snmp++ ; $(MAKE)

netMgnt:  snmpplusplus
	cd proxies/netMgnt; $(MAKE)

cdrlib:
	cd cdr/cdrLib; $(MAKE)

pslib: contrib 
	-( test -d provisioning/util ) && ( cd provisioning/util; $(MAKE) )
	-( test -d provisioning/psLib ) && ( cd provisioning/psLib; $(MAKE) )

dbmodules: contrib_xerces_c util
	cd prov/cpp/pserver/dbmodules; $(MAKE)

provcommon: contrib_xerces_c util
	cd prov/cpp/common; $(MAKE) 

provlib: provcommon contrib
	cd prov/cpp/psLib; $(MAKE)

psagent: provlib
	cd prov/cpp/psAgent; $(MAKE)

libpep:
	cd policy/libpep; $(MAKE)

ospold:
	-( test -d do-not-distribute/osptk-2.5.1 ) && (cd do-not-distribute/osptk-2.5.1/src ; ./osp_sdk_compile.ksh -d client)

osp: openssl
	cd contrib ; $(MAKE) osp

common: 
	cd policy/common ; $(MAKE)

libpdp: contrib
	cd policy/libpdp ; $(MAKE)
ifeq ($(USE_RSVPLIB),1)
libOSP: osp common
	-(cd policy/libOSP; $(MAKE))
else
libOSP: common
	-(cd policy/libOSP; $(MAKE))
endif

copsstack: util contrib
	cd policy/copsstack; $(MAKE)

trip: util contrib 
	cd tripstack; $(MAKE) all

policy: libpdp common libOSP copsstack
	cd policy/policyServer; $(MAKE)

policy_test: libpep common libOSP copsstack
	cd policy/test ; $(MAKE)

util: contrib
	@( test -d util && test -f util/Makefile ) || ( echo; echo; echo "please check out the util directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co util" ; echo;echo && exit -1 )
	cd util; $(MAKE)

util_test: contrib
	@( test -d util && test -f util/Makefile ) || ( echo; echo; echo "please check out the util directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co util" ; echo;echo && exit -1 )
	cd util; $(MAKE) test

rtp: contrib util
	@( test -d rtp && test -f rtp/Makefile ) || ( echo; echo; echo "please check out the rtp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtp" ; echo;echo && exit -1 )
	cd rtp; $(MAKE)

rtp_test: contrib util
	@( test -d rtp && test -f rtp/Makefile ) || ( echo; echo; echo "please check out the rtp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtp" ; echo;echo && exit -1 )
	cd rtp; $(MAKE) test

libsoundcard: rtp util
	@ test -d libsoundcard || ( echo "libsoundcard directory does not exist" && exit -1 )
	cd libsoundcard; $(MAKE)

rtspstack: util sdp
	@( test -d rtsp/rtspstack && test -f rtsp/rtspstack/Makefile ) || ( echo; echo; echo "please check out the rtsp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtsp" ; echo;echo && exit -1 )
	cd rtsp/rtspstack; $(MAKE)

rtspif: util sdp rtp rtspstack
	@( test -d rtsp/rtspif && test -f rtsp/rtspif/Makefile ) || ( echo; echo; echo "please check out the rtsp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtsp" ; echo;echo && exit -1 )
	cd rtsp/rtspif; $(MAKE)

rtspserver: util sdp rtp rtspif rtspstack
	@( test -d rtsp/rtspserver && test -f rtsp/rtspserver/Makefile ) || ( echo; echo; echo "please check out the rtsp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtsp" ; echo;echo && exit -1 )
	cd rtsp/rtspserver; $(MAKE)

rtspclient: util
	@( test -d rtsp/rtspclient && test -f rtsp/rtspclient/Makefile ) || ( echo; echo; echo "please check out the rtsp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtsp" ; echo;echo && exit -1 )
	cd rtsp/rtspclient; $(MAKE)

sdp: util
	@( test -d sdp && test -f sdp/Makefile ) || ( echo; echo; echo "please check out the sdp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co sdp" ; echo;echo && exit -1 )
	cd sdp; $(MAKE) 

sdp2: util
	cd sdp/sdp2; $(MAKE) 

sdp_test: util
	@( test -d sdp && test -f sdp/Makefile ) || ( echo; echo; echo "please check out the sdp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co sdp" ; echo;echo && exit -1 )
	cd sdp; $(MAKE) test

rtp_demo: rtp
	@( test -d rtp && test -f rtp/Makefile ) || ( echo; echo; echo "please check out the rtp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co rtp" ; echo;echo && exit -1 )
	(cd rtp; $(MAKE) install)
	(cd rtp/test ; $(MAKE) ; $(MAKE) install)

libmgcp: contrib sdp util
	@( test -d mgcp && test -f mgcp/Makefile ) || ( echo; echo; echo "please check out the mgcp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co mgcp" ; echo;echo && exit -1 )
	cd mgcp; $(MAKE) mgcp

mgcp_examples:
	@( test -d mgcp && test -f mgcp/Makefile ) || ( echo; echo; echo "please check out the mgcp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co mgcp" ; echo;echo && exit -1 )
	cd mgcp; $(MAKE) examples

mgcp_demo: mgcp
	@( test -d mgcp && test -f mgcp/Makefile ) || ( echo; echo; echo "please check out the mgcp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co mgcp" ; echo;echo && exit -1 )
	cd mgcp; $(MAKE) install

mgcp: libmgcp mgcp_examples

mgcp_test:
	@( test -d mgcp && test -f mgcp/Makefile ) || ( echo; echo; echo "please check out the mgcp directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co mgcp" ; echo;echo && exit -1 )
	cd mgcp; $(MAKE) test

proxy_base: heartbeat contrib sip sdp2 pslib util 
	@( test -d sip/base && test -f sip/base/LICENSE ) || ( echo; echo; echo "please check out the proxyBase directory first" ; echo;echo && exit -1 )
	cd sip/base; $(MAKE)

sip: heartbeat contrib sdp2  util
	@( test -d sip/sipstack && test -f sip/sipstack/LICENSE ) || ( echo; echo; echo "please check out the sip directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co sip" ; echo;echo && exit -1 )
	cd sip/sipstack; $(MAKE)
	cd sip/base; $(MAKE)

sip_test: contrib sdp2 util
	@( test -d sip/sipstack && test -f sip/sipstack/LICENSE ) || ( echo; echo; echo "please check out the sip directory first by doing cvs -d :pserver:<random-login>:/home/cvsroot co sip" ; echo;echo && exit -1 )
	cd sip/test; $(MAKE) test

newsndfile:
	cd contrib ; $(MAKE) newsndfile


http: util
	@( test -d http && test -f http/LICENSE ) || ( echo; echo; echo "please check out the sip directory first by doing cvs -d :pserver:anonymous@sniff.gloo.net:/home/cvsroot co sip" ; echo;echo && exit -1 )
	cd http && $(MAKE)

http_test: http
	cd http && $(MAKE) unit_test

#b2bua: proxy_base sip
#	@( test -d sip/b2b/ && test -f sip/b2b/Makefile ) || ( echo; echo; echo "please check out the b2b directory first" ; echo;echo && exit -1 )
#	cd sip/b2b/; $(MAKE)

sipset: uagui 
gua: proxy_base newsndfile libsoundcard rtp libmedia libsipua 
	@( test -d sip/gua/ && test -f sip/gua/Makefile ) || ( echo; echo; echo "please check out the sip/gua directory first" ; echo;echo && exit -1 )
	cd sip/gua/; $(MAKE)
libsipua: util 
	@( test -d sip/libsipua && test -f sip/libsipua/Makefile ) || ( echo; echo; echo "please check out the libsipua directory first " ; echo;echo && exit -1 )
	cd sip/libsipua; $(MAKE)
libmedia:
	@( test -d libmedia && test -f libmedia/Makefile ) || ( echo; echo; echo "please check out the libmedia directory first " ; echo;echo && exit -1 )
	cd libmedia ; $(MAKE) 

uagui: gua
	@( test -d sip/gua/ && test -f sip/gua/Makefile ) || ( echo; echo; echo "please check out the sip/gua directory first" ; echo;echo && exit -1 )
	cd sip/gua/; $(MAKE) gui

ua: proxy_base newsndfile rtp
	@( test -d sip/ua/ && test -f sip/ua/Makefile ) || ( echo; echo; echo "please check out the ua directory first" ; echo;echo && exit -1 )
	cd sip/ua/; $(MAKE)

ua_test: 
	@( test -d sip/ua/ && test -f sip/ua/Makefile ) || ( echo; echo; echo "please check out the ua directory first" ; echo;echo && exit -1 )
	cd sip/ua/; $(MAKE) test

callAgent:  heartbeat pslib sip  libmgcp rtp sdp util
	@( test -d translators/mgcp/ && test -f translators/mgcp/Makefile ) || ( echo; echo; echo "please check out the translators/mgcp directory first" ; echo;echo && exit -1 )
	cd translators/mgcp; $(MAKE)

callAgentNCS:
	echo

# install assumes that staging has been done, so it is equivalent to install_nostage

install_note:
	@echo "**********************************************************************"
	@echo "    Congratulations: you have successfully installed VOCAL!"
	@echo "    If this is a new installation of VOCAL, you must"
	@echo "    now run allinoneconfigure to configure your VOCAL"
	@echo "    system.  allinoneconfigure is located at"
	@echo
	@echo "      $(INSTALL_PATH)/bin/allinoneconfigure/allinoneconfigure"
	@echo
	@echo "**********************************************************************"

install: install_bin install_note

BIN = bin.$(TARGET_NAME)


installmin: installmin_ps installmin_rs installmin_fs installmin_ms

installmin_ps:
	install -m 0755 provisioning/pserver/$(BIN)/pserver /usr/local/vocal/bin

installmin_rs:
	install -m 0755 proxies/rs/$(BIN)/rs /usr/local/vocal/bin

installmin_fs:
	install -m 0755 proxies/fs/base/$(BIN)/fs /usr/local/vocal/bin

installmin_ms:
	install -m 0755 proxies/marshal/base/$(BIN)/ms /usr/local/vocal/bin

installmin_vocald:
	install -m 0755 vocald/vocald /usr/local/vocal/bin
	install -m 0755 vocald/vocalctl /usr/local/vocal/bin
	install -m 0755 provisioning/web/provactions.pm /usr/local/vocal/bin/webpages
	install -m 0755 provisioning/web/psinslib.pm /usr/local/vocal/bin/webpages
	install -m 0755 provisioning/web/provtemplates.pm /usr/local/vocal/bin/webpages
	install -m 0755 provisioning/web/webconf.cgi /usr/local/vocal/bin/webpages
	install -m 0755 install/allinoneconfigure/initalprovdata.pl /usr/local/vocal/bin/allinoneconfigure
	install -m 0755 install/allinoneconfigure/allinoneconfigure /usr/local/vocal/bin/allinoneconfigure


VERSION=$(shell cat stage/version.conf)

install_nostage:
	cd stage ; $(MAKE) install_nostage

install_bin: staging_notar install_nostage

allinoneconfigure:
	cd install/allinoneconfigure ; ./allinoneconfigure

allinone: install_bin allinoneconfigure

fs: sip proxy_base
	@( test -d proxies/fs/cpl && test -f proxies/fs/cpl/Makefile && test -d proxies/fs/base && test -f proxies/fs/base/Makefile ) || ( echo; echo; echo "please check out the proxies/fs directory first" ; echo;echo && exit -1 )
	cd proxies/fs/cpl; $(MAKE)
	cd proxies/fs/base; $(MAKE)

cpl: sip
	@( test -d proxies/fs/cpl && test -f proxies/fs/cpl/Makefile ) || ( echo; echo; echo "please check out the proxies/fs/cpl directory first" ; echo;echo && exit -1 )
	cd proxies/fs/cpl; $(MAKE)

cplfs: cpl
	@( test -d proxies/fs/base && test -f proxies/fs/base/Makefile ) || ( echo; echo; echo "please check out the proxies/fs/base directory first" ; echo;echo && exit -1 )
	cd proxies/fs/base; $(MAKE)

fsvm: sip proxy_base
	cd proxies/fs/cpl; $(MAKE)
	cd proxies/fs/vm; $(MAKE)

js: sip proxy_base heartbeat
	@( test -d proxies/fs/jtapi/server && test -f proxies/fs/jtapi/server/Makefile ) || ( echo; echo; echo "please check out the proxies/fs/jtapi/server directory first" ; echo;echo && exit -1 )
	cd proxies/fs/jtapi/server; $(MAKE)

heartbeat: util pslib
	-( test -d heartbeat && test -f heartbeat/Makefile ) && ( cd heartbeat; $(MAKE) )

hbs: util heartbeat 
	@( test -d heartbeat/hbs && test -f heartbeat/hbs/Makefile ) || ( echo; echo; echo "please check out the hbs (heartbeat server) directory first" ; echo;echo && exit -1 )
	cd heartbeat/hbs; $(MAKE)

rs: sip proxy_base
	@( test -d proxies/rs && test -f proxies/rs/Makefile ) || ( echo; echo; echo "please check out the proxies/rs directory first" ; echo;echo && exit -1 )
	cd proxies/rs; $(MAKE)

ls: ps
	@( test -d tripstack/tripcore && test -f tripstack/tripcore/Makefile ) || ( echo; echo; echo "please check out the tripstack/tripcore directory first" ; echo;echo && exit -1 )
	@( test -d tripstack/lsfunctions && test -f tripstack/lsfunctions/Makefile ) || ( echo; echo; echo "please check out the tripstack/lsfunctions directory first" ; echo;echo && exit -1 )
	@( test -d tripstack/lsserver && test -f tripstack/lsserver/Makefile ) || ( echo; echo; echo "please check out the tripstack/lsserver directory first" ; echo;echo && exit -1 )
	cd tripstack/tripcore; $(MAKE)
	cd tripstack/lsfunctions; $(MAKE)
	cd tripstack/lsserver; $(MAKE)

ls_test: ls
	@( test -d tripstack/test && test -f tripstack/test/Makefile ) || ( echo; echo; echo "please check out the tripstack/test directory first" ; echo;echo && exit -1 )
	cd tripstack/test; $(MAKE)


ms: sip proxy_base pslib cdrlib 
	@( test -d proxies/marshal/base && test -f proxies/marshal/base/Makefile ) || ( echo; echo; echo "please check out the proxies/marshal/base directory first" ; echo;echo && exit -1 )
	cd proxies/marshal/base; $(MAKE)

inet_ms: sip proxy_base pslib cdrlib libpep copsstack libOSP
	@( test -d proxies/marshal/base && test -f proxies/marshal/base/Makefile ) || ( echo; echo; echo "please check out the proxies/marshal/base directory first" ; echo;echo && exit -1 )
	@( test -d do-not-distribute ) || ( echo; echo; echo "please check out the do-not-distribute directory first" ; echo;echo && exit -1 )
	cd proxies/marshal/base; cp ms.cxx inet_ms.cxx; USE_POLICY=1; export USE_POLICY; $(MAKE)

vme: vmcp
	cd vm/vme ; $(MAKE)

vmcp:
	cd vm/vmcp ; $(MAKE)

mail: contrib contrib_imap
	cd vm/mail ; $(MAKE)

base64encoder:
	cd tools/base64encoder ; $(MAKE)

media_server: contrib vme vmcp mail base64encoder
	cd vm/media_server ; $(MAKE)

prov: provcommon dbmodules util
	cd prov/cpp/pserver; $(MAKE)

ps: util pslib
	cd provisioning/pserver ; $(MAKE)
	cd provisioning/conversion ; $(MAKE)

cdr: cdrlib contrib
	cd cdr/cdrServer ; $(MAKE)

openssl:
	-(cd do-not-distribute/openssl; $(MAKE_NOJ))

osptnep: pslib libOSP
	cd policy/enroll ; $(MAKE)

deploy:
	cd install ; $(MAKE)

siph323csgw: util contrib_equiv sip sdp proxy_base
	@( test -d translators/h323/siph323csgw && test -f translators/h323/siph323csgw/Makefile ) || ( echo; echo; echo "please check out the translators/h323 directory first" ; echo;echo && exit -1 )
	cd translators/h323/util; $(MAKE)
	cd translators/h323/info; $(MAKE)
	cd translators/h323/events; $(MAKE)
	cd translators/h323/sipevents; $(MAKE)
	cd translators/h323/gk; $(MAKE)
	cd translators/h323/proxy; $(MAKE)
	cd translators/h323/router; $(MAKE)
	cd translators/h323/translator; $(MAKE)
	cd translators/h323/siph323csgw; $(MAKE)

vocal_gk: util contrib_equiv
	@( test -d translators/h323/gk/unit_test && test -f translators/h323/gk/unit_test/Makefile ) || ( echo; echo; echo "please check out the translators/h323/gk/unit_test directory first" ; echo; echo && exit -1 )
	cd translators/h323/util; $(MAKE)
	cd translators/h323/info; $(MAKE)
	cd translators/h323/gk; $(MAKE)
	cd translators/h323/gk/unit_test; $(MAKE)

radius: util
	@( test -d radius && test -f radius/Makefile ) || ( echo; echo; echo "please check out the radius directory from CVS first" ; echo;echo && exit -1 )
	cd radius; $(MAKE) 

radius_test: radius util
	@( test -d radius/test && test -f radius/test/Makefile ) || ( echo; echo; echo "please check out the radius/test directory from CVS first" ; echo;echo && exit -1 )
	cd radius/test; $(MAKE)
#	cd radius/test; $(MAKE) test

icc: fs rs hbs ps ms cdr fsvm ua vme vmcp mail media_server staging_notar

verify:
	for i in $(VERIFY_SUBDIRS) ; do ( cd $$i && $(MAKE) verify ) ; done

verifyall:
	for i in $(VERIFY_SUBDIRS) ; do ( cd $$i && $(MAKE) verifyall ) ; done


ifeq ($(OSTYPE),Linux)
SIPSET = sipset
endif

ifeq ($(OSTYPE),FreeBSD)
SIPSET = gua
endif


# NOTE:  b2bua was after cdr in this list.  Removing for now. --Ben
all: fs rs hbs ps ms cdr radius_test vme vmcp mail media_server $(SIPSET) staging_notar 
libs: sip proxy_base cdrlib rtspstack http

extras: 

extras_broken: siph323csgw snmp netMgnt mrtg

kitchen_sink: all extras extras_broken vocal_gk ls rtspserver rtspclient loadgen http http_test sdp_test rtp_test sip_test util_test ldap2vocaluser staging_notar

releasedir:
	mkdir -p release/bin.$(TARGET_NAME)
	mkdir -p release/lib.$(TARGET_NAME)

subdirs: 
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) ; $(MAKE) install ) ; done

cleanall:
	touch $$.$$.$$.d
	find . -name 'obj.*' -print -exec /bin/rm -rf {} \;
	find . -name 'bin.*' -print -exec /bin/rm -rf {} \;
	find . -name 'lib.*' -print -exec /bin/rm -rf {} \;
	find . -name '*.o' -print -exec /bin/rm -f {} \;
	find . -name '*.a' -print -exec /bin/rm -f {} \;
	find . -name '*.d' -print -exec /bin/rm -f {} \;
	find . -name 'core' -print -exec /bin/rm -f {} \;
	find . -name '.link_host' -print -exec /bin/rm -f {} \;
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) cleanall ) ; done
	rm -rf release/bin.*/*
	rm -rf release/lib.*/*
	rm -rf release/include/*
	rm -f stage/gua.gz stage/rs stage/psClient.jar stage/cdrsrv stage/sipset
	rm -f stage/ms stage/pserver stage/fs
	rm -f build/Makefile.conf

veryclean: cleanall

cleanln:
	ln -s LICENSE .asdfasdfadsasdfadsf
	find . -type l -print -exec /bin/rm -f {} \;

clean:
	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) clean ) ; done
	- find . -name '*.d' -print -exec /bin/rm -f {} \;
	rm -f vm/vmserver/dummy_crypt.*

doc: .
	for i in $(DOCPPSUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

cppdoc:
	rm -rf cppdoc
	-mkdir cppdoc
	(cd cppdoc ; ../tools/cppdoc-linkfarm ../build/cppdoc-dirs)
	(cd cppdoc ; cppdoc -D__FAKE_CPPDOC__ -accept3slash -namespaces-global -sourcedir=fakefakedeleteme/ .)

docdir:
	cvs -d '/home/cvsroot' co doc

staging:
	cd stage; $(MAKE) tar

staging_notar:
	cd stage; $(MAKE)

release: staging

staging-clean:
	cd stage; $(MAKE) clean

rpmonly:
	cd stage; $(MAKE) rpm
pkg:
	cd stage; $(MAKE) install
	cd stage; $(MAKE) pkg
rpm: all extras rpmonly

#	find . | fgrep -vi cvs | fgrep -vi do-not-distribute | egrep -v '^\.$$' | egrep -v './bin/' | egrep -v './stage/' > list_of_files; tar cvf Vocal-Sources-1.0.tar -T list_of_files; cp Vocal-Sources-1.0.tar /usr/src/redhat/SOURCES; rpm -ba install/rpm_specs/Vocal-Binaries-1.0-1.spec; rpm -ba install/rpm_specs/Vocal-1.0-1.spec

java:
	cd provisioning/java/build; ./build.sh clean; ./build.sh

jtapi:
	cd proxies/fs/jtapi/client/build; ./build.sh clean; ./build.sh

ldap2vocaluser:
	cd tools/ldap2vocaluser; $(MAKE)

distro:
	for i in $(NO_DISTRO_SUBDIRS) ; do ( \rm -rf $$i ) ; done

loadgen: sip proxy_base
	cd loadgen/elgee ; $(MAKE)
	cd loadgen/elgeeReceiver ; $(MAKE)

loadbalance:
	cd proxies/lb ; $(MAKE)
