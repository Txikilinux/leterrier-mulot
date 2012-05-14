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


#include "widgetdeplacesouris.h"
#include "ui_widgetdeplacesouris.h"

widgetDeplaceSouris::widgetDeplaceSouris(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetDeplaceSouris)
{
    ui->setupUi(this);

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsItem *item;
    QPixmap image;

    image.load("data/images/animaux/marmotte.jpg", 0, Qt::AutoColor);
    item = scene->addPixmap(image);
    ui->graphicsView->setScene(scene);

    //Calcul du nombre de lignes et de colonnes necessaires
    for(int i = 0; i < image.height(); i+=50) {
        for(int j = 0; j < image.width(); j+=50) {
            masqueDeplaceSouris *m = new masqueDeplaceSouris();
            m->setParent(scene);
            m->moveBy(j,i);
            ui->graphicsView->scene()->addItem(m);
        }
    }


}

widgetDeplaceSouris::~widgetDeplaceSouris()
{
    delete ui;
}
