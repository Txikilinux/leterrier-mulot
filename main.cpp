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

#include "abuleduapplicationv1.h"
#include "activityfilter.h"
#include "version.h"
#include "mainwindow.h"
#include "thread.h"

int main(int argc, char *argv[])
{
    AbulEduApplicationV1 a(argc, argv,VER_INTERNALNAME_STR, VER_PRODUCTVERSION_STR, VER_COMPANYDOMAIN_STR, "leterrier");
    a.setAbeApplicationLongName(QObject::trUtf8(VER_FILEDESCRIPTION_STR));


    // ================== splashscreen
//    AbulEduSplashScreenV1 *splash = new AbulEduSplashScreenV1();
//    splash->show();

    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;

    //Verifier si le chemin de stockage des langues est bien celui la
    translator.load("leterrier-mulot_"+locale, "./conf/lang");
    a.installTranslator(&translator);

    MainWindow *w;

    for (int i = 1; i <= 5; i++)
    {
        //On lance le constructeur de la mainwindows en "arrière plan" lors du 1er passage
        //ca permet d'avoir les requetes reseau en arriere plan (mise a jour etc.) c'est "cool" :)
        if(i == 1) {
            w = new MainWindow(0);
        }
//        splash->showMessage(QObject::trUtf8("Chargement en cours, merci de patienter ... %1%").arg(QString::number(i*20)), Qt::AlignBottom | Qt::AlignHCenter, Qt::black);
        //tres important si vous voulez que le splash s'affiche !
        qApp->processEvents();
        //Pour les developpeurs presses
        AbulEduSleeperThread::msleep(10);
        //Quand on passe en production
        //AbulEduSleeperThread::msleep(1000);
    }
//    splash->deleteLater();
    // ================== splashscreen end


    //Permet de detecter qu'il n'y a aucune activite et lance le mode "demo" du logiciel
    //note: expérimental Eric S. 2012 en attendant de voir, je teste dans ce logiciel
    activityFilter *ef;
    ef = new activityFilter(&a);
    a.installEventFilter(ef);

    QObject::connect(ef, SIGNAL( userInactive() ),
                     w,  SLOT( slotDemo() ));


    w->show();

    qDebug() << "demarrage du thread";

    Thread *toto;
    toto = new Thread(QDir::homePath());
//    toto->start(QThread::IdlePriority); // idle = qd aucun autre thread ne tourne
    toto->start(QThread::HighPriority);   // ça va bcp plus vite
    QObject::connect(toto, SIGNAL(finished()), toto, SLOT(slotFinished()));
    return a.exec();
}
