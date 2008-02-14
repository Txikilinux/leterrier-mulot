#!/bin/sh
# ajouter_images.tcl
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
#  File  : $Id: ajouter_images.tcl,v 1.6 2006/03/15 10:11:30 abuledu_andre Exp $
#  Author  : andre.connes@toulouse.iufm.fr
#  Date    : 23/06/2003 Modification : 01/12/2004
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
source lanceapplication.tcl
source etre_prof.tcl

set WHAUTEUR 658
set WLARGEUR 650

  #
  # langue par defaut
  #
  set f [open [file join $glob(home_reglages) lang.conf] "r"]
  gets $f lang
  close $f
  ::msgcat::mclocale $lang
  ::msgcat::mcload [file join [file dirname [info script]] msgs]

########## Construction de l'interface###########
#frame wg pour gerer les images
wm geometry . +0+0
wm title . [mc gerer_images]

set dir_origine [pwd]

frame .wg -background #aaaaaa -height $WHAUTEUR -width $WLARGEUR
grid .wg -column 0 -row 0

##
## mot de passe
##

#si abuledu necessairement l'utilisateur est prof d'apres mulot.tcl
set pass $glob(passwd)

set prof [est_prof]

if { ! $prof } {
  label .wg.lab_passwd -text [mc Password] -bg red
  place .wg.lab_passwd -x 150 -y 10
  entry .wg.ent_passwd -show "*"
  place .wg.ent_passwd -x 250 -y 10
  .wg.ent_passwd delete 0 end
}

##
## abandon
##

button .wg.abandonner -text [mc Quitter] \
	-command "cd \"$dir_origine\" ; lanceappli mulot.tcl"
place .wg.abandonner -x [expr $WLARGEUR - 80] -y 7


##
## gerer les images : racines supposee de la source, reelle pour le but (?)
##

  set racine_dirIn  $glob(home) ;# source chez l'utilisateur par defaut
  set racine_dirOut "[pwd]/images" ;# but par defaut mais...

  set tdir "/usr/share/abuledu/applications/abuledu-mulot/images"
  if { [file exists $tdir] } { 
    set racine_dirOut $tdir ;# mandrake et/ou abuledu-mdk
  } else {
    set tdir "/usr/share/abuledu-mulot/images"
    if { [file exists $tdir] } {
      set racine_dirOut $tdir ;# debian et/ou abuledu-debian
    }
  }

##
## gerer les images : ajouter un dossier
##

label .wg.lab_ajouter_dossier -background green -text [mc ajouter_dossier]
place .wg.lab_ajouter_dossier -x 20 -y 43
label .wg.lab_ajouter_dossier_quelNom -background yellow -text [mc dir_name]
place .wg.lab_ajouter_dossier_quelNom -x 20 -y 63
entry .wg.ent_ajouter_dossier_leNom
place .wg.ent_ajouter_dossier_leNom -x 20 -y 93

button .wg.but_ajouter_dossier -text [mc Ajouter] -command "ajouter_dossier"
place .wg.but_ajouter_dossier -x 40 -y 120


##
## gerer les images : effacer des dossiers
##

  set ed_dirOut $racine_dirOut

label .wg.lab_effacer_dossiers -background green -text [mc effacer_dossiers]
place .wg.lab_effacer_dossiers -x 20 -y 370
label .wg.lab_effacer_dossiers_selection -background yellow -text [mc selections]
place .wg.lab_effacer_dossiers_selection -x 20 -y 390

listbox .wg.listb_effacer_dossiers \
  -background #c0c0c0 -height 10 -width 20 \
  -listvariable listeDossiers_ed \
  -selectmode extended \
  -yscrollcommand ".wg.scroll_effacer_dossiers set"

scrollbar .wg.scroll_effacer_dossiers -command ".wg.listb_effacer_dossiers yview"
place .wg.listb_effacer_dossiers -x 20 -y 447
place .wg.scroll_effacer_dossiers -x 167 -y 447 -height 172

bind .wg.listb_effacer_dossiers <Double-ButtonRelease-1> "capture_ed"

button .wg.but_effacer_dossiers -text [mc Effacer] -command "effacer_dossiers"
place .wg.but_effacer_dossiers -x 40 -y 625

##
## gerer les images : ajouter des images
##

label .wg.lab_ajouter_images -background green -text [mc ajouter_images]
place .wg.lab_ajouter_images -x 300 -y 43
label .wg.lab_ajouter_images_5minimum -background yellow -text [mc  minimum5]
place .wg.lab_ajouter_images_5minimum -x 300 -y 63

  set ai_dirIn  $racine_dirIn
  set ai_dirOut $racine_dirOut

button .wg.but_ajouter_images_dirIn -text [mc dossier_in] ;# message initial
place .wg.but_ajouter_images_dirIn -x 300 -y 90
bind .wg.but_ajouter_images_dirIn <ButtonRelease-1> changer_ai_dirIn

listbox .wg.listb_ajouter_images_dirIn \
	-background #c0c0c0 -height 12 -width 20 \
	-listvariable listeImages \
	-selectmode extended \
	-yscrollcommand ".wg.scroll_ajouter_images_dirIn set"

scrollbar .wg.scroll_ajouter_images_dirIn -command ".wg.listb_ajouter_images_dirIn yview"
place .wg.listb_ajouter_images_dirIn -x 300 -y 120
place .wg.scroll_ajouter_images_dirIn -x 447 -y 120 -height 210

bind .wg.listb_ajouter_images_dirIn <Double-ButtonRelease-1> "capture_ai_dirIn"

button .wg.but_ajouter_images_dirOut -text [mc dossier_out] ;# message intial
place .wg.but_ajouter_images_dirOut -x 475 -y 90
bind .wg.but_ajouter_images_dirOut <ButtonRelease-1> changer_ai_dirOut

listbox .wg.listb_ajouter_images_dirOut \
	-background #c0c0c0 -height 12 -width 20 \
	-listvariable listeImages_out \
	-selectmode single \
	-yscrollcommand ".wg.scroll_ajouter_images_dirOut set"
scrollbar .wg.scroll_ajouter_images_dirOut -command ".wg.listb_ajouter_images_dirOut yview"
place .wg.listb_ajouter_images_dirOut -x 475 -y 120
place .wg.scroll_ajouter_images_dirOut -x 622 -y 120 -height 210

bind .wg.listb_ajouter_images_dirOut <Double-ButtonRelease-1> "capture_ai_dirOut"

button .wg.but_ajouter_images_faire -text [mc Ajouter ] -command "ajouter_images"
place .wg.but_ajouter_images_faire -x 420 -y 333

##
## gerer les images : effacer des images
##

label .wg.lab_effacer_images -background green -text [mc effacer_images]
place .wg.lab_effacer_images -x 300 -y 370
label .wg.lab_effacer_images_5minimum -background yellow -text [mc  minimum5]
place .wg.lab_effacer_images_5minimum -x 300 -y 390

  set ei_dirOut $racine_dirOut

button .wg.but_effacer_images_dirOut -text [mc dossier_out] ;# message intial
place .wg.but_effacer_images_dirOut -x 300 -y 417
bind .wg.but_effacer_images_dirOut <ButtonRelease-1> changer_ei_dirOut

listbox .wg.listb_effacer_images \
	-background #c0c0c0 -height 10 -width 20 \
	-listvariable listeImages_ei \
	-selectmode extended \
	-yscrollcommand ".wg.scroll_effacer_images set"
scrollbar .wg.scroll_effacer_images -command ".wg.listb_effacer_images yview"
place .wg.listb_effacer_images -x 300 -y 447
place .wg.scroll_effacer_images -x 447 -y 447 -height 172

bind .wg.listb_effacer_images <Double-ButtonRelease-1> "capture_ei"

button .wg.but_effacer_images -text [mc Effacer ] -command "effacer_images"
place .wg.but_effacer_images -x 420 -y 625

###########################################################################################
#
#  procedures et fonctions
#
###########################################################################################

proc get_passwd {} {
  global glob
  if {[file exists /etc/abuledu]} {
    set pass $glob(passwd)
  } else {
    set pass [.wg.ent_passwd get]
  }
  return $pass
}

############################################
#
#  gestion des dossiers : ajouter
#
############################################

#
proc ajouter_dossier {} {
#
  global glob prof racine_dirOut ai_dirOut 

  if { $prof } {
    set nom_dossier [.wg.ent_ajouter_dossier_leNom get]
    if { [string length $nom_dossier] == 0 } {
      tk_messageBox -default ok -message [mc "pas_de_nom"] -parent .
      return
    } 
    catch { file mkdir [file join $racine_dirOut $nom_dossier] }
    # maj de la liste de zones effacer_dossiers, ajouter_images et effacer_images
    creer_ed_listOut
    creer_ai_listOut
    creer_ei_listOut
    tk_messageBox -default ok -message [mc Fait] -parent .
  } else {
    tk_messageBox -type ok -message [mc pbpasse] -parent .
  }
}; #fin ajouter_dossier
                                                
############################################
#
#  gestion des dossiers : effacer
#
############################################

#
proc creer_ed_listOut { } {
#
  global ed_dirOut
  .wg.listb_effacer_dossiers delete 0 end
  catch {foreach i [lsort [glob -nocomplain $ed_dirOut/* ]] { 
          .wg.listb_effacer_dossiers insert end [file tail $i]
         }
  }
}

creer_ed_listOut

#
proc effacer_dossiers {} {
#
  global glob prof ed_dirOut listeDossiers_ed
  if { ! $prof } {
    tk_messageBox -type ok -message [mc pbpasse] -parent .
  } else {
    set lselection [.wg.listb_effacer_dossiers curselection]
    if { [llength $lselection] == 0 } {
      tk_messageBox -type ok -message [mc "pas_de_selection"] -parent .
      return
    }
    foreach i $lselection {
      file delete -force $ed_dirOut/[lindex $listeDossiers_ed $i]
    }      
    creer_ed_listOut
    creer_ai_listOut
    creer_ei_listOut
    tk_messageBox -type ok -message [mc Fait] -parent .
  }
  
}

############################################
#
#  gestion des images : ajouter
#
############################################

#
proc changer_ai_dirIn {} {
#
  global ai_dirIn
  set tmp_ai_dirIn $ai_dirIn
  cd $ai_dirIn
  if {[catch { tk_chooseDirectory -initialdir $tmp_ai_dirIn]} r ]} {
    set ai_dirIn $tmp_ai_dirIn
  } else { 
    if { [string length $r] > 0 } {
      set ai_dirIn $r
    } else {
      set ai_dirIn $tmp_ai_dirIn
    }
  }
  set text_dirIn [file tail $ai_dirIn]
  .wg.but_ajouter_images_dirIn configure -text $text_dirIn
  cd $ai_dirIn
  creer_ai_listIn
} ;# changer_ai_dirIn

#
proc changer_ai_dirOut {} {
#
  global ai_dirOut
  set tmp_ai_dirOut $ai_dirOut
  set curDir [pwd]
  cd $ai_dirOut
  if {[catch { tk_chooseDirectory -initialdir $tmp_ai_dirOut]} r ]} {
    set ai_dirOut $tmp_ai_dirOut
  } else { 
    if { [string length $r] > 0 } {
      set ai_dirOut $r
    } else {
      set ai_dirOut $tmp_ai_dirOut
    }
  }
  cd $ai_dirOut
  set but_text_out [file tail $ai_dirOut]
  .wg.but_ajouter_images_dirOut configure -text $but_text_out
  creer_ai_listOut
  cd $curDir
} ;# changer_ai_dirOut

#
proc creer_ai_listIn { } {
#
  global ai_dirIn
  .wg.listb_ajouter_images_dirIn delete 0 end
  catch {foreach i [lsort [glob -nocomplain $ai_dirIn/* ]] {
          .wg.listb_ajouter_images_dirIn insert end [file tail $i]
        }
  }
}
creer_ai_listIn

#
proc creer_ai_listOut { } {
#
  global ai_dirOut
  .wg.listb_ajouter_images_dirOut delete 0 end
  catch {foreach i [lsort [glob -nocomplain $ai_dirOut/* ]] {
          .wg.listb_ajouter_images_dirOut insert end [file tail $i]
        }
  }
}
creer_ai_listOut

#
proc capture_ai_dirIn { } {
#
    global ai_dirIn
    set tmp "$ai_dirIn/[.wg.listb_ajouter_images_dirIn get active]"
    if [file isdirectory $tmp] {
      set ai_dirIn $tmp
      cd $ai_dirIn
      .wg.but_ajouter_images_dirIn configure -text [file tail $tmp]
      creer_ai_listIn
    }
}

#
proc capture_ai_dirOut { } {
#
    global ai_dirOut
    set tmp "$ai_dirOut/[.wg.listb_ajouter_images_dirOut get active]"
    if [file isdirectory $tmp] {
      set ai_dirOut $tmp
      cd $ai_dirOut
      .wg.but_ajouter_images_dirOut configure -text [file tail $tmp]
      creer_ai_listOut
    }
}

proc ajouter_images {} {
  global glob prof ai_dirIn ai_dirOut listeImages

  if {  $prof } {
    set lselection [.wg.listb_ajouter_images_dirIn curselection]
    if { [llength $lselection] == 0 } {
      tk_messageBox -type ok -message [mc "pas_de_selection"] -parent .
      return
    }
    foreach i $lselection {
      set img [lindex $listeImages $i]
      catch { file copy $ai_dirIn/$img $ai_dirOut }
      if { $glob(platform) == "unix" } {
        catch {exec mogrify -geometry 600x400 $ai_dirOut/$img }
      }
      creer_ei_listOut
    }
    tk_messageBox -default ok -message "Ok" -parent .
  } else {
    tk_messageBox -type ok -message [mc pbpasse] -parent .
  }
  creer_ai_listOut
} ;# ajouter_images

############################################
#
#  gestion des images : effacer
#
############################################

#
proc changer_ei_dirOut {} {
#
  global ei_dirOut
  set tmp_ei_dirOut $ei_dirOut
  set curDir [pwd]
  cd $ei_dirOut
  if {[catch { tk_chooseDirectory -initialdir $tmp_ei_dirOut]} r ]} {
    set ei_dirOut $tmp_ei_dirOut
  } else { 
    if { [string length $r] > 0 } {
      set ei_dirOut $r
    } else {
      set ei_dirOut $tmp_ei_dirOut
    }
  }
  cd $ei_dirOut
  set but_text_out [file tail $ei_dirOut]
  .wg.but_effacer_images_dirOut configure -text $but_text_out
  creer_ei_listOut
  cd $curDir
} ;# changer_ei_dirOut

#
proc creer_ei_listOut { } {
#
  global ei_dirOut
  .wg.listb_effacer_images delete 0 end
  catch {foreach i [lsort [glob -nocomplain $ei_dirOut/* ]] { 
          .wg.listb_effacer_images insert end [file tail $i]
         }
  }
}

creer_ei_listOut

#
proc capture_ei { } {
#
    global ei_dirOut
    set tmp "$ei_dirOut/[.wg.listb_effacer_images get active]"
    if [file isdirectory $tmp] {
      set ei_dirOut $tmp
      cd $ei_dirOut
      .wg.but_effacer_images_dirOut configure -text [file tail $tmp]
      creer_ei_listOut
    }
}

proc effacer_images {} {
  global glob prof ei_dirOut listeImages_ei

  if { $prof } {
    set lselection [.wg.listb_effacer_images curselection]
    if { [llength $lselection] == 0 } {
      tk_messageBox -type ok -message [mc "pas_de_selection"] -parent .
      return
    }
    foreach i $lselection {
      set img [lindex $listeImages_ei $i]
      file delete $ei_dirOut/$img
    }
    creer_ai_listOut
    tk_messageBox -default ok -message "Ok" -parent .
  } else {
    tk_messageBox -type ok -message [mc pbpasse] -parent .
  }
  creer_ei_listOut
} ;# effacer_images

