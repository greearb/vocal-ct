OLD_HXX = nullEvent.hxx

GEN_HXX =  mg.hxx mgc.hxx mgEvents.hxx mgcEvents.hxx mgcpEventTypes.hxx

TEST_OBJS = examples/testMgc.o examples/testMg.o

GEN_DEPENDS = gen-package build-iface config-build.pl package-info parameter-types request-parameter-matrix response-parameter-matrix verb-data

OTHER_FILES = Makefile README

all: $(GEN_HXX)

%.hxx: $(GEN_DEPENDS)
	./gen-package
	./build-iface

test: $(TEST_OBJS)

%.o: %.cxx
	g++ -Wall -c $<

clean:
	rm $(GEN_HXX)
	rm *.o 

backup:
	mkdir -p mgcp-builder
	cp $(OLD_HXX) $(GEN_DEPENDS) $(OTHER_FILES) mgcp-builder
	tar -zcf mgcp-builder-`date +'%Y-%m-%d-%H%M'`.tar.gz mgcp-builder
