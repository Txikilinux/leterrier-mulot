#!/bin/sh
#mulot.tcl
# \
exec wish "$0" ${1+"$@"}

proc est_prof { } {
  global env
  set je_suis_prof 0
  catch { set nom_utilisateur [file tail $env(HOME)]  }
  catch { set classe [exec grep $nom_utilisateur [file join /etc/group] | grep prof] }
  catch { set je_suis_prof [string match *$nom_utilisateur* $classe] }
  return $je_suis_prof
}          
