/**
  * Classe 
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
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


#ifndef WIDGETDEPLACESOURIS_H
#define WIDGETDEPLACESOURIS_H

#include <QDebug>
#include <QWidget>
#include <QGraphicsItem>
#include <QFileInfoList>
#include <QDateTime>
#include <QDir>
#include <QTimer>
#include "masquedeplacesouris.h"

namespace Ui {
class widgetDeplaceSouris;
}

class widgetDeplaceSouris : public QWidget
{
    Q_OBJECT
    
private slots:
    void lanceLeJeu();
    void slotCacheMasque();

public:
    explicit widgetDeplaceSouris(QWidget *parent = 0);
    ~widgetDeplaceSouris();
    
private:
    Ui::widgetDeplaceSouris *ui;
    int m_nbTotalMasques;
    QGraphicsScene *m_scene;
    QStringList m_ListeFichiers;
    int m_numImage; //pour savoir si on est a la 1ere ou 10 image -> taille des caches
    QList<masqueDeplaceSouris*> m_listeMasquesFixes;
};

#endif // WIDGETDEPLACESOURIS_H
