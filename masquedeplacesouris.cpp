/** Classe masqueDeplaceSouris
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
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

masqueDeplaceSouris::masqueDeplaceSouris(QGraphicsObject *parent, int numero) :
    QGraphicsObject(parent)
{
    m_taille = QRectF(0,0,50,50); //Une taille par défaut de 50x50
    m_couleur = QColor(Qt::black);
    m_hideOnMouseOver = false;
    m_hideOnClick = false;
    m_hideOnDoubleClick = false;
    m_isEditable = false;
    m_numero = numero;
    setAcceptsHoverEvents(true);

    m_menuMasque = new QMenu();
    m_menuMasque->addAction(trUtf8("Réinitialiser"), this, SLOT(on_action_Reinitialiser()));
    m_menuMasque->addAction(trUtf8("Sauvegarder"), this, SLOT(on_action_Sauvegarder()));
    connect(m_menuMasque, SIGNAL(triggered(QAction *)), this, SLOT(triggered(QAction *)));

    cptClic = 0;
    QApplication::setDoubleClickInterval(1000);
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

void masqueDeplaceSouris::setHideOnClick(bool hide)
{
    m_hideOnClick = hide;
}

void masqueDeplaceSouris::setHideOnDoubleClick(bool hide)
{
    m_hideOnDoubleClick = hide;
}

void masqueDeplaceSouris::setIsEditable(bool isEditable)
{
    m_isEditable = isEditable;
}

QColor masqueDeplaceSouris::getColor()
{
    return m_couleur;
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
    if(m_hideOnMouseOver)
    {
        setVisible(false);
        emit signalCacheMasque();
    }
}

void masqueDeplaceSouris::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons()== Qt::LeftButton)
    {
        qDebug() << "clic gauche";
        if(m_hideOnClick)
        {
            event->accept();
            setVisible(false);
            emit signalCacheMasque();
        }
        else if(m_isEditable)
        {
            event->accept();
            qDebug() << "Click gauche masque";
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
                    qDebug() << "Double Clic evenement";
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
        qDebug() << "clic droit";
        if (m_isEditable)
        {
            m_menuMasque->popup(event->screenPos());
        }
    }
}

void masqueDeplaceSouris::mouseDoubleClickEvent(QGraphicsSceneEvent *event)
{
    qDebug()<< __FUNCTION__;

    if(m_hideOnDoubleClick)
    {
        qDebug() << "DOUBLE CLIC";
        event->accept();
        setVisible(false);
        emit signalCacheMasque();
    }
}

void masqueDeplaceSouris::on_action_Reinitialiser()
{
    emit signalReinitialisationMasque();
}

void masqueDeplaceSouris::on_action_Sauvegarder()
{
    emit signalSauvegarderParcours();
}

/** Permet d'activer ou de désactiver l'entrée "Sauvegarder" du menu du masque
  */
void masqueDeplaceSouris::setMenuSauvegarderEnabled(bool yesNo)
{
    // Chercher l'action Sauvegarder
    for (int i =0; i < m_menuMasque->actions().count(); i++)
    {
        if (m_menuMasque->actions().at(i)->iconText() == trUtf8("Sauvegarder"))
        {
            if(!yesNo) // si false = desactivation
            {
                m_menuMasque->actions().at(i)->setEnabled(false);
            }
            else // si true = activation
            {
                m_menuMasque->actions().at(i)->setEnabled(true);
            }
        }
    }
}

