# $Id: Makefile,v 1.1 2004/05/01 04:15:33 greear Exp $

BUILD = ../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

# Fixes screwup with RH 9.0 (NPTL libraries & kernel, specifically)
MKSHLIB_LIBS += -lpthread -lstdc++ -lm -lgcc_s

TARGET_LIBRARY = libvutil

PACKAGES += PTHREAD OPENSSL

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

CODE_SUBDIRS =  threads logging crypto statistics snmp signals behavior io \
                services transport config dnssrv adts

SRC += \
	misc.cxx \
	Data2.cxx \
	RandomHex.cxx \
	Sptr.cxx \
	threads/Condition.cxx \
	transport/InitTransport.cxx \
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
        HostMatch.cxx \
        Literal.cxx\
        MemoryManager.cxx \
        NetworkConfig.cxx \
        Runnable.cxx \
        SystemException.cxx \
        SystemStatus.cxx \
        TimeVal.cxx\
        Timer.cxx \
        VEnvVar.cxx \
        VException.cxx \
        VFilter.cxx \
        VTest.cxx \
        VTime.cxx \
        Verify.cxx \
        VocalCommon.cxx \
        behavior/Action.cxx \
        behavior/Context.cxx \
        behavior/Observer.cxx \
        behavior/StateInterface.cxx \
        behavior/Stimulus.cxx \
        behavior/Subject.cxx \
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
        logging/CpPriorityLog.cxx \
        logging/Log.cxx \
        logging/PriorityLog.cxx \
        logging/VLog.cxx \
        logging/cpLog.cxx \
        mstring.cxx \
        services/ControlEvent.cxx \
        services/Event.cxx \
        services/FifoService.cxx \
        services/MTServiceManager.cxx \
        services/PollFifoService.cxx \
        services/ProtocolService.cxx \
        services/STServiceManager.cxx \
        services/Service.cxx \
        services/ServiceApplication.cxx \
        services/ServiceManager.cxx \
        signals/SignalAction.cxx \
        signals/SignalSet.cxx \
        snmp/AgentApi.cxx \
        snmp/AgentRegister.cxx \
        statistics/DurationStatistic.cxx \
        statistics/PerfDuration.cxx \
        statistics/PerfTotal.cxx \
        statistics/PerformanceDb.cxx \
        statistics/Statistic.cxx \
        statistics/StatisticsDb.cxx \
        statistics/TickCount.cxx \
        statistics/TotalStatistic.cxx \
        support.cxx \
        threads/AssertSingleThreaded.cxx \
        threads/Condition.cxx \
        threads/Lock.cxx \
        threads/Lockable.cxx \
        threads/Mutex.cxx \
        threads/RWMutex.cxx \
        threads/Thread.cxx \
        threads/ThreadIf.cxx \
        threads/VSemaphore.cxx \
        threads/VThreadEvent.cxx \
        threads/VThreadException.cxx \
        threads/VThreadGate.cxx \
        threads/VThreadPool.cxx \
        threads/vthread-linux.cxx \
        threads/vthread-vxworks.cxx \
        transport/AddressFamily.cxx \
        transport/Connection.cxx \
        transport/ConnectionBrokenException.cxx \
        transport/DatagramSocket.cxx \
        transport/IP6Address.cxx \
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
        transport/StreamClientSocket.cxx \
        transport/StreamServerSocket.cxx \
        transport/TCPClientSocket.cxx \
        transport/TCPServerProtocol.cxx \
        transport/TCPServerSocket.cxx \
        transport/TPKTClientProtocol.cxx \
        transport/TPKTClientSocket.cxx \
        transport/TPKTServerProtocol.cxx \
        transport/TPKTServerSocket.cxx \
        transport/Tcp_ClientSocket.cxx \
        transport/Tcp_ServerSocket.cxx \
        transport/TlsConnection.cxx \
        transport/TransportAddress.cxx \
        transport/UDPSocket.cxx \
        transport/UdpStack.cxx \
        transport/vsock.cxx \

ifeq ($(USE_YARROW),1)
SRC += 	\
        YarrowPseudoRandom.cxx
endif

ifeq ($(OSTYPE),cygwin)
else
SRC +=  \
        io/VNamedPipeFC.cxx \
        Random.cxx \
        signals/SignalHandler.cxx \
        io/VFileBrowser.cxx \
        io/VFileSystem.cxx \

endif

ifeq ($(USE_BIND),1)
SRC += 	\
        dnssrv/DnsRecord.cxx \
        dnssrv/DnsResolver.cxx
endif

#for doc++
EXTRA_HEADERS=\
behavior/Action.hxx \
behavior/Context.hxx \
behavior/EventObserver.hxx \
behavior/EventSubject.hxx \
behavior/Observer.hxx \
behavior/StateInterface.hxx \
behavior/Stimulus.hxx \
behavior/Subject.hxx \
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
io/VFileBrowser.hxx \
io/VFile.hxx \
io/VFileSystem.hxx \
io/VIoException.hxx \
io/VNamedPipeFC.hxx \
io/Writer.hxx \
logging/cpLog.h \
logging/CpPriorityLog.hxx \
logging/Log.hxx \
logging/PriorityLog.hxx \
logging/VLog.hxx \
services/ControlEvent.hxx \
services/Event.hxx \
services/FifoService.hxx \
services/MTServiceManager.hxx \
services/PollFifoService.hxx \
services/ProtocolService.hxx \
services/ServiceApplication.hxx \
services/Service.hxx \
services/ServiceManager.hxx \
services/STServiceManager.hxx \
signals/SignalAction.hxx \
signals/SignalHandler.hxx \
signals/SignalSet.hxx \
snmp/AgentApi.hxx \
snmp/AgentApiMibVars.hxx \
snmp/AgentRegister.hxx \
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
threads/AssertSingleThreaded.hxx \
threads/Condition.hxx \
threads/Lockable.hxx \
threads/Lock.hxx \
threads/Mutex.hxx \
threads/RWMutex.hxx \
threads/Thread.hxx \
threads/ThreadIf.hxx \
threads/vthread.h \
threads/VRWLock.hxx \
threads/VSemaphore.hxx \
threads/VThreadEvent.hxx \
threads/VThreadException.hxx \
threads/VThreadGate.hxx \
threads/VThread.hxx \
threads/VThreadInitException.hxx \
threads/VThreadPool.hxx \
threads/VThread_posix.hxx \
threads/VThread_vxworks.hxx \
transport/AddressFamily.hxx \
transport/ConnectionBrokenException.hxx \
transport/Connection.hxx \
transport/DatagramSocket.hxx \
transport/InitTransport.hxx \
transport/IP6Address.hxx \
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
transport/TCPClientSocket.hxx \
transport/TCPServerProtocol.hxx \
transport/Tcp_ServerSocket.hxx \
transport/TCPServerSocket.hxx \
transport/TlsConnection.hxx \
transport/TPKTClientProtocol.hxx \
transport/TPKTClientSocket.hxx \
transport/TPKTServerProtocol.hxx \
transport/TPKTServerSocket.hxx \
transport/TransportAddress.hxx \
transport/TransportCommon.hxx \
transport/UDPSocket.hxx \
transport/UdpStack.hxx \
transport/VNetworkException.hxx \
transport/vsock.hxx \
transport/W32McastCfg.hxx 
 

include $(BUILD)/Makefile.post
