#-------------------------------------------------
#
# Project created by QtCreator 2012-02-26T22:15:12
#
#-------------------------------------------------

QT       += core gui script xml network
system(ccache -V):QMAKE_CXX = ccache g++

TARGET = leterrier-mulot
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    masquedeplacesouris.cpp \
    exercicesurvol.cpp \
    editeur.cpp \
    visionneuseimage.cpp \
    exercicedoubleclic.cpp \
    lib/abuleduaproposv1/abuleduaproposv1.cpp \
    abuledutools.cpp \
    exerciceclic.cpp \
    exerciceparcours.cpp \
    abstractexercice.cpp

HEADERS  += mainwindow.h \
    version.h \
    masquedeplacesouris.h \
    exercicesurvol.h \
    editeur.h \
    visionneuseimage.h \
    exercicedoubleclic.h \
    lib/abuleduaproposv1/abuleduaproposv1.h \
    abuledutools.h \
    exerciceclic.h \
    exerciceparcours.h \
    abstractexercice.h

FORMS    += mainwindow.ui \
    widgetdeplacesouris.ui \
    editeur.ui \
    lib/abuleduaproposv1/abuleduaproposv1.ui

RESOURCES += \
    ressources.qrc

win32 {
    OTHER_FILES  += windows/windows.rc
    RC_FILE       = windows/windows.rc
}

macx {
 ICON = macos/icones/leterrier-mulot.icns
}

include(lib/abuleduapplicationv1/abuleduapplicationv1.pri)
include(lib/abuleduconstantesv1/abuleduconstantesv1.pri)
include(lib/abuleducommonstatesv1/abuleducommonstatesv1.pri)
include(lib/abuledustatemachinev1/abuledustatemachinev1.pri)
include(lib/abuleduexercicev1/abuleduexercicev1.pri)
include(lib/abuledutetev1/abuledutetev1.pri)
include(lib/abuledunetworkaccessmanagerv1/abuledunetworkaccessmanagerv1.pri)
include(lib/abuleduflatboutonv1/abuleduflatboutonv1.pri)
include(lib/abuledupageaccueilv1/abuledupageaccueilv1.pri)
include(lib/abuledugraphicmenuv1/abuledugraphicmenuv1.pri)
include(lib/abuledulabelv1/abuledulabelv1.pri)
include(lib/maia/maia.pri)
include(lib/abuleduetiquettesv1/abuleduetiquettesv1.pri)
include(lib/abuledufilev1/abuledufilev1.pri)
include(lib/abuledumessageboxv1/abuledumessageboxv1.pri)
include(lib/abuledusplashscreenv1/abuledusplashscreenv1.pri)
include(lib/abuleduidentitesv1/abuleduidentitesv1.pri)
include(lib/abuleduwidgetassistantetapesv1/abuleduwidgetassistantetapesv1.pri)

#AbulEduLangV1
include(lib/abuledulangv1/abuledulangv1.pri)
include(lang/lang.pri)

