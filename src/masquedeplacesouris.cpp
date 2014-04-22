/** Classe MasqueDeplaceSouris
  * @file masquedeplacesouris.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2013 Icham Sirat <icham.sirat@ryxeo.com>
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

MasqueDeplaceSouris::MasqueDeplaceSouris(QGraphicsObject *parent, int numero) :
    QGraphicsObject(parent), m_taille(QRectF()), m_numero(numero)
{
    m_couleur = QColor(Qt::black);
    m_hideOnMouseOver = false;
    m_hideOnClick = false;
    m_hideOnDoubleClick = false;
    m_isEditable = false;
    setAcceptHoverEvents(true);

    m_cptClic = 0;
    QApplication::setDoubleClickInterval(1000);
}

void MasqueDeplaceSouris::setSize(float width, float height)
{
    m_taille = QRectF(0,0,width,height);
}

void MasqueDeplaceSouris::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath zoneDessin;
    zoneDessin.addRect(boundingRect());
    painter->setBrush(QBrush(m_couleur));
    //    painter->setPen(Qt::NoPen);
    painter->drawRect(m_taille);
}

void MasqueDeplaceSouris::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    if(m_hideOnMouseOver){
        setVisible(false);
        emit signalCacheMasque();
    }
}

void MasqueDeplaceSouris::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons()== Qt::LeftButton){
        /* clic gauche */
        if(m_hideOnClick)
        {
            event->accept();
            setVisible(false);
            emit signalCacheMasque();
        }
        else if(m_isEditable)
        {
            event->accept();
            emit signalPoseSurParcours(this);
        }
        else if (m_hideOnDoubleClick)
        {
            if (m_cptClic == 0)
            {
                m_timerDoubleClic.start();
                m_cptClic ++;
            }
            else if (m_cptClic == 1)
            {
                if (m_timerDoubleClic.elapsed() < QApplication::doubleClickInterval() )
                {
                    /* Double Clic */
                    event->accept();
                    setVisible(false);
                    emit signalCacheMasque();

                    m_cptClic = 0;
                }
                else
                    m_cptClic =0;
            }
        }
    }
    else if (event->buttons() == Qt::RightButton)
    {
        //qDebug() << "clic droit";
//        if (m_isEditable)
//        {
//            m_menuMasque->popup(event->screenPos());
//        }
    }
}

void MasqueDeplaceSouris::mouseDoubleClickEvent(QGraphicsSceneEvent *event)
{
    if(m_hideOnDoubleClick){
        event->accept();
        setVisible(false);
        emit signalCacheMasque();
    }
}

