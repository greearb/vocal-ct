# Microsoft Developer Studio Project File - Name="base" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=base - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "base.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "base.mak" CFG="base - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "base - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "base - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\build" /I "." /I "..\..\util" /I "..\..\util\threads" /I "..\..\util\logging" /I "..\..\util\crypto" /I "..\..\util\statistics" /I "..\..\util\snmp" /I "..\..\util\signals" /I "..\..\util\behavior" /I "..\..\util\io" /I "..\..\util\services" /I "..\..\util\transport" /I "..\..\util\config" /I "..\..\util\dnssrv" /I "..\..\util\deprecated" /I "..\..\sdp" /I "..\sipstack" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\build" /I "..\..\radius" /I "..\..\util" /I "..\..\util\threads" /I "..\..\util\logging" /I "..\..\util\crypto" /I "..\..\util\statistics" /I "..\..\util\snmp" /I "..\..\util\signals" /I "..\..\util\behavior" /I "..\..\util\io" /I "..\..\util\services" /I "..\..\util\transport" /I "..\..\util\config" /I "..\..\util\dnssrv" /I "..\..\util\deprecated" /I "..\..\sdp" /I "..\sipstack" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "base - Win32 Release"
# Name "base - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BaseAuthentication.cxx
# End Source File
# Begin Source File

SOURCE=.\BaseCommandLine.cxx
# End Source File
# Begin Source File

SOURCE=.\Builder.cxx
# End Source File
# Begin Source File

SOURCE=.\CallContainer.cxx
# End Source File
# Begin Source File

SOURCE=.\CallInfo.cxx
# End Source File
# Begin Source File

SOURCE=.\DeviceEvent.cxx
# End Source File
# Begin Source File

SOURCE=.\Feature.cxx
# End Source File
# Begin Source File

SOURCE=.\HeartLessProxy.cxx
# End Source File
# Begin Source File

SOURCE=.\Operator.cxx
# End Source File
# Begin Source File

SOURCE=.\SipEvent.cxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyEvent.cxx
# End Source File
# Begin Source File

SOURCE=.\SipThread.cxx
# End Source File
# Begin Source File

SOURCE=.\State.cxx
# End Source File
# Begin Source File

SOURCE=.\StatelessWorkerThread.cxx
# End Source File
# Begin Source File

SOURCE=.\StateMachnData.cxx
# End Source File
# Begin Source File

SOURCE=.\TimerEvent.cxx
# End Source File
# Begin Source File

SOURCE=.\WorkerThread.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BaseAuthentication.hxx
# End Source File
# Begin Source File

SOURCE=.\BaseCommandLine.hxx
# End Source File
# Begin Source File

SOURCE=.\Builder.hxx
# End Source File
# Begin Source File

SOURCE=.\CallContainer.hxx
# End Source File
# Begin Source File

SOURCE=.\CallInfo.hxx
# End Source File
# Begin Source File

SOURCE=.\DeviceEvent.hxx
# End Source File
# Begin Source File

SOURCE=.\Feature.hxx
# End Source File
# Begin Source File

SOURCE=.\HeartLessProxy.hxx
# End Source File
# Begin Source File

SOURCE=.\Operator.hxx
# End Source File
# Begin Source File

SOURCE=.\SipEvent.hxx
# End Source File
# Begin Source File

SOURCE=.\SipProxyEvent.hxx
# End Source File
# Begin Source File

SOURCE=.\SipThread.hxx
# End Source File
# Begin Source File

SOURCE=.\State.hxx
# End Source File
# Begin Source File

SOURCE=.\StatelessWorkerThread.hxx
# End Source File
# Begin Source File

SOURCE=.\StateMachnData.hxx
# End Source File
# Begin Source File

SOURCE=.\TimerEvent.hxx
# End Source File
# Begin Source File

SOURCE=.\WorkerThread.hxx
# End Source File
# End Group
# End Target
# End Project
