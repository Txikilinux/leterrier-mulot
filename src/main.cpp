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

int main(int argc, char *argv[])
{
    AbulEduApplicationV1 a(argc, argv,VER_INTERNALNAME_STR, VER_PRODUCTVERSION_STR, VER_COMPANYDOMAIN_STR, VER_COMPANYNAME_STR, VER_UNITVERSION_STR);
    a.setAbeApplicationLongName(QObject::trUtf8(VER_FILEDESCRIPTION_STR));

    if(a.isRunning())
        return 0;

    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;

    //Verifier si le chemin de stockage des langues est bien celui la
    translator.load("leterrier-mulot_"+locale, "./conf/lang");
    a.installTranslator(&translator);

    MainWindow *w;
    /* Splashscreen */
    AbulEduSplashScreenV1 *splash = new AbulEduSplashScreenV1(0,true);
    splash->show();
    w = new MainWindow(0);
    splash->setMainWindow(w);
    //pour les developpeurs presses, remplacer le 10000 par 1000
    splash->launch(1000);

    return a.exec();
}
