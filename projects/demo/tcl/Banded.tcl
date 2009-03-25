# Tcl/Tk Bowed Bar Model GUI for the Synthesis Toolkit (STK)

set press 64.0
set pitch 64.0
set cont1 127.0
set cont2 0.0
set cont4 0.0
set cont11 0.0
set outID "stdout"
set commtype "stdout"
set struckbow 0
set preset 0
 
# Configure main window
wm title . "STK Bowed Bar Controller"
wm iconname . "bowedbar"
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

# Configure preset radio buttons
frame .radio1 -bg black

radiobutton .radio1.0 -text Bar -bg grey66  \
    -command {printWhatz "ControlChange    0.0 1 " 16 0} \
    -variable preset -value 0
radiobutton .radio1.1 -text Marimba -bg grey66  \
    -command {printWhatz "ControlChange    0.0 1 " 16 1} \
    -variable preset -value 1
radiobutton .radio1.2 -text GlassHarmonica -bg grey66  \
    -command {printWhatz "ControlChange    0.0 1 " 16 2} \
    -variable preset -value 2
radiobutton .radio1.3 -text PrayerBowl -bg grey66  \
    -command {printWhatz "ControlChange    0.0 1 " 16 3} \
    -variable preset -value 3

pack .radio1.0 -side left -padx 5
pack .radio1.1 -side left -padx 5 -pady 10
pack .radio1.2 -side left -padx 5 -pady 10
pack .radio1.3 -side left -padx 5 -pady 10
pack .radio1

# Configure message box
label .note -font {Times 10 normal} -background white \
    -foreground darkred -relief raised -height 4 \
    -wraplength 300 -width 60 \
    -text "To strike, set the Bow Pressure to zero and hit NoteOn or the spacebar.  To bow, use the 'Bow Velocity' slider to set a fixed velocity or move the 'Bow Motion' slider as if it were the bow (with a non-zero Bow Pressure)."
pack .note -padx 5 -pady 10

# Configure "note-on" buttons
frame .noteOn -bg black
button .noteOn.on -text NoteOn -bg grey66 -command { noteOn $pitch $press }
button .noteOn.off -text NoteOff -bg grey66 -command { noteOff $pitch 127.0 }
button .noteOn.exit -text "Exit Program" -bg grey66 -command myExit
pack .noteOn.on -side left -padx 5
pack .noteOn.off -side left -padx 5 -pady 10
pack .noteOn.exit -side left -padx 5 -pady 10
pack .noteOn -pady 10

# Configure sliders
frame .left -bg black
frame .right -bg black

scale .left.bPressure -from 0 -to 128 -length 200 \
-command {changePress } -variable press\
-orient horizontal -label "Strike/Bow Velocity" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.pitch -from 0 -to 128 -length 200 \
-command {changePitch } -variable pitch \
-orient horizontal -label "MIDI Note Number" \
-tickinterval 32 -showvalue true -bg grey66

scale .left.cont2 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 2} \
-orient horizontal -label "Bowing Pressure (0 = Strike)" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont2

scale .right.cont4 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 4} \
-orient horizontal -label "Bowing Motion" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont4

scale .right.cont11 -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 11} \
-orient horizontal -label "Integration" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont11 

scale .right.reson -from 0 -to 128 -length 200 \
-command {printWhatz "ControlChange    0.0 1 " 1} \
-orient horizontal -label "Mode Resonance" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont1

. config -bg grey20

pack .left.bPressure -padx 10 -pady 10
pack .left.pitch	-padx 10 -pady 10
pack .left.cont2	-padx 10 -pady 10
pack .right.cont4	-padx 10 -pady 10
pack .right.cont11 	-padx 10 -pady 10
pack .right.reson 	-padx 10 -pady 10

pack .left -side left
pack .right -side right



bind all <KeyPress> {
    noteOn $pitch $press 
}

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

proc noteOff {pitchVal pressVal} {
    global outID
    puts $outID [format "NoteOff          0.0 1 %f %f" $pitchVal $pressVal]
    flush $outID
}

proc patchChange {value} {
    global outID patch
    set patch $value
    puts $outID [format "ProgramChange    0.0 1 %i" $value]
    flush $outID
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
    puts $outID [format "PitchChange      0.0 1 %.3f" $value]
    flush $outID
}

proc activateVel {} {
  global pitch
  noteOn $pitch 127
  printWhatz "ControlChange    0.0 1 " 65  0
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

