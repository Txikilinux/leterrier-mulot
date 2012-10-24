/** leterrier-mulot: reprise en Qt/C++ du logiciel Mulot d'André Connes
  *
  * @see https://redmine.ryxeo.com/projects/
  * @author 2010 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#include "mainwindow.h"

void debugOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
#ifdef QT_NO_DEBUG_OUTPUT
        fprintf(stderr, "Debug: %s\n", msg);
#endif
        break;
    case QtWarningMsg:
#ifdef QT_NO_WARNING_OUTPUT
        fprintf(stderr, "Warning: %s\n", msg);
#endif
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

int main(int argc, char *argv[])
{
    //    qInstallMsgHandler(debugOutput);
    AbulEduApplicationV1 a(argc, argv,VER_INTERNALNAME_STR, VER_PRODUCTVERSION_STR, VER_COMPANYDOMAIN_STR, "leterrier");
    a.setAbeApplicationLongName(QObject::trUtf8(VER_FILEDESCRIPTION_STR));
#ifdef __ABULEDUTABLETTEV1__MODE__
    QApplication::setStyle("plastique");
#endif


    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;

    //Verifier si le chemin de stockage des langues est bien celui la
    translator.load("leterrier-mulot_"+locale, "./conf/lang");
    a.installTranslator(&translator);

    MainWindow *w;

    /// Splashscreen
    AbulEduSplashScreenV1 *splash = new AbulEduSplashScreenV1(0, true);
    splash->show();
    for (int i = 1; i <= 15; i++)
    {
        //On lance le constructeur de la mainwindows en "arrière plan" lors du 1er passage
        //ca permet d'avoir les requetes reseau en arriere plan (mise a jour etc.) c'est "cool" :)
        if(i == 1) {
            w = new MainWindow(0);
            splash->setMainWindow(w);
        }
        if(i < 6) {
            splash->showMessage(QObject::trUtf8("      Chargement en cours, merci de patienter ...... %1%").arg(QString::number(i*20)), Qt::AlignBottom | Qt::AlignHCenter, Qt::black);
        }


        //tres important si vous voulez que le splash s'affiche !
        qApp->processEvents();
        //Pour les developpeurs presses
        AbulEduSleeperThread::msleep(100);
        //Quand on passe en production
        //AbulEduSleeperThread::msleep(1000);
    }
    splash->showButtonShop();
    splash->finish();

    //Permet de detecter qu'il n'y a aucune activite et lance le mode "demo" du logiciel
    //note: expérimental Eric S. 2012 en attendant de voir, je teste dans ce logiciel
    activityFilter *ef;
    ef = new activityFilter(&a);
    a.installEventFilter(ef);

    QObject::connect(ef, SIGNAL( userInactive() ),
                     w,  SLOT( slotDemo() ));

    //Gestion de la ligne de commande pour activer ou pas le clavier virtuel
    //    if(abeApp->arguments().size() > 1)
    //    {
    //        if(abeApp->arguments().contains("--virtualkeyboard")) {
    //AbulEduVirtualKeyboardV1 *ic = new AbulEduVirtualKeyboardV1();
    //a.setInputContext(ic);
    //        }
    //    }

    /// TODO -> A mettre dans splashScreen !!
//#ifdef __ABULEDUTABLETTEV1__MODE__
//    w->showFullScreen();
//#else
//    // jlf 2012/09/29 Fenetre maximisée par défaut
//    w->showMaximized();
//#endif

    return a.exec();
}
