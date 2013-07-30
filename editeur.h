/** Classe Editeur
  * @file editeur.h
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012/2013 Icham Sirat <icham.sirat@ryxeo.com>
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
#include "editeurparcourswidget.h"
#include "abuledumediathequepushv1.h"
#include "abuledutools.h"
//#include "mainwindow.h"

namespace Ui {
class Editeur;
}

class Editeur : public QWidget
{
    Q_OBJECT

    /* Enum pour les pages afin de ne pas se tromper de numero dans le code  */
    enum Pages {
        PageAccueil         = 0x0,
        PageGestionImages   = 0x1,
        PageParametres      = 0x2,
        PageFin             = 0x3,
        PageVisio           = 0x5
    };

public:
    /** Constructeur de la classe Editeur */
    explicit Editeur(QWidget *parent);

    /** Destructeur de la classe Editeur */
    ~Editeur();

//    void abeEditeurSetMainWindow(QWidget* mw);
    void setAbeFile(const QSharedPointer<AbulEduFileV1>& abuleduFile){ _abuleduFile = abuleduFile ;}
    const QSharedPointer<AbulEduFileV1> abeFile(){ return _abuleduFile ; }

    QStackedWidget* stackedWidget(){ return ui->stackedWidgetEditeur ;}
    QPushButton* btnModificationCourant(){ return ui->btnModificationCourant ;}

private slots:
    /** Permet de supprimer une image du listWidgetImagesSelection
      * connecté à un menu contextuel appelé au clic droit sur le listWidgetImagesSelection */
    void slotSupprimerImage();

    /** Ajoute une image dans le listWidgetImagesSelection, avec icône et nom (sans extension, les images sont toutes enregistrées en .jpg) */
    void ajouterImage(QFileInfo monFichier);

    /** Crée un module à partir du contenu du dossier temporaire de l'application */
    void createAbe();

    /** Gère l'appui sur les boutons de parcours */
    void slotBtnParcours_clicked(const int&);

    /** Réinitialise l'editeur de parcours */
    void reinitialiserGvParcours();

    /** Sauvegarde le parcours conçu avec l'éditeur de parcours */
    void sauvegarderParcours();

    /** Gère la pose des masques dans l'éditeur de parcours */
    void masquePoseParcours(MasqueDeplaceSouris*);

    /** Fait apparaitre sur le clic droit le menu contextuel créé dans la méthode creationMenu(); */
    void on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos);

    /** Ouvre au double clic l'image dans la visionneuse */
    void on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item);

    /** Appelle la fonction ajouterImage pour une image provenant de la médiathèque */
    void slotImportImageMediatheque(QSharedPointer<AbulEduFileV1> fichierABB, const int &success);

//    /** Passe à la page précédente */
//    void on_btnPrecedent_clicked();

//    /** Passe à la page suivante
//      * Dans le cas de la dernière page, le bouton a pris l'icône d'enregistrement, la fonction createAbe() est appelée et l'éditeur fermé */
//    void on_btnSuivant_clicked();

//    /** Ferme l'application au clic sur le bouton Quitter*/
//    void on_btnQuitter_clicked();

    /** Création du module à partir du contenu du répertoire temporaire de l'application */
    void on_btnCreationAbe_clicked();

//    /** Charge un fichier reçu de l'AbulEduFileManagerV1 et le passe à la MainWindow */
//    void slotOpenFile(QSharedPointer<AbulEduFileV1>);

    /** Charge les paramètres de l'AbulEduFileV1 instancié dans l'application */
    void slotLoadUnit();

    /** Appelle la fonction ajouterImage pour une image provenant du parcours du disque dur */
    void on_btnAjouterImageQFileDialog_clicked();

    /** Appelle la fonction slotLoadUnit */
    void on_btnModificationCourant_clicked();

    /** Fait apparaitre l'AbulEduFileManagerV1, dont le signal signalAbeFileSelected lance slotOpenFile, et donc slotLoadUnit */
    void on_btnModificationAutre_clicked();

    /** Charge la position des masques en lisant le fichier de paramètre */
    bool chargerPositionMasque(const int& numeroParcours);

    void on_btnEssayer_clicked();

    void on_btnEnregistrer_clicked();

    void on_btnPublier_clicked();

    void slotSortieVisionneuse();

    /** Affiche si la publication a réussi et ramène à la dernière page de l'éditeur */
    void slotAfficheEtatPublication(const int& code);

    //! MODIF ICHAM 29.07.13
    void slotGestionPage(int);

    void on_btnRetourAccueil_clicked();

    void on_btnSuivantPageGestionImages_clicked();

private:
    Ui::Editeur *ui;
    bool m_localDebug;
    bool m_modeModificationAbe;

    QString m_lastOpenDir;

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

    QList<MasqueDeplaceSouris *> m_listeMasquesFixes;
    MasqueDeplaceSouris         *m_masqueDepart;
    MasqueDeplaceSouris         *m_masqueArrivee;
    MasqueDeplaceSouris         *m_masqueParcours;

    QDir *m_dir;
    QDir *m_dirAbe;

    QList<QString> m_listeFichiersImages;
    QMenu *m_menuListWidget;

    MasqueDeplaceSouris             *m_masque;
    QList<MasqueDeplaceSouris *>    m_listeMasques;
    QList<MasqueDeplaceSouris *>    m_listeMasquesParcours;
    EditeurParcoursWidget           *m_editeurParcoursWidget;

    QSharedPointer<AbulEduFileV1> _abuleduFile;

    QString _messageAidePageAccueil;
    QString _messageAidePageGestionImages;
    QString _messageAidePageParametres;
    QString _messageAidePageFin;
    QString _messageAidePageVisio;

    /** Pointeur vers le parent. C'est un QWidget* qu'il faudra caster en MainWindow*, mais qu'on ne peut pas déclarer tel pour cause d'inclusion circulaire */
    QWidget* m_parent;

    /** Créer le menu "supprimer" sur un item contenu dans listWidgetImagesSelection */
    void creationMenu();

    /** Remplie la scène de l'éditeur de parcours en fonction des paramètres définies (nombre de masques largeur/hauteur)
      * Si mode création, la scène n'aura pas de parcours
      * Si mode modification, la scène créera le parcours contenu dans l'abe courant
      * @param int, numeroParcours, le numéro du parcours à créer/modifier
      * @todo commenter le bool retour
      */
    bool remplirGvParcours(const int& numeroParcours);

    /** Définit le mode de l'éditeur (création/modification) bool m_modeModificationAbe
      * @param bool yesNo, true = mode modification, false = mode création
      */
    void setModeModificationAbe(const bool& yesNo);

    /** Gestion du drop
      * @param QDropEvent *event, un pointeur sur l'évènement drop
      */
    void dropEvent(QDropEvent *event);

    /** Gestion du drag
      * @param QDragEnterEvent *event, un pointeur sur l'évènement drag
      */
    void dragEnterEvent(QDragEnterEvent *event);

    /** Factorisation de la préparation de la sauvegarde, que ce soit pour enregistrement du fichier ou publication pour la médiathèque
      * Écriture des informations ScoLOMFR et copie des informations et fichiers dans les dossiers conf et data */
    bool preparerSauvegarde();

    /** Publie le fichier édité sur la médiathèque */
    void releaseAbe();

    /** Permet de mapper les signaux des boutons de parcours. (pour factorisation)
      * @see QSignalMapper.
      */
    void mapSignalBtnParcours();

    void initMessagesAide();
signals:
    /** Sert à réinitialiser le titre dans la barre de la Mainwindow si jamais on créé un nouveau module, en ayant déjà un d'ouvert (et son nom dans la barre des titres) */
    void editorNewAbe(const int& newAbe);
    void editorExited();
    void editorTest();
    void editorChooseOrSave(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave openOrSave);
};

#endif // EDITEUR_H
