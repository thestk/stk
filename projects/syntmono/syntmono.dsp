# Microsoft Developer Studio Project File - Name="syntmono" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
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
!MESSAGE "syntmono - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "syntmono - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
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
# ADD CPP /nologo /MT /W3 /GX /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__OS_Win_" /YX /FD /c
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
# ADD CPP /nologo /MTd /W3 /GX /Zd /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__OS_Win_" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib dsound.lib Wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "syntmono - Win32 Release"
# Name "syntmono - Win32 Debug"
# Begin Source File

SOURCE=..\..\src\ADSR.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\ADSR.h
# End Source File
# Begin Source File

SOURCE=..\..\src\AifWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\AifWvOut.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BeeThree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\BeeThree.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BiQuad.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\BiQuad.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BlowHole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\BlowHole.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Bowed.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Bowed.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BowedBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\BowedBar.h
# End Source File
# Begin Source File

SOURCE=..\..\src\BowTabl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\BowTabl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Brass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Brass.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ByteSwap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\ByteSwap.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Clarinet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Clarinet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Controller.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Controller.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DCBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\DCBlock.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DLineA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\DLineA.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DLineL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\DLineL.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DLineN.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\DLineN.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DrumSynt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\DrumSynt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Envelope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Envelope.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Filter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Flute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Flute.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FM4Alg3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FM4Alg3.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FM4Alg4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FM4Alg4.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FM4Alg5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FM4Alg5.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FM4Alg6.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FM4Alg6.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FM4Alg8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FM4Alg8.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FM4Op.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FM4Op.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FMVoices.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FMVoices.h
# End Source File
# Begin Source File

SOURCE=..\..\src\FormSwep.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\FormSwep.h
# End Source File
# Begin Source File

SOURCE=..\..\src\HeavyMtl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\HeavyMtl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Instrmnt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Instrmnt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\JetTabl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\JetTabl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\LipFilt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\LipFilt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Mandolin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Mandolin.h
# End Source File
# Begin Source File

SOURCE=..\..\src\MatWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\MatWvIn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\MatWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\MatWvOut.h
# End Source File
# Begin Source File

SOURCE=.\miditabl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Modal4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Modal4.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ModalBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\ModalBar.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Modulatr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Modulatr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Moog1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Moog1.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Noise.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Noise.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Object.h
# End Source File
# Begin Source File

SOURCE=..\..\src\OnePole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\OnePole.h
# End Source File
# Begin Source File

SOURCE=..\..\src\OneZero.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\OneZero.h
# End Source File
# Begin Source File

SOURCE=..\..\src\PercFlut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\PercFlut.h
# End Source File
# Begin Source File

SOURCE=..\..\include\phontabl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Plucked.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Plucked.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Plucked2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Plucked2.h
# End Source File
# Begin Source File

SOURCE=..\..\src\PoleZero.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\PoleZero.h
# End Source File
# Begin Source File

SOURCE=..\..\src\PRCRev.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\PRCRev.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RawWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\RawWvIn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RawWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\RawWvOut.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ReedTabl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\ReedTabl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Reverb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Reverb.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Rhodey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Rhodey.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RtAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\RtAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RtMidi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\RtMidi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RtWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\RtWvIn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\RtWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\RtWvOut.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Sampler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Sampler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SamplFlt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\SamplFlt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Shakers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Shakers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Simple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Simple.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SingWave.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\SingWave.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SKINI11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\SKINI11.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SndWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\SndWvIn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SndWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\SndWvOut.h
# End Source File
# Begin Source File

SOURCE=..\..\src\StkError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\StkError.h
# End Source File
# Begin Source File

SOURCE=..\..\src\SubNoise.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\SubNoise.h
# End Source File
# Begin Source File

SOURCE=.\syntmono.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TubeBell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\TubeBell.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TwoPole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\TwoPole.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TwoZero.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\TwoZero.h
# End Source File
# Begin Source File

SOURCE=.\utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\utilities.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VoicForm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\VoicForm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\WavWvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\WavWvIn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\WavWvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\WavWvOut.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Wurley.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\Wurley.h
# End Source File
# Begin Source File

SOURCE=..\..\src\WvIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\WvIn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\WvOut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\WvOut.h
# End Source File
# End Target
# End Project
