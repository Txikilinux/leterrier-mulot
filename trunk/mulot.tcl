#!/bin/sh
#mulot.tcl
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
#  Date    : 15/04/2003 modifié le 06/12/2004
#  Licence : GNU/GPL Version 2 ou plus
# 
#  Description:
#  ------------
# 
#  @version    $Id: mulot.tcl,v 1.16 2006/03/25 00:55:15 abuledu_francois Exp $
#  @author     André Connes
#  @modifier   
#  @project    Le terrier
#  @copyright  André Connes
# 
#***********************************************************************
global sysFont glob

source mulot.conf
source msg.tcl
source aider.tcl
source credits.tcl
source fonts.tcl
source lanceapplication.tcl

set maxcolonnes 2

# gestion de l'espace de l'utilisateur
  if {![file exists [file join  $glob(home_mulot)]]} {
	catch { file mkdir [file join $glob(home_mulot)]}
  }
  if {![file exists [file join  $glob(home_reglages)]]} {
	catch { file mkdir [file join $glob(home_reglages)] }
  }
  #
  # langue par defaut
  #
  if {![file exists [file join  $glob(home_reglages) lang.conf]]} {
	set f [open [file join $glob(home_reglages) lang.conf] "w"]
      if { $glob(platform) == "windows" } {
		puts $f "fr"
      } else {
  		puts $f [string range $env(LANG) 0 1]
      }
      close $f
  }
  set f [open [file join $glob(home_reglages) lang.conf] "r"]
  gets $f lang
  close $f

  ::msgcat::mclocale $lang
  ::msgcat::mcload [file join [file dirname [info script]] msgs]

  #
  # ordre aleatoire par defaut
  #
  if {![file exists [file join  $glob(home_reglages) ordre.conf]]} {
	set f [open [file join $glob(home_reglages) ordre.conf] "w"]
	puts $f "0"
      	close $f
  }
  set f [open [file join $glob(home_reglages) ordre.conf] "r"]
  gets $f glob(ordre)
  close $f

source fin_sequence.tcl

  if {![file exists [file join  $glob(home_reglages) dir_exos.conf]]} {
	set f [open [file join $glob(home_reglages) dir_exos.conf] "w"]
	puts $f "Commun"
	close $f
  }

  catch { file mkdir [file join $glob(home_msgs)] }
  # astuce suivante : ajoute automatiquement toute nouvelle langue chez l'utilisateur
  foreach f [glob [file join msgs *.msg]] {
	catch { file copy [file join $f] [file join $glob(home_msgs)]}
  }
  if {![file exists [file join  $glob(trace_dir)]]} {
	file mkdir [file join $glob(trace_dir)]
  }
  catch { file mkdir [file join $glob(home_chemin) grilles] }
  #
  # Etat des boutons des menus
  #
  set glob(etat_boutons) normal
  set rdir $glob(home_reglages)

  #set nom_utilisateur defaut
  #set classe defaut
  
  if { $glob(autorise) } {
    # recherche du repertoire de reglage de la classe
    if {[file exists /usr/share/abuledu-mulot]} {
      set rdir "/usr/share/abuledu-mulot/reglages"
    }
  }
  # maintenant classe = profs ou cp, ce1, etc. ou defaut 
  # et rdir = 4abuledu_mdk or 4abuledu_debian or glob(home_reglages)  
  # definir l'etat des boutons des menus si non prof
#  if { $classe != "profs" && [file exists [file join $rdir boutons.$classe]] } {
#    set f [open [file join $rdir boutons.$classe] "r"]
#    gets $f glob(etat_boutons)
#    close $f
#  }
  set glob(etat_boutons) normal

proc setlang {lang} {
  global env glob
  set env(LANG) $lang
  catch {set f [open [file join $glob(home_reglages) lang.conf] "w"]}
  puts $f [file rootname [file tail $lang]]
  close $f
}

proc set_img_dir {img_dir} {
  global glob
  catch {set f [open [file join $glob(home_reglages) dir_images.conf] "w"]}
  puts $f $img_dir
  close $f
  lanceappli choisir_activite.tcl
}

proc setwindowsusername {} {
    global user
    catch {destroy .utilisateur}
    toplevel .utilisateur -background grey -width 250 -height 100
    wm geometry .utilisateur +50+50
    frame .utilisateur.frame -background grey -width 250 -height 100
    pack .utilisateur.frame -side top
    label .utilisateur.frame.labobj -font {Helvetica 10} -text [mc "Ton nom"] -background grey
    pack .utilisateur.frame.labobj -side top 
    entry .utilisateur.frame.entobj -font {Helvetica 10} -width 10
    pack .utilisateur.frame.entobj -side top 
    button .utilisateur.frame.ok -background gray75 -text "Ok" -command "verifnom"
    pack .utilisateur.frame.ok -side top -pady 10
}

  proc verifnom {} {
   global glob
   set nom [string tolower [string trim [string map {\040 ""} [.utilisateur.frame.entobj get]]]]
    if {$nom ==""} {
	set nom eleve
    }
    # sauver le réglage du nom
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "w"]}
    set glob(trace_user) "$glob(trace_dir)/$nom"
    puts $f $glob(trace_user)
    close $f
    catch {destroy .utilisateur}
  } ;# fin verifnom

proc ordonner { ordre } {
  global glob
  set glob(ordre) $ordre
  set f [open [file join $glob(home_reglages) ordre.conf] "w"]
  puts $f $ordre
  close $f
}

#########################################################"
proc main_loop {} {
  global . sysFont glob maxcolonnes env

  #
  # Creation du menu utilise comme barre de menu:
  #
  catch {destroy .menu}
  catch { destroy .frame.c}
  catch {destroy .frame}
  menu .menu -tearoff 0

  #
  # Creation du menu Fichier
  #
  menu .menu.fichier -tearoff 0
  .menu add cascade -state $glob(etat_boutons) \
	-label [mc "Fichier"] -menu .menu.fichier

  set etat_fichier "normal"

  .menu.fichier add command -label [mc "Bilans"] -command "lanceappli bilan.tcl 0"

  .menu.fichier add separator

  .menu.fichier add command -label [mc "Quitter"] -command exit

  #
  # Creation du menu Reglages
  #
  menu .menu.reglages -tearoff 0
  .menu add cascade -state $glob(etat_boutons) \
	-label [mc "Reglages"] -menu .menu.reglages

  set etat_reglages "normal"

  # langues
  menu .menu.reglages.lang -tearoff 0 
  .menu.reglages add cascade -label "[mc "Langue"]" -menu .menu.reglages.lang

  foreach i [glob [file join  $glob(home_msgs) *.msg]] {
    set langue [string map {.msg "" } [file tail $i]]
    .menu.reglages.lang add radio -label $langue -variable langue -command "setlang $langue; lanceappli mulot.tcl 0"
  }

  # quel ordre ?
  menu .menu.reglages.ordre -tearoff 0
  .menu.reglages add cascade -label [mc "Déroulement..."] \
	-state $etat_fichier \
	-menu .menu.reglages.ordre
  .menu.reglages.ordre add radio -label [mc "dans l'ordre"] \
	-command "ordonner 1"
  .menu.reglages.ordre add radio -label [mc "au hasard"] \
	-command "ordonner 0"

  #
  # Creation du menu Options
  #
  if { $glob(autorise) } {
    menu .menu.options -tearoff 0
    .menu add cascade -state $glob(etat_boutons) \
	-label [mc "Options"] -menu .menu.options
    # gerer les images
    .menu.options add command -label [mc "Images"] -command "lanceappli gerer_images.tcl 0"
    # montrer/masquer des dossiers
    .menu.options add command -label [mc "Dossiers"] -command "lanceappli montrer_cacher.tcl 0"
    # autoriser les menus
    menu .menu.options.menus -tearoff 0
    .menu.options add cascade -label [mc "Menus"] \
	-state $etat_fichier \
	-menu .menu.options.menus
    .menu.options.menus add command -label [mc "Autoriser les menus"] \
	-command "lanceappli autoriser_menus.tcl normal"
    .menu.options.menus add command -label [mc "Ne pas autoriser les menus"] \
	-command "lanceappli autoriser_menus.tcl disabled"
  }

  # dossiers images

  #charger la liste des dossiers images (peu importe le type ?)
  set all_img_dirs [glob -directory images -type d *]
  # verification : non CVS et plus de glob(bouclemax) images par thème
  foreach tmp_img_dir $all_img_dirs {
    if { $tmp_img_dir == "images/CVS" } continue
    if { ! [catch {set t [glob [file join $tmp_img_dir *]]}] } {
      if { [llength $t] >= $glob(bouclemax) } {
        lappend glob(img_dirs) [file tail $tmp_img_dir]
      }
    }
  }
  set glob(img_dirs) [lsort $glob(img_dirs)]

  # Nom de l'utilisateur sous windows
  if {$glob(platform) == "windows"} {
    .menu add command -label [mc "Utilisateur"] -command "setwindowsusername"
  }
  #
  # Creation du menu aide
  #
  menu .menu.aide -tearoff 0
  .menu add cascade -state $glob(etat_boutons) \
	-label [mc "Aide"] -menu .menu.aide
  # langues
  set l_langues [glob  [file join [pwd] aides aide.*.html]]
  foreach langue $l_langues {
    set lang [lindex [split $langue "."] end-1]
    .menu.aide add command -label "[mc Aide] $lang" -command "aider $lang"
  }
    # credit
  if { [llength [glob -nocomplain [file join  [pwd] credits *.html]]] > 0 } {
    menu .menu.aide.credits -tearoff 0 
    .menu.aide add cascade -label [mc "credits"] -menu .menu.aide.credits
       foreach i [glob [file join  [pwd] credits *.html]] {
        set c [string map {.html "" } [file tail $i]]
        .menu.aide.credits add radio -label $c -variable credit -command "credits $c"
      } 
  }
  # a propos
  .menu.aide add command -label [mc "A_propos ..."] -command "source apropos.tcl"

  . configure -menu .menu

  #######################################################################"
  . configure -background blue
  frame .frame -background blue
  pack .frame -side top -fill both -expand yes
  ###################On crée un canvas####################################

  set c .frame.c
  canvas $c -bg blue -highlightbackground blue
  pack $c -expand true

  set nligne 0
  set ncolonne 0
  #
  # creation des boutons a la volee en utilisant la premiere image
  #
  foreach d $glob(img_dirs) {
    set i [lindex [glob [file join images $d *]] 0]
    image create photo but_$d
    but_$d copy [image create photo -file [file join $i]] \
	-subsample 5 5
    if { $glob(platform) == "windows" || $glob(tcl_version) < "8.4" } {
      button $c.$d \
        -image but_$d \
        -borderwidth 10 -bg lightblue -cursor heart -relief raised \
        -text [mc $d] \
        -command "set_img_dir $d"
    } else {
      button $c.$d \
        -image but_$d \
        -borderwidth 10 -bg lightblue -cursor heart -relief raised \
        -text [mc $d] -compound top \
        -command "set_img_dir $d"
    }
    grid $c.$d -column $ncolonne -row $nligne -sticky e -padx 10 -pady 10
    incr ncolonne
    if { $ncolonne > $maxcolonnes } {
      set ncolonne 0
      incr nligne
    } 
  }

  if { $nligne == 0 } {
    incr nligne ;# pour un affichage reussi !
  }
  if { $glob(platform) == "windows"  || $glob(tcl_version) < "8.4" } {
    button $c.quitter \
        -image [image create photo -file sysdata/quitter_minus.png] \
        -borderwidth 10 -bg pink -cursor heart \
        -text [mc "Quitter"] \
        -command exit
  } else {
    button $c.quitter \
        -image [image create photo -file sysdata/quitter_minus.png] \
        -borderwidth 10 -bg pink -cursor heart \
        -text [mc "Quitter"] -compound top \
        -command exit
  }
  grid $c.quitter -column 3 -row 1 -sticky e -padx 10 -pady 10

  set myimage [image create photo -file sysdata/background.png]
  label $c.imagedisplayer -image $myimage -background blue
  grid $c.imagedisplayer -column 3 -row 0 -sticky e

} ;# fin main_loop

########################################################################"

  # Nom de l'utilisateur par défaut sous windows
  if {$glob(platform) == "windows"} {
    set nom eleve
    # sauver le réglage du nom
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "w"]}
    puts $f "$glob(trace_dir)/$nom"
    close $f
  }

bind . <Control-q> {exit}

wm resizable . 0 0
wm geometry . [expr [winfo screenwidth .]-10]x[expr [winfo screenheight .]-110]+0+0
. configure -background blue
wm title . [mc "Titre"]

main_loop
