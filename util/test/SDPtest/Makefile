# ========================================================
#
# Makefile for gmake utility
# Anatoly Kuznetsov
#
# Usage :
# 	Put 'DEBUG=yes' as command line argument to
#	build project as Debug version
#	By default the will be compiled Release version
#
# ========================================================

PRJNAME=sdptest


SRC:= support.cxx main.cxx SDPMedia.cxx sdp.cxx cpLog.cxx debug.cxx	

CC=gcc
CPP=g++
CPPFLAGS= -DSIP
LNKFLAGS=
DEBUGDIR=Debug/
RELDIR=Release/


ifdef DEBUG
OUTDIR:=$(DEBUGDIR)
DBGFLAGS=-g
else
OUTDIR:=$(RELDIR)
endif

DEPFILE=$(OUTDIR)depend.d

OBJS+=$(patsubst %.cxx,$(OUTDIR)%.o,$(SRC))
DEPS+=$(patsubst %.cxx,%.d,$(SRC))

.SILENT: clean init

$(OUTDIR)%.o: %.cxx
	$(CPP) -c $(CPPFLAGS) $*.cxx -o $@ $(DBGFLAGS)

all: banner dep $(PRJNAME) doc

$(PRJNAME): $(OBJS) $(INCL) 
	@echo ====== Linking 
	$(CPP) $(OBJS) $(CPPFLAGS) $(DBGFLAGS) $(LNKFLAGS) -o $@
	@echo ====== Build Complete 

dep:
	@echo ====== Dependencies regeneration
	$(CPP) -MM $(CPPFLAGS) $(SRC) > $(DEPFILE)~
	cat $(DEPFILE)~ | awk '{if ( index($$0,".o:" ) !=0 ) print "$(OUTDIR)"$$0; else print $$0}' > $(DEPFILE)
	rm -f $(DEPFILE)~

rebuild: clean all

clean:
	@echo ====== Clean Up directories
	-rm -f $(OUTDIR)*

banner:
	@echo ====== Making

init:
	@echo ====== Making directories structure
	-mkdir $(DEBUGDIR) 2>/dev/null
	-mkdir $(RELDIR) 2>/dev/null

doc:
	@echo ====== Creating documentation under html directory 
#	doc++ -d html -f -a -p -H *.h *.cxx
	@echo ====== Documentation Creation Completed

-include $(DEPFILE)

