/** Classe masqueDeplaceSouris
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
#include <QDebug>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

masqueDeplaceSouris::masqueDeplaceSouris(QGraphicsObject *parent, int numero) :
    QGraphicsObject(parent)
{
    //Une taille par défaut de 50x50
    m_taille = QRectF(0,0,50,50);
    m_couleur = QColor(Qt::black);
    m_hideOnMouseOver = true;
    m_isEditable = false;
    m_numero = numero;
    setAcceptsHoverEvents(true);

}

void masqueDeplaceSouris::setSize(float width, float height)
{
    m_taille = QRectF(0,0,width,height);
}

void masqueDeplaceSouris::setColor(QColor couleur)
{
    m_couleur = couleur;
}

void masqueDeplaceSouris::setHideOnMouseOver(bool hide)
{
    m_hideOnMouseOver = hide;
}

void masqueDeplaceSouris::setIsEditable(bool isEditable)
{
    m_isEditable = isEditable;
}

int masqueDeplaceSouris::getNumero()
{
    return m_numero;
}

void masqueDeplaceSouris::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath zoneDessin;
    zoneDessin.addRect(boundingRect());
    painter->setBrush(QBrush(m_couleur));
//    painter->setPen(Qt::NoPen);
    painter->drawRect(m_taille);

}

QRectF masqueDeplaceSouris::boundingRect() const
{
    return m_taille;
}

void masqueDeplaceSouris::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    if(m_hideOnMouseOver) {
        setVisible(false);
        emit signalCacheMasque();
    }
}

//void masqueDeplaceSouris::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    qDebug() << "Je passe sur cette Item";
//    this->acceptedMouseButtons();
//    qDebug() << this->getNumero();
//}

void masqueDeplaceSouris::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (m_isEditable)
    {
        QMenu *menu = new QMenu;
        menu->addAction("Action 1");
        menu->addAction("Action 2");
        menu->popup(event->screenPos());
        qDebug() << this->getNumero();

        connect(menu, SIGNAL(triggered(QAction *)),
            this, SLOT(triggered(QAction *)));
    }
}
