#!/bin/sh
# mulot.tcl
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
#  File  : $Id: glisser_mulot.tcl,v 1.11 2007/01/18 08:51:11 abuledu_andre Exp $
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

  #
  # ordre
  #
  set f [open [file join $glob(home_reglages) ordre.conf] "r"]
  gets $f glob(ordre)
  close $f

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
  if { $glob(ordre) == "1" } {
    set glob(images_selectionnees) [lsort $glob(images_selectionnees)]
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
  set categorie "$glob(theme) Glisser-deposer=$glob(boucle)"
    set heure_fin [clock seconds]
    set duree [expr int($heure_fin-$heure_debut)]
    set date_heure [clock format [clock seconds] -format "%c"]
    set quoi "classe \{$date_heure\} $duree"
  enregistreval $titre $categorie $quoi $eleve

}

# ####################### proc main ########################
proc main {c} {
  global glob nv im_choix heure_debut v_placees
  # nv = numero de vignette
  # v_placees = nombre de vignettes placees

  $c delete all
  set v_placees 0

  ####################################################################

  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(theme) [gets $f]
  close $f

  set message ""; append message [mc "Glisser-déposer"] " : " $glob(theme)
  wm title . $message

  ################### choix de l'image
  # choisir une image dans la liste im_choix(im_liste)
  set r [lindex $glob(images_selectionnees) $glob(boucle)]
  set im_choix(fname) [lindex $im_choix(im_liste) $r]
  set im_choix(image) [image create photo \
	-file [file join images $glob(dossier) $im_choix(fname)]]
  # dim image choisie
  set im_choix(height) [image height $im_choix(image)]
  set im_choix(width) [image width $im_choix(image)]
  # dim des zones virtuelles
  # les dim des vignettes (par crop) et zones réelles sont plus petites
  # et fonction de glob(boucle) : plus on avance, plus c'est petit
  set im_choix(v_width) [expr $im_choix(width) / $glob(nvignettes)]
  set im_choix(v_height) [expr $im_choix(height) / $glob(nvignettes)]

  #################### afficher l'image de fond ###############################
  $c create image \
	[expr $glob(org) + $im_choix(v_width) + int($im_choix(width)/2)+1] \
	[expr $glob(org) + int($im_choix(height)/2)+1] \
	-image $im_choix(image)

  ################### liste des vignettes et des zones ##### ##############
  # im_choix(vignettes_liste) contiendra les numeros de vignettes
  # les premieres vignettes vont recouvrir les zonnes par glisser/deposer
  # les 2 dernieres sont des zones intruses

  set im_choix(vignettes_liste) {}
  while { [llength $im_choix(vignettes_liste)] < [expr $glob(nvignettes)+2+$glob(boucle)] } {
    set r [expr int(rand()*[expr $glob(nvignettes)*$glob(nvignettes)])]
    if { [lsearch $im_choix(vignettes_liste) $r] == -1 } {
      set im_choix(vignettes_liste) [linsert $im_choix(vignettes_liste) end $r]
    }
  }
   
  ####################Creation des vignettes et des zones #################
  # Les vignettes peuvent constituer des objets
  # independants, utile si on veut leur associer des comportements

  # on cree puis on affiche les vignettes et les zones

  for {set nv 0} { $nv < [expr $glob(nvignettes)+2+$glob(boucle)]} {incr nv 1} {
    set r [lindex $im_choix(vignettes_liste) $nv]
    # creer la zone
    set i [expr int($r/$glob(nvignettes))]
    set j [expr $r%$glob(nvignettes)]
    $c create rect [expr $glob(org) + ($i+1)*$im_choix(v_width)	+1+7*$glob(boucle)]\
	  [expr $glob(org) + $j*$im_choix(v_height) 		+1+7*$glob(boucle)] \
          [expr $glob(org) + ($i+2)*$im_choix(v_width) 		+1-7*$glob(boucle)] \
          [expr $glob(org) + ($j+1)*$im_choix(v_height) 	+1-7*$glob(boucle)] \
          -fill $glob(rectcolor) -width 0 -tag rect$nv 
  }
  for {set nv 0} { $nv < $glob(nvignettes)} {incr nv 1} {
    set r [lindex $im_choix(vignettes_liste) $nv]
	image create photo vignette($nv)
	vignette($nv) copy $im_choix(image) -from \
          [expr int($r/$glob(nvignettes))*$im_choix(v_width)	+7*$glob(boucle)] \
          [expr ($r%$glob(nvignettes))*$im_choix(v_height)	+7*$glob(boucle)] \
          [expr (int($r/$glob(nvignettes))+1)*$im_choix(v_width)-7*$glob(boucle)] \
          [expr (($r%$glob(nvignettes))+1)*$im_choix(v_height)	-7*$glob(boucle)]
	$c create image \
	  [expr $glob(org) + int($im_choix(v_width)/2)] \
	  [expr $glob(org) + $nv + $nv*$im_choix(v_height) + int($im_choix(v_height)/2)] \
	  -image vignette($nv) -tags drag$nv
  } ;# fin for

  set heure_debut [clock seconds]

} ;# fin proc main

# ################ Lorsque l'on clique sur la vignette (bouton appuye)
# On capture la position de la souris dans les variables lastX et lastY
# de meme que les coordonnees de la vignette 'sourcecoord'

proc itemStartDrag {c nv x y} {
  global lastX lastY sourcecoord
  set sourcecoord [$c coords current]
  set lastX [$c canvasx $x]
  set lastY [$c canvasy $y]
  . config -cursor fleur
}

# ############### D�placement de la vignette, gestion des coordonn�es avec lastX et lastY
proc itemDrag {c nv x y} {
  global lastX lastY glob im_choix
  set x [$c canvasx $x]
  set y [$c canvasy $y]
  # On deplace le tag 'drag$nv'
  $c move drag$nv [expr $x-$lastX] [expr $y-$lastY]
  set lastX $x
  set lastY $y
}

# ################ On rel�che la souris.
# traitement et l'analyse de la position de la vignette

proc itemStopDrag {c nv x y} {
  global glob im_choix sourcecoord  v_placees

  . config -cursor left_ptr

  set coord [$c coords current]
  #On calcule le num�ro de la case o� se trouve la vignette (xc et yc)
  set xc [expr int(([lindex $coord 0] - $im_choix(v_width))/$im_choix(v_width))]
  set yc [expr int([lindex $coord 1]/$im_choix(v_height))]

  set r [lindex $im_choix(vignettes_liste) $nv]
  set xindice [expr int($r/$glob(nvignettes))]
  set yindice [expr $r%$glob(nvignettes)]

  # On teste si la vignette est dans une case valide

  if {$xc==$xindice && $yc==$yindice} {
    # Si oui, on deplace le personnage             
      set i [$c find withtag drag$nv] 
      $c coords $i \
	[expr $glob(org) + $im_choix(v_width) + $xc*$im_choix(v_width) + int($im_choix(v_width)/2)] \
	[expr $glob(org) + $yc*$im_choix(v_height) + int($im_choix(v_height)/2)]
      $c itemconf $i -tag dead
      vignette($nv) blank
      image delete vignette($nv)
      $c delete rect$nv
    incr v_placees 1    
    if { $v_placees <  $glob(nvignettes) } {
      return 
    } else {
      for { set nv 4 } { $nv < [expr $glob(nvignettes)+2+$glob(boucle)] } { incr nv} {
        $c delete rect$nv
      }
      incr glob(boucle)
      .bframe.tete$glob(boucle) configure -image pbien
      sauver_trace_parcours

      after [expr $glob(attente) * 1000] {
	if { $glob(boucle) < $glob(bouclemax) } {
	  set glob(session_finie) non
	} else {
          set glob(session_finie) oui
	} 
      }

      vwait glob(session_finie)

      if { $glob(session_finie) == "non" } {
        main $c
      } else {
        .bframe.but_gauche configure -state normal
        set sourires [glob [file join $glob(fin_sequence) sourire*.png]]
        set r [expr int(rand()*[llength $sourires])]     
        $c create image [expr int($glob(width)/2)] [expr int($glob(height)/2)]\
          -image [image create photo -file [file join [lindex $sourires $r]]]
        # exit
      }

      return 
    } 
  } else {
    # gestion des erreurs
    bell
    set i [$c find withtag drag$nv]
    $c coords $i [lindex $sourcecoord 0] [lindex $sourcecoord 1]
  }
}


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


# #######################On cree un canvas###########################################
# charge d'accueillir les sorties graphiques,
# qui peuvent etre des images, des textes, des formes geometriques ...

set c .frame.c
canvas $c -width $glob(width) -height $glob(height) -bg $glob(bgcolor) -highlightbackground $glob(bgcolor)
pack $c -expand true 

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
# On associe la gestion des deplacements avec l'item portant le tag 'drag$nv'
# %x %y recuperent la position de la souris.  

for { set i 0 } { $i < $glob(nvignettes) } { incr i 1 } {
  $c bind drag$i <ButtonRelease-1> "itemStopDrag $c $i %x %y"
  $c bind drag$i <1> "itemStartDrag $c $i %x %y"
  $c bind drag$i <B1-Motion> "itemDrag $c $i %x %y"
}

main $c
