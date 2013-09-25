# Tcl/Tk Demo GUI for the Synthesis Toolkit (STK)
# by Gary P. Scavone, CCRMA, Stanford University, 1999.

# Set initial control values
set pitch 64.0
set osc1on 1
set osc2on 1
set freq1 200.0
set freq2 200.0
set amp1 20
set amp2 20
set outID "stdout"
set commtype "stdout"

# Configure main window
wm title . "MUS152 Two Oscillator Controller"
wm iconname . "TwoOsc"
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

# Configure title display
label .title -text "MUS151 Two Oscillator Controller" \
				-font {Times 14 bold} -background white \
				-foreground darkred -relief raised

label .title2 -text "by Gary P. Scavone\n Center for Computer Research in Music & Acoustics (CCRMA) \n Stanford University" \
				-font {Times 12 bold} -background white \
				-foreground darkred -relief raised

pack .title -padx 5 -pady 10
pack .title2 -padx 5 -pady 10

# Configure "note-on" buttons
frame .noteOn -bg black

button .noteOn.on -text NoteOn -bg grey66 -command { noteOn $pitch 64.0 }
button .noteOn.off -text NoteOff -bg grey66 -command { noteOff $pitch 127.0 }
button .noteOn.exit -text "Exit Program" -bg grey66 -command myExit
pack .noteOn.on -side left -padx 5
pack .noteOn.off -side left -padx 5 -pady 10
pack .noteOn.exit -side left -padx 5 -pady 10

pack .noteOn

# Configure sliders
frame .left -bg black

scale .left.freq1 -from 0 -to 10000  -length 400 \
-command {changeParam 20 } \
-orient horizontal -label "Frequency 1" \
-tickinterval 2000 -showvalue true -bg grey66 \
-variable freq1

scale .left.amp1 -from 0 -to 127 -length 400 \
-command {changeParam 22 } \
-orient horizontal -label "Amp 1" \
-tickinterval 32 -showvalue true -bg grey66 \
-variable amp1

scale .left.freq2 -from 0 -to 10000 -length 400 \
-command {changeParam 21 } \
-orient horizontal -label "Frequency 2" \
-tickinterval 2000 -showvalue true -bg grey66 \
-variable freq2

scale .left.amp2 -from 0 -to 127 -length 400 \
-command {changeParam 23 } \
-orient horizontal -label "Amp 2" \
-tickinterval 32 -showvalue true -bg grey66 \
-variable amp2

pack .left.freq1 -padx 10 -pady 3 
pack .left.amp1 -padx 10 -pady 3
pack .left.freq2 -padx 10 -pady 3
pack .left.amp2 -padx 10 -pady 3

pack .left -side left

# Configure more buttons
frame .onOff -bg black

checkbutton .onOff.1 -text "Play Osc 1" -variable osc1on -relief flat \
    -command {setPlayStatus 22 $osc1on}
checkbutton .onOff.2 -text "Play Osc 2" -variable osc2on -relief flat \
    -command {setPlayStatus 23 $osc2on}

pack .onOff.1 .onOff.2 -padx 5
pack .onOff -side right -padx 5 -pady 10

# Bind an X windows "close" event with the Exit routine
bind . <Destroy> +myExit

proc myExit {} {
    global outID
    puts $outID [format "NoteOff          0.0 1 64.0 127.0" ]
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

proc changeParam {controlNum value } {
    global outID freq1 freq2 amp1 amp2 osc1on osc2on
    if {$controlNum==20 || $controlNum==22} {
        if {$osc1on==1} {
            puts $outID [format "ControlChange   0.0 1 %d  %f" $controlNum $value ]
        }
    } elseif {$controlNum==21 || $controlNum==23} {
        if {$osc2on==1} {
            puts $outID [format "ControlChange   0.0 1 %d  %f" $controlNum $value ]
        }
    }
    flush $outID
}	

proc setPlayStatus {controlNum value } {
    global outID amp1 amp2 freq1 freq2
    if {$value==1} {
        if {$controlNum==22} {
            puts $outID [format "ControlChange   0.0 1 %d  %f" 20 $freq1 ]
            puts $outID [format "ControlChange   0.0 1 %d  %f" $controlNum $amp1 ]
        } elseif {$controlNum==23} {
            puts $outID [format "ControlChange   0.0 1 %d  %f" 21 $freq2 ]
            puts $outID [format "ControlChange   0.0 1 %d  %f" $controlNum $amp2 ]
        }
    } elseif {$value==0} {
        puts $outID [format "ControlChange   0.0 1 %d  %f" $controlNum 0.0 ]
    }
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



