# Microsoft Developer Studio Project File - Name="ffplay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ffplay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ffplay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ffplay.mak" CFG="ffplay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ffplay - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ffplay - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ffplay - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ffplay - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ffplay - Win32 Release"
# Name "ffplay - Win32 Debug"
# Begin Group "libavcodec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libavcodec\allcodecs.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\avcodec.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\dsputil.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\dsputil.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\imgconvert.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\imgconvert_template.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msrle.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\truespeech.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\truespeech_data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\utils_codec.c
# End Source File
# End Group
# Begin Group "libavformat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libavformat\allformats.c
# End Source File
# Begin Source File

SOURCE=.\libavformat\avformat.h
# End Source File
# Begin Source File

SOURCE=.\libavformat\avidec.c
# End Source File
# Begin Source File

SOURCE=.\libavformat\avio.c
# End Source File
# Begin Source File

SOURCE=.\libavformat\avio.h
# End Source File
# Begin Source File

SOURCE=.\libavformat\aviobuf.c
# End Source File
# Begin Source File

SOURCE=.\libavformat\cutils.c
# End Source File
# Begin Source File

SOURCE=.\libavformat\file.c
# End Source File
# Begin Source File

SOURCE=.\libavformat\utils_format.c
# End Source File
# End Group
# Begin Group "libavutil"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libavutil\avutil.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\bswap.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\common.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\mathematics.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\rational.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\berrno.h
# End Source File
# Begin Source File

SOURCE=.\ffplay.c
# End Source File
# End Target
# End Project
