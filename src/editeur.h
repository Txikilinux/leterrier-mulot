/** Classe Editeur
  * @file editeur.h
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012-2014 Icham Sirat <icham.sirat@ryxeo.com>
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
#include <QRegExp>
#include <QGraphicsProxyWidget>

#include "abuledufilev1.h"
#include "masquedeplacesouris.h"
#include "abuleduboxfilemanagerv1.h"
#include "ui_editeur.h"
#include "abuledumediathequepushv1.h"
#include "abuledutools.h"
#include "abuleduwidgetassistantetapesv1.h"

namespace Ui {
class Editeur;
}

class Editeur : public QWidget
{
    Q_OBJECT

    /* Enum pour les pages afin de ne pas se tromper de numero dans le code
     * Attention, ne pas toucher la numérotation des pageEtape*
     */
    enum Pages {
        PageEtapeAccueil         = 0x0,
        PageEtapeGestionImages   = 0x1,
        PageEtapeParametres      = 0x2,
        PageEtapeFin             = 0x3,
        PageParcours             = 0x4,
        /* @todo page MediathequePush */
        PageVisio           = 0x6
    };

public:
    /** Constructeur de la classe Editeur */
    explicit Editeur(QWidget *parent);

    /** Destructeur de la classe Editeur */
    ~Editeur();

//    void abeEditeurSetMainWindow(QWidget* mw);
    void setAbeFile(const QSharedPointer<AbulEduFileV1>& abuleduFile){ m_abuleduFile = abuleduFile ;}
    const QSharedPointer<AbulEduFileV1> abeFile(){ return m_abuleduFile ; }

    QStackedWidget* stackedWidget(){ return ui->stackedWidgetEditeur ;}
    QPushButton* btnModificationCourant(){ return ui->btnModificationCourant ;}

    /** Méthode de bind pour avoir la possibilité de charger un module existant dans l'editeur. */
    void modificationAbe();

private slots:
    /** Permet de supprimer une image du listWidgetImagesSelection
      * connecté à un menu contextuel appelé au clic droit sur le listWidgetImagesSelection */
    void slotSupprimerImage();

    /** Permet de renommer une image du listWidgetImagesSelection
      * connecté à un menu contextuel appelé au clic droit sur le listWidgetImagesSelection
      * Le nom défini sera celui affiché */
    void slotRenommerImage();

    /** Ajoute une image dans le listWidgetImagesSelection, avec icône et nom (sans extension, les images sont toutes enregistrées en .jpg) */
    void ajouterImage(QFileInfo monFichier);

    /** Crée un module à partir du contenu du dossier temporaire de l'application */
    void createAbe();

    /** Gère l'appui sur les boutons de parcours */
    void slotBtnParcours_clicked(const int&);

    /** Gère la pose des masques dans l'éditeur de parcours */
    void masquePoseParcours(MasqueDeplaceSouris*);

    /** Fait apparaitre sur le clic droit le menu contextuel créé dans la méthode creationMenu(); */
    void on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos);

    /** Ouvre au double clic l'image dans la visionneuse */
    void on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item);

    /** Appelle la fonction ajouterImage pour une image provenant de la médiathèque */
    void slotImportImageMediatheque(QSharedPointer<AbulEduFileV1> fichierABB, const int &success);

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
    void slotEditorChangePageRequested(int);
    void slotCloseEditor();

    bool slotCheckBoxParametres_clicked(int);

    /** Sert au controle de l'intégrité des parcours */
    bool slotParcoursSave();

    void slotSetNombreMasquesParcours(int);

    /** Réinitialise l'editeur de parcours */
    void on_btnResetParcours_clicked();

    /** Sauvegarde le parcours conçu avec l'éditeur de parcours */
    void on_btnSaveParcours_clicked();

    void on_sbParcoursMasque_valueChanged(int arg1);

    void slotListWidgetImagesSelectionEditEnd(QWidget*, QAbstractItemDelegate::EndEditHint);

    void slotQuitPublicationWidget();

private:
    Ui::Editeur *ui;
    QMenu *m_menuListWidget;

    bool m_modeModificationAbe;

    QString m_lastOpenDir;

    int m_OPT_nbMasquesChoisisParcours;
    int m_opt_nbMasquesLargeur;
    int m_opt_nbMasquesHauteur;
    int m_numeroParcours;

    QMap<QString, int> m_positionMasquesParcours;

    QList<MasqueDeplaceSouris *> m_listeMasquesFixes;
    MasqueDeplaceSouris         *m_masqueDepart;
    MasqueDeplaceSouris         *m_masqueArrivee;
    MasqueDeplaceSouris         *m_masqueParcours;

    QDir *m_dir;
    QDir *m_dirAbe;

    QList<QString> m_listeFichiersImages;

    MasqueDeplaceSouris             *m_masque;
    QList<MasqueDeplaceSouris *>    m_listeMasques;
    QList<MasqueDeplaceSouris *>    m_listeMasquesParcours;

    QSharedPointer<AbulEduFileV1> m_abuleduFile;

    QString m_messageAidePageAccueil;
    QString m_messageAidePageGestionImages;
    QString m_messageAidePageParametres;
    QString m_messageAidePageParcours;
    QString m_messageAidePageFin;
    QString m_messageAidePageVisio;

    AbulEduWidgetAssistantEtapesV1 *m_assistantEtapes;

    int m_nombreParcoursSave;

    QSettings *m_parametres;

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

    /** Permet de mapper les signaux des checkBox de parametres. (pour factorisation)
      * @see QSignalMapper.
      */
    void mapSignalCheckBoxParametres();

    void initMessagesAide();

    void controlNumberOfImages();
signals:
    /** Sert à réinitialiser le titre dans la barre de la Mainwindow si jamais on créé un nouveau module, en ayant déjà un d'ouvert (et son nom dans la barre des titres) */
    void editorNewAbe(const int& newAbe);
    void editorExited();
    void editorTest();
    void editorChooseOrSave(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave openOrSave);

    /** Ce signal sert à la gestion de l'intégrité des parcours */
    void signalParcoursSave();
};

#endif // EDITEUR_H
