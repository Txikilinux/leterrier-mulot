/**
  * Classe 
  * @see https://redmine.ryxeo.com/projects/
  * @author 2010 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
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


#include "widgetchoixtheme.h"
#include "ui_widgetchoixtheme.h"

widgetChoixTheme::widgetChoixTheme(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetChoixTheme)
{
    ui->setupUi(this);

    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png";

    QGridLayout * layout = new QGridLayout(this);
    layout->setContentsMargins(20,20,10,10);
    layout->setSpacing(5);

    //Recherche les themes disponibles et fabrique les boutons en fonction ...
    QDir aDir(abeApp->applicationDirPath()+"/data/images/");
    QFileInfoList entries = aDir.entryInfoList(QDir::NoDotAndDotDot |
                                               QDir::Dirs);
    for (int idx = 0; (idx < entries.size()) ; idx++) {
        QFileInfo entryInfo = entries[idx];
        if (entryInfo.isDir()) {
            QDir bDir(entryInfo.absoluteFilePath());
            bDir.setNameFilters(filters);
            QFileInfoList photos = bDir.entryInfoList(QDir::NoDotAndDotDot |
                                                       QDir::Files);

//            qDebug() << "On a " << entryInfo.absoluteFilePath();
            QLabel *lbl = new QLabel(entryInfo.fileName(),this);
            lbl->setMaximumSize(200,200);
            lbl->setPixmap(QPixmap(photos.at(0).absoluteFilePath()));
            lbl->setScaledContents(true);
            layout->addWidget(lbl);
        }
    }

    this->setLayout(layout);
}

widgetChoixTheme::~widgetChoixTheme()
{
    delete ui;
}
