/** Classe EditeurParcoursWidget
  * @file editeurparcourswidget.h
  * @see https://redmine.ryxeo.com/projects/
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
#ifndef EDITEURPARCOURSWIDGET_H
#define EDITEURPARCOURSWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QPushButton>
#include <QDebug>

namespace Ui {
class EditeurParcoursWidget;
}

class EditeurParcoursWidget : public QWidget
{
    Q_OBJECT
    
public:
    /** Constructeur de la classe EditeurParcoursWidget */
    explicit EditeurParcoursWidget(QWidget *parent = 0);

    /** Destructeur de la classe EditeurParcoursWidget */
    ~EditeurParcoursWidget();
    
    /** Retourne la QGraphicsScene contenue dans la QGraphicsView
      * @return QGraphicsScene *, un pointeur sur la scene
      */
    QGraphicsScene *getScene() const;

    /** Retourne la QGraphicsView
      * @return QGraphicsView *, un pointeur sur la vue
      */
    QGraphicsView *getGraphicsView() const;

    /** Retourne le bouton sauvegarder
      * @return QPushButton *, un pointeur sur le bouton sauvegarder
      */
    QPushButton *getBtnSave() const;

    /** Retourne le bouton réinitialiser
      * @return QPushButton *, un pointeur sur le bouton réinitialiser
      */
    QPushButton *getBtnReset() const;

    /** Retourne la hauteur d'un bouton (sauvegarder ou quitter)
      * @return int, la hauteur du bouton
      */
    int getBoutonHeight() const;

    /** Retourne la largeur d'un bouton (sauvegarder ou quitter)
      * @return int, la largeur du bouton
      */
    int getBoutonWidth() const;

    /** Permet d'activer/desactiver le bouton sauvegarder
      * @param bool enable, true = bouton activé, false = bouton desactiver
      */
    void connectBtnSave(bool enable);

private:
    Ui::EditeurParcoursWidget *ui;
    QGraphicsScene *m_scene;

    bool m_localDebug;

private slots:
    /** Méthode appelée lors de fermeture de la fenetre
      * @param QCloseEvent *event, un pointeur sur l'évènement fermeture
      */
    void closeEvent(QCloseEvent *event);

    /** Méthode appelée lors d'un click sur le bouton réinitialiser
      */
    void on_btnReset_clicked();

signals:
    /** Signal émis lors de la fermeture de la fenetre
      * @param QCloseEvent *event, un pointeur sur l'évènement fermeture
      */
    void signalCloseEvent(QCloseEvent *event);
};

#endif // EDITEURPARCOURSWIDGET_H
