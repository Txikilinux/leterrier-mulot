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
    QGraphicsObject(parent)
{
    m_taille = QRectF(0,0,50,50); //Une taille par défaut de 50x50
    m_couleur = QColor(Qt::black);
    m_hideOnMouseOver = false;
    m_hideOnClick = false;
    m_hideOnDoubleClick = false;
    m_isEditable = false;
    m_numero = numero;
    setAcceptHoverEvents(true);

    cptClic = 0;
    QApplication::setDoubleClickInterval(1000);
}

MasqueDeplaceSouris::~MasqueDeplaceSouris()
{
}

void MasqueDeplaceSouris::setSize(float width, float height)
{
    m_taille = QRectF(0,0,width,height);
}

void MasqueDeplaceSouris::setColor(QColor couleur)
{
    m_couleur = couleur;
}

void MasqueDeplaceSouris::setHideOnMouseOver(bool hide)
{
    m_hideOnMouseOver = hide;
}

void MasqueDeplaceSouris::setHideOnClick(bool hide)
{
    m_hideOnClick = hide;
}

void MasqueDeplaceSouris::setHideOnDoubleClick(bool hide)
{
    m_hideOnDoubleClick = hide;
}

void MasqueDeplaceSouris::setIsEditable(bool isEditable)
{
    m_isEditable = isEditable;
}

void MasqueDeplaceSouris::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath zoneDessin;
    zoneDessin.addRect(boundingRect());
    painter->setBrush(QBrush(m_couleur));
    //    painter->setPen(Qt::NoPen);
    painter->drawRect(m_taille);
}

QRectF MasqueDeplaceSouris::boundingRect() const
{
    return m_taille;
}

void MasqueDeplaceSouris::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    if(m_hideOnMouseOver)
    {
        setVisible(false);
        emit signalCacheMasque();
    }
}

void MasqueDeplaceSouris::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons()== Qt::LeftButton)
    {
        //qDebug() << "clic gauche";
        if(m_hideOnClick)
        {
            event->accept();
            setVisible(false);
            emit signalCacheMasque();
        }
        else if(m_isEditable)
        {
            event->accept();
            //qDebug() << "Click gauche masque";
            emit signalPoseSurParcours(this);
        }
        else if (m_hideOnDoubleClick)
        {
            if (cptClic == 0)
            {
                timerDoubleClic.start();
                cptClic ++;
            }
            else if (cptClic == 1)
            {
                if (timerDoubleClic.elapsed() < QApplication::doubleClickInterval() )
                {
                    //qDebug() << "Double Clic evenement";
                    event->accept();
                    setVisible(false);
                    emit signalCacheMasque();

                    cptClic = 0;
                }
                else
                    cptClic =0;
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
    qDebug()<< __FUNCTION__;

    if(m_hideOnDoubleClick)
    {
        //qDebug() << "DOUBLE CLIC";
        event->accept();
        setVisible(false);
        emit signalCacheMasque();
    }
}

