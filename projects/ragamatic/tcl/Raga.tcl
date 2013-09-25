# Tcl/Tk GUI for the RagaMatic
# by Perry R. Cook

# Set initial control values
set cont1 10.0
set cont2 7.0
set cont4 0.0
set cont11 10.0
set cont7 3.0
set outID "stdout"
set commtype "stdout"

# Configure main window
wm title . "STK RagaMatic Controller"
wm iconname . "raga"
. config -bg grey20

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

# Configure bitmap display
if {[file isdirectory bitmaps]} {
    set bitmappath bitmaps
} else {
    set bitmappath tcl/bitmaps
}

frame .banner -bg black

button .banner.top -bitmap @$bitmappath/ragamat2.xbm \
    -background white -foreground black

frame .banner.bottom -bg black
label .banner.bottom.ragamat -text " * * RagaMatic * *\n\n \
				by Perry R. Cook\n for Ken's Birthday\n \
				January, 1999\n\n (thanks also to\n \
				Brad for rough\n riff inspirations)" \
				-background black -foreground white -relief groove \
				-width 20 -height 10

# Bind an X windows "close" event with the Exit routine
bind . <Destroy> +myExit

proc myExit {} {
    global outID
    puts $outID [format "NoteOff          0.0 1 60 127"]
    flush $outID
    puts $outID [format "ExitProgram"]
    flush $outID
    close $outID
    exit
}

proc mellow {} {
    global cont1 cont2 cont4 cont7 cont11
    set cont1 10.0
    set cont2 7.0
    set cont4 0.0
    set cont11 10.0
    set cont7 3.0
    printWhatz "ControlChange    0.0 1 " 1 $cont1
    printWhatz "ControlChange    0.0 1 " 2 $cont2
    printWhatz "ControlChange    0.0 1 " 4 $cont4
    printWhatz "ControlChange    0.0 1 " 7 $cont7
    printWhatz "ControlChange    0.0 1 " 11 $cont11
}

proc nicevibe {} {
    global cont1 cont2 cont4 cont7 cont11
    set cont1 6.0
    set cont2 72.0
    set cont4 21.0
    set cont11 50.0
    set cont7 60.0
    printWhatz "ControlChange    0.0 1 " 1 $cont1
    printWhatz "ControlChange    0.0 1 " 2 $cont2
    printWhatz "ControlChange    0.0 1 " 4 $cont4
    printWhatz "ControlChange    0.0 1 " 7 $cont7
    printWhatz "ControlChange    0.0 1 " 11 $cont11
}

proc voicSolo {} {
    global cont1 cont2 cont4 cont7 cont11
    set cont1 2.0
    set cont2 37.0
    set cont4 90.0
    set cont11 10.0
    set cont7 120.0
    printWhatz "ControlChange    0.0 1 " 1 $cont1
    printWhatz "ControlChange    0.0 1 " 2 $cont2
    printWhatz "ControlChange    0.0 1 " 4 $cont4
    printWhatz "ControlChange    0.0 1 " 7 $cont7
    printWhatz "ControlChange    0.0 1 " 11 $cont11
}

proc drumSolo {} {
    global cont1 cont2 cont4 cont7 cont11
    set cont1 3.0
    set cont2 37.0
    set cont4 0.0
    set cont11 100.0
    set cont7 120.0
    printWhatz "ControlChange    0.0 1 " 1 $cont1
    printWhatz "ControlChange    0.0 1 " 2 $cont2
    printWhatz "ControlChange    0.0 1 " 4 $cont4
    printWhatz "ControlChange    0.0 1 " 7 $cont7
    printWhatz "ControlChange    0.0 1 " 11 $cont11
}

proc rockOut {} {
    global cont1 cont2 cont4 cont7 cont11
    set cont1 1.0
    set cont2 97.0
    set cont4 52.0
    set cont11 120.0
    set cont7 123.0
    printWhatz "ControlChange    0.0 1 " 1 $cont1
    printWhatz "ControlChange    0.0 1 " 2 $cont2
    printWhatz "ControlChange    0.0 1 " 4 $cont4
    printWhatz "ControlChange    0.0 1 " 7 $cont7
    printWhatz "ControlChange    0.0 1 " 11 $cont11
}

proc raga {scale} {
    global outID
    puts $outID [format "ControlChange    0.0 1 64 %f" $scale]
    flush $outID
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

proc printWhatz {tag value1 value2 } {
    global outID
    puts $outID [format "%s %i %f" $tag $value1 $value2]
    flush $outID
}

frame .banner.butts -bg black

frame .banner.butts.ragas -bg black
button .banner.butts.ragas.raga0 -text "Raga1" \
	-bg grey66 -command {raga 0}
button .banner.butts.ragas.raga1 -text "Raga2" \
	-bg grey66 -command {raga 1}

frame .banner.butts.presets1 -bg black
button .banner.butts.presets1.warmup -text "Warmup" \
	-bg grey66 -command mellow
button .banner.butts.presets1.nicevibe -text "NiceVibe" \
	-bg grey66 -command nicevibe

frame .banner.butts.presets2 -bg black
button .banner.butts.presets2.voicsolo -text "VoiceSolo" \
	-bg grey66 -command voicSolo
button .banner.butts.presets2.drumsolo -text "DrumSolo" \
	-bg grey66 -command drumSolo

button .banner.butts.rockout -text "RockOut" \
	-bg grey66 -command rockOut

button .banner.butts.noteOn -text "Cease Meditations and Exit" \
	-bg grey66 -command myExit

frame .controls -bg black

scale .controls.cont1 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 1} \
-orient horizontal -label "Drone Probability" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont1

scale .controls.cont2 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 2} \
-orient horizontal -label "Sitar Probability" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont2

scale .controls.cont4 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 4} \
-orient horizontal -label "Voice Drum  Probability" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont4 

scale .controls.cont11 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 11} \
-orient horizontal -label "Tabla  Probability" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont11

scale .controls.cont7 -from 0 -to 128 -length 300 \
-command {printWhatz "ControlChange    0.0 1 " 7} \
-orient horizontal -label "Tempo" \
-tickinterval 32 -showvalue true -bg grey66  \
-variable cont7

pack .banner.top -pady 10 -padx 10
pack .banner.bottom.ragamat -padx 5 -pady 5
pack .banner.bottom -pady 10

pack .banner.butts.ragas.raga0 -side left
pack .banner.butts.ragas.raga1 -side left
pack .banner.butts.ragas
pack .banner.butts.presets1.warmup -side left
pack .banner.butts.presets1.nicevibe -side left
pack .banner.butts.presets1
pack .banner.butts.presets2.voicsolo -side left
pack .banner.butts.presets2.drumsolo -side left
pack .banner.butts.presets2
pack .banner.butts.rockout
pack .banner.butts.noteOn
pack .banner.butts -side left -padx 5 -pady 10
pack .banner -side left

pack .controls.cont1 	-padx 10 -pady 10
pack .controls.cont2 	-padx 10 -pady 10
pack .controls.cont4 	-padx 10 -pady 10
pack .controls.cont11	-padx 10 -pady 10
pack .controls.cont7 	-padx 10 -pady 10

pack .controls -side left -padx 10 -pady 10

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

