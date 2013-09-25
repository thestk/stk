set pitch 64.0
set press 64.0
set cont1 20.0
set cont2 64.0
set cont4 64.0
set cont11 64.0
set outID "stdout"
set commtype "stdout"
set patchnum 16

# Configure main window
wm title . "STK Voice Model Controller"
wm iconname . "voice"
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
. configure -menu .menu

# Configure patch change buttons
frame .instChoice -bg black

radiobutton .instChoice.fm -text "FMVoice" -bg grey66  \
				-command { patchChange 16 } -value 16 -variable patchnum
radiobutton .instChoice.form -text "Formant" -bg grey66 \
				-command { patchChange 17 } -value 17 -variable patchnum

pack .instChoice.fm -side left -padx 5
pack .instChoice.form -side left -padx 5 -pady 10

pack .instChoice -side top

# Configure bitmap display
if {$tcl_platform(platform) == "windows"} {
		set bitmappath bitmaps
} else {
		set bitmappath TCLSpecs/bitmaps
}
button .pretty -bitmap @$bitmappath/prcFunny.xbm \
				-background white -foreground black
.pretty config -bitmap @$bitmappath/prc.xbm
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

# Configure sliders
frame .left -bg black
frame .right -bg black

scale .left.bPressure -from 0 -to 128 -length 200 \
-command {changePress } -variable press \
-orient horizontal -label "Loudness (Spectral Tilt)" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.pitch -from 0 -to 128 -length 200 \
-command {changePitch } -variable pitch \
-orient horizontal -label "MIDI Note Number" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.cont1 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 2} \
-orient horizontal -label "Formant Q / Voiced/Un." \
-tickinterval 32 -showvalue true -bg grey66 \
-variable cont2

scale .right.cont2 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 4} \
-orient horizontal -label "Vowel (Bass, Tenor, Alto, Sop.)" \
-tickinterval 32 -showvalue true -bg grey66 \
-variable cont4

scale .right.cont3 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 11} \
-orient horizontal -label "Vibrato Rate" \
-tickinterval 32 -showvalue true -bg grey66 \
-variable cont11

scale .right.vibrato -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 1} \
-orient horizontal -label "Vibrato Amount" \
-tickinterval 32 -showvalue true -bg grey66\
-variable cont1

pack .left.bPressure -padx 10 -pady 10
pack .left.pitch -padx 10 -pady 10
pack .left.cont1 -padx 10 -pady 10
pack .right.cont2 -padx 10 -pady 10
pack .right.cont3 -padx 10 -pady 10
pack .right.vibrato -padx 10 -pady 10

pack .left -side left
pack .right -side right

proc myExit {} {
    global pitch
    global outID
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

proc noteOff {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOff          0.0 1 %f %f" $pitchVal $pressVal]
    flush $outID
}

proc patchChange {value} {
		global outID
		global bitmappath
    global cont1
    global cont2
    global cont4
    global cont11
    puts $outID [format "ProgramChange    0.0 1 %i" $value]
    if {$value==16}	{
				.pretty config -bitmap @$bitmappath/KVoiceFM.xbm
    }
    if {$value==17}	{
				.pretty config -bitmap @$bitmappath/KVoicForm.xbm
    }
    flush $outID
    set cont1 0.0
    set cont2 20.0
    set cont4 64.0
    set cont11 64.0
}

proc printWhatz {tag value1 value2 } {
    global outID
    puts $outID [format "%s %i %f" $tag $value1 $value2]
    flush $outID
}

proc changePress {value} {
    global outID
    puts $outID [format "AfterTouch       0.0 1 %f" $value]
    flush $outID
}

proc changePitch {value} {
    global outID
    puts $outID [format "PitchBend        0.0 1 %.3f" $value]
    flush $outID
}

# Socket connection procedure
set d .socketdialog

proc setComm {} {
		global outID
		global commtype
		global d
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
				toplevel $d
				wm title $d "STK Client Socket Connection"
				wm resizable $d 0 0
				grab $d
				label $d.message -text "Specify a socket port number below (if different than the STK default of 2001) and then click the \"Connect\" button to invoke a socket-client connection attempt to the STK socket server." \
								-background white -font {Helvetica 10 bold} \
								-wraplength 3i -justify left
				frame $d.sockport
				entry $d.sockport.entry -width 6
				label $d.sockport.text -text "Socket Port Number:" \
								-font {Helvetica 10 bold}
				pack $d.message -side top -padx 5 -pady 10
				pack $d.sockport.text -side left -padx 1 -pady 10
				pack $d.sockport.entry -side right -padx 5 -pady 10
				pack $d.sockport -side top -padx 5 -pady 10
				$d.sockport.entry insert 0 $sockport
				frame $d.buttons
				button $d.buttons.cancel -text "Cancel" -bg grey66 \
								-command { set commtype "stdout"
				                   set outID "stdout"
				                   destroy $d }
				button $d.buttons.connect -text "Connect" -bg grey66 \
								-command {
						set sockport [$d.sockport.entry get]
					  set err [catch {socket localhost $sockport} outID]
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
