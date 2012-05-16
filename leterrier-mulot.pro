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
    widgetdeplacesouris.cpp \
    masquedeplacesouris.cpp

HEADERS  += mainwindow.h \
    version.h \
    widgetchoixtheme.h \
    widgetdeplacesouris.h \
    masquedeplacesouris.h

FORMS    += mainwindow.ui \
    widgetchoixtheme.ui \
    widgetdeplacesouris.ui

#AbulEduApplicationV1
include(lib/abuleduapplicationv1/abuleduapplicationv1.pri)

#AbulEduConstantesV1
include(lib/abuleduconstantesv1/abuleduconstantesv1.pri)

#AbulEduCommonStatesV1
include(lib/abuleducommonstatesv1/abuleducommonstatesv1.pri)

#AbulEduStateMachineV1
include(lib/abuledustatemachinev1/abuledustatemachinev1.pri)

#AbulEduExerciceV1
include(lib/abuleduexercicev1/abuleduexercicev1.pri)

#AbulEduTeteV1
include(lib/abuledutetev1/abuledutetev1.pri)

#AbulEduNetworkAccessManagerV1
include(lib/abuledunetworkaccessmanagerv1/abuledunetworkaccessmanagerv1.pri)

#AbulEduFlatBoutonV1
include(lib/abuleduflatboutonv1/abuleduflatboutonv1.pri)

#AbulEduPageAccueilV1
include(lib/abuledupageaccueilv1/abuledupageaccueilv1.pri)

#AbulEduGraphicMenuV1
include(lib/abuledugraphicmenuv1/abuledugraphicmenuv1.pri)

#AbulEduLabelV1
include(lib/abuledulabelv1/abuledulabelv1.pri)

#Maia
include(lib/maia/maia.pri)
