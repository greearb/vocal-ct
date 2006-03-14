# $Id: Makefile,v 1.10 2006/03/14 18:05:22 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

# Fixes screwup with RH 9.0 (NPTL libraries & kernel, specifically)
#MKSHLIB_LIBS += -lstdc++ -lm -lgcc_s
MKSHLIB_LIBS += -lstdc++ -lm

TARGET_LIBRARY = libvutil

PACKAGES += OPENSSL

ifeq ($(USE_YARROW),1)
PACKAGES += YARROW
endif

ifeq ($(USE_BIND),1)
PACKAGES += BIND 
endif

PACKAGES += LIBCEXT

INCDIRS += $(UTIL_INCLUDEDIRS)


# USE_CRYPTO_RANDOM enables using /dev/urandom instead of rand() to do
# random number generation.  NOTE that the code itself also has
# __linux__ defines, so they need to be removed if you use another OS.

ifeq ($(OSTYPE),FreeBSD)
DEFINES += USE_CRYPTO_RANDOM
endif

ifeq ($(OSTYPE),Linux)
DEFINES += USE_CRYPTO_RANDOM
endif

CODE_SUBDIRS =  logging crypto statistics signals io \
                transport config dnssrv adts

SRC += \
	misc.cxx \
	BugCatcher.cxx \
	Data2.cxx \
	RandomHex.cxx \
	Sptr.cxx \
	vstring.cxx\
        Application.cxx \
        Base64.cxx \
        CryptoRandom.cxx \
        Daemon.cxx \
        DataException.cxx \
        DataStore.cxx \
        FileDataStore.cxx \
        Garbage.cxx \
        GarbageCan.cxx \
        Literal.cxx\
        MemoryManager.cxx \
        Runnable.cxx \
        SystemException.cxx \
        SystemStatus.cxx \
        TimeVal.cxx\
        Timer.cxx \
        VEnvVar.cxx \
        VException.cxx \
        VTest.cxx \
        VTime.cxx \
        Verify.cxx \
        VocalCommon.cxx \
	config/ConfFile.cxx \
        config/CommandLine.cxx \
        config/Config.cxx \
        config/GetOpt.cxx \
        config/ParsePair.cxx \
        config/parse3tuple.cxx \
        crypto/Md5.cxx \
        crypto/digest.cxx \
        crypto/vmd5.c \
        debug.cxx \
        io/FileDescriptor.cxx \
        io/FileStat.cxx \
        io/Pipe.cxx \
        io/Serialize.cxx \
        io/VFile.cxx \
        io/Writer.cxx \
        IOBufferv.cxx \
        logging/CpPriorityLog.cxx \
        logging/Log.cxx \
        logging/PriorityLog.cxx \
        logging/VLog.cxx \
        logging/cpLog.cxx \
        mstring.cxx \
        signals/SignalAction.cxx \
        signals/SignalSet.cxx \
        statistics/DurationStatistic.cxx \
        statistics/PerfDuration.cxx \
        statistics/PerfTotal.cxx \
        statistics/PerformanceDb.cxx \
        statistics/Statistic.cxx \
        statistics/StatisticsDb.cxx \
        statistics/TickCount.cxx \
        statistics/TotalStatistic.cxx \
        support.cxx \
        transport/AddressFamily.cxx \
        transport/Connection.cxx \
        transport/ConnectionBrokenException.cxx \
        transport/DatagramSocket.cxx \
        transport/IPAddress.cxx \
        transport/MACAddress.cxx \
        transport/NetworkAddress.cxx \
        transport/Poll.cxx \
        transport/Protocol.cxx \
        transport/ProtocolException.cxx \
        transport/ProtocolFactory.cxx \
        transport/Socket.cxx \
        transport/SocketOptions.cxx \
        transport/SocketType.cxx \
        transport/Tcp_ClientSocket.cxx \
        transport/Tcp_ServerSocket.cxx \
        transport/TlsConnection.cxx \
        transport/TransportAddress.cxx \
        transport/UDPSocket.cxx \
        transport/UdpStack.cxx \
        transport/vsock.cxx \
	FileStackLock.cxx

#        transport/TCPClientSocket.cxx \
#        transport/TCPServerProtocol.cxx \
#        transport/TCPServerSocket.cxx \
#        transport/TPKTClientProtocol.cxx \
#        transport/TPKTClientSocket.cxx \
#        transport/TPKTServerProtocol.cxx \
#        transport/TPKTServerSocket.cxx \
#        transport/StreamClientSocket.cxx \
#        transport/StreamServerSocket.cxx \

ifeq ($(USE_YARROW),1)
SRC += 	\
        YarrowPseudoRandom.cxx
endif

ifeq ($(OSTYPE),cygwin)
else
SRC +=  \
        Random.cxx \
        signals/SignalHandler.cxx
endif

ifeq ($(USE_BIND),1)
SRC += 	\
        dnssrv/DnsRecord.cxx \
        dnssrv/DnsResolver.cxx
endif

#for doc++
EXTRA_HEADERS=\
config/CommandLine.hxx \
config/ConfFile.hxx \
config/Config.hxx \
config/GetOpt.hxx \
config/NameValue.hxx \
config/ParsePair.hxx \
crypto/digest.hxx \
crypto/Md5.hxx \
io/FileDescriptor.hxx \
io/FileStat.hxx \
io/Pipe.hxx \
io/Serialize.hxx \
io/VFile.hxx \
io/VIoException.hxx \
io/Writer.hxx \
logging/cpLog.h \
logging/CpPriorityLog.hxx \
logging/Log.hxx \
logging/PriorityLog.hxx \
logging/VLog.hxx \
signals/SignalAction.hxx \
signals/SignalHandler.hxx \
signals/SignalSet.hxx \
statistics/Duration.hxx \
statistics/DurationStatistic.hxx \
statistics/PerfDuration.hxx \
statistics/PerformanceDb.hxx \
statistics/PerfTotal.hxx \
statistics/ScopedDuration.hxx \
statistics/Statistic.hxx \
statistics/StatisticsDb.hxx \
statistics/TickCount.hxx \
statistics/TotalStatistic.hxx \
transport/AddressFamily.hxx \
transport/ConnectionBrokenException.hxx \
transport/Connection.hxx \
transport/DatagramSocket.hxx \
transport/IPAddress.hxx \
transport/MACAddress.hxx \
transport/NetworkAddress.h \
transport/Poll.hxx \
transport/ProtocolCreator.hxx \
transport/ProtocolException.hxx \
transport/ProtocolFactory.hxx \
transport/Protocol.hxx \
transport/Socket.hxx \
transport/SocketOptions.hxx \
transport/SocketType.hxx \
transport/StreamClientSocket.hxx \
transport/StreamServerSocket.hxx \
transport/Tcp_ClientSocket.hxx \
transport/Tcp_ServerSocket.hxx \
transport/TlsConnection.hxx \
transport/TransportAddress.hxx \
transport/TransportCommon.hxx \
transport/UDPSocket.hxx \
transport/UdpStack.hxx \
transport/VNetworkException.hxx \
transport/vsock.hxx \
transport/W32McastCfg.hxx 
 
#transport/TCPClientSocket.hxx \
#transport/TCPServerProtocol.hxx \
#transport/TCPServerSocket.hxx \
#transport/TPKTClientProtocol.hxx \
#transport/TPKTClientSocket.hxx \
#transport/TPKTServerProtocol.hxx \
#transport/TPKTServerSocket.hxx \

include $(BUILD)/Makefile.post
