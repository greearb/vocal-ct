# Microsoft Developer Studio Project File - Name="uaState" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=uaState - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uaState.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uaState.mak" CFG="uaState - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uaState - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "uaState - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uaState - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\\" /I "..\..\..\build" /I "..\..\base" /I "..\..\sipstack" /I "..\..\..\sdp\sdp2" /I "..\..\..\util" /I "..\..\..\util\snmp" /I "..\..\..\util\io" /I "..\..\..\util\logging" /I "..\..\..\util\deprecated" /I "..\..\..\util\transport" /I "..\..\..\util\threads" /I "..\..\..\contrib\win32\include" /I "..\..\..\contrib\win32\pthreads" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "uaState - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\\" /I "..\..\..\build" /I "..\..\base" /I "..\..\sipstack" /I "..\..\..\sdp\sdp2" /I "..\..\..\util" /I "..\..\..\util\snmp" /I "..\..\..\util\io" /I "..\..\..\util\logging" /I "..\..\..\util\deprecated" /I "..\..\..\util\transport" /I "..\..\..\util\threads" /I "..\..\..\contrib\win32\include" /I "..\..\..\contrib\win32\pthreads" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "uaState - Win32 Release"
# Name "uaState - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\UacStateTrying.cxx
# End Source File
# Begin Source File

SOURCE=.\UasStateTrying.cxx
# End Source File
# Begin Source File

SOURCE=.\UaState.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateEnd.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateFactory.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateFailure.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateIdle.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateInCall.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateInHold.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateRedirect.cxx
# End Source File
# Begin Source File

SOURCE=.\UaStateRinging.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\UacStateTrying.hxx
# End Source File
# Begin Source File

SOURCE=.\UasStateTrying.hxx
# End Source File
# Begin Source File

SOURCE=.\UaState.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateEnd.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateFactory.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateFailure.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateIdle.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateInCall.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateInHold.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateRedirect.hxx
# End Source File
# Begin Source File

SOURCE=.\UaStateRinging.hxx
# End Source File
# End Group
# End Target
# End Project
