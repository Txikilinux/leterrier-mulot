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

#ifndef EDITEUR_H
#define EDITEUR_H

#include <QDialog>
#include <QDebug>
#include <QDir>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QLabel>
#include <QDesktopWidget>
#include <QFileSystemModel>
#include <QImageReader>

#include "abuledufilev1.h"
#include "visionneuseimage.h"
#include "masquedeplacesouris.h"
#include "abuleduetiquettesv1.h"
#include "abuleduboxfilemanagerv1.h"
#include "ui_editeur.h"
#include "mainwindow.h"

namespace Ui {
class Editeur;
}

class Editeur : public QDialog
{
    Q_OBJECT

public:
    explicit Editeur(QWidget *parent);
    ~Editeur();

private slots:

    void slotSupprimerImage();
//    void slotResizeColumn(const QModelIndex& index);
    void createAbe();

    // Edition Parcours
    void on_btnParcours1_clicked();
    void on_btnParcours2_clicked();
    void on_btnParcours3_clicked();
    void on_btnParcours4_clicked();
    void on_btnParcours5_clicked();

    void reinitialiserGvParcours();
    void sauvegarderParcours();
    void masquePoseParcours(masqueDeplaceSouris*);

    QList<int> masquesVoisins(int numeroMasque, int largeur, int hauteur);

    void on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos);
    void on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item);

    void ajouterImage(QFileInfo monFichier);
    void slotImportImageMediatheque();

//    void on_treeViewArborescence_doubleClicked(const QModelIndex &index);


    void on_btnPrecedent_clicked();
    void on_btnSuivant_clicked();
    void majBarreNavigation(int numPage);
    void on_btnQuitter_clicked();

    void on_btnCreationAbe_clicked();

    void on_btnModificationAbe_clicked();
    void slotOpenFile();

    /** Charge les paramètres d'abeFile */
    void slotLoadUnit();

//    void testThread();
    void slotTestImportImage(QString cheminFichier, QString nomFichier);

    void on_btnAjouterImageQFileDialog_clicked();

    void on_btnModificationCourant_clicked();

private:
    Ui::Editeur *ui;
    bool m_localDebug;

//    QStringList m_listeFichiers; // la liste des fichiers images présents dans le dossier choisi
    QStringList m_listeDossiers; // la liste des dossiers ouverts

    int m_opt_nbMasquesChoisisParcours;
    int m_opt_nbMasquesLargeur;
    int m_opt_nbMasquesHauteur;
    int m_numeroParcours;

    QMap<QString, QVariant> m_parametresParcours1;
    QMap<QString, QVariant> m_parametresParcours2;
    QMap<QString, QVariant> m_parametresParcours3;
    QMap<QString, QVariant> m_parametresParcours4;
    QMap<QString, QVariant> m_parametresParcours5;

    QDir *m_dir;
    QDir *m_dirAbe;
    VisionneuseImage *m_visionneuseImage;

    QList<QString> m_listeFichiersImages;      // pour ranger chaque chemin d'images
    QMenu *m_menuListWidget;   // menu contextuel listWidget
//---------------Chemin temp
    AbulEduFileV1 *m_abuledufilev1;
    QDir *m_destImage;
    QDir *m_destImageABE;

    QString m_destinationIdUnique;
    QString m_cheminImage;
    QString m_cheminConf;

    //Gestion des images
    void remplirArborescence();
    void creationMenu();
    bool copierImageDansTemp(QFileInfo cheminOriginal, QString dossierDestination);

    bool supprimerDir(const QString& dirPath);
    QStringList parcoursRecursif(QString dossier);

    // Parcours
    masqueDeplaceSouris *m_masque;
    QList<masqueDeplaceSouris *> m_listeMasques;
    QList<masqueDeplaceSouris *> m_listeMasquesParcours;
    AbulEduEtiquettesV1 *gv_AireParcours;
    void remplirGvParcours();
    bool controleVoisinMasque(masqueDeplaceSouris *masque);

    // Mode d'édition
    bool m_modeModificationAbe; // ce booleen permet de definir si on est en creation ou en modification d'un .abe
    void setModeModificationAbe(bool yesNo);
    AbulEduBoxFileManagerV1 *m_abuleduFileManager;
    void initCheminTemp();

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

    QString m_lastOpenDir;
    MainWindow* m_parent;


//protected:
//    bool eventFilter(QObject *obj, QEvent *ev);

};

#endif // EDITEUR_H
