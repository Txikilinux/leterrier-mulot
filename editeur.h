#ifndef EDITEUR_H
#define EDITEUR_H

#include <QDialog>
#include <QDebug>
#include <QDir>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QMenu>
#include <QFileDialog>
/** Classe Editeur
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

#include <QMessageBox>
#include <QIcon>
#include <QLabel>

#include "visionneuseimage.h"


namespace Ui {
class Editeur;
}

class Editeur : public QDialog
{
    Q_OBJECT
    
public:
    explicit Editeur(QWidget *parent = 0);
    ~Editeur();

private slots:

    void on_btnImporterDossierImage_clicked();

    // actions menu TreeWidet
    void on_action_Supprimer_album_triggered();
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    // actions list Widget
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::Editeur *ui;

    bool m_localDebug;

    QStringList m_listeFichiers; // la liste des fichiers images présents dans le dossier choisi
    QStringList m_listeDossiers; // la liste des dossiers ouverts
    QDir *m_dir;
    QMenu *m_menuTreeWidget;
    VisionneuseImage *m_visionneuseImage;

    void rafraichirListeImages();


};

#endif // EDITEUR_H
