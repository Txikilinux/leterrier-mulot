#-------------------------------------------------
#
# Project created by QtCreator 2012-02-26T22:15:12
#
#-------------------------------------------------

QT       += core gui script xml network
system(ccache -V):QMAKE_CXX = ccache g++

#a activer ou pas si on souhaite compiler pour tablette
#DEFINES += __ABULEDUTABLETTEV1__MODE__=1

TARGET = leterrier-mulot
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    masquedeplacesouris.cpp \
    exercicesurvol.cpp \
    editeur.cpp \
    visionneuseimage.cpp \
    exerciceparcours.cpp \
    exerciceclic.cpp \
    exercicedoubleclic.cpp \
    thread.cpp \
    lib/abuleduaproposv1/abuleduaproposv1.cpp

HEADERS  += mainwindow.h \
    version.h \
    masquedeplacesouris.h \
    exercicesurvol.h \
    editeur.h \
    visionneuseimage.h \
    exerciceparcours.h \
    exerciceclic.h \
    exercicedoubleclic.h \
    thread.h \
    lib/abuleduaproposv1/abuleduaproposv1.h

FORMS    += mainwindow.ui \
    widgetdeplacesouris.ui \
    editeur.ui \
    lib/abuleduaproposv1/abuleduaproposv1.ui

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
include(lib/abuleduvirtualkeyboardv1/abuleduvirtualkeyboardv1.pri)

#AbulEduLangV1
include(lib/abuledulangv1/abuledulangv1.pri)
include(lang/lang.pri)

win32 {
    DEFINES      += __WINDOWS__=1
    INCLUDEPATH  += $$quote(C:/msys/1.0/local/include)
    LIBS         += $$quote(C:/msys/1.0/local/lib/libexiv2.dll.a)
    OTHER_FILES  += windows/windows.rc
    RC_FILE       = windows/windows.rc
}
