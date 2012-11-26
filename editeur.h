/** Classe Editeur
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
  * @author 2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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
    /** Permet de supprimer une image du listWidgetImagesSelection
      * connecté à un menu contextuel appelé au clic droit sur le listWidgetImagesSelection */
    void slotSupprimerImage();

    /** Crée un module à partir du contenu du dossier temporaire de l'application */
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

    /** Fait apparaitre sur le clic droit le menu contextuel créé dans la méthode creationMenu(); */
    void on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos);

    /** Ouvre au double clic l'image dans la visionneuse */
    void on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item);

    /** Ajoute une image dans le listWidgetImagesSelection, avec icône en et nom (sans extension, les images sont toutes enregistrées en .jpg) */
    void ajouterImage(QFileInfo monFichier);

    /** Appelle la fonction ajouterImage pour une image provenant de la médiathèque */
    void slotImportImageMediatheque();

    /** Passe à la page précédente */
    void on_btnPrecedent_clicked();

    /** Passe à la page suivante
      * Dans le cas de la dernière page, le bouton a pris l'icône d'enregistrement, la fonction createAbe() est appelée et l'éditeur fermé */
    void on_btnSuivant_clicked();

    /** Gère les boutons Précédent et Suivant
      * Le bouton Précédent est masqué si on est à la première page
      * Le bouton Suivant prend l'icône d'enregistrement si on est à la dernière page */
    void majBarreNavigation(int numPage);

    /** Ferme l'application au clic sur le bouton Quitter*/
    void on_btnQuitter_clicked();

    /** Création du module à partir du contenu du répertoire temporaire de l'application */
    void on_btnCreationAbe_clicked();

    /** Charge un fichier reçu de l'AbulEduFileManagerV1 et le passe à la MainWindow */
    void slotOpenFile();

    /** Charge les paramètres de l'AbulEduFileV1 instancié dans l'application */
    void slotLoadUnit();

    void slotTestImportImage(QString cheminFichier, QString nomFichier);

    /** Appelle la fonction ajouterImage pour une image provenant du parcours du disque dur */
    void on_btnAjouterImageQFileDialog_clicked();

    /** Appelle la fonction slotLoadUnit */
    void on_btnModificationCourant_clicked();

    /** Fait apparaitre l'AbulEduFileManagerV1, dont le signal signalAbeFileSelected lance slotOpenFile, et donc slotLoadUnit */
    void on_btnModificationAutre_clicked();

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

    //Gestion des images
    void creationMenu();
    bool copierImageDansTemp(QFileInfo fi);

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

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

    QString m_lastOpenDir;
    MainWindow* m_parent;
};

#endif // EDITEUR_H
