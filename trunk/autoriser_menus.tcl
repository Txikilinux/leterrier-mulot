#!/bin/sh
# autoriser_menus.tcl projet mulot
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
#  File  : $Id: autoriser_menus.tcl,v 1.6 2006/03/15 10:11:30 abuledu_andre Exp $
#  Author  : andre.connes@wanadoo.fr
#  Date    : 23/06/2003 Modification : 19/12/2004
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

set etat_menus [lindex $argv 0]

source mulot.conf
source msg.tcl
source etre_prof.tcl
source lanceapplication.tcl

  #
  # langue par defaut
  #
  set f [open [file join $glob(home_reglages) lang.conf] "r"]
  gets $f lang
  close $f
  ::msgcat::mclocale $lang
  ::msgcat::mcload [file join [file dirname [info script]] msgs]


########## Construction de l'interface###########
#frame wa pour autoriser ou pas les menus
wm geometry . +50+50
wm title . [mc Menus]

frame .wa -background #aaaaaa -height 420 -width 400
grid .wa -column 0 -row 0

#si abuledu nécessairement l'utilisateur est prof d'après mulot.tcl

set prof [est_prof]

if { ! $prof } {
  label .wa.passwd -text [mc Password] -bg red
  grid .wa.passwd -row 1 -column 0 -padx 4 -sticky e
  entry .wa.password -show "*"
  grid .wa.password -row 1 -column 1 -padx 4 -sticky e
  .wa.password delete 0 end
}

label .wa.lab_classe -text [mc class_name]
grid .wa.lab_classe -row 2 -column 0 -padx 4 -sticky e
entry .wa.ent_classe
grid .wa.ent_classe -row 2 -column 1 -padx 4 -sticky e

button .wa.ok -text [mc Faire] -command "autoriser $etat_menus ; lanceappli mulot.tcl 0"
grid .wa.ok -row 3 -column 0
button .wa.cancel -text [mc Annuler] -command exit
grid .wa.cancel -row 3 -column 1

proc existe_classe { n } {
  set cclasse "undef:"
  catch { set cclasse [exec grep $n [file join /etc/group]] }
  set classe [lindex [split $cclasse ":"] 0]
  if { $classe == "undef" } {
    return 0
  } else {
    return 1
  }
} ;# existe_classe

proc autoriser { e } {
#  global glob newdir
global glob prof

  # astuce : si abuledu le mot de passe est auto mais
  #          droits d'ajout pour les profs only
  if { $prof } {
    set groupe "leterrier"
    set pass $glob(passwd)
  } else {
    set groupe "root"
    set pass [.wa.password get]
  }
  set rdir $glob(home_reglages)
  if {[file exists /usr/share/abuledu/applications/abuledu-mulot]} {
    set rdir "/usr/share/abuledu/applications/abuledu-mulot/reglages"
  }
  if {[file exists /usr/share/abuledu-mulot]} {
    set rdir "/usr/share/abuledu-mulot/reglages"
  }
  if { [ok_passwd] } {
    set nom_classe [.wa.ent_classe get]
    if { [ existe_classe $nom_classe ] } {
      # file attributes ne fonctionne que pour le proprietaire du fichier
      # effacer d'abord puis creer
      file delete [file join $rdir boutons.$nom_classe]
      set f [open [file join $rdir boutons.$nom_classe] "w" ]
      puts $f $e
      close $f
#     file attributes [file join $rdir boutons.$nom_classe] -permissions 00775
      tk_messageBox -default ok -message "Ok" -parent .
    } else {
      tk_messageBox -default ok -message "La classe n'existe pas." -parent .
    }
  }
} ; #fin autoriser


proc ok_passwd {} {
  global glob prof
  
  if { $prof } {
    return 1
  } else {
    set pass [.wa.ent_passwd get]
    if { $pass == $glob(passwd) } {
      return 1
    } else {
      return 0
    }
  }
}
                                        
