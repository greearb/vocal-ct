# $Id: Makefile,v 1.1 2004/05/01 04:15:28 greear Exp $

# DO NOT MOVE THIS RULE!
stage_notar:

.PHONY: stage_notar default tar tar.gz clean install_nostage install rpmrc rpmonly buildpath.tmp rpm_dirstructure

VERSION = $(shell cat version.conf)

BUILD = ../build

RPMBUILD=rpmbuild

include $(BUILD)/Makefile.pre

# figure out which apache version is here

ifndef APACHE
APACHE = $(shell rpm -q httpd apache | egrep -v '^package.*is not installed' | sed -e 's/-.*$$//;')
endif

install: stage_notar install_nostage

install_nostage:
	-mkdir -p $(INSTALL_PATH)/bin
	(cd vocal-$(VERSION)/$(BIN) ; tar cf - . ) \
		| (cd $(INSTALL_PATH)/bin ; tar xf - )
	-mkdir -p $(INSTALL_PATH)/man
	-cp -r $(INSTALL_PATH)/bin/manpages/* $(INSTALL_PATH)/man

stage_notar: filelist.conf
	-mkdir vocal-$(VERSION)
	scripts/newstage.pl $(ROOT) $(BIN) $(OS_ARCH) vocal-$(VERSION)/$(BIN) filelist.conf $(INSTALL_PATH) $(TARGET_NAME)

tar: stage_notar
	tar cf vocal-$(VERSION).tar vocal-$(VERSION)/$(BIN)

tar.gz: tar
	gzip vocal-$(VERSION).tar

PKG=VOCALb
ARCH=sparc
PKGFILE=$(PKG)-$(ARCH).pkg
pkg:
	@echo "Making Package ...." 
	@sed -e "s#INSTALLROOT#$(INSTALL_PATH)#" < Prototype.tmpl > Prototype
	@pkgmk -o -r / -d ./ -f Prototype -a $(ARCH)
	@touch $(PKGFILE)
	@pkgtrans -s ./ $(PKGFILE) $(PKG)
	@echo "Doing Clean Up for local tmp space used..."
	@rm -r .//$(PKG)
	@echo "Building Tar for pkg...."
	@tar -cvf $(PKGFILE).tar $(PKGFILE)
	@echo "Compressing the package tar ball ...."
	@compress $(PKGFILE).tar

###############
#  RPM rules  #
###############

rpm_dirstructure:
	mkdir -p rpm/SOURCES
	mkdir -p rpm/SPECS
	mkdir -p rpm/BUILD
	mkdir -p rpm/RPMS
	mkdir -p rpm/SRPMS

rpmrc:
	rpm --showrc | sed -e 's/_topdir[^}].*$$/_topdir	rpm/;' > rpmrc

RPMDIR = $(shell pwd)/rpm

ifdef BUILDROOT
#USE_BUILDROOT = --buildroot $(BUILDROOT)
USE_BUILDROOT = --define 'buildroot $(BUILDROOT)'
endif

buildpath.tmp:
	@if [ x`cat buildpath.tmp` != "x$(INSTALL_PATH)" ] ; then echo $(INSTALL_PATH) > buildpath.tmp; echo 'need to rebuild vocalbin.spec' ; fi

vocalbin.spec: vocalbin.spec.in version.conf buildpath.tmp
	@echo rebuilding vocalbin.spec
	@test "z$(APACHE)" != "z" || ( echo "no version of apache is installed! run make with APACHE=name-of-apache-rpm" ; echo "set to the name of your apache package (e.g. httpd or apache)" ; echo examples: ; echo "    make APACHE=apache rpm" ; echo "    make APACHE=httpd rpm"; exit 1 )
	@sed -e 's/@@VERSION@@/$(VERSION)/g;s#@prefix@#$(INSTALL_PATH)#;s#@apache@#$(APACHE)#' < vocalbin.spec.in > vocalbin.spec
rpmonly: rpm_dirstructure vocalbin.spec
	-rm rpm/SOURCES/vocal-$(VERSION).tar
	ln -s ../../vocal-$(VERSION).tar rpm/SOURCES
	$(RPMBUILD) $(USE_BUILDROOT) --define '_topdir $(RPMDIR)' -ba ./vocalbin.spec

rpm: tar rpmonly

######################################################################

include $(BUILD)/Makefile.post

clean:
	rm -rf vocal-$(VERSION) vocal-$(VERSION).tar

