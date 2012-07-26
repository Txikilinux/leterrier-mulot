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

void masqueDeplaceSouris::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (m_isEditable)
    {
        QMenu *menu = new QMenu;
        menu->addAction(trUtf8("Depart"),this,SLOT(on_action_Depart()));
        menu->addAction(trUtf8("Arrivée"),this, SLOT(on_action_Arrivee()));
        menu->addAction(trUtf8("Parcours"), this, SLOT(on_action_Parcours()));
        menu->addSeparator();
        menu->addAction(trUtf8("Enlever"), this, SLOT(on_action_Enlever()));
        menu->addAction(trUtf8("Réinitialiser"), this, SLOT(on_action_Reinitialiser()));
        menu->addAction(trUtf8("Sauvegarder"), this, SLOT(on_action_Sauvegarder()));
        menu->popup(event->screenPos());

        connect(menu, SIGNAL(triggered(QAction *)),
            this, SLOT(triggered(QAction *)));
    }
}

void masqueDeplaceSouris::on_action_Depart()
{
    qDebug() << "APPUI Menu Depart sur le n° " << this->getNumero();
//    this->setColor(QColor(Qt::green));
//    this->setProperty("Role", "Depart");
//    this->update();
    emit signalMasqueDepart(this);
}

void masqueDeplaceSouris::on_action_Arrivee()
{
    qDebug() << "APPUI Menu Arrivee sur le n° " << this->getNumero();
//    this->setColor(QColor(Qt::red));
//    this->setProperty("Role", "Arrivee");
//    this->update();
    emit signalMasqueArrivee();
}

void masqueDeplaceSouris::on_action_Parcours()
{
    qDebug() << "APPUI Menu Parcours sur le n° " << this->getNumero();
//    this->setColor(QColor(Qt::black));
//    this->setProperty("Role", "Chemin");
//    this->update();
    emit signalMasqueParcours();
}

void masqueDeplaceSouris::on_action_Enlever()
{
    qDebug() << "APPUI Menu Enlever sur le n° " << this->getNumero();
//    this->setColor(QColor(Qt::white));
//    this->setProperty("Role", "Fixe");
//    this->update();
    emit signalMasqueEnlever();
}

void masqueDeplaceSouris::on_action_Reinitialiser()
{
    qDebug() << "APPUI Menu Reinitialiser sur le n° " << this->getNumero();
    emit signalReinitialisationMasque();
}

void masqueDeplaceSouris::on_action_Sauvegarder()
{
    qDebug() << "APPUI Menu Sauvegarder sur le n° " << this->getNumero();
    emit signalSauvegarderParcours();
}


