# A simple Tcl/Tk example script

# Set initial control values
set pitch 64.0
set press 64.0
set outID "stdout"

# Configure main window
wm title . "A Simple GUI"
wm iconname . "simple"
. config -bg black

# Configure a "note-on" button
frame .noteOn -bg black

button .noteOn.on -text NoteOn -bg grey66 -command { noteOn $pitch $press }
pack .noteOn.on -side left -padx 5
pack .noteOn

# Configure sliders
frame .slider -bg black

scale .slider.pitch -from 0 -to 128 -length 200 \
-command {changePitch } -variable pitch \
-orient horizontal -label "MIDI Note Number" \
-tickinterval 32 -showvalue true -bg grey66

pack .slider.pitch -padx 10 -pady 10
pack .slider -side left

# Bind an X windows "close" event with the Exit routine
bind . <Destroy> +myExit

proc myExit {} {
    global pitch outID
    puts $outID [format "NoteOff          0.0 1 %f 127" $pitch ]
    flush $outID
    puts $outID [format "ExitProgram"]
    flush $outID
    close $outID
    exit
}

proc noteOn {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOn           0.0 1 %f %f" $pitchVal $pressVal]
    flush $outID
}

proc changePitch {value} {
    global outID
    puts $outID [format "PitchChange      0.0 1 %.3f" $value]
    flush $outID
}
