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
#include "editeurparcourswidget.h"

namespace Ui {
class Editeur;
}

class Editeur : public QDialog
{
    Q_OBJECT

public:
    /** Constructeur de la classe Editeur */
    explicit Editeur(QWidget *parent);

    /** Destructeur de la classe Editeur */
    ~Editeur();

private slots:
    /** Permet de supprimer une image du listWidgetImagesSelection
      * connecté à un menu contextuel appelé au clic droit sur le listWidgetImagesSelection */
    void slotSupprimerImage();

    /** Crée un module à partir du contenu du dossier temporaire de l'application */
    void createAbe();

    /** Gère l'appui sur le bouton parcours 1*/
    void on_btnParcours1_clicked();

    /** Gère l'appui sur le bouton parcours 2*/
    void on_btnParcours2_clicked();

    /** Gère l'appui sur le bouton parcours 3*/
    void on_btnParcours3_clicked();

    /** Gère l'appui sur le bouton parcours 4*/
    void on_btnParcours4_clicked();

    /** Gère l'appui sur le bouton parcours 5*/
    void on_btnParcours5_clicked();

    /** Réinitialise l'editeur de parcours */
    void reinitialiserGvParcours();

    /** Sauvegarde le parcours conçu avec l'éditeur de parcours */
    void sauvegarderParcours();

    /** Gère la pose des masques dans l'éditeur de parcours */
    void masquePoseParcours(masqueDeplaceSouris*);

    /** Calcule et retourne la liste des voisins possibles d'un masque
      * @param numeroMasque, int, le numero du masque courant
      * @param largeur, int, le nombre de masques dans la largeur du parcours
      * @param hauteur, int, le nombre de masques dans la hauteur du parcours
      * @return voisinsMasques, QList<Int>, la liste des voisins possibles (gauche, droite, haut, bas)
      */
    QList<int> masquesVoisins(int numeroMasque, int largeur, int hauteur);

    /** Fait apparaitre sur le clic droit le menu contextuel créé dans la méthode creationMenu(); */
    void on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos);

    /** Ouvre au double clic l'image dans la visionneuse */
    void on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item);

    /** Ajoute une image dans le listWidgetImagesSelection, avec icône et nom (sans extension, les images sont toutes enregistrées en .jpg) */
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

    /** Appelle la fonction ajouterImage pour une image provenant du parcours du disque dur */
    void on_btnAjouterImageQFileDialog_clicked();

    /** Appelle la fonction slotLoadUnit */
    void on_btnModificationCourant_clicked();

    /** Fait apparaitre l'AbulEduFileManagerV1, dont le signal signalAbeFileSelected lance slotOpenFile, et donc slotLoadUnit */
    void on_btnModificationAutre_clicked();

    /** Charge la position des masques en lisant le fichier de paramètre */
    void chargerPositionMasque(int numeroParcours);

    /** Gère la fermeture de l'editeur de parcours */
    void slotFermetureEditeurParcoursWidget(QCloseEvent*);

private:
    Ui::Editeur *ui;
    bool m_localDebug;
    bool m_modeModificationAbe;

    QString m_lastOpenDir;
    MainWindow* m_parent;

    QStringList m_listeDossiers;

    int m_opt_nbMasquesChoisisParcours;
    int m_opt_nbMasquesLargeur;
    int m_opt_nbMasquesHauteur;
    int m_numeroParcours;

    QMap<QString, QVariant> m_parametresParcours1;
    QMap<QString, QVariant> m_parametresParcours2;
    QMap<QString, QVariant> m_parametresParcours3;
    QMap<QString, QVariant> m_parametresParcours4;
    QMap<QString, QVariant> m_parametresParcours5;

    QMap<QString, int> positionMasquesParcours;

    QList<masqueDeplaceSouris *> m_listeMasquesFixes;
    masqueDeplaceSouris *m_masqueDepart;
    masqueDeplaceSouris *m_masqueArrivee;
    masqueDeplaceSouris *m_masqueParcours;

    QDir *m_dir;
    QDir *m_dirAbe;
    VisionneuseImage *m_visionneuseImage;

    QList<QString> m_listeFichiersImages;
    QMenu *m_menuListWidget;

    masqueDeplaceSouris *m_masque;
    QList<masqueDeplaceSouris *> m_listeMasques;
    QList<masqueDeplaceSouris *> m_listeMasquesParcours;
    EditeurParcoursWidget *gv_AireParcours;

    AbulEduBoxFileManagerV1 *m_abuleduFileManager;

    /** Créer le menu "supprimer" sur un item contenu dans listWidgetImagesSelection */
    void creationMenu();

    /** Copie une image (téléchargée ou locale) dans le dossier de travail de l'éditeur
      * @param QFileInfo fi, le chemin de l'image à copier
      * @return bool, true si la copie est effective, false sinon
      */
    bool copierImageDansTemp(QFileInfo fi);

    /** Supprime un répertoire et tout son contenu
      * Le répertoire passé en paramètre est aussi supprimé
      * @param const QString& dirPath, le chemin du répertoire à supprimer (ex : "/home/user/monRepertoire")
      * @return bool, true si suppression ok, false sinon
      */
    bool supprimerDir(const QString& dirPath);

    /** Retourne une liste de chemin correspondant à l'arborescence située en dessous du dossier donné en paramètre
      * @param const QString dossier, le chemin du dossier à analyser
      * @return QStringList, la liste des chemins correspondant à l'arborescence descandante du dossier
      */
    QStringList parcoursRecursif(const QString dossier);

    /** Remplie la scène de l'éditeur de parcours en fonction des paramètres définies (nombre de masques largeur/hauteur)
      * Si mode création, la scène n'aura pas de parcours
      * Si mode modification, la scène créera le parcours contenu dans l'abe courant
      * @param int, numeroParcours, le numéro du parcours à créer/modifier
      */
    void remplirGvParcours(int numeroParcours);

//    bool controleVoisinMasque(masqueDeplaceSouris *masque);

    /** Définit le mode de l'éditeur (création/modification) bool m_modeModificationAbe
      * @param bool yesNo, true = mode modification, false = mode création
      */
    void setModeModificationAbe(bool yesNo);

    /** Gestion du drop
      * @param QDropEvent *event, un pointeur sur l'évènement drop
      */
    void dropEvent(QDropEvent *event);

    /** Gestion du drag
      * @param QDragEnterEvent *event, un pointeur sur l'évènement drag
      */
    void dragEnterEvent(QDragEnterEvent *event);

};

#endif // EDITEUR_H
