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


#include "masquedeplacesouris.h"

masqueDeplaceSouris::masqueDeplaceSouris(QGraphicsObject *parent) :
    QGraphicsObject(parent)
{
    m_taille = QRectF(0,0,50,50);
    setAcceptsHoverEvents(true);
}

void masqueDeplaceSouris::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath zoneDessin;
    zoneDessin.addRect(boundingRect());
    painter->setBrush(Qt::SolidPattern);
    painter->drawRect(m_taille);
}

QRectF masqueDeplaceSouris::boundingRect() const
{
    return QRectF(0,0,50,50);
}

void masqueDeplaceSouris::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    setVisible(false);
}

void masqueDeplaceSouris::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    setVisible(false);
}
