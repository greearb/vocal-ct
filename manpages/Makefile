# $Id: Makefile,v 1.1 2004/05/01 04:15:16 greear Exp $

MANPAGES_DIR = generated_man
HTMLPAGES_DIR = generated_html

.PHONY: default html man
MANPAGE_NAMES = \
	allinoneconfigure.8 \
	fs.1 \
	ms.1 \
	pserver.1 \
	rs.1 \
	sipset.1 \
	vmserver.1 \
	vocalconf.5 \
	vocalctl.8 \
	vocald.8 \
	vocal.8 \
	lz.1 \
	b2bua.1



MANPAGES = $(addprefix $(MANPAGES_DIR)/,$(MANPAGE_NAMES))
HTMLPAGES = $(addsuffix .html,$(basename $(addprefix $(HTMLPAGES_DIR)/,$(MANPAGE_NAMES))))

default: man html
man: $(MANPAGES)
html: $(HTMLPAGES)
$(MANPAGES_DIR)/%.1: %.xml
	docbook2manxml $< | (cd $(MANPAGES_DIR); db2x_manxml )

$(MANPAGES_DIR)/%.5: %.xml
	docbook2manxml $< | (cd $(MANPAGES_DIR); db2x_manxml )

$(MANPAGES_DIR)/%.8: %.xml
	docbook2manxml $< | (cd $(MANPAGES_DIR); db2x_manxml )

$(HTMLPAGES_DIR)/%.html: %.xml
	docbook2html -o $(HTMLPAGES_DIR) -u $<
