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


#ifndef MASQUEDEPLACESOURIS_H
#define MASQUEDEPLACESOURIS_H

#include <QGraphicsObject>
#include <QPainter>

class masqueDeplaceSouris : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit masqueDeplaceSouris(QGraphicsObject *parent = 0, int numero = 0);
    void setSize(float width,float height);
    void setColor(QColor couleur);
    void setHideOnMouseOver(bool hide);
    void setHideOnClick(bool hide);
    void setIsEditable(bool isEditable);
    QColor getColor();
    int getNumero();
    void setMenuSauvegarderEnabled(bool yesNo);



private:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF m_taille;
        QRectF boundingRect() const;
        QColor m_couleur;
        bool   m_hideOnMouseOver;
        bool   m_hideOnClick;
        bool   m_isEditable;
        int    m_numero;
        QMenu  *m_menuMasque;

signals:
        void signalCacheMasque();
        void signalReinitialisationMasque();
        void signalSauvegarderParcours();
        void signalPoseSurParcours(masqueDeplaceSouris*);

public slots:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void on_action_Reinitialiser();
    void on_action_Sauvegarder();
};

#endif // MASQUEDEPLACESOURIS_H
