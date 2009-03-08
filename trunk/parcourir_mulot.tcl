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
#  File  : $Id: parcourir_mulot.tcl,v 1.10 2006/03/25 00:55:15 abuledu_francois Exp $
#  Author  : andre.connes@wanadoo.fr
#  Date    : 10/07/2003 Modification : 16/02/2004
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
            
set ident $tcl_platform(user)

#charger la liste (triee) des images
  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(dossier) [gets $f]
  set limages [lsort [glob [file join images $glob(dossier) *.*]]]

if { $glob(platform) == "windows" } {
  set f [open [file join $glob(home_mulot) reglages trace_user] "r"]
  set glob(trace_user) [gets $f]
  close $f
}

# ##############################################################################

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

################################################

# ##
# sauver trace-eleve et parcours
# ##

proc sauver_trace_parcours {} {
  global glob heure_debut

  ## trace
  # utilisateur/classe/date/durée/didacticiel/difficulte/version
  set eleve $glob(trace_user)
  set titre "mulot-$glob(version)"
  set categorie "$glob(theme) Parcourir=$glob(boucle)"
    set heure_fin [clock seconds]
    set duree [expr int($heure_fin-$heure_debut)]
    set date_heure [clock format [clock seconds] -format "%c"]
    set quoi "classe \{$date_heure\} $duree" 
  enregistreval $titre $categorie $quoi $eleve

}

# ##############################################################################
# Dans un tableau de nc_piste colonnes et nr_piste lignes, 
# la piste est constitué de certaines cases lues dans un fichier
# ##############################################################################

proc max { a b } {
  if { $a > $b } {
    return $a
  } else {
    return $b
  }
}

proc avance {} {
  global glob
  set tmp [expr $glob(difficulte_piste) + 1 ]
  if [catch {set tmp_liste [glob pistes/$tmp.*]} resultat ] {
    # en cas d'erreur, on ne change pas la difficulté
  } else {
    if { [llength $tmp_liste ] > 0 } {
      set glob(difficulte_piste) $tmp
    }
  }
}

proc recule {} {
  global glob
  set glob(boucle) 0
  set glob(difficulte_piste) 0
  for {set i 1} {$i <= $glob(bouclemax)} {incr i 1} {
    .bframe.tete$i configure -image pneutre
  }
  .bframe.but_gauche configure -state disable
  selectionner_images
  
  set tmp 0
  if [catch {set tmp_liste [glob pistes/$tmp.*]} resultat ] {
    # en cas d'erreur, on ne change pas la difficulté
  } else {
    if { [llength $tmp_liste ] > 0 } {
      set glob(difficulte_piste) $tmp
    }
  }
}

proc extreme_case { c i } {
  global glob case
  if { $glob(sens) == 9 } {
    if { $case($i) == 1 } {
    # on va entrer en piste
     . configure -cursor target 
      set glob(precedent_piste) $i
      $c delete case($i)
      set case($i) 0
      if { $i == $glob(first_piste) } {
        set glob(sens) 1
      } else {
        set glob(sens) -1
      }
    } else {
      # rien (sinon erreur de code ?)
    }
  } else {
    if { $case($i) == 1 } {
      # on est déjà en piste : c'est l'autre bout !
      # vérifier la case précédente (fonction du sens !)
      set actuel [lsearch $glob(cases_piste) $i]
      set precedent [lsearch $glob(cases_piste) $glob(precedent_piste)]
      if { $glob(sens) == [expr $actuel - $precedent] } {
        # c'est bon, on continue
        # tout effacer
        # toutes les cases sont initialement vides
        for { set k 0 } { $k < [expr $glob(nr_piste) * $glob(nc_piste)] } { incr k } {
          $c delete case($k)
        }
        incr glob(boucle)
        .bframe.tete$glob(boucle) configure -image pbien
        sauver_trace_parcours
        . configure -cursor left_ptr
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
        # on n'est pas/plus en piste
      }
    } else {
      # rien (sinon erreur de code ?)
    }
  }
} ;# proc extreme_case

proc enterCase { c i } {
  global glob case
  if { $glob(sens) != 9 && $case($i) == 2 } {
  # on est en piste, vérifier la case précédente (fonction du sens !)
    set actuel [lsearch $glob(cases_piste) $i]
    set precedent [lsearch $glob(cases_piste) $glob(precedent_piste)]
    if { $glob(sens) == [expr $actuel - $precedent] } {
      # c'est bon, on continue
      set glob(precedent_piste) $i
      $c delete case($i)      
      set case($i) 0
    } else {
    # on n'est pas/plus en piste
    }
  }
} ;# proc enterCase

proc vider_case { c i } {
  global glob case
  set i_row [expr int($i / $glob(nc_piste))]
  set i_col [expr $i % $glob(nc_piste)]
  $c create rect \
	[expr $glob(org)+$i_col*$glob(width_case)+1] [expr $glob(org)+$i_row*$glob(width_case)+1] \
	[expr $glob(org)+($i_col+1)*$glob(width_case)+1] [expr $glob(org)+($i_row+1)*$glob(width_case)+1] \
	-fill $glob(bgcolor) -width 0 -tag case($i)
  set case($i) 9
}

proc remplir_case { c i } {
  global glob case
  set i_row [expr int($i / $glob(nc_piste))]
  set i_col [expr $i % $glob(nc_piste)]
  if { $i == $glob(first_piste) || $i == $glob(last_piste) } {
    $c create rect \
	[expr $glob(org)+$i_col*$glob(width_case)+1] [expr $glob(org)+$i_row*$glob(width_case)+1] \
	[expr $glob(org)+($i_col+1)*$glob(width_case)+1] [expr $glob(org)+($i_row+1)*$glob(width_case)+1] \
	-fill red -width 1 -tag case($i)
    $c bind case($glob(first_piste)) <Button-1> "extreme_case $c $glob(first_piste)"
    $c bind case($glob(last_piste)) <Button-1> "extreme_case $c $glob(last_piste)"
    set case($i) 1 ;#extrémité
  } else {
    $c create rect \
	[expr $glob(org)+$i_col*$glob(width_case)+1] [expr $glob(org)+$i_row*$glob(width_case)+1] \
	[expr $glob(org)+($i_col+1)*$glob(width_case)+1] [expr $glob(org)+($i_row+1)*$glob(width_case)+1] \
	-fill green -width 1 -tag case($i)
    $c bind case($i) <Enter> "enterCase $c $i"
    set case($i) 2 ;#non extrémité
  }
} ;# remplir_case

proc init_piste { c nom } {
  global glob limages case

  # afficher le fond, fonction de la boucle,
  # image de la liste des images sÃ©lectionnÃ©es

  set bgimg [lindex $limages [lindex $glob(images_selectionnees) $glob(boucle)]]

  set bgimage [image create photo -file $bgimg]
  set hbgimage [image height $bgimage]
  set wbgimage [image width $bgimage]

  set glob(width_case) [max \
	[expr int($wbgimage/$glob(nc_piste))] \
	[expr int($hbgimage/$glob(nr_piste))] ] 

  $c create image \
	[expr $glob(org) + int($wbgimage/2)+1] [expr $glob(org) + int($hbgimage/2)+1] -image $bgimage

  ####################Création du tableau###################################
  # On utilise des rectangles pour dessiner les cases
  # L'avantage des rectangles, c'est qu'ils peuvent constituer des objets
  # indépendants, utile si on veut leur associer des comportements

  # toutes les cases sont initialement vides
  for { set i 0 } { $i < [expr $glob(nr_piste) * $glob(nc_piste)] } { incr i } {
    vider_case $c $i
  } 

  # lire la liste des cases à remplir
  set f [ open [ file join $nom] "r"]
  set glob(cases_piste) [gets $f]
  close $f 

  # maintenant les cases de la piste
  # les extrémités de la piste
  set glob(first_piste) [lindex $glob(cases_piste) 0]
  set glob(last_piste) [lindex $glob(cases_piste) [expr [llength $glob(cases_piste)] - 1]]
  # remplir les cases de la piste
  foreach i $glob(cases_piste) {
    remplir_case $c $i
  } 
  # sens de parcours, dans la piste
  set glob(sens) 9 ;# sens de parcours 1=direct -1=indirect 9=undef

} ;# proc init_piste


# ####################### proc main ########################
proc main {c} {
  global glob heure_debut

  $c delete all

  set f [open [file join $glob(home_mulot) reglages dir_images.conf] "r"]
  set glob(theme) [gets $f]
  close $f

  set message ""; append message [mc "Parcourir"] " : " $glob(theme)
  wm title . $message

  set liste_pistes [glob pistes/$glob(difficulte_piste).*]
  set r [expr int(rand()*[llength $liste_pistes])]
  init_piste $c [lindex $liste_pistes $r]

  set heure_debut [clock seconds]

} ;# proc main

##################################################################"

  # Relire le nom regle de l'utilisateur sous windows
  if {$glob(platform) == "windows"} {
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "r"]}
    gets $f glob(trace_user)
    close $f
  }

########################### On cree la fenetre principale###########################

set glob(difficulte_piste) 0

# placement de la fenetre en haut et a gauche de l'ecran

wm resizable . 0 0
wm geometry . [expr [winfo screenwidth .]-10]x[expr [winfo screenheight .]-80]+0+0
. configure -background $glob(bgcolor)

frame .frame -bg $glob(bgcolor)
pack .frame -side top -fill both -expand yes


# #######################On cree un canvas###########################################
# charge d'accueillir les sorties graphiques,
# qui peuvent etre des images, des textes, des formes geometriques ...

set c .frame.c
canvas $c -width $glob(width) -height $glob(height) -bg $glob(bgcolor) -highlightbackground $glob(bgcolor)
pack $c -expand true 

# #################################################################################
# frame en bas avec boutons 'continuer', flèches gauche et droite, le score et une étiquette de controle de la réponse

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
grid .bframe.but_quitter -column  [expr $glob(bouclemax)+3]  -row 1

selectionner_images

main $c
