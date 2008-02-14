#!/bin/sh
#eval.tcl
# \
exec wish "$0" ${1+"$@"}

#*************************************************************************
#  Copyright (C) 2002 Eric Seigne <erics@rycks.com>
#  Copyright (C) 2003 André Connes <andre.connes@toulouse.iufm.fr>
#  Copyright (C) 2003 David Lucardi <davidlucardi@aol.com>
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
#  File  : $Id: eval.tcl,v 1.7 2006/01/27 16:59:21 abuledu_andre Exp $
#  Author  : davidlucardi@aol.com
#  Modifier: 
#  Date    : 
#  Licence : GNU/GPL Version 2 ou plus
# 
#  Description:
#  ------------
# 
#  @version    
#  @author     David Lucardi
#  @modifier   
#  @project    Le terrier
#  @copyright  Eric Seigne 
# 
#  *************************************************************************

proc enregistreval {titre categorie quoi user} {

  global tcl_platform

  set listeval \173$titre\175\040$categorie\040\173$quoi\175
  if { $tcl_platform(platform) == "unix" } {
    set f [open [file join $user.log] "a+"]
    puts $f $listeval
    close $f
  } elseif { $tcl_platform(platform) == "windows" } {
    set f [open [file join $user.log] "a+"]
    puts $f $listeval
    close $f
  } else {
  }
  
  # sauvegarder au format csv
  set nom [lindex [split $user "/"] end]
  set l_categorie [split $categorie " "]
  set theme         [lindex $l_categorie 0]
  set action_niveau [lindex $l_categorie 1]
  set l_action_niveau [split $action_niveau "="]
  set action [lindex $l_action_niveau 0]
  set niveau [lindex $l_action_niveau 1]
  set p1 [string first "{" $quoi]
  set p2 [string last "}" $quoi]
  set classe [string range $quoi 0 [expr $p1-1]] 
  set dh     [string range $quoi [expr $p1+1] [expr $p2-1]]
  set duree  [string range $quoi [expr $p2+1] end]
  set ligneval "$nom;$titre;$theme;$action;$niveau;$classe;$dh;$duree"
  if { $tcl_platform(platform) == "unix" } {
    if { ![file exists [file join $user.csv]] } { 
      set f [open [file join $user.csv] "a"]
      puts $f "Nom;Logiciel;Thème;Action;Niveau;Classe;Date/heure;Durée"
      close $f
    }
    set f [open [file join $user.csv] "a+"]
    puts $f $ligneval
    close $f
  } elseif { $tcl_platform(platform) == "windows" } {
    if { ![file exists [file join $user.csv]] } { 
      set f [open [file join $user.csv] "a"]
      puts $f "Nom;Logiciel;Thème;Action;Niveau;Classe;Date/heure;Durée"
      close $f
    }
    set f [open [file join $user.csv] "a+"]
    puts $f $ligneval
    close $f
  } else {
  }
}




