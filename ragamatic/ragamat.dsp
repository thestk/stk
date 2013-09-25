# Microsoft Developer Studio Project File - Name="ragamatic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ragamatic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ragamat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ragamat.mak" CFG="ragamatic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ragamatic - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "ragamatic - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ragamatic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__OS_Win_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Wsock32.lib dsound.lib winmm.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ragamatic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__OS_Win_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Wsock32.lib dsound.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ragamatic - Win32 Release"
# Name "ragamatic - Win32 Debug"
# Begin Source File

SOURCE=..\Stk\ADSR.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\ADSR.h
# End Source File
# Begin Source File

SOURCE=..\Stk\DLineA.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\DLineA.h
# End Source File
# Begin Source File

SOURCE=..\Stk\DLineL.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\DLineL.h
# End Source File
# Begin Source File

SOURCE=..\Stk\DLineN.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\DLineN.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Envelope.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Envelope.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Filter.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Filter.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Instrmnt.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Instrmnt.h
# End Source File
# Begin Source File

SOURCE=..\Stk\JCRev.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\JCRev.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Noise.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Noise.h
# End Source File
# Begin Source File

SOURCE=..\Stk\NRev.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\NRev.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Object.h
# End Source File
# Begin Source File

SOURCE=..\Stk\OnePole.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\OnePole.h
# End Source File
# Begin Source File

SOURCE=..\Stk\OneZero.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\OneZero.h
# End Source File
# Begin Source File

SOURCE=..\Stk\PRCRev.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\PRCRev.h
# End Source File
# Begin Source File

SOURCE=.\ragamat.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RawWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RawWvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Reverb.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Reverb.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RTSoundIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RTSoundIO.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RTWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RTWvOut.h
# End Source File
# Begin Source File

SOURCE=.\Sitar1.cpp
# End Source File
# Begin Source File

SOURCE=.\Sitar1.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SKINI11.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SKINI11.h
# End Source File
# Begin Source File

SOURCE=.\StrDrone.cpp
# End Source File
# Begin Source File

SOURCE=.\StrDrone.h
# End Source File
# Begin Source File

SOURCE=..\Stk\swapstuf.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\swapstuf.h
# End Source File
# Begin Source File

SOURCE=.\Tabla.cpp
# End Source File
# Begin Source File

SOURCE=.\Tabla.h
# End Source File
# Begin Source File

SOURCE=.\threads.cpp
# End Source File
# Begin Source File

SOURCE=.\threads.h
# End Source File
# Begin Source File

SOURCE=.\VoicDrum.cpp
# End Source File
# Begin Source File

SOURCE=.\VoicDrum.h
# End Source File
# Begin Source File

SOURCE=..\Stk\WvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\WvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\WvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\WvOut.h
# End Source File
# End Target
# End Project
