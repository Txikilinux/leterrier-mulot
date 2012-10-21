#!/bin/bash
#script de creation de l'image pour OSX
#Eric Seigne 2011-2012 <eric.seigne@ryxeo.com>
#documentation https://redmine.ryxeo.com/projects/ryxeo/wiki/Cr%C3%A9er_un_paquet_avec_OSX
APPNAME="leterrier-mulot"
APPVERSION="1.1.0"
VOLNAME="AbulEdu LeTerrier Mulot ${APPVERSION}"
VOLICON="macos/icones/leterrier-mulot.icns"

#On stoppe dès qu'on rencontre un problème
set -e

if [ -d /tmp/build-dmg-${APPNAME} ]; then
    rm -rf /tmp/build-dmg-${APPNAME}
fi
if [ -f ${APPNAME}-${APPVERSION}-osx.dmg ]; then
    rm -f ${APPNAME}-${APPVERSION}-osx.dmg
fi
if [ -e ${APPNAME}.app ]; then
    rm -rf ${APPNAME}.app
fi

#on fabrique une arborescence "dmg" temporaire
mkdir /tmp/build-dmg-${APPNAME}

#on cherche ou on se trouve et on change de repertoire le cas echeant
if [ -f ../${APPNAME}.pro ]; then
    cd ..
fi

qmake ${APPNAME}.pro -r -spec macx-g++ CONFIG+=release

#compilation
make -w -j4

#on utilise les outils de Qt pour trimbaler le Qt framework et les dependances Qt
macdeployqt ${APPNAME}.app

#note 20121021: je ne sais pas si c'est encore utile
ln -s /Applications /tmp/build-dmg-${APPNAME}/Applications

#copie des donnees
cp -a data ${APPNAME}.app/Contents/Resources/
#cp -a conf ${APPNAME}.app/Contents/Resources/
mkdir ${APPNAME}.app/Contents/Resources/lang
cp -a lang/*.qm ${APPNAME}.app/Contents/Resources/lang/

#creation du fichier dmg
~/create-dmg/create-dmg --window-size 415 295 --volname "${VOLNAME}" --volicon ${VOLICON} --background "macos/.background/background.png" ${APPNAME}-${APPVERSION}-osx.dmg ${APPNAME}.app

#nettoyage
rm -rf ${APPNAME}.app
rm -rf /tmp/build-dmg-${APPNAME}
