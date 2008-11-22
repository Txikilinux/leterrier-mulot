##############################################################################
# $Id: apropos.tcl,v 1.6 2007/01/05 12:48:38 david Exp $
#
# apropos.tcl - inspiré du about dialog "about Visual Tcl"
# adapté pour abuledu par Eric Seigne <eric.seigne@ryxeo.com>
#
# Copyright (C) 1996-1998 Stewart Allen
# Copyright (C) 2004 Eric Seigne
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

##############################################################################
#
global Home
variable langue

set version "Version 8.11a du 22/11/2008"
package require Img
###################
# CREATING WIDGETS
###################
toplevel .apropos -background grey
.apropos configure -width 200 -height 300
wm title .apropos "A propos de AbulEdu - MULOT"
wm resizable .apropos 0 0

	set fich "background.png"
	#set fich $langue$fich

label .apropos.lab28 \
	-background blue -image [image create photo fgauche -file [file join sysdata $fich]]
label .apropos.lab29 -font {Helvetica 10 bold} -background blue -text "ABULEDU MULOT \n Crédits\n A. CONNES : <andre.connes@wanadoo.fr>\nActivités et documentation :\nA. Lesca <als@calestampar.org>\nH. Baronnet <herve.baronnet@free.fr>\nGraphismes : F. Audirac et E. François\n<http://webaf.net/spip/>\n Logiciel en licence GPL" -fg white

frame .apropos.fra30 \
        -borderwidth 2 -height 300 -width 200 -background grey
button .apropos.fra30.but31 \
        -text Fermer -width 8 \
        -command "destroy .apropos" \
        -borderwidth 1
label .apropos.lab21 \
        -borderwidth 1 -text $version -foreground black -background grey
###################
# SETTING GEOMETRY
###################
pack .apropos.lab29 \
        -in .apropos -anchor center -expand 1 -fill both -side top
pack .apropos.lab28 \
        -in .apropos -anchor center -expand 1 -fill both -side top
pack .apropos.fra30 \
        -in .apropos -anchor center -expand 0 -fill none -side bottom
pack .apropos.fra30.but31 \
        -in .apropos.fra30 -anchor center -expand 0 -fill none -padx 5 -pady 5 \
        -side right
pack .apropos.lab21 \
        -in .apropos -anchor center -expand 0 -fill none -pady 2 -side top

