##############################################################################
# $Id: apropos.tcl,v 1.10 2006/03/24 23:54:15 abuledu_francois Exp $
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

###################
# CREATING WIDGETS
###################
toplevel .apropos -background grey
.apropos configure -width 200 -height 300
wm title .apropos "À propos de Mulot"
wm resizable .apropos 0 0

label .apropos.lab28 \
	-background #000000 -borderwidth 1 -image [image create photo fgauche -file [file join sysdata apropos.png]] -relief groove \
        -text label
frame .apropos.fra30 \
        -borderwidth 2 -height 300 -width 200 -background grey
button .apropos.fra30.but31 \
        -text [mc Fermer] -width 8 \
        -command "destroy .apropos" \
        -borderwidth 1
label .apropos.lab21 \
        -borderwidth 1 -text "Version $glob(version)" -foreground black -background grey
###################
# SETTING GEOMETRY
###################
pack .apropos.lab28 \
        -in .apropos -anchor center -expand 1 -fill both -side top
pack .apropos.fra30 \
        -in .apropos -anchor center -expand 0 -fill none -side bottom
pack .apropos.fra30.but31 \
        -in .apropos.fra30 -anchor center -expand 0 -fill none -padx 5 -pady 5 \
        -side right
pack .apropos.lab21 \
        -in .apropos -anchor center -expand 0 -fill none -pady 2 -side top

