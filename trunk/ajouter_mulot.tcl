#!/bin/sh
#ajouter_mulot.tcl
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
#  File  : $Id: ajouter_mulot.tcl,v 1.3 2006/03/15 10:11:30 abuledu_andre Exp $
#  Author  : andre.connes@wanadoo.fr
#  Date    : 23/06/2003 Modification : 01/05/2004
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

# Décommenter la ligne suivante si Img est installée
# package require Img

########## Construction de l'interface###########
#frame wa pour ajouter une image
wm geometry . +0+0
wm title . [mc Ajouter]

frame .wa -background #aaaaaa -height 420 -width 400
grid .wa -column 0 -row 0

label .wa.passwd -text [mc Password] -bg red
place .wa.passwd -x 100 -y 10
entry .wa.password -show "*"
place .wa.password -x 100 -y 30
.wa.password delete 0 end

label .wa.dirin -text $glob(home)

place .wa.dirin -x 10 -y 55
listbox .wa.listin -background #c0c0c0 -height 10 -width 20 -yscrollcommand ".wa.scrollin set"
scrollbar .wa.scrollin -command ".wa.listin yview"
place .wa.listin -x 10 -y 80
place .wa.scrollin -x 157 -y 80 -height 172
label .wa.labelin -text [mc Source]
place .wa.labelin -x 10 -y 270
entry .wa.textin
place .wa.textin -x 10 -y 290
.wa.textin insert 0 "."

set newdir $glob(home)

bind .wa.listin <Double-ButtonRelease-1> "capturenomin"

listbox .wa.listout -background #c0c0c0 -height 10 -width 20 -yscrollcommand ".wa.scrollout set"
scrollbar .wa.scrollout -command ".wa.listout yview"
place .wa.listout -x 200 -y 80
place .wa.scrollout -x 350 -y 80 -height 172
label .wa.labelout -text [mc Destination]
place .wa.labelout -x 200 -y 270
entry .wa.textout
place .wa.textout -x 200 -y 290
.wa.textout delete 0 end


button .wa.faire -text [mc Faire ] -command "ajouter_image"
place .wa.faire -x 130 -y 325
button .wa.abandonner -text [mc Quitter] -command exit
place .wa.abandonner -x 185 -y 325

# lister les images in
proc peuplelistin { } {
        global newdir
        .wa.listin delete 0 end
        catch {foreach i [lsort [glob -nocomplain $newdir/.* $newdir/* ]] {
                .wa.listin insert end [file tail $i]
            }
        }
    }
peuplelistin

# lister les images out
proc peuplelistout {} {
        .wa.listout delete 0 end
        catch {foreach i [lsort [glob images/*]] {
                .wa.listout insert end [file tail $i]
            }
        }
    }
peuplelistout 


proc capturenomin { } {
    global newdir
    .wa.textin delete 0 end
    set fichier "$newdir/[.wa.listin get active]"
    if [file isdirectory $fichier] {
      .wa.dirin configure -text $fichier
      set newdir $fichier
      peuplelistin
      .wa.dirin configure -text $newdir
    } else {
      set simage [.wa.listin get active]
#      set n [string last ".gif" $simage]
#      if { $n == -1 } {
#        set n [string last "." $simage]
#        set dimage [string range $simage 0 $n]gif
#      } else {
        set dimage $simage
#      }
      .wa.textin insert 0 $simage
      .wa.textout insert 0 $dimage
    }
}

proc ajouter_image {} {
  global glob newdir

  # astuce : si abuledu le mot de passe est auto mais
  #          droits d'ajout pour les profs only
  if {[file exists /etc/abuledu]} {
    set pass $glob(passwd)
  } else {
    set pass [.wa.password get]
  }

  if {$pass == $glob(passwd)} {

    set simage [.wa.textin get]
    set dimage [.wa.textout get]

    catch {file copy $newdir/$simage images/$simage }

    if { $glob(platform) == "unix" } {
      catch {exec convert images/$simage images/$dimage}
      catch {exec mogrify -geometry 600x400 images/$dimage}
    } else {
        catch {file copy images/$simage images/$dimage }
    }

    if { [string compare $simage $dimage] != 0 } {
      catch {file delete images/$simage}
    }
    peuplelistout
  }
  .wa.textin delete 0 end
  .wa.textout delete 0 end
}
