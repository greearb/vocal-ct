# $Id: Makefile,v 1.1 2004/05/01 04:15:38 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

PACKAGES += MAIL LIBIMAP VME VMCP UTIL PAM PTHREAD LIBXML OPENSSL


TESTPROGRAMS = \
	vmserver.cxx \



SRC = \
StateVM.cxx \
Configuration.cxx \
VmSession.cxx \
MenuState.cxx \
DTMFCollector.cxx \
StateLogin.cxx \
StateMain.cxx \
MailboxOptions.cxx \
StatePlayGreeting.cxx \
StatePlayOwnerGreeting.cxx \
StatePlaySync.cxx \
StateRecordGreeting.cxx \
StateRecordMessage.cxx \
StateSetup.cxx \
UserConfiguration.cxx \
cfgParser.cxx

include $(BUILD)/Makefile.post

