/** Classe VisionneuseImage
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

#ifndef VISIONNEUSEIMAGE_H
#define VISIONNEUSEIMAGE_H

#include <QMainWindow>
#include <QPrinter>
#include <QtGui>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class VisionneuseImage : public QMainWindow
{
    Q_OBJECT

public:
    VisionneuseImage(QWidget *parent = 0);
    ~VisionneuseImage();

    void ouvrirFicher(QString chemin);

private slots:
    void ouvrir();
    void imprimer();
    void zoomIn();
    void zoomOut();
    void tailleNormale();
    void ajusterFenetre();
    void aPropos();

private:
    void creerActions();
    void creerMenus();
    void majActions();
    void scaleImage(double facteur);
    void adjustScrollBar(QScrollBar *scrollBar, double facteur);

    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
    QWidget *m_parent;
    bool m_localDebug;

#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    QAction *a_ouvrir;
    QAction *a_imprimer;
    QAction *a_quitter;
    QAction *a_zoomIn;
    QAction *a_zoomOut;
    QAction *a_tailleNormal;
    QAction *a_ajusterFenetre;
    QAction *a_aPropos;
    QAction *a_aProposQt;

    QMenu *menu_fichier;
    QMenu *menu_vue;
    QMenu *menu_aide;
};

#endif // VISIONNEUSEIMAGE_H
