# Microsoft Developer Studio Project File - Name="util" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=util - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "util.mak" CFG="util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "util - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "util - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "util - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "obj.nodebug.Win32.i686"
# PROP BASE Intermediate_Dir "obj.nodebug.Win32.i686"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj.nodebug.Win32.i686"
# PROP Intermediate_Dir "obj.nodebug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\build" /I "." /I ".\threads" /I ".\logging" /I ".\crypto" /I ".\statistics" /I ".\snmp" /I ".\signals" /I ".\behavior" /I ".\io" /I ".\services" /I ".\transport" /I ".\config" /I ".\dnssrv" /I ".\deprecated" /I "..\contrib\win32" /I "..\contrib\win32\include" /I "..\contrib\win32\libxml" /I "..\contrib\win32\pthreads" /I "..\contrib\libcext_bsd" /I "..\contrib\win32\regex" /I "..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "obj.debug.Win32.i686"
# PROP BASE Intermediate_Dir "obj.debug.Win32.i686"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj.debug.Win32.i686"
# PROP Intermediate_Dir "obj.debug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\build" /I "." /I ".\threads" /I ".\logging" /I ".\crypto" /I ".\statistics" /I ".\snmp" /I ".\signals" /I ".\behavior" /I ".\io" /I ".\services" /I ".\transport" /I ".\config" /I ".\dnssrv" /I ".\deprecated" /I "..\contrib\win32" /I "..\contrib\win32\include" /I "..\contrib\win32\libxml" /I "..\contrib\win32\pthreads" /I "..\contrib\libcext_bsd" /I "..\contrib\win32\regex" /I "..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "util - Win32 Release"
# Name "util - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Application.cxx
# End Source File
# Begin Source File

SOURCE=.\Base64.cxx
# End Source File
# Begin Source File

SOURCE=.\CryptoRandom.cxx
# End Source File
# Begin Source File

SOURCE=.\CWBuffer.cxx
# End Source File
# Begin Source File

SOURCE=.\Daemon.cxx
# End Source File
# Begin Source File

SOURCE=.\Data2.cxx
# End Source File
# Begin Source File

SOURCE=.\DataException.cxx
# End Source File
# Begin Source File

SOURCE=.\DataStore.cxx
# End Source File
# Begin Source File

SOURCE=.\debug.cxx
# End Source File
# Begin Source File

SOURCE=.\FileDataStore.cxx
# End Source File
# Begin Source File

SOURCE=.\Garbage.cxx
# End Source File
# Begin Source File

SOURCE=.\GarbageCan.cxx
# End Source File
# Begin Source File

SOURCE=.\HostMatch.cxx
# End Source File
# Begin Source File

SOURCE=.\Literal.cxx
# End Source File
# Begin Source File

SOURCE=.\MemoryManager.cxx
# End Source File
# Begin Source File

SOURCE=.\misc.cxx
# End Source File
# Begin Source File

SOURCE=.\mstring.cxx
# End Source File
# Begin Source File

SOURCE=.\NetworkConfig.cxx
# End Source File
# Begin Source File

SOURCE=.\Random.cxx
# End Source File
# Begin Source File

SOURCE=.\RandomHex.cxx
# End Source File
# Begin Source File

SOURCE=.\Runnable.cxx
# End Source File
# Begin Source File

SOURCE=.\Sptr.cxx
# End Source File
# Begin Source File

SOURCE=.\support.cxx
# End Source File
# Begin Source File

SOURCE=.\SystemException.cxx
# End Source File
# Begin Source File

SOURCE=.\SystemStatus.cxx
# End Source File
# Begin Source File

SOURCE=.\Timer.cxx
# End Source File
# Begin Source File

SOURCE=.\TimeVal.cxx
# End Source File
# Begin Source File

SOURCE=.\VEnvVar.cxx
# End Source File
# Begin Source File

SOURCE=.\Verify.cxx
# End Source File
# Begin Source File

SOURCE=.\VException.cxx
# End Source File
# Begin Source File

SOURCE=.\VFilter.cxx
# End Source File
# Begin Source File

SOURCE=.\VocalCommon.cxx
# End Source File
# Begin Source File

SOURCE=.\VTest.cxx
# End Source File
# Begin Source File

SOURCE=.\VTime.cxx
# End Source File
# Begin Source File

SOURCE=.\VXmlParser.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Application.hxx
# End Source File
# Begin Source File

SOURCE=.\atomic.h
# End Source File
# Begin Source File

SOURCE=.\Base64.hxx
# End Source File
# Begin Source File

SOURCE=.\BugCatcher.hxx
# End Source File
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\CountSemaphore.hxx
# End Source File
# Begin Source File

SOURCE=.\CryptoRandom.hxx
# End Source File
# Begin Source File

SOURCE=.\CWBuffer.hxx
# End Source File
# Begin Source File

SOURCE=.\Daemon.hxx
# End Source File
# Begin Source File

SOURCE=.\Data.hxx
# End Source File
# Begin Source File

SOURCE=.\Data2.hxx
# End Source File
# Begin Source File

SOURCE=.\DataException.hxx
# End Source File
# Begin Source File

SOURCE=.\DataStore.hxx
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\Fifo.h
# End Source File
# Begin Source File

SOURCE=.\FifoBase.h
# End Source File
# Begin Source File

SOURCE=.\FileDataStore.hxx
# End Source File
# Begin Source File

SOURCE=.\Garbage.hxx
# End Source File
# Begin Source File

SOURCE=.\GarbageCan.hxx
# End Source File
# Begin Source File

SOURCE=.\HostMatch.hxx
# End Source File
# Begin Source File

SOURCE=.\Http_def.h
# End Source File
# Begin Source File

SOURCE=.\Literal.hxx
# End Source File
# Begin Source File

SOURCE=.\MemoryManager.hxx
# End Source File
# Begin Source File

SOURCE=.\misc.hxx
# End Source File
# Begin Source File

SOURCE=.\mstring.hxx
# End Source File
# Begin Source File

SOURCE=.\NetworkConfig.hxx
# End Source File
# Begin Source File

SOURCE=.\NonCopyable.hxx
# End Source File
# Begin Source File

SOURCE=.\PollFifo.hxx
# End Source File
# Begin Source File

SOURCE=.\PthreadFifo.h
# End Source File
# Begin Source File

SOURCE=.\Random.hxx
# End Source File
# Begin Source File

SOURCE=.\RandomHex.hxx
# End Source File
# Begin Source File

SOURCE=.\Runnable.hxx
# End Source File
# Begin Source File

SOURCE=.\Singleton.hxx
# End Source File
# Begin Source File

SOURCE=.\Sptr.hxx
# End Source File
# Begin Source File

SOURCE=.\SptrRefCount.hxx
# End Source File
# Begin Source File

SOURCE=.\support.hxx
# End Source File
# Begin Source File

SOURCE=.\SystemException.hxx
# End Source File
# Begin Source File

SOURCE=.\SystemStatus.hxx
# End Source File
# Begin Source File

SOURCE=.\ThreadSafeBuffer.hxx
# End Source File
# Begin Source File

SOURCE=.\TimeFunc.hxx
# End Source File
# Begin Source File

SOURCE=.\Timer.h
# End Source File
# Begin Source File

SOURCE=.\TimerContainer.hxx
# End Source File
# Begin Source File

SOURCE=.\TimerEntry.hxx
# End Source File
# Begin Source File

SOURCE=.\TimeTracker.hxx
# End Source File
# Begin Source File

SOURCE=.\TimeVal.hxx
# End Source File
# Begin Source File

SOURCE=.\VBadDataException.hxx
# End Source File
# Begin Source File

SOURCE=.\VEnvVar.hxx
# End Source File
# Begin Source File

SOURCE=.\Verify.hxx
# End Source File
# Begin Source File

SOURCE=.\VException.hxx
# End Source File
# Begin Source File

SOURCE=.\VFilter.hxx
# End Source File
# Begin Source File

SOURCE=.\VFunctor.hxx
# End Source File
# Begin Source File

SOURCE=.\VMissingDataException.hxx
# End Source File
# Begin Source File

SOURCE=.\VocalCommon.hxx
# End Source File
# Begin Source File

SOURCE=.\VTest.hxx
# End Source File
# Begin Source File

SOURCE=.\VTime.hxx
# End Source File
# Begin Source File

SOURCE=.\vtypes.h
# End Source File
# Begin Source File

SOURCE=.\VXmlParser.hxx
# End Source File
# End Group
# End Target
# End Project
