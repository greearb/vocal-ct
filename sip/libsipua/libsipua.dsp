# Microsoft Developer Studio Project File - Name="libsipua" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libsipua - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libsipua.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libsipua.mak" CFG="libsipua - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libsipua - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libsipua - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libsipua - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj.nodebug.Win32.i686"
# PROP Intermediate_Dir "obj.nodebug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\build" /I ".\uaState" /I "..\base" /I "..\sipstack" /I "..\..\sdp" /I "..\..\sdp\sdp2" /I "..\..\util" /I "..\..\util\deprecated" /I "..\..\util\io" /I "..\..\util\logging" /I "..\..\util\snmp" /I "..\..\util\threads" /I "..\..\util\transport" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\pthreads" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libsipua - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj.debug.Win32.i686"
# PROP Intermediate_Dir "obj.debug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\build" /I ".\uaState" /I "..\base" /I "..\sipstack" /I "..\..\sdp" /I "..\..\sdp\sdp2" /I "..\..\util" /I "..\..\util\deprecated" /I "..\..\util\io" /I "..\..\util\logging" /I "..\..\util\snmp" /I "..\..\util\threads" /I "..\..\util\transport" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\pthreads" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "libsipua - Win32 Release"
# Name "libsipua - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BasicAgent.cxx
# End Source File
# Begin Source File

SOURCE=.\CallControl.cxx
# End Source File
# Begin Source File

SOURCE=.\CallDB.cxx
# End Source File
# Begin Source File

SOURCE=.\ContactData.cxx
# End Source File
# Begin Source File

SOURCE=.\MultiLegCallData.cxx
# End Source File
# Begin Source File

SOURCE=.\SipCallLegData.cxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionPeers.cxx
# End Source File
# Begin Source File

SOURCE=.\UaBase.cxx
# End Source File
# Begin Source File

SOURCE=.\UaClient.cxx
# End Source File
# Begin Source File

SOURCE=.\UaServer.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AccountingData.hxx
# End Source File
# Begin Source File

SOURCE=.\BasicAgent.hxx
# End Source File
# Begin Source File

SOURCE=.\CallControl.hxx
# End Source File
# Begin Source File

SOURCE=.\CallDB.hxx
# End Source File
# Begin Source File

SOURCE=.\CallTimerEvent.hxx
# End Source File
# Begin Source File

SOURCE=.\CInvalidStateException.hxx
# End Source File
# Begin Source File

SOURCE=.\ContactData.hxx
# End Source File
# Begin Source File

SOURCE=.\MultiLegCallData.hxx
# End Source File
# Begin Source File

SOURCE=.\SipCallLegData.hxx
# End Source File
# Begin Source File

SOURCE=.\SipTransactionPeers.hxx
# End Source File
# Begin Source File

SOURCE=.\UaBase.hxx
# End Source File
# Begin Source File

SOURCE=.\UaClient.hxx
# End Source File
# Begin Source File

SOURCE=.\UaDef.hxx
# End Source File
# Begin Source File

SOURCE=.\UaServer.hxx
# End Source File
# End Group
# End Target
# End Project
