# Microsoft Developer Studio Project File - Name="rtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rtp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rtp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rtp.mak" CFG="rtp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rtp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "rtp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rtp - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "codec" /I "..\build" /I "..\util" /I "..\util\threads" /I "..\util\logging" /I "..\util\crypto" /I "..\util\statistics" /I "..\util\snmp" /I "..\util\signals" /I "..\util\behavior" /I "..\util\io" /I "..\util\services" /I "..\util\transport" /I "..\util\config" /I "..\util\dnssrv" /I "..\util\deprecated" /I "..\contrib\win32" /I "..\contrib\win32\include" /I "..\contrib\win32\libxml" /I "..\contrib\win32\pthreads" /I "..\contrib\libcext_bsd" /I "..\contrib\win32\regex" /I "..\contrib\win32\getopt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "rtp - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "codec" /I "..\build" /I "..\util" /I "..\util\threads" /I "..\util\logging" /I "..\util\crypto" /I "..\util\statistics" /I "..\util\snmp" /I "..\util\signals" /I "..\util\behavior" /I "..\util\io" /I "..\util\services" /I "..\util\transport" /I "..\util\config" /I "..\util\dnssrv" /I "..\util\deprecated" /I "..\contrib\win32" /I "..\contrib\win32\include" /I "..\contrib\win32\libxml" /I "..\contrib\win32\pthreads" /I "..\contrib\libcext_bsd" /I "..\contrib\win32\regex" /I "..\contrib\win32\getopt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "rtp - Win32 Release"
# Name "rtp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\codec\g711.c
# End Source File
# Begin Source File

SOURCE=.\codec\g721.c
# End Source File
# Begin Source File

SOURCE=.\codec\g723_24.c
# End Source File
# Begin Source File

SOURCE=.\codec\g723_40.c
# End Source File
# Begin Source File

SOURCE=.\codec\g72x.c
# End Source File
# Begin Source File

SOURCE=.\NtpTime.cxx
# End Source File
# Begin Source File

SOURCE=.\RtcpPacket.cxx
# End Source File
# Begin Source File

SOURCE=.\RtcpReceiver.cxx
# End Source File
# Begin Source File

SOURCE=.\RtcpTransmitter.cxx
# End Source File
# Begin Source File

SOURCE=.\rtpCodec.cxx
# End Source File
# Begin Source File

SOURCE=.\RtpEvent.cxx
# End Source File
# Begin Source File

SOURCE=.\RtpEventReceiver.cxx
# End Source File
# Begin Source File

SOURCE=.\RtpPacket.cxx
# End Source File
# Begin Source File

SOURCE=.\RtpReceiver.cxx
# End Source File
# Begin Source File

SOURCE=.\RtpSession.cxx
# End Source File
# Begin Source File

SOURCE=.\rtpTools.cxx
# End Source File
# Begin Source File

SOURCE=.\RtpTransmitter.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\codec\g711.h
# End Source File
# Begin Source File

SOURCE=.\codec\g72x.h
# End Source File
# Begin Source File

SOURCE=.\NtpTime.hxx
# End Source File
# Begin Source File

SOURCE=.\Rtcp.hxx
# End Source File
# Begin Source File

SOURCE=.\RtcpPacket.hxx
# End Source File
# Begin Source File

SOURCE=.\RtcpReceiver.hxx
# End Source File
# Begin Source File

SOURCE=.\RtcpTransmitter.hxx
# End Source File
# Begin Source File

SOURCE=.\Rtp.hxx
# End Source File
# Begin Source File

SOURCE=.\rtpCodec.h
# End Source File
# Begin Source File

SOURCE=.\RtpEventReceiver.hxx
# End Source File
# Begin Source File

SOURCE=.\RtpPacket.hxx
# End Source File
# Begin Source File

SOURCE=.\RtpReceiver.hxx
# End Source File
# Begin Source File

SOURCE=.\RtpSession.hxx
# End Source File
# Begin Source File

SOURCE=.\rtpTools.hxx
# End Source File
# Begin Source File

SOURCE=.\RtpTransmitter.hxx
# End Source File
# Begin Source File

SOURCE=.\rtpTypes.h
# End Source File
# End Group
# End Target
# End Project
