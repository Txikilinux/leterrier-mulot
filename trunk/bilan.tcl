#!/bin/sh
#bilan.tcl
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
#  File  : $$$
#  AUthor  : andre.connes@wanadoo.fr
#  Date    : 13/08/2004 modification :
#  Licence : GNU/GPL Version 2 ou plus
#
#  Description:
#  ------------
#
#  @version
#  @author     Andre Connes
#  @project    Le terrier
#  @copyright  Eric Seigne 24/04/2002
#
#  *************************************************************************

source mulot.conf
source msg.tcl
source fonts.tcl
source lanceapplication.tcl

# les sentinelles sont en place

  #
  # langue par defaut
  #
  set f [open [file join $glob(home_reglages) lang.conf] "r"]
  gets $f lang
  close $f
  ::msgcat::mclocale $lang
  ::msgcat::mcload [file join [file dirname [info script]] msgs]

. configure -width 640 -height 480
wm geometry . +0+0
wm title . [mc bilan]
#
# initialiser la tableau des resultats
#

  #charger la liste des dossiers images (peu importe le type ?)
  set all_img_dirs [glob -directory images -type d *]
  # verification : non CVS et plus de glob(bouclemax) images par thème
  foreach tmp_img_dir $all_img_dirs {
    if { $tmp_img_dir != "images/CVS" } {
      lappend glob(img_dirs) [file tail $tmp_img_dir]
    }
  }
  set glob(img_dirs) [lsort $glob(img_dirs)]
  set glob(themes) $glob(img_dirs)
  
foreach theme $glob(themes) {
  foreach action [list Survoler Cliquer Double-cliquer Parcourir Glisser-deposer Deviner] {
    set table($theme,$action,nbfois) 0
    set table($theme,$action,niveau_max) 0
    set table($theme,$action,duree_cumul) 0
    set table($theme,$action,duree_min) 3600
    set table($theme,$action,duree_max) 0
  }
}
#
#frame pour les boutons de choix : eleve classe tous
#
frame .top -borderwidth 10
pack .top -side top -fill x
#
#creer les boutons
#
  # Relire le nom réglé de l'utilisateur sous windows
  if {$glob(platform) == "windows"} {
    catch {set f [open [file join $glob(home_mulot) reglages trace_user] "r"]}
    gets $f glob(trace_user)
    close $f
  }
set env(USER) [lindex [split $glob(trace_user) / ] end]
if { [file exists [file join $glob(trace_dir) $env(USER).log]] } {
  button .top.but_lemien -text [mc mon_bilan] -command mon_bilan
} else {
  button .top.but_lemien -state disable -text [mc mon_bilan]
}
if {[lsearch [exec id -nG $env(USER)] "leterrier"] != -1} {
  button .top.but_eleve -text [mc eleve] -command bilan_eleve
  button .top.but_classe -state disable -text [mc classe] -command bilan_classe
  button .top.but_tous -text [mc tous] -command bilan_tous
} elseif { $glob(platform) == "windows" } {
  button .top.but_eleve -state disable -text [mc eleve] -command bilan_eleve
  button .top.but_classe -state disable -text [mc classe] -command bilan_classe
  button .top.but_tous -state disable -text [mc tous] -command bilan_tous
} else {
  button .top.but_eleve -state disable -text [mc eleve] -command bilan_eleve
  button .top.but_classe -state disable -text [mc classe] -command bilan_classe
  button .top.but_tous -text [mc tous] -command bilan_tous
}
button .top.but_efface -state disabled -text [mc effacer_bilan] -command efface_bilan
button .top.but_exit -text [mc Fermer] -command "lanceappli mulot.tcl 0"
pack .top.but_lemien .top.but_eleve .top.but_classe .top.but_tous .top.but_efface .top.but_exit -side left

set bg #ffffff

frame .t
text .t.text -yscrollcommand {.t.scroll set} -setgrid true \
-width 80 -height 25 -wrap word -background $bg
scrollbar .t.scroll -command {.t.text yview}
pack .t.scroll -side right -fill y
pack .t.text -side left -fill both -expand true
pack .t -side top -fill both -expand true

frame .b
pack .b -side bottom -fill x

############################################################
proc bilan {qui} {
  global env glob table fnom file
  set curdir [pwd]
  cd $glob(trace_dir)
  set file $qui
  if {[catch { set f [open [file join $file] "r" ] }] } {
        exit
  }
  close $f
  #
  # on recupere le nom
  #
  set xnom [lindex [split $file "/"] end]
  set ppoint [string last "." $xnom]
  set nom [string range $xnom 0 [expr $ppoint-1]]
  #
  # lire et traiter les resulats
  #
  set f [open [file join $file] "r" ]
  while {![eof $f]} {
    set ligne [gets $f]
    if { [string length $ligne] > 0 } {
      #
      # afficher les traces brutes ligne a ligne
      #
      
      # decomposition des traces
      #set titre [lindex $ligne 0]
      set theme [lindex $ligne 1]
      catch {
        set action [lindex [split [lindex $ligne 2] "="] 0]
        set niveau [lindex [split [lindex $ligne 2] "="] 1]
        incr table($theme,$action,nbfois)
        if { $niveau > $table($theme,$action,niveau_max) } {
          set table($theme,$action,niveau_max) $niveau
        }
        set d [lindex [lindex $ligne 3] 2]
        if { $d < $table($theme,$action,duree_min)} {
          set table($theme,$action,duree_min) $d
        }
        if { $d > $table($theme,$action,duree_max)} {
          set table($theme,$action,duree_max) $d
        }
        incr table($theme,$action,duree_cumul) $d
      }
    }
  }
  close $f

  #
  # synthetiser
  # ecrire le bilan.html dans $glob(trace_dir)/nom_utilisateur.bilan.html
  #
  set fnom [file join [pwd] $nom.bilan]
  set f [open $fnom.html w]
  set ojourdui [clock format [clock seconds] -format "%d/%m/%Y"]
  puts $f "<html>\n<head>\n<title>Bilan Mulot de $nom</title>\n</head><body>\n"
  puts $f "<h2> $ojourdui - [mc synthese] : $nom </h2>"
  puts $f "<table border='1' cellspacing='0'>\n<tr>\n<th>[mc categorie]</th>"
  puts $f "<th>[mc n_fois]</th><th>[mc niveau_atteint]</th>"
  puts $f "<th>[mc duree_min]</th><th>[mc duree_moyenne]</th><th>[mc duree_max]</th></tr>"

  foreach theme $glob(themes) {
    foreach action [list Survoler Cliquer Double-cliquer Parcourir Glisser-deposer Deviner] {
      if { $table($theme,$action,nbfois)>0 } {
        puts $f "<tr>"
        puts $f "<td>$theme $action</td>"
        puts $f "<td align='center'>$table($theme,$action,nbfois)</td>"
        puts $f "<td align='center'>$table($theme,$action,niveau_max)</td>"
        puts $f "<td align='center'>$table($theme,$action,duree_min)</td>"
        puts $f "<td align='center'>[expr int($table($theme,$action,duree_cumul)/$table($theme,$action,nbfois))]</td>"
        puts $f "<td align='center'>$table($theme,$action,duree_max) </td>"
        puts $f "</tr>"
      }
    }
  }
  puts $f "</table>\n</body>\n</html>"
  close $f
  #fin bilan.html
  #
  # on change les boutons
  #
  if { [file exists [file join $glob(trace_dir) $env(USER).log]] } {
    .top.but_lemien configure -state normal
  } else {
    .top.but_lemien configure -state disable
  }
  .t.text insert end "\n[mc trace_en] \n $file (+ ...cvs)"
  .t.text see end
  .t.text insert end "\n[mc synthese_en] \n $fnom.html "
  .t.text see end
  .t.text insert end "\n\n*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n "
  .t.text see end
  #
  # on revient au répertoire d'avant le bilan
  #
  cd $curdir
  show_html_bilan
  .top.but_efface configure -state normal
} ;# fin bilan

###################################################################################

proc mon_bilan {} {
  global env glob
  set qui [file join $glob(trace_user).log]
  bilan $qui
}

proc bilan_eleve { } {
  set types {
        {"Categories" {/home/eleves/*/leterrier/mulot/log/*.log}        }
  }
  catch {set qui [tk_getOpenFile -filetypes $types]}
  if {[catch { set f [open [file join $qui] "r" ] }] } {
        exit
  }
  close $f
  bilan $qui
}

proc bilan_classe {} {
  .t.text insert end "\n\nBilan classe : [mc non_implante].\n\n"
}

proc bilan_tous {} {
  global glob
  # exec cp_bilans ne fonctionne pas : bug ?
  set cpbilans ./cp_bilans
  exec $cpbilans
  set qui [glob [file join $glob(trace_dir) *_eleves.log]] 
  bilan $qui
}

proc efface_bilan {} {
  global fnom file
  catch { file delete $file}
  file delete "$fnom.html"
  tk_messageBox -message "[mc bilan_efface]"
  .top.but_efface configure -state disabled
}

proc show_html_bilan {} {
  global fnom glob
  if {$glob(platform) == "windows"} {
    set fichier [file attributes "$fnom.html" -shortname]
  } else {
    set fichier "$fnom.html"
  }
  catch { exec $glob(progaide) file:$fichier & }

}
