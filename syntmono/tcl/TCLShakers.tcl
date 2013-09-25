# Tcl/Tk Shakers GUI for the Synthesis Toolkit (STK)

# Set initial control values
set press 64.0
set cont1 64.0
set cont4 64.0
set cont11 64.0
set outID "stdout"
set commtype "stdout"
set patchnum 0

# Configure main window
wm title . "STK Shakers Controller"
wm iconname . "shakers"
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

# Configure sliders
frame .right -bg black

scale .right.bPressure -from 0 -to 128 -length 300 \
-command {changePress } -variable press\
-orient horizontal -label "Shake Energy" \
-tickinterval 32 -showvalue true -bg grey66

scale .right.cont2 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 4} \
-orient horizontal -label "(<--High) System Damping (Low-->)" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont4

scale .right.cont3 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 11} \
-orient horizontal -label "Number of Objects" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont11 

scale .right.vibrato -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 1} \
-orient horizontal -label "Resonance Center Freq." \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont1

pack .right.bPressure -padx 10 -pady 10
pack .right.vibrato 	-padx 10 -pady 10
pack .right.cont2 	-padx 10 -pady 10
pack .right.cont3 	-padx 10 -pady 10

pack .right -side right -padx 5 -pady 5

# Configure radio buttons
frame .left -bg black

radiobutton .left.maraca -text Maraca -bg grey66  \
				-command { patchChange 0 } -variable patchnum -width 8 \
				-justify left -value 0
radiobutton .left.sekere -text Sekere -bg grey66 \
				-command { patchChange 2 } -variable patchnum -width 8 \
				-justify left -value 2
radiobutton .left.cabasa -text Cabasa -bg grey66 \
				-command { patchChange 1 } -variable patchnum -width 8 \
				-justify left -value 1
radiobutton .left.bamboo -text Bamboo -bg grey66 \
				-command { patchChange 5 } -variable patchnum -width 8 \
				-justify left -value 5
radiobutton .left.waterdrp -text Waterdrp -bg grey66 \
				-command { patchChange 4 } -variable patchnum -width 8 \
				-justify left -value 4
radiobutton .left.tambourn -text Tambourn -bg grey66 \
				-command { patchChange 6 } -variable patchnum -width 8 \
				-justify left -value 6
radiobutton .left.sleighbl -text Sleighbl -bg grey66 \
				-command { patchChange 7 } -variable patchnum -width 8 \
				-justify left -value 7
radiobutton .left.guiro -text Guiro -bg grey66 \
				-command { patchChange 3 } -variable patchnum -width 8 \
				-justify left -value 3
radiobutton .left.stix1 -text Stix1 -bg grey66 \
				-command { patchChange 8 } -variable patchnum -width 8 \
				-justify left -value 8
radiobutton .left.crunch1 -text Crunch1 -bg grey66 \
				-command { patchChange 9 } -variable patchnum -width 8 \
				-justify left -value 9
radiobutton .left.wrench -text Wrench -bg grey66 \
				-command { patchChange 10 } -variable patchnum -width 8 \
				-justify left -value 10
radiobutton .left.sandpapr -text SandPaper -bg grey66 \
				-command { patchChange 11 } -variable patchnum -width 8 \
				-justify left -value 11
radiobutton .left.cokecan -text CokeCan -bg grey66 \
				-command { patchChange 12 } -variable patchnum -width 8 \
				-justify left -value 12

pack .left.maraca -pady 5
pack .left.sekere -pady 5
pack .left.cabasa -pady 5
pack .left.bamboo -pady 5
pack .left.waterdrp -pady 5
pack .left.tambourn -pady 5
pack .left.sleighbl -pady 5
pack .left.guiro -pady 5
pack .left.stix1 -pady 5
pack .left.crunch1 -pady 5
pack .left.wrench -pady 5
pack .left.sandpapr -pady 5
pack .left.cokecan -pady 5

pack .left -side left -padx 10

# Configure exit button
button .left.exit -text "Exit Program" -bg grey66 -command myExit
pack .left.exit -pady 10

#bind all <KeyPress> {
bind . <KeyPress> {
    patchChange $patchnum
}

# Bind an X windows "close" event with the Exit routine
bind . <Destroy> +myExit

proc myExit {} {
    global outID
    puts $outID [format "ExitProgram"]
    flush $outID
    close $outID
    exit
}

proc patchChange {value} {
    global outID press
    puts $outID [format "NoteOn    0.0 1 %i $press" $value]
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
