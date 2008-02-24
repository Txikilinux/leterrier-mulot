#!/bin/sh
#mulot.tcl
# \
exec wish "$0" ${1+"$@"}

#*************************************************************************
#  Copyright (C) 2002 André Connes <davidlucardi@aol.com>
#  Copyright (C) 2002 André Connes <andre.connes@wanadoo.fr>
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
#  File  : mulot.tcl
#  Author  : André Connes <andre.connes@wanadoo.fr>
#  Modifier:
#  Date    : 15/04/2003 modifié le 06/12/2004
#  Licence : GNU/GPL Version 2 ou plus
# 
#  Description:
#  ------------
#  
#  @version    $Id: mulot.tcl,v 1.16 2006/03/25 00:55:15 abuledu_francois Exp $
#  @author     André Connes
#  @modifier   
#  @project    Le terrier
#  @copyright  André Connes
# 
#***********************************************************************

proc lanceappli {appli {niveau 0 }} {
        set appli [file join $appli]  
        exec wish $appli $niveau &
        exit   
}
