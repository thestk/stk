# Tcl/Tk Drum GUI for the Synthesis Toolkit (STK)

# Set initial control values
set press 127
set outID "stdout"
set commtype "stdout"

# Configure main window
wm title . "STK Drum Controller"
wm iconname . "drum"
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

# Configure slider
scale .bPressure -from 0 -to 128 -length 100 \
-command {changePress } -variable press\
-orient horizontal -label "Velocity" \
-tickinterval 64 -showvalue true -bg grey66

pack .bPressure -pady 5 -padx 5

# Configure buttons
frame .buttons -bg black
frame .buttons.left -bg black
frame .buttons.right -bg black

button .buttons.left.bass -text Bass  -bg grey66 \
-command { playDrum 36 } -width 7
button .buttons.left.snare -text Snare  -bg grey66 \
-command { playDrum 38 } -width 7
button .buttons.left.tomlo -text LoTom  -bg grey66 \
-command { playDrum 41 } -width 7
button .buttons.left.tommid -text MidTom  -bg grey66 \
-command { playDrum 45 } -width 7
button .buttons.left.tomhi -text HiTom  -bg grey66 \
-command { playDrum 50 } -width 7
button .buttons.left.homer -text Homer -bg grey66 \
-command { playDrum 90 } -width 7
button .buttons.right.hat -text Hat  -bg grey66 \
-command { playDrum 42 } -width 7
button .buttons.right.ride -text Ride -bg grey66 \
-command { playDrum 46 } -width 7
button .buttons.right.crash -text Crash -bg grey66 \
-command { playDrum 49 } -width 7
button .buttons.right.cowbel -text CowBel -bg grey66 \
-command { playDrum 56 } -width 7
button .buttons.right.tamb -text Tamb -bg grey66 \
-command { playDrum 54 } -width 7
button .buttons.right.homer -text Homer -bg grey66 \
-command { playDrum 90 } -width 7

pack .buttons.left.bass -pady 5
pack .buttons.left.snare -pady 5
pack .buttons.left.tomlo -pady 5
pack .buttons.left.tommid -pady 5
pack .buttons.left.tomhi -pady 5
pack .buttons.left.homer -pady 5
pack .buttons.right.hat -pady 5
pack .buttons.right.ride -pady 5
pack .buttons.right.crash -pady 5
pack .buttons.right.cowbel -pady 5
pack .buttons.right.tamb -pady 5
pack .buttons.right.homer -pady 5

pack .buttons.left -side left -pady 5 -padx 5
pack .buttons.right -side right -pady 5 -padx 5
pack .buttons -pady 5 -padx 5

# Configure exit button
button .exit -text "Exit Program" -bg grey66 -command myExit
pack .exit -side bottom -pady 20

#bind all <KeyPress> {
bind . <KeyPress> {
    noteOn $pitch $press 
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

proc playDrum {value}	{
    global press
		global outID
    puts $outID [format "NoteOn           -1.0 1 %i %f" $value $press]
    flush $outID
}

proc changePress {value} {
    global press
    set press $value
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
