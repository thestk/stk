set pitch 64.0
set press 64.0
set cont1 0.0
set cont2 64.0
set cont4 64.0
set cont7 128.0
set cont11 64.0
set cont44 24.0
set outID "stdout"
set commtype "stdout"
set preset 0

# Configure main window
wm title . "STK Modal Bar Interface"
wm iconname . "modal"
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

# Configure preset radio buttons
frame .radio1 -bg black
frame .radio2 -bg black

radiobutton .radio1.0 -text Marimba -bg grey66  \
    -variable preset -value 0 -command { patchChange $preset }
radiobutton .radio1.1 -text Vibraphone -bg grey66  \
    -variable preset -value 1 -command { patchChange $preset }
radiobutton .radio1.2 -text Agogo -bg grey66  \
    -variable preset -value 2 -command { patchChange $preset }
radiobutton .radio1.3 -text Wood1 -bg grey66  \
    -variable preset -value 3 -command { patchChange $preset }
radiobutton .radio2.4 -text Reso -bg grey66 \
    -variable preset -value 4 -command { patchChange $preset }
radiobutton .radio2.5 -text Wood2 -bg grey66 \
    -variable preset -value 5 -command { patchChange $preset }
radiobutton .radio2.6 -text Beats -bg grey66 \
    -variable preset -value 6 -command { patchChange $preset }
radiobutton .radio2.7 -text 2Fix -bg grey66 \
    -variable preset -value 7 -command { patchChange $preset }
radiobutton .radio2.8 -text Clump -bg grey66 \
    -variable preset -value 8 -command { patchChange $preset }

pack .radio1.0 -side left -padx 5
pack .radio1.1 -side left -padx 5 -pady 10
pack .radio1.2 -side left -padx 5 -pady 10
pack .radio1.3 -side left -padx 5 -pady 10
pack .radio1
pack .radio2.4 -side left -padx 5
pack .radio2.5 -side left -padx 5
pack .radio2.6 -side left -padx 5 -pady 10
pack .radio2.7 -side left -padx 5 -pady 10
pack .radio2.8 -side left -padx 5 -pady 10
pack .radio2

# Configure bitmap display
if {[file isdirectory bitmaps]} {
    set bitmappath bitmaps
} else {
    set bitmappath tcl/bitmaps
}

button .pretty -bitmap @$bitmappath/KModal.xbm \
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

# Configure sliders
frame .left -bg black
frame .right -bg black

scale .left.volume -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0  1 " 7} \
-orient horizontal -label "Volume" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont7

scale .left.bPressure -from 0 -to 128 -length 200 \
-command {changePress } -variable press \
-orient horizontal -label "Strike Vigor" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.pitch -from 0 -to 128 -length 200 \
-command {changePitch } -variable pitch \
-orient horizontal -label "MIDI Note Number" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.cont2 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0  1 " 2} \
-orient horizontal -label "Stick Hardness" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont2

scale .right.reverb -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0  1 " 44} \
-orient horizontal -label "Reverb Mix" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont44

scale .right.cont4 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0  1 " 4} \
-orient horizontal -label "Stick Position" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont4

scale .right.cont11 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0  1 " 11} \
-orient horizontal -label "Disabled" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont11 -state disabled

scale .right.cont1 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0  1 " 1} \
-orient horizontal -label "Direct Stick Mix" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont1

pack .left.volume -padx 10 -pady 10
pack .left.bPressure -padx 10 -pady 10
pack .left.pitch -padx 10 -pady 10
pack .left.cont2 -padx 10 -pady 10
pack .right.reverb -padx 10 -pady 10
pack .right.cont4 -padx 10 -pady 10
pack .right.cont11 -padx 10 -pady 10
pack .right.cont1 -padx 10 -pady 10

pack .left -side left
pack .right -side right

#bind all <KeyPress> {
bind . <KeyPress> {
    noteOn $pitch $press 
}

# Bind an X windows "close" event with the Exit routine
bind . <Destroy> +myExit

proc myExit {} {
    global pitch outID
    puts $outID [format "ExitProgram"]
    flush $outID
    close $outID
    exit
}

proc noteOn {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOn           0.0  1  %3.2f  %3.2f" $pitchVal $pressVal]
    flush $outID
}

proc noteOff {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOff          0.0  1  %3.2f %3.2f" $pitchVal $pressVal]
    flush $outID
}

proc patchChange {value} {
    global outID preset
    if {$preset == 1} {
        .right.cont11 config -state normal -label "Vibrato Rate"
    } else {
        .right.cont11 config -state disabled -label "Disabled"
    }
    printWhatz "ControlChange    0.0 1 " 16  $preset
}

proc printWhatz {tag value1 value2 } {
    global outID
    puts $outID [format "%s %2i  %3.2f" $tag $value1 $value2]
    flush $outID
}

proc changePress {value} {
    global outID
    puts $outID [format "AfterTouch       0.0  1  %3.2f" $value]
    flush $outID
}

proc changePitch {value} {
    global outID
    puts $outID [format "PitchChange      0.0  1  %3.2f" $value]
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


