#!/bin/sh
#aider.tcl
# \
exec wish "$0" ${1+"$@"}

#*************************************************************************
#  Copyright (C) 2002 Eric Seigne <erics@rycks.com>
#  Copyright (C) 2003 André Connes <andre.connes@wanadoo.fr>
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
#  File  : $Id: aider.tcl,v 1.8 2006/01/27 16:59:21 abuledu_andre Exp $
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

proc aider { lang } {
  global glob
  . configure -cursor watch
  after 4000 {
	. configure -cursor left_ptr
        set attente_erreur non
  }
  if {$glob(platform) == "windows"} {
  	set fichier [file attributes [file join [pwd] aides aide.$lang.html] -shortname]
  } else {
  	set fichier [file join [pwd] aides aide.$lang.html]
  }

#  if { $glob(platform) == "unix" } {
#    set base "[pwd]"
#  } else {
#    set base "."
#  }
#  if {![file exists [file join $base aides aide.$lang.html]]} {
#	set lang fr
#  }
#  set fichier [file join $base aides aide.$lang.html]
  exec $glob(progaide) file:$fichier &
  vwait attente_erreur
}

bind . <F1> aider

