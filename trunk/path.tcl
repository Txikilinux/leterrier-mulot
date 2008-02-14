#!/bin/sh
#menus.tcl
# \
exec wish "$0" ${1+"$@"}

#*************************************************************************
#  Copyright (C) 2002 Eric Seigne <erics@rycks.com>
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
#**************************************************************************
#  File  : $Id: path.tcl,v 1.7 2006/01/27 16:59:21 abuledu_andre Exp $
#  Author  : davidlucardi@aol.com
#  Modifier: 
#  Date    : 
#  Licence : GNU/GPL Version 2 ou plus
# 
#  Description:
#  ------------
# 
#  @version    $Id: path.tcl,v 1.7 2006/01/27 16:59:21 abuledu_andre Exp $
#  @author     David Lucardi
#  @modifier   
#  @project    Le terrier
#  @copyright  Eric Seigne 
# 
#  *************************************************************************
proc setwindowsusername {} {
global user
catch [destroy .utilisateur]
toplevel .utilisateur -background grey -width 250 -height 100
wm geometry .utilisateur +50+50
frame .utilisateur.frame -background grey -width 250 -height 100
pack .utilisateur.frame -side top
label .utilisateur.frame.labobj -font {Helvetica 10} -text "Quel est ton nom?" -background grey
pack .utilisateur.frame.labobj -side top 
entry .utilisateur.frame.entobj -font {Helvetica 10} -width 10
pack .utilisateur.frame.entobj -side top 
button .utilisateur.frame.ok -background gray75 -text "Ok" -command "verifnom"
pack .utilisateur.frame.ok -side top -pady 10
}

proc verifnom {} {
global env
set nom [string tolower [string trim [string map {\040 ""} [.utilisateur.frame.entobj get]]]]
if {$nom !=""} {
set env(USERNAME) $nom
}
catch [destroy .utilisateur]
}

proc init {plateforme} {
global Home basedir baseHome

if {![file exists [file join $Home]]} {
	file mkdir [file join $Home]
	file copy -force [file join reglages] [file join $Home]
}

switch $plateforme {
    unix {
##on pose tout dans le home, modif eric suite au stage info du 16 avril 2004
##	if {![file exists [file join  /var leterrier mulot]]} {
##	file mkdir [file join  /var leterrier mulot]
#}
	if {![file exists [file join  $baseHome log]]} {
	    file mkdir [file join  $baseHome log]
	}
    }
    windows {
	if {![file exists [file join mulot Log]]} {
	file mkdir [file join mulot Log]
	}
	
    	}
}

}


proc initlog {plateforme ident} {
global LogHome baseHome user
switch $plateforme {
    unix {
	set LogHome [file join $baseHome log]
#	set LogHome [file join /var leterrier mulot]
    }
    windows {
	set LogHome [file join mulot Log]
    }
}

if {$ident != ""} {
     set user [file join $LogHome $ident.log]
     } else {
     set user [file join $LogHome mulot.log]
     }
}

proc inithome {} {
	global baseHome basedir Homeconf Home
	variable reperttext
	variable repertconf
	set f [open [file join $baseHome reglages dir_exos.conf] "r"]
	set reperttext [gets $f]
	set repertconf [gets $f]
	close $f
	#on synchronise les 2 variables en attendant

	switch $repertconf {
		0 {set Home $baseHome}
		1 {set Home [file join $basedir textes]}
	}
	switch $repertconf {
		0 {set Homeconf $baseHome}
		1 {set Homeconf $basedir}
	}

}


proc changehome {} {
	global Home basedir baseHome Homeconf
	variable reperttext
	variable repertconf
	set f [open [file join $baseHome reglages repert.conf] "w"]
	#on synchronise les 2 variables en attendant
	puts $f $repertconf
	puts $f $repertconf
	close $f
	switch $repertconf {
		0 {set Home $baseHome}
		1 {set Home [file join $basedir textes]}
	}
	switch $repertconf {
		0 {set Homeconf $baseHome}
		1 {set Homeconf $basedir}
	}
}

global basedir Home baseHome iwish Homeconf progaide

set basedir [file dir $argv0]
cd $basedir
if {$env(HOME) == "c:\\"} {
	set Home [file join $basedir]
	set Homeconf [file join $basedir]
} else {
	set Home [file join $env(HOME) leterrier mulot]
	set Homeconf [file join $env(HOME) leterrier mulot]
}
set baseHome $Home



switch $tcl_platform(platform) {
    unix {
#	set progaide konqueror
#erics 3 mars 2004, cf dans abuledu-leterrier on a un script dispo pour
#tous les logiciels du terrier qui trouve tout seul le navigateur à lancer
	set progaide ./runbrowser
	set iwish wish
    }
    windows {
	set progaide shellexec.exe
	set iwish wish83
    }
}


