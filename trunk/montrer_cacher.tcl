#!/bin/sh
# montrer_cacher.tcl
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
#  Author  : andre.connes@wanadoo.fr
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

set WHAUTEUR 358
set WLARGEUR 450

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
wm title . [mc montrer_cacher]

set dir_origine [pwd]

frame .wg -background #aaaaaa -height $WHAUTEUR -width $WLARGEUR
grid .wg -column 0 -row 0

##
## abandon
##

button .wg.abandonner -text [mc Quitter] \
	-command "cd \"$dir_origine\" ; lanceappli mulot.tcl"
place .wg.abandonner -x [expr $WLARGEUR - 80] -y 7


##
## gerer les dossiers
##

  set racine "[pwd]/images" ;# chez l'utilisateur par defaut

    set tdir "/usr/share/abuledu-mulot/images"
    if { [file exists $tdir] } {
      set racine $tdir ;# debian et/ou abuledu-debian
    }

##
## gerer les dossiers a montrer
##

label .wg.lab_montrer -background green -text [mc a_montrer]
place .wg.lab_montrer -x 20 -y 43
label .wg.lab_selection_deplacer -background yellow -text [mc selections_deplacer]
place .wg.lab_selection_deplacer -x 70 -y 63

listbox .wg.listb_montrer \
  -background #c0c0c0 -height 14 -width 20 \
  -listvariable listeDossiers_montrer \
  -selectmode extended \
  -yscrollcommand ".wg.scroll_montrer set"

scrollbar .wg.scroll_montrer -command ".wg.listb_montrer yview"
place .wg.listb_montrer -x 20 -y 88
place .wg.scroll_montrer -x 167 -y 88 -height 243

bind .wg.listb_montrer <Double-ButtonRelease-1> "capture_montrer"

button .wg.but_cacher -text " -> " -command "cacher_dossiers"
place .wg.but_cacher -x 200 -y 150

##
## gerer les dossiers a cacher
##

label .wg.lab_cacher -background green -text [mc a_cacher]
place .wg.lab_cacher -x 260 -y 43
#label .wg.lab_cacher_selection -background yellow -text [mc selections]
#place .wg.lab_cacher_selection -x 260 -y 63

listbox .wg.listb_cacher \
  -background #c0c0c0 -height 14 -width 20 \
  -listvariable listeDossiers_cacher \
  -selectmode extended \
  -yscrollcommand ".wg.scroll_cacher set"

scrollbar .wg.scroll_cacher -command ".wg.listb_cacher yview"
place .wg.listb_cacher -x 260 -y 88
place .wg.scroll_cacher -x 407 -y 88 -height 243

bind .wg.listb_cacher <Double-ButtonRelease-1> "capture_cacher"

button .wg.but_montrer -text " <- " -command "montrer_dossiers"
place .wg.but_montrer -x 200 -y 180


###########################################################################################
#
#  procedures et fonctions
#
###########################################################################################

############################################
#
#  gestion des dossiers a montrer
#
############################################

#
proc creer_montrer { } {
#
  global racine
  .wg.listb_montrer delete 0 end
  catch {foreach i [lsort [glob -nocomplain $racine/* ]] { 
          .wg.listb_montrer insert end [file tail $i]
         }
  }
}

creer_montrer

#
proc cacher_dossiers {} {
#
  global glob racine listeDossiers_montrer
  if { ! $glob(autorise) } {
    tk_messageBox -type ok -message [mc pbpasse] -parent .
  } else {
    set lselection [.wg.listb_montrer curselection]
    if { [llength $lselection] == 0 } {
      tk_messageBox -type ok -message [mc "pas_de_selection_dossiers"] -parent .
      return
    }
    foreach i $lselection {
      set cache [lindex $listeDossiers_montrer $i]
      file rename $racine/$cache $racine/.$cache
    }      
    creer_cacher
    creer_montrer
    #tk_messageBox -type ok -message [mc Fait] -parent .
  }
  
}

############################################
#
#  gestion des dossiers a cacher
#
############################################

#
proc creer_cacher { } {
#
  global racine
  .wg.listb_cacher delete 0 end
  catch {foreach i [lsort [glob -nocomplain $racine/.* ]] { 
	    set taili [file tail $i]
            if { $taili != "." && $taili != ".." && $taili != ".svn" } {
              .wg.listb_cacher insert end [string range $taili 1 end]
            }
         }
  }
}

creer_cacher

#
proc montrer_dossiers {} {
#
  global glob racine listeDossiers_cacher
  if { ! $glob(autorise) } {
    tk_messageBox -type ok -message [mc pbpasse] -parent .
  } else {
    set lselection [.wg.listb_cacher curselection]
    if { [llength $lselection] == 0 } {
      tk_messageBox -type ok -message [mc "pas_de_selection_dossiers"] -parent .
      return
    }
    foreach i $lselection {
      set cache [lindex $listeDossiers_cacher $i]
      file rename $racine/.$cache $racine/$cache
    }      
    creer_cacher
    creer_montrer
    #tk_messageBox -type ok -message [mc Fait] -parent .
  }
  
}

