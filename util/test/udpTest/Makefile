# $Id: Makefile,v 1.1 2004/05/01 04:15:38 greear Exp $

# $Log: Makefile,v $
# Revision 1.1  2004/05/01 04:15:38  greear
# Initial revision
#
# Revision 1.16  2000/09/19 23:58:10  wenqing
# new test
#
# Revision 1.15  2000/07/22 03:06:33  bogawa
# more test cases for udp
#
# Revision 1.14  2000/05/24 19:40:00  wenqing
# test bind after sendto
#
# Revision 1.13  2000/04/24 23:54:57  wenqing
# test with send+recv or recv+send using the same socket
#
# Revision 1.12  2000/03/30 02:01:05  wenqing
# update for testing mem leak
#
# Revision 1.11  2000/02/17 02:18:54  wenqing
# testing for multicast
#
# Revision 1.10  2000/02/15 03:43:10  wenqing
# adding files of inject udp pkts from log files.
# update for creating the log files.
#
# Revision 1.9  2000/01/25 21:24:59  wenqing
# test programs for testing connect disconnect
#
# Revision 1.8  2000/01/25 02:42:11  wenqing
# update
#
# Revision 1.7  1999/12/22 01:37:23  wenqing
# add mode(recvonly, sendonly, sendrecv) to UdpStack
#
# Revision 1.6  1999/12/21 21:51:46  bogawa
# fixes for the great renaming
#
# Revision 1.5  1999/12/21 18:21:38  eckelcu
# Modified Files:
# 	Makefile test_udp.cxx
# Improved version of test program for both linux and vxwork
#
# Revision 1.4  1999/12/18 04:28:48  eckelcu
# Modified Files:
# 	NetworkAddress.cxx UdpStack.cxx test/VThreadTest.cxx
# 	test/udpTest/Makefile test/udpTest/test_udp.cxx
# Changes for udp to work for vxworks
#
# Revision 1.3  1999/12/17 22:51:36  bogawa
# new test commits
#
# Revision 1.2  1999/12/17 01:28:14  wenqing
# add in test programs
#
# Revision 1.1  1999/12/16 20:27:26  wenqing
# add test programs
#
# Revision 1.14  1999/12/14 00:31:38  eckelcu
# Modified Files:
# 	Makefile vthread-linux.cxx VThread.h
# Added Files:
# 	VException.cxx VException.hxx VThreadException.cxx
# 	VThreadException.hxx test/Makefile test/VThreadTest.cxx
# Created base exception class, VException, and modified VThread class
# to make use of it.
# Also created a new test program which exercises the new VThread class.
#
# Revision 1.13  1999/12/06 21:28:59  bogawa
# make file fixes for rtp and util
#
# Revision 1.12  1999/12/04 22:51:26  cullen
# *** empty log message ***
#
# Revision 1.11  1999/12/04 20:26:44  cullen
# added mstring
#
# Revision 1.10  1999/11/29 19:07:00  cullen
# added SDPMedia
#
# Revision 1.9  1999/11/27 02:15:09  cullen
# added Data and DataBuff
#
# Revision 1.8  1999/11/25 01:47:27  eckelcu
# Modified Files:
# 	Makefile Fifo.cxx Fifo.h UdpStack.cxx VCondition.h VMutex.h
# 	vthread-vxworks.cxx VThread.h vtime.h
# Added Files:
# 	vtime-vxworks.cxx
# Removed Files:
# 	vos.h
# Added additional functionality to OS abstraction for sleep() and
# gettimeofday(). Modified fifo and udp so they compile for vxworks. Moved
# definitions in vos.h to VThread.h.
#
# Revision 1.7  1999/11/23 23:35:53  wenqing
# add UdpStack.cxx in SRC after ifdef-ed for vxworks
#
# Revision 1.6  1999/11/19 01:39:56  bogawa
# fixes and changes for building the SH targets successfully
#
# Revision 1.5  1999/11/19 00:38:00  bogawa
# changes for the util directory as well as using the new vmutex, etc. classes
#
# Revision 1.4  1999/11/18 00:28:49  bogawa
# Add changes to use the util directory as well as try to make it possible to take out the calls to cout.
#
# Revision 1.3  1999/11/12 22:26:50  wenqing
# Add in NetworkAddress.cxx
#
# Revision 1.2  1999/11/05 21:24:53  bogawa
#
# added util support (preliminary)
#
# Revision 1.1  1999/11/03 23:01:16  bogawa
#
# add make file for the util directory
#

BUILD = ../../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += PTHREAD UTIL

# names of programs (e.g. test programs) to be compiled
TESTPROGRAMS = \
	test_recv.cxx \
	test_send.cxx \
#         test_bind.cxx \
#        test_sendrecv_recv.cxx \
#        test_sendrecv_send.cxx \
#	test_udp_sendloop.cxx

#        test_networkaddr.cxx \
#        test_sendrecv_recv.cxx \
#        test_sendrecv_send.cxx \
#        test_udp_sendrecv_recv.cxx \
#        test_udp_sendrecv_send.cxx \
#	test_udp_send.cxx \
#	test_udp_receive.cxx
#	test_udp_mcast_rcv.cxx \
#        test_udp_mcast_snd.cxx

#        test_connect.cxx \
#        test_connect_receive.cxx \
#        test_connect_send.cxx 
#        test_createUdp.cxx \
#        test_udp.cxx \

# name of all files to be compiled (don't include header files)
SRC = \

include $(BUILD)/Makefile.post
