set pitch 64.0
set press 64.0

button .pretty -bitmap @TCLSpecs/bitmaps/prcFunny.xbm \
-background white -foreground black
.pretty config -bitmap @TCLSpecs/bitmaps/prc.xbm

proc myExit {} {
    puts stdout [format "ExitProgram"]
    flush stdout
    exit
}

proc noteOn {pitchVal pressVal} {
    puts stdout [format "NoteOn           0.0 1 %f %f" $pitchVal $pressVal]
    flush stdout
}

proc noteOff {pitchVal pressVal} {
    puts stdout [format "NoteOff          0.0 1 %f %f" $pitchVal $pressVal]
    flush stdout
}

proc patchChange {value} {
    global .pretty
    puts stdout [format "ProgramChange    0.0 1 %i" $value]
    if {$value==11}	{
	.pretty config -bitmap @TCLSpecs/bitmaps/KVoiceFM.xbm
    }
    if {$value==12}	{
	.pretty config -bitmap @TCLSpecs/bitmaps/KVoicForm.xbm
    }
    flush stdout
}

proc printWhatz {tag value1 value2 } {
    puts stdout [format "%s %i %f" $tag $value1 $value2]
    flush stdout
}

proc changePress {value} {
    global press
    set press $value
    puts stdout [format "AfterTouch       0.0 1 %f" $press]
    flush stdout
}

proc changePitch {value} {
    global pitch
    set pitch $value     
    puts stdout [format "PitchBend        0.0 1 %f" $pitch]
    flush stdout
}

scale .bPressure -from 0 -to 128 -length 200 \
-command {changePress } \
-activeforeground white -sliderforeground grey80\
-orient horizontal -label "Loudness (Spectral Tilt)" \
-tickinterval 32 -showvalue true -bg grey66

scale .pitch -from 0 -to 128 -length 200 \
-command {changePitch }  \
-activeforeground white -sliderforeground grey80\
-orient horizontal -label "MIDI Note Number" \
-tickinterval 32 -showvalue true -bg grey66

scale .cont1 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 2} \
-activeforeground white -sliderforeground grey80\
-orient horizontal -label "Formant Q / Voiced/Un." \
-tickinterval 32 -showvalue true -bg grey66 

scale .cont2 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 4} \
-activeforeground white -sliderforeground grey80\
-orient horizontal -label "Vowel (Bass, Tenor, Alto, Sop.)" \
-tickinterval 32 -showvalue true -bg grey66 

scale .cont3 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 11} \
-activeforeground white -sliderforeground grey80\
-orient horizontal -label "Vibrato Rate" \
-tickinterval 32 -showvalue true -bg grey66 

scale .vibrato -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 1} \
-activeforeground white -sliderforeground grey80\
-orient horizontal -label "Vibrato Amount" \
-tickinterval 32 -showvalue true -bg grey66

. config -bg grey20

frame .instChoice -bg black

radiobutton .instChoice.fm -text FMVoice -bg grey66  \
-command { patchChange 11 }
radiobutton .instChoice.form -text Formant -bg grey66 \
-command { patchChange 12 }

pack .instChoice.fm -side left -padx 5
pack .instChoice.form -side left -padx 5 -pady 10

pack .instChoice

pack .pretty -padx 5 -pady 10

pack .bPressure -padx 10 -pady 10
pack .pitch 	-padx 10 -pady 10
pack .vibrato 	-padx 10 -pady 10
pack .cont1 	-padx 10 -pady 10
pack .cont2 	-padx 10 -pady 10
pack .cont3 	-padx 10 -pady 10

frame .noteOn -bg black

button .noteOn.on -text NoteOn -bg grey66 -command { noteOn $pitch $press }
button .noteOn.off -text NoteOff -bg grey66 -command { noteOff $pitch 127.0 }
button .noteOn.exit -text "Exit Program" -bg grey66 -command myExit
pack .noteOn.on -side left -padx 5
pack .noteOn.off -side left -padx 5 -pady 10
pack .noteOn.exit -side left -padx 5 -pady 10

pack .noteOn

