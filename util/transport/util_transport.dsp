# Microsoft Developer Studio Project File - Name="util_transport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=util_transport - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "util_transport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "util_transport.mak" CFG="util_transport - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "util_transport - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "util_transport - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "util_transport - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\build" /I ".." /I "..\threads" /I "..\logging" /I "..\crypto" /I "..\statistics" /I "..\snmp" /I "..\signals" /I "..\behavior" /I "..\io" /I "..\services" /I "..\transport" /I "..\config" /I "..\dnssrv" /I "..\deprecated" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "util_transport - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\build" /I ".." /I "..\threads" /I "..\logging" /I "..\crypto" /I "..\statistics" /I "..\snmp" /I "..\signals" /I "..\behavior" /I "..\io" /I "..\services" /I "..\transport" /I "..\config" /I "..\dnssrv" /I "..\deprecated" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "_WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "util_transport - Win32 Release"
# Name "util_transport - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddressFamily.cxx
# End Source File
# Begin Source File

SOURCE=.\Connection.cxx
# End Source File
# Begin Source File

SOURCE=.\ConnectionBrokenException.cxx
# End Source File
# Begin Source File

SOURCE=.\DatagramSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\InitTransport.cxx
# End Source File
# Begin Source File

SOURCE=.\IP6Address.cxx
# End Source File
# Begin Source File

SOURCE=.\IPAddress.cxx
# End Source File
# Begin Source File

SOURCE=.\MACAddress.cxx
# End Source File
# Begin Source File

SOURCE=.\NetworkAddress.cxx
# End Source File
# Begin Source File

SOURCE=.\Poll.cxx
# End Source File
# Begin Source File

SOURCE=.\Protocol.cxx
# End Source File
# Begin Source File

SOURCE=.\ProtocolException.cxx
# End Source File
# Begin Source File

SOURCE=.\ProtocolFactory.cxx
# End Source File
# Begin Source File

SOURCE=.\Socket.cxx
# End Source File
# Begin Source File

SOURCE=.\SocketOptions.cxx
# End Source File
# Begin Source File

SOURCE=.\SocketType.cxx
# End Source File
# Begin Source File

SOURCE=.\StreamClientSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\StreamServerSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\Tcp_ClientSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\Tcp_ServerSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\TCPClientSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\TCPServerProtocol.cxx
# End Source File
# Begin Source File

SOURCE=.\TCPServerSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\TPKTClientProtocol.cxx
# End Source File
# Begin Source File

SOURCE=.\TPKTClientSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\TPKTServerProtocol.cxx
# End Source File
# Begin Source File

SOURCE=.\TPKTServerSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\TransportAddress.cxx
# End Source File
# Begin Source File

SOURCE=.\UDPSocket.cxx
# End Source File
# Begin Source File

SOURCE=.\UdpStack.cxx
# End Source File
# Begin Source File

SOURCE=.\vsock.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddressFamily.hxx
# End Source File
# Begin Source File

SOURCE=.\Connection.hxx
# End Source File
# Begin Source File

SOURCE=.\ConnectionBrokenException.hxx
# End Source File
# Begin Source File

SOURCE=.\DatagramSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\InitTransport.hxx
# End Source File
# Begin Source File

SOURCE=.\IP6Address.hxx
# End Source File
# Begin Source File

SOURCE=.\IPAddress.hxx
# End Source File
# Begin Source File

SOURCE=.\MACAddress.hxx
# End Source File
# Begin Source File

SOURCE=.\NetworkAddress.h
# End Source File
# Begin Source File

SOURCE=.\Poll.hxx
# End Source File
# Begin Source File

SOURCE=.\Protocol.hxx
# End Source File
# Begin Source File

SOURCE=.\ProtocolCreator.hxx
# End Source File
# Begin Source File

SOURCE=.\ProtocolException.hxx
# End Source File
# Begin Source File

SOURCE=.\ProtocolFactory.hxx
# End Source File
# Begin Source File

SOURCE=.\Socket.hxx
# End Source File
# Begin Source File

SOURCE=.\SocketOptions.hxx
# End Source File
# Begin Source File

SOURCE=.\SocketType.hxx
# End Source File
# Begin Source File

SOURCE=.\StreamClientSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\StreamServerSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\Tcp_ClientSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\Tcp_ServerSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\TCPClientSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\TCPServerProtocol.hxx
# End Source File
# Begin Source File

SOURCE=.\TCPServerSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\TPKTClientProtocol.hxx
# End Source File
# Begin Source File

SOURCE=.\TPKTClientSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\TPKTServerProtocol.hxx
# End Source File
# Begin Source File

SOURCE=.\TPKTServerSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\TransportAddress.hxx
# End Source File
# Begin Source File

SOURCE=.\TransportCommon.hxx
# End Source File
# Begin Source File

SOURCE=.\UDPSocket.hxx
# End Source File
# Begin Source File

SOURCE=.\UdpStack.hxx
# End Source File
# Begin Source File

SOURCE=.\vin.h
# End Source File
# Begin Source File

SOURCE=.\VNetworkException.hxx
# End Source File
# Begin Source File

SOURCE=.\vsock.hxx
# End Source File
# Begin Source File

SOURCE=.\W32McastCfg.hxx
# End Source File
# End Group
# End Target
# End Project
