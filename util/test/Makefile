# $Id: Makefile,v 1.1 2004/05/01 04:15:37 greear Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += UTIL LIBCEXT PTHREAD LIBXML OPENSSL

#        testServer.cxx \
#       ttest.cxx \
#        testClient.cxx \
#        perfDataFunctionsTest.cxx\
#	DataPerformanceTest.cxx\
#	AllDataTest.cxx\
#	AllDataPerformanceTest.cxx\
#	CopyOnWriteDataTest.cxx\
#	TimeDataCreation.cxx\
#


# names of programs (e.g. test programs) to be compiled
TESTPROGRAMS = \
	GuessNetwork.cxx \
	TlsClient.cxx \
	TlsServer.cxx \
       MemoryManagerTest.cxx\
        Base64Test.cxx\
	FileDataStoreTest.cxx\
	CountSemaphoreTest.cxx\
	CommandLineTest.cxx\
	DaemonTest.cxx\
	DatathreadTest.cxx\
	FifoSpaceTest.cxx\
	FifoTest.cxx\
	FifoTest02.cxx\
	FifoTest03.cxx\
	HostMatchTest.cxx\
	NetworkAddressTest.cxx\
	ProfileTest.cxx\
	SptrTest01.cxx\
	SptrTest02.cxx\
	SptrTest03.cxx\
	SptrTest04.cxx\
	SptrTest05.cxx\
	SptrTest06.cxx\
	VTestTest.cxx\
	VThreadTest.cxx\
	VThreadTest02.cxx\
	VThreadTest03.cxx\
	cpLogConcurrencyTest.cxx\
	cpLogTest.cxx\
	stringConcurrency.cxx\
	stringConcurrency2.cxx\
	DataMethodPerformanceTest.cxx\
	DataAllocTest.cxx\
	SptrTest07.cxx\
	SptrTest09.cxx\
	VThreadPoolConcurrencyTest.cxx\
	VThreadPoolConnectTest.cxx\
	CoreDumpTest.cxx\
	AssertSingleThreadedTest.cxx\
	DataMultiThread.cxx \
	FifoJunkTest.cxx


include $(BUILD)/Makefile.post
