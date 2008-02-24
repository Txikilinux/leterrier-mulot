#!/bin/sh
#mulot.tcl
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
#  File  : $Id: play_mulot.tcl,v 1.9 2006/03/25 00:55:15 abuledu_francois Exp $
#  Author  : andre.connes@wanadoo.fr
#  Date    : 05/01/2003 Modification : 16/02/2004
#  Licence : GNU/GPL Version 2 ou plus
# 
#  Description:
#  ------------
# 
#  @version    
#  @author     Andre Connes
#  @modifier   
#  @project    Le terrier
#  @copyright  Eric Seigne 24/04/2002
# 
#  *************************************************************************

source mulot.conf
source msg.tcl
source fin_sequence.tcl
source eval.tcl
source lanceapplication.tcl

  #
  # langue par defaut
  #
  set f [open [file join $glob(home_reglages) lang.conf] "r"]
  gets $f lang
  close $f
  ::msgcat::mclocale $lang
  ::msgcat::mcload [file join [file dirname [info script]] msgs]

  #
  # ordre
  #
  set f [open [file join $glob(home_reglages) ordre.conf] "r"]
  gets $f glob(ordre)
  close $f


if { $glob(platform) == "windows" } {
  set f [open [file join $glob(home_mulot) Reglages trace_user] "r"]
  set glob(trace_user) [gets $f]
  close $f
}

#mode d'effacement : survol ou clic ou double-clic ?
if { [lindex $argv 0] == 1 } {
  set modeEffacement Button-1
  set glob(modeEffacement) Cliquer
} elseif { [lindex $argv 0] == 2 } {
  set modeEffacement Double-ButtonPress-1
  set glob(modeEffacement) Double-cliquer
} else {
  set modeEffacement Enter
  set glob(modeEffacement) Survoler
}

#charger la liste (triee) des images
  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(dossier) [gets $f]
  set limages [lsort [glob [file join images $glob(dossier) *.*]]]

# Decommenter la ligne suivante si Img est installee
# package require Img

########################### On cree la fenetre principale###########################

# placement de la fenetre en haut et a gauche de l'ecran

wm resizable . 0 0
wm geometry . [expr int([winfo vrootwidth .]*0.99)]x[expr int([winfo vrootheight .]*0.9)]+0+0
. configure -bg $glob(bgcolor)

frame .frame -width $glob(width) -height $glob(height) -bg $glob(bgcolor)
pack .frame -side top -fill both -expand yes


# #######################On cree un canvas###########################################
# charge d'accueillir les sorties graphiques,
# qui peuvent etre des images, des textes, des formes geometriques ...

set c .frame.c
canvas $c -width $glob(width) -height $glob(height) -bg $glob(bgcolor) -highlightbackground $glob(bgcolor)
pack $c -expand true 

################################################

proc selectionner_images { } {
  global glob limages
  set glob(images_selectionnees) { }
  while { [llength $glob(images_selectionnees)] < $glob(bouclemax) } {
    set r [expr int(rand()*[llength $limages])]
    if { [lsearch $glob(images_selectionnees) $r] == -1 } {
      set glob(images_selectionnees) [linsert $glob(images_selectionnees) end $r]
    }
  }
  if { $glob(ordre) == "1" } {
    set glob(images_selectionnees) [lsort $glob(images_selectionnees)]
  }
}

proc avance { } {
  global glob

  if { $glob(nbcol) < $glob(nbmax) } {
    set glob(nbrow) [expr $glob(nbrow) + 3]
    set glob(nbcol) [expr $glob(nbcol) + 3]
  }
}

proc recule { } {
  global glob
  set glob(nbcol) 3
  set glob(nbrow) $glob(nbcol)
  set glob(boucle) 0
  for {set i 1} {$i <= $glob(bouclemax)} {incr i 1} {
    .bframe.tete$i configure -image pneutre
  }
  .bframe.but_gauche configure -state disable
  selectionner_images
}

proc delete_rect { c i j } {
  global glob cache ncache
  if { $cache($i,$j) == 1 } {
    $c delete rect($i,$j)
    set cache($i,$j) 0
    set ncache [expr $ncache - 1]
  }
  if { $ncache == 0 } {
    incr glob(boucle)
    .bframe.tete$glob(boucle) configure -image pbien
    for {set i 0} {$i < $glob(nbrow)} {incr i 1} {
      for {set j 0} {$j < $glob(nbcol)} {incr j 1} {
        $c delete rect($i,$j)
      }
    }
    sauver_trace_parcours
      set glob(nbmax) [expr $glob(nbcol) + 3]
      after [expr $glob(attente) * 1000] {
	if { $glob(boucle) < $glob(bouclemax) } {
	  set glob(session_finie) non
	} else {
          set glob(session_finie) oui
	} 
    }
    vwait glob(session_finie)

    if { $glob(session_finie) == "non" } {
      avance
      main $c
    } else {
      .bframe.but_gauche configure -state normal
      set sourires [glob [file join $glob(fin_sequence) sourire*.png]]
      set r [expr int(rand()*[llength $sourires])]     
      $c create image [expr int($glob(width)/2)] [expr int($glob(height)/2)]\
        -image [image create photo -file [file join [lindex $sourires $r]]]
      # exit
    }

  }
}

# ##
# sauver trace-eleve et parcours
# ##

proc sauver_trace_parcours {} {
  global glob modeEffacement heure_debut

  ## trace
  # utilisateur/classedate/durée/didacticiel/glob(boucle)/version
  set eleve $glob(trace_user)
  set titre "mulot-$glob(version)"
  set categorie "$glob(theme) $glob(modeEffacement)=$glob(boucle)"
    set heure_fin [clock seconds]
    set duree [expr int($heure_fin-$heure_debut)]
    set date_heure [clock format [clock seconds] -format "%c"]
    set quoi "classe \{$date_heure\} $duree"
  enregistreval $titre $categorie $quoi $eleve
}

# ###########################################################"
# on cree une frame en bas en avant-derniere ligne avec
#   le score affiche sous forme de tetes (bien passable mal)
#   un bouton 'continuer'
# #################################################################################

frame .bframe -bg $glob(bgcolor)
pack .bframe -side bottom -expand true

  image create photo pbien -file [file join sysdata pbien.png] 
  image create photo ppass -file [file join sysdata ppass.png]
  image create photo pmal -file [file join sysdata pmal.png]
  image create photo pneutre -file [file join sysdata pneutre.png]

for {set i 1} {$i <= $glob(bouclemax)} {incr i 1} {
  label .bframe.tete$i -bg $glob(bgcolor) -width 4
  grid .bframe.tete$i -column [expr $i -1] -row 1 -sticky e
  .bframe.tete$i configure -image pneutre -width 85
}

button .bframe.but_gauche -image \
  [image create photo fgauche -file [file join sysdata fgauche.png]] -command "recule; 
main $c"
grid .bframe.but_gauche -column [expr $glob(bouclemax)+2] -row 1
.bframe.but_gauche configure -state disable

    set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
    set glob(dossier) [gets $f]
    set i [lindex [glob [file join images $glob(dossier) *.*]] 0]
      
    image create photo lab_theme
    lab_theme copy [image create photo -file [file join $i]] \
	-subsample 5 5
    label .bframe.img_theme \
        -image lab_theme \
        -borderwidth 2
    grid .bframe.img_theme -column [expr $glob(bouclemax)+4] -row 1 -sticky e -padx 10 -pady 10

button .bframe.but_quitter -image \
  [image create photo fquitter \
  -file [file join sysdata quitter_minus.png]] \
  -command "lanceappli choisir_activite.tcl"
grid .bframe.but_quitter  -column [expr $glob(bouclemax)+3] -row 1

selectionner_images

# #################################################################################
# main, appelée depuis le bouton 'recommencer' cad 'next'

# #######################Initialisation de variables###############################
# glob(nbcol) : nombre de colonnes du tableau
# glob(nbrow) : nombre de lignes 

proc main {c} {
  global glob modeEffacement limages \
	wlen hlen cache ncache listeval  heure_debut
  $c delete all
  ####################################################################

  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(theme) [gets $f]
  close $f

  wm title . "[mc $glob(modeEffacement)] : $glob(theme)"

  # afficher le fond, fonction de la boucle,
  # image de la liste des images sÃ©lectionnÃ©es

  set bgimg [lindex $limages [lindex $glob(images_selectionnees) $glob(boucle)]]
  
  set bgimage [image create photo -file $bgimg]
  set hbgimage [image height $bgimage]
  set wbgimage [image width $bgimage]

  set hlen [expr $hbgimage / $glob(nbrow) +1]
  set wlen [expr $wbgimage / $glob(nbcol) +1]

  $c create image \
	[expr $glob(org) + int($wbgimage/2)+1] [expr $glob(org) + int($hbgimage/2)+1] -image $bgimage

  ####################Création du tableau###################################
  # On utilise des rectangles pour dessiner les cases
  # L'avantage des rectangles, c'est qu'ils peuvent constituer des objets
  # indépendants, utile si on veut leur associer des comportements

    # on cache tout
    for {set i 0} {$i < $glob(nbrow)} {incr i 1} {
      for {set j 0} {$j < $glob(nbcol)} {incr j 1} {
        $c create rect [expr $glob(org) + $i*$wlen +1] [expr $glob(org) + $j*$hlen +1] \
	  [expr $glob(org) + ($i+1)*$wlen +1] [expr $glob(org) + ($j+1)*$hlen +1] \
	  -fill $glob(bgcolor) -width 0 -tag rect($i,$j) 
	set cache($i,$j) 0  
      }
    }

    # on tire 7 cases au hasard et on les cache
    set ncache 0
    while { $ncache < 7 } {
      set i [expr int(rand()*$glob(nbrow))]
      set j [expr int(rand()*$glob(nbcol))]
      if { $cache($i,$j) == 0 } {
        $c create rect [expr $glob(org) + $i*$wlen +1] [expr $glob(org) + $j*$hlen +1] \
	  [expr $glob(org) + ($i+1)*$wlen +1] [expr $glob(org) + ($j+1)*$hlen +1] \
	  -fill $glob(rectcolor) -width 0 -tag rect($i,$j) 
	set cache($i,$j) 1
	set ncache [expr $ncache + 1]
        $c bind rect($i,$j) <$modeEffacement> "delete_rect $c $i $j "
      }
    }

  set heure_debut [clock seconds]

}

##################################################################"
  # Relire le nom réglé de l'utilisateur sous windows
  if {$glob(platform) == "windows"} {
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "r"]}
    gets $f glob(trace_user)
    close $f
  }

# appel de la procédure principale
set glob(nbrow) 3
set glob(nbcol) 3
set glob(nbmax) $glob(nbcol)
main $c
