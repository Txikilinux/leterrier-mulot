#!/bin/sh
#mulot.tcl
# \
exec wish "$0" ${1+"$@"}

proc est_prof { } {
  global env
  catch { set nom_utilisateur [file tail $env(HOME)]  }
  catch { set classe [exec grep $nom_utilisateur [file join /etc/group] | grep prof] }
  return  [string match *$nom_utilisateur* $classe]
}          
