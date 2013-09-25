# Tcl/Tk Demo GUI for the Synthesis Toolkit (STK)
# by Gary P. Scavone, CCRMA, Stanford University, 1999.

# Set initial control values
set pitch 64.0
set press 64.0
set velocity 96.0
set cont1 0.0
set cont2 10.0
set cont4 20.0
set cont11 64.0
set cont44 24.0
set outID "stdout"
set commtype "stdout"
set patchnum 0
set oldpatch 0
set temp 0

# Configure main window
wm title . "STK Demo GUI"
wm iconname . "demo"
. config -bg black

# Configure "communications" menu
menu .menu -tearoff 0
menu .menu.communication -tearoff 0 
.menu add cascade -label "Communication" -menu .menu.communication \
    -underline 0
.menu.communication add radio -label "Console" -variable commtype \
    -value "stdout" -command { setComm }
.menu.communication add radio -label "Socket" -variable commtype \
    -value "socket" -command { setComm }

# Configure instrument change menu
menu .menu.instrument -tearoff 0
.menu add cascade -label "Instrument" -menu .menu.instrument \
    -underline 0
.menu.instrument add radio -label "Clarinet" -variable patchnum \
    -value 0 -command { patchChange $patchnum }
.menu.instrument add radio -label "Flute" -variable patchnum \
    -value 1 -command { patchChange $patchnum }
.menu.instrument add radio -label "Brass" -variable patchnum \
    -value 2 -command { patchChange $patchnum }
.menu.instrument add radio -label "Bowed String" -variable patchnum \
    -value 3 -command { patchChange $patchnum }
.menu.instrument add radio -label "BlowHole" -variable patchnum \
    -value 4 -command { patchChange $patchnum }
.menu.instrument add radio -label "Plucked String" -variable patchnum \
    -value 5 -command { patchChange $patchnum }
.menu.instrument add radio -label "Mandolin" -variable patchnum \
    -value 6 -command { patchChange $patchnum }
.menu.instrument add radio -label "Rhodey" -variable patchnum \
    -value 7 -command { patchChange $patchnum }
.menu.instrument add radio -label "Wurley" -variable patchnum \
    -value 8 -command { patchChange $patchnum }
.menu.instrument add radio -label "Tubular Bell" -variable patchnum \
    -value 9 -command { patchChange $patchnum }
.menu.instrument add radio -label "Heavy Metal" -variable patchnum \
    -value 10 -command { patchChange $patchnum }
.menu.instrument add radio -label "Percussive Flute" -variable patchnum \
    -value 11 -command { patchChange $patchnum }
.menu.instrument add radio -label "B3 Organ" -variable patchnum \
    -value 12 -command { patchChange $patchnum }
.menu.instrument add radio -label "Moog" -variable patchnum \
    -value 13 -command { patchChange $patchnum }
.menu.instrument add radio -label "FM Voices" -variable patchnum \
    -value 14 -command { patchChange $patchnum }
.menu.instrument add radio -label "Drum Kit" -variable patchnum \
    -value 16 -command { patchChange $patchnum }
.menu.instrument add radio -label "Maraca" -variable patchnum \
    -value 17 -command { patchChange $patchnum }
.menu.instrument add radio -label "Sekere" -variable patchnum \
    -value 18 -command { patchChange $patchnum }
.menu.instrument add radio -label "Cabasa" -variable patchnum \
    -value 19 -command { patchChange $patchnum }
.menu.instrument add radio -label "Bamboo" -variable patchnum \
    -value 20 -command { patchChange $patchnum }
.menu.instrument add radio -label "Waterdrop" -variable patchnum \
    -value 21 -command { patchChange $patchnum }
.menu.instrument add radio -label "Tambourine" -variable patchnum \
    -value 22 -command { patchChange $patchnum }
.menu.instrument add radio -label "Sleigh Bell" -variable patchnum \
    -value 23 -command { patchChange $patchnum }
.menu.instrument add radio -label "Guiro" -variable patchnum \
    -value 24 -command { patchChange $patchnum }
.menu.instrument add radio -label "Sticks" -variable patchnum \
    -value 25 -command { patchChange $patchnum }
.menu.instrument add radio -label "Crunch" -variable patchnum \
    -value 26 -command { patchChange $patchnum }
.menu.instrument add radio -label "Wrench" -variable patchnum \
    -value 27 -command { patchChange $patchnum }
.menu.instrument add radio -label "SandPaper" -variable patchnum \
    -value 28 -command { patchChange $patchnum }
.menu.instrument add radio -label "CokeCan" -variable patchnum \
    -value 29 -command { patchChange $patchnum }
.menu.instrument add radio -label "Marimba" -variable patchnum \
    -value 30 -command { patchChange $patchnum }
.menu.instrument add radio -label "Vibraphone" -variable patchnum \
    -value 31 -command { patchChange $patchnum }
.menu.instrument add radio -label "Agogo Bell" -variable patchnum \
    -value 32 -command { patchChange $patchnum }
.menu.instrument add radio -label "Wood 1" -variable patchnum \
    -value 33 -command { patchChange $patchnum }
.menu.instrument add radio -label "Reso" -variable patchnum \
    -value 34 -command { patchChange $patchnum }
.menu.instrument add radio -label "Wood 2" -variable patchnum \
    -value 35 -command { patchChange $patchnum }
.menu.instrument add radio -label "Beats" -variable patchnum \
    -value 36 -command { patchChange $patchnum }

. configure -menu .menu

# Configure bitmap display
if {[file isdirectory bitmaps]} {
    set bitmappath bitmaps
} else {
    set bitmappath tcl/bitmaps
}

button .pretty -bitmap @$bitmappath/Klar.xbm \
    -background white -foreground black
pack .pretty -padx 5 -pady 10

# Configure "note-on" buttons
frame .noteOn -bg black

button .noteOn.on -text NoteOn -bg grey66 -command { noteOn $pitch $press }
button .noteOn.off -text NoteOff -bg grey66 -command { noteOff $pitch 127.0 }
button .noteOn.exit -text "Exit Program" -bg grey66 -command myExit
pack .noteOn.on -side left -padx 5
pack .noteOn.off -side left -padx 5 -pady 10
pack .noteOn.exit -side left -padx 5 -pady 10

pack .noteOn

# Configure reverb slider
frame .reverb -bg black

scale .reverb.mix -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    -1.0  1 " 44} \
-orient horizontal -label "Reverb Mix" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont44

pack .reverb.mix -padx 10 -pady 10
pack .reverb

# Configure sliders
frame .left -bg black
frame .right -bg black

scale .left.bPressure -from 0 -to 128 -length 200 \
-command {changePress } -variable press \
-orient horizontal -label "Breath Pressure" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.pitch -from 0 -to 128 -length 200 \
-command {changePitch } -variable pitch \
-orient horizontal -label "MIDI Note Number" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.cont2 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    -1.0  1 " 2} \
-orient horizontal -label "Reed Stiffness" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont2

scale .right.cont4 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    -1.0  1 " 4} \
-orient horizontal -label "Breath Noise" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont4

scale .right.cont11 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    -1.0  1 " 11} \
-orient horizontal -label "Vibrato Rate" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont11 

scale .right.cont1 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    -1.0  1 " 1} \
-orient horizontal -label "Vibrato Amount" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont1

pack .left.bPressure -padx 10 -pady 10
pack .left.pitch -padx 10 -pady 10
pack .left.cont2 -padx 10 -pady 10
pack .right.cont4 -padx 10 -pady 10
pack .right.cont11 -padx 10 -pady 10
pack .right.cont1 -padx 10 -pady 10

pack .left -side left
pack .right -side right

# DrumKit popup window
set p .drumwindow

proc myExit {} {
    global pitch outID
    puts $outID [format "ExitProgram"]
    flush $outID
    close $outID
    exit
}

proc noteOn {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOn           -1.0  1  %3.2f  %3.2f" $pitchVal $pressVal]
    flush $outID
}

proc noteOff {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOff          -1.0  1  %3.2f %3.2f" $pitchVal $pressVal]
    flush $outID
}

proc patchChange {value} {
    global outID bitmappath cont1 cont2 cont4 cont11 oldpatch press pitch temp
    if {$value!=$oldpatch} {
        if {$value < 17} {
            puts $outID [format "ProgramChange    -1.0  1  %2i" $value]
            flush $outID
        }
        if {($value > 16 && $value < 30) && ($oldpatch < 17 || $oldpatch > 29)} {
            puts $outID [format "ProgramChange    -1.0  1  17"]
            flush $outID
        }
        if {$value > 29 && $oldpatch < 30} {
            puts $outID [format "ProgramChange    -1.0  1  19"]
            flush $outID
        }
        # This stuff below sets up the correct bitmaps, slider labels, and control
        # parameters.
        if {$value==0}	{ # Clarinet
            .pretty config -bitmap @$bitmappath/Klar.xbm
            .left.bPressure config -state normal -label "Breath Pressure"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Reed Stiffness"
            .right.cont4 config -state normal -label "Breath Noise"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
            set cont1 20.0
            set cont2 64.0
            set cont4 20.0
            set cont11 64.0
            printWhatz "ControlChange    -1.0  1 " 1  $cont1
            printWhatz "ControlChange    -1.0  1 " 2  $cont2
            printWhatz "ControlChange    -1.0  1 " 4  $cont4
            printWhatz "ControlChange    -1.0  1 " 11 $cont11
        }
        if {$value==1}	{ # Flute
            .pretty config -bitmap @$bitmappath/KFloot.xbm
            .left.bPressure config -state normal -label "Breath Pressure"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Embouchure Adjustment"
            .right.cont4 config -state normal -label "Breath Noise"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
            set cont1 20.0
            set cont2 64.0
            set cont4 20.0
            set cont11 64.0
            printWhatz "ControlChange    -1.0  1 " 1  $cont1
            printWhatz "ControlChange    -1.0  1 " 2  $cont2
            printWhatz "ControlChange    -1.0  1 " 4  $cont4
            printWhatz "ControlChange    -1.0  1 " 11 $cont11
        }
        if {$value==2}	{ # Brass
            .pretty config -bitmap @$bitmappath/KHose.xbm
            .left.bPressure config -state normal -label "Breath Pressure"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Lip Adjustment"
            .right.cont4 config -state normal -label "Slide Length"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
            set cont1 0.0
            set cont2 64.0
            set cont4 20.0
            set cont11 64.0
            printWhatz "ControlChange    -1.0  1 " 1  $cont1
            printWhatz "ControlChange    -1.0  1 " 2  $cont2
            printWhatz "ControlChange    -1.0  1 " 4  $cont4
            printWhatz "ControlChange    -1.0  1 " 11 $cont11
        }

        if {$value==3}	{ # Bowed String
            .pretty config -bitmap @$bitmappath/KFiddl.xbm
            .left.bPressure config -state normal -label "Volume"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Bow Pressure"
            .right.cont4 config -state normal -label "Bow Position"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
            set cont1 4.0
            set cont2 64.0
            set cont4 64.0
            set cont11 64.0
            printWhatz "ControlChange    -1.0  1 " 1  $cont1
            printWhatz "ControlChange    -1.0  1 " 2  $cont2
            printWhatz "ControlChange    -1.0  1 " 4  $cont4
            printWhatz "ControlChange    -1.0  1 " 11 $cont11
        }
        if {$value==4}	{ # BlowHole
            .pretty config -bitmap @$bitmappath/Klar.xbm
            .left.bPressure config -state normal -label "Breath Pressure"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Reed Stiffness"
            .right.cont4 config -state normal -label "Breath Noise"
            .right.cont11 config -state normal -label "Tonehole Openness"
            .right.cont1 config -state normal -label "Register Vent Openness"
            set cont1 0.0
            set cont2 64.0
            set cont4 20.0
            set cont11 0.0
            printWhatz "ControlChange    -1.0  1 " 1  $cont1
            printWhatz "ControlChange    -1.0  1 " 2  $cont2
            printWhatz "ControlChange    -1.0  1 " 4  $cont4
            printWhatz "ControlChange    -1.0  1 " 11 $cont11
        }
        if {$value==5}	{ # Yer Basic Pluck
            .pretty config -bitmap @$bitmappath/KPluk.xbm
            .left.bPressure config -state disabled -label "Disabled"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state disabled -label "Disabled"
            .right.cont4 config -state disabled -label "Disabled"
            .right.cont11 config -state disabled -label "Disabled"
            .right.cont1 config -state disabled -label "Disabled"
        }
        if {$value==6}	{ # Mandolin
            .pretty config -bitmap @$bitmappath/KPluk.xbm
            .left.bPressure config -state normal -label "Microphone Position"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Mandolin Body Size"
            .right.cont4 config -state normal -label "Pick Position"
            .right.cont11 config -state normal -label "String Sustain"
            .right.cont1 config -state normal -label "String Detune"
            set cont1 10.0
            set cont2 64.0
            set cont4 64.0
            set cont11 96.0
            set press 64.0
            printWhatz "ControlChange    -1.0  1 " 1  $cont1
            printWhatz "ControlChange    -1.0  1 " 2  $cont2
            printWhatz "ControlChange    -1.0  1 " 4  $cont4
            printWhatz "ControlChange    -1.0  1 " 11 $cont11
            changePress $press
        }
        if {$value>=7 && $value <=12}	{ # FM Instruments
            .pretty config -bitmap @$bitmappath/KFMod.xbm
            .left.bPressure config -state normal -label "Modulator ADSR Amplitudes"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Modulator Index"
            .right.cont4 config -state normal -label "FM Pair Crossfader"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
        }
        if {$value==13}	{ # Moog
            .pretty config -bitmap @$bitmappath/prcFunny.xbm
            .left.bPressure config -state normal -label "Volume"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Filter Q"
            .right.cont4 config -state normal -label "Filter Sweep Rate"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
        }
        if {$value==14}	{ # FM Voices
            .pretty config -bitmap @$bitmappath/KVoiceFM.xbm
            .left.bPressure config -state normal -label "Loudness (Spectral Tilt)"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Formant Q"
            .right.cont4 config -state normal -label "Vowel (Bass, Tenor, Alto, Sop.)"
            .right.cont11 config -state normal -label "Vibrato Rate"
            .right.cont1 config -state normal -label "Vibrato Amount"
        }
        if {$value==16}	{ # Drum Kit
            # Given the vastly different interface for the Drum Kit, we open
            # a new GUI popup window with the appropriate controls and lock
            # focus there until the user hits the "Close" button.  We then
            # switch back to the Clarinet (0) instrument.
            global p
            toplevel $p
            wm title $p "STK DrumKit"
            $p config -bg black
            wm resizable $p 0 0
            grab $p
            scale $p.velocity -from 0 -to 128 -length 100 \
                -variable velocity -orient horizontal -label "Velocity" \
                -tickinterval 64 -showvalue true -bg grey66
            pack $p.velocity -pady 5 -padx 5
            # Configure buttons
            frame $p.buttons -bg black
            frame $p.buttons.left -bg black
            frame $p.buttons.right -bg black

            button $p.buttons.left.bass -text Bass  -bg grey66 \
                -command { playDrum 36 } -width 7
            button $p.buttons.left.snare -text Snare  -bg grey66 \
                -command { playDrum 38 } -width 7
            button $p.buttons.left.tomlo -text LoTom  -bg grey66 \
                -command { playDrum 41 } -width 7
            button $p.buttons.left.tommid -text MidTom  -bg grey66 \
                -command { playDrum 45 } -width 7
            button $p.buttons.left.tomhi -text HiTom  -bg grey66 \
                -command { playDrum 50 } -width 7
            button $p.buttons.left.homer -text Homer -bg grey66 \
                -command { playDrum 90 } -width 7
            button $p.buttons.right.hat -text Hat  -bg grey66 \
                -command { playDrum 42 } -width 7
            button $p.buttons.right.ride -text Ride -bg grey66 \
                -command { playDrum 46 } -width 7
            button $p.buttons.right.crash -text Crash -bg grey66 \
                -command { playDrum 49 } -width 7
            button $p.buttons.right.cowbel -text CowBel -bg grey66 \
                -command { playDrum 56 } -width 7
            button $p.buttons.right.tamb -text Tamb -bg grey66 \
                -command { playDrum 54 } -width 7
            button $p.buttons.right.homer -text Homer -bg grey66 \
                -command { playDrum 90 } -width 7

            pack $p.buttons.left.bass -pady 5
            pack $p.buttons.left.snare -pady 5
            pack $p.buttons.left.tomlo -pady 5
            pack $p.buttons.left.tommid -pady 5
            pack $p.buttons.left.tomhi -pady 5
            pack $p.buttons.left.homer -pady 5
            pack $p.buttons.right.hat -pady 5
            pack $p.buttons.right.ride -pady 5
            pack $p.buttons.right.crash -pady 5
            pack $p.buttons.right.cowbel -pady 5
            pack $p.buttons.right.tamb -pady 5
            pack $p.buttons.right.homer -pady 5

            pack $p.buttons.left -side left -pady 5 -padx 5
            pack $p.buttons.right -side right -pady 5 -padx 5
            pack $p.buttons -padx 5 -pady 10

            set temp $oldpatch
            button $p.close -text "Close" -bg grey66 \
								-command { destroy $p
                    set patchnum $temp
                    patchChange $patchnum}
            pack $p.close -side bottom -padx 5 -pady 10
        }
        if {$value>=17 && $value <=29}	{ # Shakers
            .pretty config -bitmap @$bitmappath/phism.xbm
            .left.bPressure config -state normal -label "Shake Energy"
            .left.pitch config -state disabled -label "Disabled"
            .left.cont2 config -state disabled -label "Disabled"
            .right.cont4 config -state normal -label "Number of Objects"
            .right.cont11 config -state normal -label "(<--High) Damping (Low-->)"
            .right.cont1 config -state normal -label "Resonance Center Frequency"
            switch $value {
                17 {
                    set pitch 0
                    .pretty config -bitmap @$bitmappath/maraca.xbm
                }
                18 {set pitch 2}
                19 {
                    set pitch 1
                    .pretty config -bitmap @$bitmappath/cabasa.xbm
                }
                20 {
                    set pitch 5
                    .pretty config -bitmap @$bitmappath/bamboo.xbm
                }
                21 {set pitch 4}
                22 {
                    set pitch 6
                    .pretty config -bitmap @$bitmappath/tambourine.xbm
                }
                23 {
                    set pitch 7
                    .pretty config -bitmap @$bitmappath/sleighbell.xbm
                }
                24 {
                    set pitch 3
                    .pretty config -bitmap @$bitmappath/guiro.xbm
                }
                25 {set pitch 8}
                26 {set pitch 9}
                27 {
                    set pitch 10
                    .pretty config -bitmap @$bitmappath/rachet.xbm
                }
                28 {set pitch 11}
                29 {set pitch 12}
            }
            set cont1 64.0
            set cont2 64.0
            set cont4 64.0
            set cont11 64.0
            puts $outID [format "NoteOn          -1.0  1  %3.2f  %3.2f" $pitch $press]
            flush $outID
        }
        if {$value>=30 && $value<=36 }	{ # Modal Instruments
            .pretty config -bitmap @$bitmappath/KModal.xbm
            .left.bPressure config -state normal -label "Strike Vigor"
            .left.pitch config -state normal -label "MIDI Note Number"
            .left.cont2 config -state normal -label "Stick Hardness"
            .right.cont4 config -state normal -label "Stick Position"
            .right.cont11 config -state normal -label "Vibrato Frequency"
            .right.cont1 config -state normal -label "Direct Stick Mix"
            switch $value {
                30 {set preset 0}
                31 {set preset 1}
                32 {set preset 2}
                33 {set preset 3}
                34 {set preset 4}
                35 {set preset 5}
                36 {set preset 6}
            }
            set cont1 0.0
            set cont2 64.0
            set cont4 64.0
            set cont11 64.0
            puts $outID [format "ControlChange    -1.0  1  16  %3.2f" $preset]
            flush $outID
        }
        set oldpatch $value
    }
}

#bind all <KeyPress> {
bind . <KeyPress> {
    noteOn $pitch $press 
}

# Bind an X windows "close" event with the Exit routine
bind . <Destroy> +myExit

proc playDrum {value}	{
    global velocity outID
    puts $outID [format "NoteOn           -1.0  1  %3i  %3.2f" $value $velocity]
    flush $outID
}

proc printWhatz {tag value1 value2 } {
    global outID
    puts $outID [format "%s %2i  %3.2f" $tag $value1 $value2]
    flush $outID
}

proc changePress {value} {
    global outID patchnum
    if { $patchnum == 6} {
        puts $outID [format "ControlChange    -1.0  1  411  %3.2f" $value]
    } else {
        puts $outID [format "AfterTouch       -1.0  1  %3.2f" $value]
    }
    flush $outID
}

proc changePitch {value} {
    global outID
    puts $outID [format "PitchBend        -1.0  1  %3.2f" $value]
    flush $outID
}

# Socket connection procedure
set d .socketdialog

proc setComm {} {
		global outID commtype d
		if {$commtype == "stdout"} {
				if { [string compare "stdout" $outID] } {
						set i [tk_dialog .dialog "Break Socket Connection?" {You are about to break an existing socket connection ... is this what you want to do?} "" 0 Cancel OK]
						switch $i {
								0 {set commtype "socket"}
								1 {close $outID
								   set outID "stdout"}
						}
				}
		} elseif { ![string compare "stdout" $outID] } {
				set sockport 2001
        set sockhost localhost
				toplevel $d
				wm title $d "STK Client Socket Connection"
				wm resizable $d 0 0
				grab $d
				label $d.message -text "Specify a socket host and port number below (if different than the STK defaults shown) and then click the \"Connect\" button to invoke a socket-client connection attempt to the STK socket server." \
								-background white -font {Helvetica 10 bold} \
								-wraplength 3i -justify left
				frame $d.sockhost
				entry $d.sockhost.entry -width 15
				label $d.sockhost.text -text "Socket Host:" \
								-font {Helvetica 10 bold}
				frame $d.sockport
				entry $d.sockport.entry -width 15
				label $d.sockport.text -text "Socket Port:" \
								-font {Helvetica 10 bold}
				pack $d.message -side top -padx 5 -pady 10
				pack $d.sockhost.text -side left -padx 1 -pady 2
				pack $d.sockhost.entry -side right -padx 5 -pady 2
				pack $d.sockhost -side top -padx 5 -pady 2
				pack $d.sockport.text -side left -padx 1 -pady 2
				pack $d.sockport.entry -side right -padx 5 -pady 2
				pack $d.sockport -side top -padx 5 -pady 2
				$d.sockhost.entry insert 0 $sockhost
				$d.sockport.entry insert 0 $sockport
				frame $d.buttons
				button $d.buttons.cancel -text "Cancel" -bg grey66 \
								-command { set commtype "stdout"
				                   set outID "stdout"
				                   destroy $d }
				button $d.buttons.connect -text "Connect" -bg grey66 \
								-command {
						set sockhost [$d.sockhost.entry get]
						set sockport [$d.sockport.entry get]
					  set err [catch {socket $sockhost $sockport} outID]

						if {$err == 0} {
								destroy $d
						} else {
								tk_dialog $d.error "Socket Error" {Error: Unable to make socket connection.  Make sure the STK socket server is first running and that the port number is correct.} "" 0 OK 
				}   }
				pack $d.buttons.cancel -side left -padx 5 -pady 10
				pack $d.buttons.connect -side right -padx 5 -pady 10
				pack $d.buttons -side bottom -padx 5 -pady 10
		}
}
