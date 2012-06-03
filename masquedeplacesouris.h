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


#ifndef MASQUEDEPLACESOURIS_H
#define MASQUEDEPLACESOURIS_H

#include <QGraphicsObject>
#include <QPainter>

class masqueDeplaceSouris : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit masqueDeplaceSouris(QGraphicsObject *parent = 0);
    void setSize(int width,int height);
    void setColor(QColor couleur);
    void setHideOnMouseOver(bool hide);

private:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF m_taille;
        QRectF boundingRect() const;
        QColor m_couleur;
        bool   m_hideOnMouseOver;

signals:
        void signalCacheMasque();

public slots:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

};

#endif // MASQUEDEPLACESOURIS_H
