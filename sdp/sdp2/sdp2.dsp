# Microsoft Developer Studio Project File - Name="sdp2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sdp2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sdp2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sdp2.mak" CFG="sdp2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sdp2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sdp2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sdp2 - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\build" /I "..\..\util" /I "..\..\util\threads" /I "..\..\util\logging" /I "..\..\util\crypto" /I "..\..\util\statistics" /I "..\..\util\snmp" /I "..\..\util\signals" /I "..\..\util\behavior" /I "..\..\util\io" /I "..\..\util\services" /I "..\..\util\transport" /I "..\..\util\config" /I "..\..\util\dnssrv" /I "..\..\util\deprecated" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "sdp2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sdp2___Win32_Debug"
# PROP BASE Intermediate_Dir "sdp2___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj.debug.Win32.i686"
# PROP Intermediate_Dir "obj.debug.Win32.i686"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\build" /I "..\..\util" /I "..\..\util\threads" /I "..\..\util\logging" /I "..\..\util\crypto" /I "..\..\util\statistics" /I "..\..\util\snmp" /I "..\..\util\signals" /I "..\..\util\behavior" /I "..\..\util\io" /I "..\..\util\services" /I "..\..\util\transport" /I "..\..\util\config" /I "..\..\util\dnssrv" /I "..\..\util\deprecated" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "sdp2 - Win32 Release"
# Name "sdp2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Sdp2Attributes.cxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Bandwidth.cxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Connection.cxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Encryptkey.cxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Media.cxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Session.cxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Time.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Sdp2Attributes.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Bandwidth.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Connection.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Encryptkey.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Exceptions.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Externs.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Media.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Session.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2Time.hxx
# End Source File
# Begin Source File

SOURCE=.\Sdp2ZoneAdjustment.hxx
# End Source File
# End Group
# End Target
# End Project
