#!/bin/sh
# deviner.tcl projet mulot
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
#  File  : $Id: deviner.tcl,v 1.11 2007/01/18 08:51:11 abuledu_andre Exp $
#  Author  : andre.connes@wanadoo.fr
#  Date    : 10/07/2003 Modification : 08/12/2005
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

if { $glob(platform) == "windows" } {
  set f [open [file join $glob(home_mulot) reglages trace_user] "r"]
  set glob(trace_user) [gets $f]
  close $f
}

#charger la liste des images
  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(dossier) [gets $f]
  set tmp_choix [lsort [glob [file join images $glob(dossier) *.*]]]
  foreach i $tmp_choix {
    lappend im_choix(im_liste) [file tail $i]
  }

# modifier la largeur de la fenetre a cause de la largeur des vignettes
set glob(width) [expr $glob(width) + 150]

# Decommenter la ligne suivante si Img est installee
# package require Img

################################################

proc selectionner_images { } {
  global glob im_choix
  set glob(images_selectionnees) { }
  while { [llength $glob(images_selectionnees)] < $glob(bouclemax) } {
    set r [expr int(rand()*[llength $im_choix(im_liste)])]
    if { [lsearch $glob(images_selectionnees) $r] == -1 } {
      set glob(images_selectionnees) [linsert $glob(images_selectionnees) end $r]
    }
  }
}

# glob(intrus contient l'image choisie + 3 intrus
proc creer_3intrus { } {
  global glob im_choix
  set glob(intrus) [list $im_choix(fname)]
  while { [llength $glob(intrus)] < 4 } {
    set r [expr int(rand()*[llength $im_choix(im_liste)])]
    set image [lindex $im_choix(im_liste) $r]
    if { [lsearch $glob(intrus) $image] == -1 } {
      set glob(intrus) [linsert $glob(intrus) end $image]
    }   
  }
  # melanger le tout
  for {set i 0} {$i < 6} {incr i 1} {
    set r [expr int(rand()*[llength $glob(intrus)])]
    set s [lindex $glob(intrus) $r]
    set glob(intrus) [linsert [lreplace $glob(intrus) $r $r] end $s]
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

# rebouche le rectangle precedent et montre la portion d'image sous le rectangle actuel
proc show_rect { c i j } {
  global glob im_choix case
  incr glob(clics_rectangle)
  # recouvrir le rectangle precedent
  set nl $case(ligne)
  set nc $case(colonne)
  $c create rect \
          [expr $glob(org) + $im_choix(v_width) + ($nc+0)*$glob(r_width) +1] [expr $glob(org) + ($nl+0)*$glob(r_height) +1] \
	  [expr $glob(org) + $im_choix(v_width) + ($nc+1)*$glob(r_width) +1] [expr $glob(org) + ($nl+1)*$glob(r_height) +1] \
	  -fill #009999 -width 0 -tag rectbis($i,$j)
  # decouvrir le rectangle actuel
  set case(ligne) $j
  set case(colonne) $i
  $c delete rect($i,$j)
  incr glob(nbrect) -1
  # pour le dernier rectangle
  if { $glob(nbrect) == 0 } {
    $c create rect \
          [expr $glob(org) + $im_choix(v_width) + ($i+0)*$glob(r_width) +1] [expr $glob(org) + ($j+0)*$glob(r_height) +1] \
	  [expr $glob(org) + $im_choix(v_width) + ($i+1)*$glob(r_width) +1] [expr $glob(org) + ($j+1)*$glob(r_height) +1] \
	  -width 0 -tag dernier
    $c bind dernier <Button-1> "show_rect $c $i $j"
  }  
}

#
# comparer l'image $nv et l'image choisie
proc analyse_reponse { nv } {
  global c glob im_choix
  incr glob(clics_vignette)
  if { [string compare $im_choix(fname) [lindex $glob(intrus) $nv]] == 0 } {
    incr glob(boucle)
    .bframe.tete$glob(boucle) configure -image pbien
    # on montre l'image choisie
    for {set i 0 } {$i < $glob(nbcol)} {incr i 1} {
      for {set j 0} {$j < $glob(nbrow)} { incr j 1} {
        $c delete rect($i,$j)
        $c delete rectbis($i,$j)
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

  } else {
    $c create line \
      [expr $glob(org)+0] [expr $glob(org)+($nv+0)*$im_choix(v_height)] \
      [expr $glob(org)+$im_choix(v_width)] [expr $glob(org)+($nv+1)*$im_choix(v_height)] \
      -width 3 -fill yellow
    $c create line \
      [expr $glob(org)+0] [expr $glob(org)+($nv+1)*$im_choix(v_height)] \
      [expr $glob(org)+$im_choix(v_width)] [expr $glob(org)+($nv+0)*$im_choix(v_height)] \
      -width 3 -fill yellow
  }

}

################################################

# ##
# sauver trace-eleve et parcours
# ##

proc sauver_trace_parcours {} {
  global glob heure_debut

  ## trace
  # utilisateur/classe/date/dur�e/didacticiel/nvignettes/version
  set eleve $glob(trace_user)
  set titre "mulot-$glob(version)"
  set categorie "$glob(theme) Deviner=$glob(boucle)"
    set heure_fin [clock seconds]
    set duree [expr int($heure_fin-$heure_debut)]
    set date_heure [clock format [clock seconds] -format "%c"]
    set quoi "classe \{$date_heure\} $duree"

  enregistreval $titre $categorie $quoi $eleve

}

# ####################### proc main ########################
proc main {c} {
  global glob nv im_choix case heure_debut v_placees
  # nv = numero de vignette
  # v_placees = nombre de vignettes placees

  $c delete all
  set v_placees 0

  ####################################################################

  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(theme) [gets $f]
  close $f

  wm title . "[mc Deviner] : $glob(theme)"

  ################### choix de l'image
  # choisir une image dans la liste im_choix(im_liste)
  set r [lindex $glob(images_selectionnees) $glob(boucle)]
  set im_choix(fname) [lindex $im_choix(im_liste) $r]
  set im_choix(image) [image create photo \
	-file [file join images $glob(dossier) $im_choix(fname)]]
  # dimensions de l'image choisie
  set im_choix(height) [image height $im_choix(image)]
  set im_choix(width) [image width $im_choix(image)]
  # dimensions des vignettes (image choisie + instrus)
  set im_choix(v_width) [expr $im_choix(width) / $glob(nvignettes)]
  set im_choix(v_height) [expr $im_choix(height) / $glob(nvignettes)]
  # dimensions des rectangles (caches de l'image choisie)
  set glob(r_width) [expr $im_choix(width) / $glob(nbcol)+1]
  set glob(r_height) [expr $im_choix(height) / $glob(nbrow)+1]
  
  ################### Creation des vignettes image choisie + 3 intruses ####
  # Les vignettes peuvent constituer des objets
  # independants, utile si on veut leur associer des comportements
  creer_3intrus
  image create photo image_aux
  for {set nv 0} {$nv < 4} {incr nv 1} {
    set r [lindex $glob(intrus) $nv]
    image create photo vignette($nv)
    image_aux copy [image create photo -file [file join images $glob(dossier) $r]] 
    vignette($nv) copy image_aux -subsample 5 5
    $c create image \
      [expr $glob(org) + int($im_choix(v_width)/2)] \
      [expr $glob(org) + int(($nv+0.5)*$im_choix(v_height))] \
      -image vignette($nv) -tags intrus($nv)
    $c bind intrus($nv) <Button-1> "analyse_reponse $nv"
  }
  #################### afficher/cacher l'image de fond ################
    $c create image \
  	[expr $glob(org) + $im_choix(v_width) + int($im_choix(width)/2)+1] \
  	[expr $glob(org) + int($im_choix(height)/2)+1] \
  	-image $im_choix(image)
    # on cache tout a l'aide de rectangles
    for {set i 0} {$i < $glob(nbcol)} {incr i 1} {
      for {set j 0} {$j < $glob(nbrow)} {incr j 1} {
        $c create rect \
          [expr $glob(org) + $im_choix(v_width) + ($i+0)*$glob(r_width) +1] [expr $glob(org) + ($j+0)*$glob(r_height) +1] \
	  [expr $glob(org) + $im_choix(v_width) + ($i+1)*$glob(r_width) +1] [expr $glob(org) + ($j+1)*$glob(r_height) +1] \
	  -fill $glob(rectcolor) -width 0 -tag rect($i,$j) 
	$c bind rect($i,$j) <Button-1> "show_rect $c $i $j"
      }
    }
    # on tire 1 rectangle au hasard et on montre dessous
      set i [expr int(rand()*$glob(nbcol))]
      set j [expr int(rand()*$glob(nbrow))]
      set case(colonne) $i
      set case(ligne) $j
      $c delete rect($i,$j)
    # nombre de rectangles restant a visualiser
    set glob(nbrect) [expr $glob(nbrow)*$glob(nbcol) -1]

  set glob(clics_rectangle) 0
  set glob(clics_vignette) 0
  set heure_debut [clock seconds]

} ;# fin main

##################################################################"

  # Relire le nom de l'utilisateur sous windows
  if {$glob(platform) == "windows"} {
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "r"]}
    gets $f glob(trace_user)
    close $f
  }

########################### On cree la fenetre principale###########################

# placement de la fenetre en haut et a gauche de l'ecran

wm resizable . 0 0
wm geometry . [expr [winfo screenwidth .]-10]x[expr [winfo screenheight .]-80]+0+0
. configure -bg $glob(bgcolor)

frame .frame -bg $glob(bgcolor)
pack .frame -side top -fill both -expand yes


# ####################### On cree un canvas #########################################
# charge d'accueillir les sorties graphiques,
# qui peuvent etre des images, des textes, des formes geometriques ...

set c .frame.c
canvas $c -width $glob(width) -height $glob(height) -bg $glob(bgcolor) -highlightbackground $glob(bgcolor)
pack $c -expand true 

# ##################################################################################"
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
  [image create photo fgauche -file [file join sysdata fgauche.png]] -command "recule; main $c"
grid .bframe.but_gauche -column [expr $glob(bouclemax)+2] -row 1
.bframe.but_gauche configure -state disable

label .bframe.lab3 -bg $glob(bgcolor) -font 12x24
grid .bframe.lab3 -column 5 -padx 20 -row 1

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
grid .bframe.but_quitter -column [expr $glob(bouclemax)+3] -row 1

selectionner_images

#####################################Gestion des evenements############################

set glob(nbrow) 3
set glob(nbcol) 3
set glob(nbmax) $glob(nbcol)
main $c
