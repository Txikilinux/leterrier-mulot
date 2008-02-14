# Recopié à partir de la librairie Img auteur?
#    This script activates msgcat if available, otherwise it
#    defines a minimal number of functions to make Internationalization
#    within Img possible using Tk 8.0 or higher.


# Convert the locale values stored in environment variables to a form
# suitable for passing to [mclocale]
proc ConvertLocale {value} {
    # Assume $value is of form: $language[_$territory][.$codeset][@modifier]
    # Convert to form: $language[_$territory][_$modifier]
    #
    # Comment out expanded RE version -- bugs alleged
    # regexp -expanded {
    #	^		# Match all the way to the beginning
    #	([^_.@]*)	# Match "lanugage"; ends with _, ., or @
    #	(_([^.@]*))?	# Match (optional) "territory"; starts with _
    #	([.]([^@]*))?	# Match (optional) "codeset"; starts with .
    #	(@(.*))?	# Match (optional) "modifier"; starts with @
    #	$		# Match all the way to the end
    # } $value -> language _ territory _ codeset _ modifier
    regexp {^([^_.@]*)(_([^.@]*))?([.]([^@]*))?(@(.*))?$} $value \
	    -> language _ territory _ codeset _ modifier
    set ret $language
    if {[string length $territory]} {
	append ret _$territory
    }
    if {[string length $modifier]} {
	append ret _$modifier
    }
    return $ret
}

proc cherchewinlangue {} {
global sysFont test
  array set WinRegToISO639 {
        00000001 ar 00000401 ar_SA 00000801 ar_IQ 00000c01 ar_EG 00001001 ar_LY 00001401 ar_DZ
        00001801 ar_MA 00001c01 ar_TN 00002001 ar_OM 00002401 ar_YE 00002801 ar_SY
        00002c01 ar_JO 00003001 ar_LB 00003401 ar_KW 00003801 ar_AE 00003c01 ar_BH 00004001 ar_QA
        00000002 bg 00000402 bg_BG 00000003 ca 00000403 ca_ES
        00000004 zh 00000404 zh_TW 00000804 zh_CN 00000c04 zh_HK 00001004 zh_SG 00001404 zh_MO
        00000005 cs 00000405 cs_CZ 00000006 da 00000406 da_DK
        00000007 de 00000407 de_DE 00000807 de_CH 00000c07 de_AT 00001007 de_LU 00001407 de_LI
        00000008 el 00000408 el_GR
        00000009 en 00000409 en_US 00000809 en_GB 00000c09 en_AU 00001009 en_CA 00001409 en_NZ
        00001809 en_IE 00001c09 en_ZA 00002009 en_JM 00002409 en_GD 00002809 en_BZ
        00002c09 en_TT 00003009 en_ZW 00003409 en_PH
        00000a es 0000040a es_ES 0000080a es_MX 00000c0a es_ES@modern 0000100a es_GT 0000140a es_CR
        0000180a es_PA 00001c0a es_DO 0000200a es_VE 0000240a es_CO 0000280a es_PE
        00002c0a es_AR 0000300a es_EC 0000340a es_CL 0000380a es_UY 00003c0a es_PY
        0000400a es_BO 0000440a es_SV 0000480a es_HN 00004c0a es_NI 0000500a es_PR 0000000b fi 0000040b fi_FI
        0000000c fr 0000040c fr_FR 0000080c fr_BE 00000c0c fr_CA 0000100c fr_CH 0000140c fr_LU
        0000180c fr_MC 0000000d he 0000040d he_IL 0000000e hu 0000040e hu_HU 0000000f is 0000040f is_IS
        00000010 it 00000410 it_IT 00000810 it_CH 00000011 ja 00000411 ja_JP 00000012 ko 00000412 ko_KR
        00000013 nl 00000413 nl_NL 00000813 nl_BE 00000014 no 00000414 no_NO 00000814 nn_NO
        00000015 pl 00000415 pl_PL 00000016 pt 00000416 pt_BR 00000816 pt_PT
        00000017 rm 00000417 rm_CH 000000018 ro 00000418 ro_RO
        00000019 ru 0000001a hr 0000041a hr_HR 0000081a sr_YU 00000c1a sr_YU@cyrillic
        0000001b sk 0000041b sk_SK 0000001c sq 0000041c sq_AL 0000001d sv 0000041d sv_SE 0000081d sv_FI
        0000001e th 0000041e th_TH 0000001f tr 0000041f tr_TR 00000020 ur 00000420 ur_PK 00000820 ur_IN
        00000021 id 00000421 id_ID 00000022 uk 00000422 uk_UA 00000023 be 00000423 be_BY
        00000024 sl 00000424 sl_SI 00000025 et 00000425 et_EE 00000026 lv 00000426 lv_LV
        00000027 lt 00000427 lt_LT 00000028 tg 00000428 tg_TJ 00000029 fa 00000429 fa_IR
        0000002a vi 0000042a vi_VN 0000002b hy 0000042b hy_AM 0000002c az 0000042c az_AZ@latin 0000082c az_AZ@cyrillic
        0000002d eu 0000002e wen 0000042e wen_DE 0000002f mk 0000042f mk_MK 00000030 bnt 00000430 bnt_TZ
        00000031 ts 00000431 ts_ZA 00000033 ven 00000433 ven_ZA 00000034 xh 00000434 xh_ZA
        00000035 zu 00000435 zu_ZA 00000036 af 00000436 af_ZA 00000037 ka 00000437 ka_GE 00000038 fo 00000438 fo_FO
        00000039 hi 00000439 hi_IN 0000003a mt 0000043a mt_MT 0000003b se 0000043b se_NO 0000043c gd_UK 0000083c ga_IE
        0000003d yi 0000043d yi_IL 0000003e ms 0000043e ms_MY 0000083e ms_BN 0000003f kk 0000043f kk_KZ
        00000040 ky 00000440 ky_KG 00000041 sw 00000441 sw_KE 00000042 tk 00000442 tk_TM
        00000043 uz 00000443 uz_UZ@latin 00000843 uz_UZ@cyrillic 00000044 tt 00000444 tt_RU
        00000045 bn 00000445 bn_IN 00000046 pa 00000446 pa_IN 00000047 gu 00000447 gu_IN 00000048 or 00000448 or_IN
        00000049 ta 0000004a te 0000044a te_IN 0000004b kn 0000044b kn_IN 0000004c ml 0000044c ml_IN 0000004d as 0000044d as_IN
        0000004e mr 0000044e mr_IN 0000004f sa 0000044f sa_IN 00000050 mn 00000051 bo 00000451 bo_CN
        00000052 cy 00000452 cy_GB 00000053 km 00000453 km_KH 00000054 lo 00000454 lo_LA 00000055 my 00000455 my_MM
        00000056 gl 00000456 gl_ES 00000057 kok 00000457 kok_IN 00000058 mni 00000458 mni_IN 00000059 sd
        0000005a syr 0000045a syr_TR 0000005b si 0000045b si_LK 0000005c chr 0000045c chr_US 0000005d iu 0000045d iu_CA
        0000005e am 0000045e am_ET 0000005f ber 0000045f ber_MA 00000060 ks 00000460 ks_PK 00000860 ks_IN
        00000061 ne 00000461 ne_NP 00000861 ne_IN 00000062 fy 00000462 fy_NL 00000063 ps 00000064 tl 00000464 tl_PH
        00000065 div 00000465 div_MV 00000066 bin 00000466 bin_NG 00000067 ful 00000467 ful_NG 00000068 ha 00000468 ha_NG
        00000069 nic 00000469 nic_NG 0000006a yo 0000046a yo_NG 00000070 ibo 00000470 ibo_NG 00000071 kau 00000471 kau_NG
        00000072 om 00000472 om_ET 00000073 ti 00000473 ti_ET 00000074 gn 00000474 gn_PY 00000075 cpe 00000475 cpe_US
        00000076 la 00000476 la_VA 00000077 so 00000477 so_SO 00000078 sit 00000478 sit_CN 00000079 pap 00000479 pap_AN
    }

set key {HKEY_CURRENT_USER\Control Panel\International}
    if {[catch {package require registry}] || [catch {registry get $key "locale"} locale]} {
        	  return C
    }

if {[catch {set locale [ConvertLocale $WinRegToISO639([string tolower $locale])] }]} {

   return C
} else {

return $locale

}

}

#########################################################################"""
global env plateforme baseHome
variable langue

####### ajoutÃ© par Andre Connes ###############
set baseHome $glob(home_mulot)
###############################################

if {[file exists [file join $baseHome reglages lang.conf]]} {
    set f [open [file join $baseHome reglages lang.conf] "r"]
    set langue [gets $f]
    close $f
    set env(LANG) $langue
}
	if {[catch {package require msgcat}] == 0} {

    if {[info exists env(LANG)]} {
	set fileName [file join [file dirname [info script]] msgs \
		[string tolower [string range $env(LANG) 0 1]].msg]
     } else {
	set fileName [file join [file dirname [info script]] msgs fr.msg]
     }
	if {[file readable $fileName]} {
	    namespace eval msgcat {
		proc mcset {args} {
		    global sysFont ::msgs
		    if {[llength $args] > 2} {
			set msgs([lindex $args 1]) [lindex $args 2]
		    }
		}
	    }
	    source $fileName
	}
    # 
    proc mc {string} {
	global sysFont msgs
	if {[info exists msgs($string)]} {
	    return [set msgs($string)]
	} else {
	    return $string
	}
    }
} else {

	if {![info exists env(LANG)]} {
	if {$plateforme== "windows"} {
      set langue [cherchewinlangue]
      set env(LANG) $langue
    	::msgcat::mclocale $langue      
	} else {

    	set env(LANG) fr
	::msgcat::mclocale fr
	}
}
    ::msgcat::mcload [file join [file dirname [info script]] msgs]
    catch {namespace import ::msgcat::mc}
}


