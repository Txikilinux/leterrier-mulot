HEADERS += abuledumessageboxv1.h

SOURCES += abuledumessageboxv1.cpp

FORMS += abuledumessageboxv1.ui

#on inclut le fichier ressources du developpeur de l'application en priorite
#mais seulement s'il existe. S'il n'existe pas on fallback sur le fichier
#de ressources propose par la lib
!exists($$PWD/../../data/abuledumessageboxv1/abuledumessageboxv1.qrc) {
  RESOURCES += abuledumessageboxv1.qrc
}
exists($$PWD/../../data/abuledumessageboxv1/abuledumessageboxv1.qrc) {
  RESOURCES += $$PWD/../../data/abuledumessageboxv1/abuledumessageboxv1.qrc
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!exists($$PWD/../abuleduflatboutonv1/abuleduflatboutonv1.h ) {
  error("la lib AbulEduMessageBoxV1 dépend de abuleduflatboutonv1: https://redmine.ryxeo.com/projects/ryxeo/wiki/AbulEduFlatBoutonV1")
}

