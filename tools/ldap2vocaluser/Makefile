BUILD = ../../build
 
include $(BUILD)/Makefile.pre
 
ifeq ($(OSTYPE),Linux)
LDLIBS += -lldap -lresolv
else
LDLIBS += -lldap 
endif

ifneq ($(OSTYPE),SunOS)
LDLIBS += -llber
endif
 
PACKAGES += UTIL PTHREAD LIBXML LIBCEXT

TESTPROGRAMS = ldap2vocaluser.cxx

SRC = LDAP2Vuser.cxx 

include $(BUILD)/Makefile.post
