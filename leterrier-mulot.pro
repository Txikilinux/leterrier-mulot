#-------------------------------------------------
#
# Project created by QtCreator 2012-02-26T22:15:12
#
#-------------------------------------------------

QT       += core gui

TARGET = leterrier-mulot
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    widgetchoixtheme.cpp \
    masquedeplacesouris.cpp \
    exercicesurvol.cpp \
    editeur.cpp \
    visionneuseimage.cpp \
    exerciceparcours.cpp

HEADERS  += mainwindow.h \
    version.h \
    widgetchoixtheme.h \
    masquedeplacesouris.h \
    exercicesurvol.h \
    editeur.h \
    visionneuseimage.h \
    exerciceparcours.h

FORMS    += mainwindow.ui \
    widgetchoixtheme.ui \
    widgetdeplacesouris.ui \
    editeur.ui

RESOURCES += \
    ressources.qrc

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

