BUILD = ../../build

include $(BUILD)/Makefile.pre

CXXFLAGS += -DUSE_VFILTER

ifeq ($(OSTYPE),SunOS)
OS_SOLARIS=1
DEFINES += OS_SOLARIS
endif

SRC = \
	get_hostmachines.cxx\
	test_remoteinstall.cxx\
	install_executables.cxx\
	print_config.cxx\
	assign_units.cxx\
	build_uavm.cxx\
	build_provdata.cxx\
	install_provdata.cxx\
	build_apache.cxx\
	build_userdata.cxx\
	restart_boxes.cxx\
	run_userconfig.cxx\
	build_mrtg.cxx\
	deploy_global.cxx


TESTPROGRAMS = \
	deploy.cxx \

include $(BUILD)/Makefile.post
