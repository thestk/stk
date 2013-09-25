# Microsoft Developer Studio Project File - Name="syntmono" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=syntmono - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "syntmono.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "syntmono.mak" CFG="syntmono - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "syntmono - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "syntmono - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "syntmono - Win32 Release"

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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib Wsock32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "syntmono - Win32 Debug"

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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib Wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "syntmono - Win32 Release"
# Name "syntmono - Win32 Debug"
# Begin Source File

SOURCE=..\Stk\ADSR.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\ADSR.h
# End Source File
# Begin Source File

SOURCE=..\Stk\AgogoBel.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\AgogoBel.h
# End Source File
# Begin Source File

SOURCE=..\Stk\BeeThree.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\BeeThree.h
# End Source File
# Begin Source File

SOURCE=..\Stk\BiQuad.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\BiQuad.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Bowed.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Bowed.h
# End Source File
# Begin Source File

SOURCE=..\Stk\BowedBar.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\BowedBar.h
# End Source File
# Begin Source File

SOURCE=..\Stk\BowTabl.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\BowTabl.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Brass.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Brass.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Clarinet.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Clarinet.h
# End Source File
# Begin Source File

SOURCE=..\Stk\DCBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\DCBlock.h
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

SOURCE=..\Stk\DrumSynt.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\DrumSynt.h
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

SOURCE=..\Stk\Flute.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Flute.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg3.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg3.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg4.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg4.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg5.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg5.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg6.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg6.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg8.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Alg8.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Op.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FM4Op.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FMVoices.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FMVoices.h
# End Source File
# Begin Source File

SOURCE=..\Stk\FormSwep.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\FormSwep.h
# End Source File
# Begin Source File

SOURCE=..\Stk\HeavyMtl.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\HeavyMtl.h
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

SOURCE=..\Stk\JetTabl.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\JetTabl.h
# End Source File
# Begin Source File

SOURCE=..\Stk\LipFilt.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\LipFilt.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Mandolin.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Mandolin.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Marimba.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Marimba.h
# End Source File
# Begin Source File

SOURCE=..\Stk\MatWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\MatWvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\MatWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\MatWvOut.h
# End Source File
# Begin Source File

SOURCE=.\Miditabl.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Modal4.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Modal4.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Modulatr.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Modulatr.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Moog1.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Moog1.h
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

SOURCE=..\Stk\PercFlut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\PercFlut.h
# End Source File
# Begin Source File

SOURCE=..\Stk\phontabl.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Plucked.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Plucked.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Plucked2.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Plucked2.h
# End Source File
# Begin Source File

SOURCE=..\Stk\PoleZero.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\PoleZero.h
# End Source File
# Begin Source File

SOURCE=..\Stk\PRCRev.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\PRCRev.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RawWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RawWvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RawWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RawWvOut.h
# End Source File
# Begin Source File

SOURCE=..\Stk\ReedTabl.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\ReedTabl.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Reverb.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Reverb.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Rhodey.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Rhodey.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RTDuplex.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RTDuplex.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RTSoundIO.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RTSoundIO.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RTWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RTWvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\RTWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\RTWvOut.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Sampler.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Sampler.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SamplFlt.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SamplFlt.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Shakers.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Shakers.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Simple.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Simple.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SingWave.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SingWave.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SKINI11.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SKINI11.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SndWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SndWvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SndWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SndWvOut.h
# End Source File
# Begin Source File

SOURCE=..\Stk\SubNoise.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\SubNoise.h
# End Source File
# Begin Source File

SOURCE=..\Stk\swapstuf.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\swapstuf.h
# End Source File
# Begin Source File

SOURCE=.\syntmono.cpp
# End Source File
# Begin Source File

SOURCE=.\threads.cpp
# End Source File
# Begin Source File

SOURCE=.\threads.h
# End Source File
# Begin Source File

SOURCE=..\Stk\TubeBell.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\TubeBell.h
# End Source File
# Begin Source File

SOURCE=..\Stk\TwoPole.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\TwoPole.h
# End Source File
# Begin Source File

SOURCE=..\Stk\TwoZero.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\TwoZero.h
# End Source File
# Begin Source File

SOURCE=.\utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\utilities.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Vibraphn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Vibraphn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\VoicForm.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\VoicForm.h
# End Source File
# Begin Source File

SOURCE=..\Stk\VoicMang.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\VoicMang.h
# End Source File
# Begin Source File

SOURCE=..\Stk\WavWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\WavWvIn.h
# End Source File
# Begin Source File

SOURCE=..\Stk\WavWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\WavWvOut.h
# End Source File
# Begin Source File

SOURCE=..\Stk\Wurley.cpp
# End Source File
# Begin Source File

SOURCE=..\Stk\Wurley.h
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
