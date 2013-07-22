/** Classe MasqueDeplaceSouris
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


#ifndef MasqueDeplaceSouris_H
#define MasqueDeplaceSouris_H

#include <QGraphicsObject>
#include <QPainter>
#include <QDebug>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QApplication>
#include <QTime>

class MasqueDeplaceSouris : public QGraphicsObject
{
    Q_OBJECT

public:
    /** Constructeur de la classe MasqueDeplaceSouris
      * @param QGraphicsObject *parent = 0, un pointeur vers le parent de l'objet, initialisé par défaut à 0
      * @param int numero = 0             , le numero du masque, initialisé par défaut à 0
      */
    explicit MasqueDeplaceSouris(QGraphicsObject *parent = 0, int numero = 0);

    /** Destructeur de la classe MasqueDeplaceSouris */
    ~MasqueDeplaceSouris();

    /** Méthode qui permet de changer la taille du masque
      * @param float width , la nouvelle largeur
      * @param float height, la nouvelle hauteur
      */
    void setSize(float width,float height);

    /** Méthode qui permet de changer la couleur du masque
      * @param QColor colour, la nouvelle couleur
      */
    void setColor(QColor couleur);

    /** Méthode qui permet de définir si le masque disparait au survol de la souris
      * @param bool hide, true si oui, false sinon
      */
    void setHideOnMouseOver(bool hide);

    /** Méthode qui permet de définir si le masque disparait au clic de la souris
      * @param bool hide, true si oui, false sinon
      */
    void setHideOnClick(bool hide);

    /** Méthode qui permet de définir si le masque disparait au double-clic de la souris
      * @param bool hide, true si oui, false sinon
      */
    void setHideOnDoubleClick(bool hide);

    /** Méthode qui permet de définir si le masque est éditable
      * @param bool hide, true si oui, false sinon
      */
    void setIsEditable(bool isEditable);

    /** Méthode qui retourne la couleur actuelle du masque
      * @return QColor, la couleur actuelle du masque
      */
    QColor getColor() const{return m_couleur;}

    /** Méthode qui retourne le numero actuel du masque
      * @return QColor, le numero actuel du masque
      */
    int getNumero() const{ return m_numero;}

private:
    /** Méthode qui dessine à l'écran le masque
      * @param QPainter *painter, un pointeur sur l'objet qui dessine le masque
      * @param const QStyleOptionGraphicsItem *option, un pointeur sur les paramètres de dessin
      * @param QWidget *widget, un pointeur sur l'objet où sera dessiné le masque
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /** Méthode qui retourne la taille du masque
      * @return QRectF, la taille du masque
      */
    QRectF boundingRect() const;
    QRectF m_taille;
    QColor m_couleur;
    bool   m_hideOnMouseOver;
    bool   m_hideOnClick;
    bool   m_hideOnDoubleClick;
    bool   m_isEditable;
    int    m_numero;
    QMenu  *m_menuMasque;

    int cptClic;
    QTime timerDoubleClic;

signals:
    void signalCacheMasque();
    void signalPoseSurParcours(MasqueDeplaceSouris*);

public slots:
    /** Méthode appelée au survol du masque par la souris */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    /** Méthode appelée au clic sur le masque par la souris */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /** Méthode appelée au double-clic sur le masque par la souris */
    void mouseDoubleClickEvent(QGraphicsSceneEvent *event);
};

#endif // MasqueDeplaceSouris_H
