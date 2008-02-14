#  This is an extract of Whiteboard.tcl, a very great project,
#  Copyright (c) 1999-2002  Mats Bengtsson
#  http://hem.fyristorg.com/matben

switch -- $tcl_platform(platform) {
    unix {
        set thisPlatform $tcl_platform(platform)
        if {[package vcompare [info tclversion] 8.3] == 1} {
            if {[string equal [tk windowingsystem] "aqua"]} {
                set thisPlatform "macosx"
            }
        }
    }
    windows - macintosh {
        set thisPlatform $tcl_platform(platform)
    }
}
set this(platform) $thisPlatform

switch -- $this(platform) {
    unix {
        set sysFont(s) {Helvetica 10 normal}
        set sysFont(sb) {Helvetica 10 bold}
        set sysFont(m) $sysFont(s)
        set sysFont(l) {Helvetica 18 normal}
        set sysFont(lb) {Helvetica 18 bold}
        set sysFont(t) {Helvetica 14 normal}
        set sysFont(esp) {adobe-courier 30 normal}
        set sysFont(bilan) {adobe-courier 10 normal}

    }
    macintosh {
        set sysFont(s) {Geneva 9 normal}
        set sysFont(sb) {Geneva 9 bold}
        set sysFont(m) application
        set sysFont(l) {Helvetica 18 normal}
        set sysFont(lb) {Helvetica 18 bold}
        set sysFont(t) application
        set sysFont(esp) {Helvetica 20 bold}
        set sysFont(bilan) {Geneva 9 normal}


    }
    macosx {
        set sysFont(s) {Geneva 9 normal}
        set sysFont(sb) {Geneva 9 bold}
        set sysFont(s) {{Lucida Grande} 11 normal}
        set sysFont(sb) {{Lucida Grande} 11 bold}
        set sysFont(m) application
        set sysFont(t) application
        set sysFont(l) {Helvetica 18 normal}
        set sysFont(lb) {Helvetica 18 bold}
        set sysFont(esp) {Helvetica 20 bold}
        set sysFont(bilan) {Geneva 9 normal}

    }
    windows {
        set sysFont(s) {Arial 8 normal}
        set sysFont(sb) {Arial 8 bold}
        set sysFont(t) {Helvetica 12 bold}

        set sysFont(m) $sysFont(s)
        set sysFont(l) {Helvetica 16 normal}
        set sysFont(lb) {Helvetica 16 bold}
        set sysFont(esp) {Courier 18 normal}
        set sysFont(bilan) {Courier 8 normal}

    }
}
