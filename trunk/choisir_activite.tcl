#!/bin/sh
# choisir_activite.tc du projet mulot.tcl
# \
exec wish "$0" ${1+"$@"}

#*************************************************************************
#  Copyright (C) 2002 André Connes <davidlucardi@aol.com>
#  Copyright (C) 2002 André Connes <andre.connes@wanadoo.fr>
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
#  File  : mulot.tcl
#  Author  : André Connes <andre.connes@wanadoo.fr>
#  Modifier:
#  Date    : 15/04/2003 modifié le 21/11/2004
#  Licence : GNU/GPL Version 2 ou plus
# 
#  Description:
#  ------------
# 
#  @version    $Id: choisir_activite.tcl,v 1.8 2006/03/25 00:55:15 abuledu_francois Exp $
#  @author     André Connes
#  @modifier   
#  @project    Le terrier
#  @copyright  André Connes
# 
#***********************************************************************
global sysFont glob

source mulot.conf
source msg.tcl
source fonts.tcl
source lanceapplication.tcl

  #
  # langue par defaut
  #
  set f [open [file join $glob(home_reglages) lang.conf] "r"]
  gets $f lang
  close $f
  ::msgcat::mclocale $lang
  ::msgcat::mcload [file join [file dirname [info script]] msgs]


  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(theme) [gets $f]
  close $f

###########################################################################################""
proc main_loop {} {
  global c glob 

  #######################################################################"
  . configure -background blue
  frame .frame -background blue
  pack .frame -side top -fill both -expand yes
  ###################On crée un canvas####################################

  set c .frame.c
  canvas $c -bg blue -highlightbackground blue
  pack $c -expand true

  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.survoler \
        -image [image create photo -file sysdata/survoler.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Survoler] \
        -command "lanceappli play_mulot.tcl 0"
  } else {
    button $c.survoler \
        -image [image create photo -file sysdata/survoler.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Survoler] -compound top \
        -command "lanceappli play_mulot.tcl 0"
  }
  grid $c.survoler -column 0 -row 0 -sticky e -padx 10 -pady 10
  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.cliquer \
        -image [image create photo -file sysdata/cliquer.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Cliquer] \
        -command "lanceappli play_mulot.tcl 1"
  } else {
    button $c.cliquer \
        -image [image create photo -file sysdata/cliquer.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Cliquer] -compound top \
        -command "lanceappli play_mulot.tcl 1"
  }
  grid $c.cliquer -column 1 -row 0 -sticky e -padx 10 -pady 10
  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.double_cliquer \
        -image [image create photo -file sysdata/double_cliquer.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Double-cliquer] \
        -command "lanceappli play_mulot.tcl 2"
  } else {
    button $c.double_cliquer \
        -image [image create photo -file sysdata/double_cliquer.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Double-cliquer] -compound top \
        -command "lanceappli play_mulot.tcl 2"
  }
  grid $c.double_cliquer -column 2 -row 0 -sticky e -padx 10 -pady 10

  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.parcourir \
        -image [image create photo -file sysdata/parcourir.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Parcourir] \
        -command "lanceappli parcourir_mulot.tcl"
  } else {
    button $c.parcourir \
        -image [image create photo -file sysdata/parcourir.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Parcourir] -compound top \
        -command "lanceappli parcourir_mulot.tcl"
  }
  grid $c.parcourir -column 0 -row 1 -sticky e -padx 10 -pady 10

  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.glisser \
        -image [image create photo -file sysdata/glisser.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Glisser] \
        -command "lanceappli glisser_mulot.tcl"
  } else {
    button $c.glisser \
        -image [image create photo -file sysdata/glisser.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Glisser] -compound top \
        -command "lanceappli glisser_mulot.tcl"
  }
  grid $c.glisser -column 1 -row 1 -sticky e -padx 10 -pady 10

  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.deviner \
        -image [image create photo -file sysdata/deviner.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Deviner] \
        -command "lanceappli deviner.tcl"
  } else {
    button $c.deviner \
        -image [image create photo -file sysdata/deviner.png] \
        -borderwidth 10 -bg lightblue -cursor heart \
        -text [mc Deviner] -compound top \
        -command "lanceappli deviner.tcl"
  }
  grid $c.deviner -column 2 -row 1 -sticky e -padx 10 -pady 10

  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.quitter \
        -image [image create photo -file sysdata/quitter.png] \
        -borderwidth 10 -bg pink -cursor heart \
        -text [mc Quitter] \
        -command "lanceappli mulot.tcl"
  } else {
    button $c.quitter \
        -image [image create photo -file sysdata/quitter.png] \
        -borderwidth 10 -bg pink -cursor heart \
        -text [mc Quitter] -compound top \
        -command "lanceappli mulot.tcl"
  }
  grid $c.quitter -column 3 -row 1 -sticky e -padx 10 -pady 10

    set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
    set glob(dossier) [gets $f]
    set i [lindex [glob [file join images $glob(dossier) *.*]] 0]
      
    image create photo lab_theme
    lab_theme copy [image create photo -file [file join $i]] \
	-subsample 5 5
    if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
      label $c.img_theme \
        -image lab_theme \
        -text [mc $glob(dossier)] \
        -borderwidth 2
    } else {
      label $c.img_theme \
        -image lab_theme \
        -text [mc $glob(dossier)] -compound top \
        -borderwidth 2
    }
    grid $c.img_theme -column 4 -row 1 -sticky e -padx 10 -pady 10

  set myimage [image create photo -file sysdata/background.png]
  label $c.imagedisplayer -image $myimage -background blue
  grid $c.imagedisplayer -column 3 -row 0 -sticky e

  label $c.nimages -bg blue
  grid $c.nimages -column 1 -row 2

}

########################################################################"

  # Nom de l'utilisateur par defaut sous windows
  if {$glob(platform) == "windows"} {
    set nom eleve
    # sauver le reglage du nom
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "w"]}
    puts $f "$glob(trace_dir)/$nom"
    close $f
  }

bind . <Control-q> {exit}

wm resizable . 0 0
wm geometry . [expr [winfo screenwidth .]-10]x[expr [winfo screenheight .]-80]+0+0
. configure -background blue
wm title . "[mc title_m] : $glob(theme)"

main_loop

