# Microsoft Developer Studio Project File - Name="util_statistics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=util_statistics - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "util_statistics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "util_statistics.mak" CFG="util_statistics - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "util_statistics - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "util_statistics - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "util_statistics - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\build" /I ".." /I "..\threads" /I "..\logging" /I "..\crypto" /I "..\statistics" /I "..\snmp" /I "..\signals" /I "..\behavior" /I "..\io" /I "..\services" /I "..\transport" /I "..\config" /I "..\dnssrv" /I "..\deprecated" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "util_statistics - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\build" /I ".." /I "..\threads" /I "..\logging" /I "..\crypto" /I "..\statistics" /I "..\snmp" /I "..\signals" /I "..\behavior" /I "..\io" /I "..\services" /I "..\transport" /I "..\config" /I "..\dnssrv" /I "..\deprecated" /I "..\..\contrib\win32" /I "..\..\contrib\win32\include" /I "..\..\contrib\win32\libxml" /I "..\..\contrib\win32\pthreads" /I "..\..\contrib\libcext_bsd" /I "..\..\contrib\win32\regex" /I "..\..\contrib\win32\getopt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "VOCAL_USE_DEPRECATED" /D "VOCAL_USE_GETOPT_LONG" /YX /FD /GZ /c
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

# Name "util_statistics - Win32 Release"
# Name "util_statistics - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DurationStatistic.cxx
# End Source File
# Begin Source File

SOURCE=.\PerfDuration.cxx
# End Source File
# Begin Source File

SOURCE=.\PerformanceDb.cxx
# End Source File
# Begin Source File

SOURCE=.\PerfTotal.cxx
# End Source File
# Begin Source File

SOURCE=.\Statistic.cxx
# End Source File
# Begin Source File

SOURCE=.\StatisticsDb.cxx
# End Source File
# Begin Source File

SOURCE=.\TickCount.cxx
# End Source File
# Begin Source File

SOURCE=.\TotalStatistic.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Duration.hxx
# End Source File
# Begin Source File

SOURCE=.\DurationStatistic.hxx
# End Source File
# Begin Source File

SOURCE=.\PerfDuration.hxx
# End Source File
# Begin Source File

SOURCE=.\PerformanceDb.hxx
# End Source File
# Begin Source File

SOURCE=.\PerfTotal.hxx
# End Source File
# Begin Source File

SOURCE=.\ScopedDuration.hxx
# End Source File
# Begin Source File

SOURCE=.\Statistic.hxx
# End Source File
# Begin Source File

SOURCE=.\StatisticsDb.hxx
# End Source File
# Begin Source File

SOURCE=.\TickCount.hxx
# End Source File
# Begin Source File

SOURCE=.\TotalStatistic.hxx
# End Source File
# End Group
# End Target
# End Project
