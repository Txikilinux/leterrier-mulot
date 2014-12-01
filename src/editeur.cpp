/** Classe Editeur
  * @file editeur.cpp
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

#include "editeur.h"
#include "mainwindow.h"

/** @todo
    à mediter :Enregistrement du 0 si < 10 (pour le QSetting) ;
    localDebug = false ;
    tracer les objets sans parents ;
    Niveau de diff à la place de licence ;
*/

Editeur::Editeur(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_lastOpenDir = QDir::homePath();

    /* Gestion AbulEduMediathequeGet */
    ui->abuleduMediathequeGet->abeSetSource("data");
    ui->abuleduMediathequeGet->abeSetCustomBouton1(trUtf8("Importer l'image"));
    ui->abuleduMediathequeGet->abeHideBoutonTelecharger();
    ui->abuleduMediathequeGet->abeCustomBouton1SetDownload(true);
    ui->abuleduMediathequeGet->abeHideInfoPanel(false);
    ui->abuleduMediathequeGet->abeHideCloseBouton(true);
    ui->abuleduMediathequeGet->abeSetDefaultView(AbulEduMediathequeGetV1::abeMediathequeThumbnails);


    connect(ui->abuleduMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(QSharedPointer<AbulEduFileV1>, int)), this,
            SLOT(slotImportImageMediatheque(QSharedPointer<AbulEduFileV1>,int)), Qt::UniqueConnection);

    connect(ui->stPageMediathequePush, SIGNAL(signalMediathequePushFileUploaded(int)),this,
            SLOT(slotAfficheEtatPublication(int)), Qt::UniqueConnection);

    QShortcut *shortcutSupprimeChoix = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidgetImagesSelection, 0, 0, Qt::WidgetShortcut);
    connect(shortcutSupprimeChoix, SIGNAL(activated()), this, SLOT(slotSupprimerImage()), Qt::UniqueConnection);

    m_OPT_nbMasquesChoisisParcours = m_opt_nbMasquesLargeur = m_opt_nbMasquesHauteur = m_numeroParcours = m_nombreParcoursSave = 0;

    /* Vidange des différentes listes */
    m_listeMasques.clear();
    m_listeMasquesParcours.clear();
    m_listeFichiersImages.clear();

    setAcceptDrops(true);

    creationMenu();

    /* Affichage de la mediatheque par defaut */
    ui->tabWidgetImages->setCurrentWidget(ui->pageMediatheque);

    ui->gvPageVisio->setScene(new QGraphicsScene(this));

    /* Mappage des signaux des 5 boutons de parcours */
    mapSignalBtnParcours();

    /* Mappage des signaux des CheckBox parametres pour controler la navigation dans l'editeur */
    mapSignalCheckBoxParametres();

    /* Gestion des messages d'aide [à effectuer avant le setCurrentIndex(...)] */
    initMessagesAide();
    ui->btnModificationCourant->setEnabled(false);

    /* Gestion Assistant Etape */
    QList<QPair<QString, QString> > listeEtapes;
    listeEtapes.append(qMakePair(trUtf8("Accueil"),                 trUtf8("Accueil","For short")));
    listeEtapes.append(qMakePair(trUtf8("Gérer les images"),        trUtf8("Images","For short")));
    listeEtapes.append(qMakePair(trUtf8("Régler les paramètres"),   trUtf8("Paramètres","For short")));
    listeEtapes.append(qMakePair(trUtf8("Sauvegarder"),             trUtf8("Sauvegarder","For short")));

    m_assistantEtapes = new AbulEduWidgetAssistantEtapesV1(listeEtapes, 12, 16, 20, this);
    m_assistantEtapes->abeWidgetAssistantEnableClick(false);
    ui->hlFilAriane->addWidget(m_assistantEtapes);

    connect(m_assistantEtapes, SIGNAL(signalEtapeHasChanged(int)), this, SLOT(slotEditorChangePageRequested(int)), Qt::UniqueConnection);
    connect(m_assistantEtapes, SIGNAL(signalQuitterRequested()), this, SLOT(slotCloseEditor()), Qt::UniqueConnection);

    /* Ceci sert à mettre la page d'accueil par dféfaut au démarrage, donc initialisation du 1er message d'aide */
    slotEditorChangePageRequested(PageEtapeAccueil);

    /* Je connecte les méthodes qui vont gérées l'intégrité des parcours */
    connect(this, SIGNAL(signalParcoursSave()), SLOT(slotParcoursSave()), Qt::UniqueConnection);

    ui->gv_editeurParcours->setScene(new QGraphicsScene(this));

    connect(ui->listWidgetImagesSelection->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
            this, SLOT(slotListWidgetImagesSelectionEditEnd(QWidget*, QAbstractItemDelegate::EndEditHint)));
}

Editeur::~Editeur()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << m_abuleduFile->abeFileGetFileName().baseName();
    delete ui;
    emit editorExited();
}

/*****************************************************************************************************************
                                    GESTION NAVIGATION
  ****************************************************************************************************************/
void Editeur::slotEditorChangePageRequested(int page)
{
    ABULEDU_LOG_DEBUG() <<__PRETTY_FUNCTION__ << page ;

    switch(page)
    {
    case PageEtapeAccueil:
        ABULEDU_LOG_DEBUG() << "PAGE Accueil";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeAccueil);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(false);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeParametres)->setEnabled(false);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(false);
        ui->lbAide->setText(m_messageAidePageAccueil);
        break;
    case PageEtapeGestionImages:
        ABULEDU_LOG_DEBUG() << "PAGE Images";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
        m_assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(true);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeAccueil)->setEnabled(false);
        ui->lbAide->setText(m_messageAidePageGestionImages);
        /* Contrôle 5 images = bouton suivant ok */
        controlNumberOfImages();
        break;
    case PageEtapeParametres:
        ABULEDU_LOG_DEBUG() << "PAGE Parametres";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeParametres);
        m_assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        /* Obligé d'avoir un int pour le connect mais je ne m'en sers pas */
        slotCheckBoxParametres_clicked(0);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(true);
        ui->lbAide->setText(m_messageAidePageParametres);
        break;
    case PageEtapeFin:
        ABULEDU_LOG_DEBUG() << "PAGE Fin";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeFin);
        m_assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        m_assistantEtapes->abeWidgetAssistantEnableClick(true);
        ui->lbAide->setText(m_messageAidePageFin);
        break;
    case PageParcours:
        ABULEDU_LOG_DEBUG() << "PAGE Parcours";
        ui->lbAide->setText(m_messageAidePageParcours);
        ui->stackedWidgetEditeur->setCurrentIndex(PageParcours);
        ui->gv_editeurParcours->setSceneRect(ui->gv_editeurParcours->rect());
        ui->gv_editeurParcours->scene()->setSceneRect(ui->gv_editeurParcours->rect());
        m_assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        /* Obligé d'avoir un int pour le connect mais je ne m'en sers pas */
        slotCheckBoxParametres_clicked(0);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(false);
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(false);
        m_assistantEtapes->abeWidgetAssistantEnableClick(false);
        m_assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        break;
    case PageVisio:
        ABULEDU_LOG_DEBUG() << "PAGE Visio";
        ui->stackedWidgetEditeur->setCurrentIndex(PageVisio);
        ui->lbAide->setText(m_messageAidePageVisio);
        m_assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        m_assistantEtapes->abeWidgetAssistantEnableClick(false);
        break;
    default:
        break;
    }
}

void Editeur::slotCloseEditor()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* Remettre le titre par defaut du boutonModifier courant */
    ui->btnModificationCourant->setText(trUtf8("Editer le module en cours"));
    /* Remettre la page d'Accueil par defaut */
    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeAccueil);
    m_assistantEtapes->abeWidgetAssistantSetEtapeCourante(PageEtapeAccueil);

    /** @todo Quitter oui/non ? */
    /* Pour l'instant, en quittant on supprime tout */
    ABULEDU_LOG_DEBUG() << "NETTOYAGE TEMPORAIRE DE : " << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    m_abuleduFile->abeCleanDirectoryRecursively(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());

    /* On dit que le fichier abe n'a pas de nom maintenant (important pr la MW) */
    m_abuleduFile->abeFileSetFilename("");
    m_abuleduFile->abeFileSetTitle("");

    m_listeMasquesFixes.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    /* Nettoyage scene */
    ui->gv_editeurParcours->scene()->clear();
    ui->sbParcoursMasque->setEnabled(true);
    ui->gv_editeurParcours->scene()->clear();

    emit editorExited();
}

/*********************************************************************************************************

  ********************************************************************************************************/
void Editeur::initMessagesAide()
{
    m_messageAidePageAccueil = trUtf8("Je suis votre guide, je vous donnerai les consignes à chaque écran rencontré.\n")
            + trUtf8("Choississez ci-dessous un mode d'édition.\n")
            + trUtf8("Les étapes sont bloquantes tant que les paramètres ne sont pas convenables.");

    m_messageAidePageGestionImages = trUtf8("Veuillez sélectionner 5 images. Il est possible de selectionner ces images sur votre ordinateur (onglet \"Disque Local\")\n")
            + trUtf8("ou sur la médiathèque d'AbulEdu (onglet \"AbulEdu Data\").\n")
            + trUtf8("Pour passer à l'ecran suivant, la liste \"Images sélectionnées\" doit comporter 5 images.");

    m_messageAidePageParametres = trUtf8("Cochez les exercices désirés et réglez les différentes valeurs.\n")
            + trUtf8("Pour passer à l'ecran suivant, au moins un exercice doit être activé.\n")
            + trUtf8("Si l'exercice parcours est activé, tous les boutons \"Parcours\" doivent être verts.");

    m_messageAidePageParcours = trUtf8("Sélectionnez le nombre de masques pour votre parcours. Le bouton \"Sauvegarder\" sera actif lorsque\n")
            + trUtf8("tous les masques seront posés en cliquant sur une case valide (n'importe où pour le départ, grisé pour le reste).\n")
            + trUtf8("Il est également possible de recommencer votre parcours en cliquant sur \"Réinitialiser\" à tout moment.");

    m_messageAidePageFin = trUtf8("Veuillez remplir au minimum le titre de votre module, ainsi que votre nom.\n")
            + trUtf8("Il vous est possible de tester votre module en cliquant sur \"Essayer\".\n")
            + trUtf8("Si les paramètres vous semblent correctes, sauvegardez votre module en cliquant sur \"Enregistrer\".");

    m_messageAidePageVisio = trUtf8("Pour revenir à l'écran précédent, cliquez sur l'image.");
}

void Editeur::controlNumberOfImages()
{
    if(m_listeFichiersImages.count() >= 5)
    {
        /* Il faut activer seulement le bouton suivant */
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeParametres)->setEnabled(true);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(true);
        m_assistantEtapes->abeWidgetAssistantEnableClick(true);
        /* Afin d'afficher le bouton comme il faut dans la riviere */
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->click();
    }
    else
    {
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeParametres)->setEnabled(false);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(false);
    }
}

void Editeur::creationMenu()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* MENU LISTWIDGET (Supprimer) */
    m_menuListWidget = new QMenu(ui->listWidgetImagesSelection);

    /* menu contextuel pour la suppression */
    QIcon iconSupprimer = QApplication::style()->standardIcon(QStyle::SP_DialogResetButton); /* On récupère l'icône désiré */
    QAction *a_supprimer = new QAction(trUtf8("&Supprimer de la selection"),m_menuListWidget);
    a_supprimer->setIcon(iconSupprimer);
    a_supprimer->setIconVisibleInMenu(true);
    a_supprimer->connect(a_supprimer, SIGNAL(triggered()), this, SLOT(slotSupprimerImage()), Qt::UniqueConnection);
    m_menuListWidget->addAction(a_supprimer);

    /* menu contextuel pour le renommage des images */
    QIcon iconRenommage = QApplication::style()->standardIcon(QStyle::SP_FileIcon); /* On récupère l'icône désiré */
    QAction *a_renommer = new QAction(trUtf8("&Renommer l'image"),m_menuListWidget);
    a_renommer->setIcon(iconRenommage);
    a_renommer->setIconVisibleInMenu(true);
    a_renommer->connect(a_renommer, SIGNAL(triggered()), this, SLOT(slotRenommerImage()), Qt::UniqueConnection);
    m_menuListWidget->addAction(a_renommer);
}

void Editeur::slotSupprimerImage()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    /* condition garde meme si j'appelle ce slot que si j'ai un item ds ma listView, donc une liste avec au moins 1 éléments =) */
    if (m_listeFichiersImages.isEmpty()){return;}
    if (ui->listWidgetImagesSelection->selectedItems().isEmpty()){ return;}

    /* Suppression des items selectionnés */
    foreach(QListWidgetItem *i, ui->listWidgetImagesSelection->selectedItems())
    {
        m_listeFichiersImages.removeOne(i->data(4).toString());
        QFileInfo fi(i->data(4).toString());
        if(QFile::remove(fi.absoluteFilePath().replace(fi.suffix(), "xml").remove("/images"))){
            ABULEDU_LOG_DEBUG() << "Suppression du fichier XML accompagnant la ressource... [OK]";
        }
        if (QFile::remove(fi.absoluteFilePath()))
        {
           ABULEDU_LOG_DEBUG() << "Suppression de l'image du fichier temporaire... [OK]";
        }
        delete i;
    }

    /* Contrôle 5 images = bouton suivant ok */
    controlNumberOfImages();
}

void Editeur::slotRenommerImage()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    /* condition garde meme si j'appelle ce slot que si j'ai un item ds ma listView, donc une liste avec au moins 1 éléments =) */
    if (m_listeFichiersImages.isEmpty()){return;}
    if (ui->listWidgetImagesSelection->selectedItems().isEmpty()){ return;}

    QListWidgetItem *item = ui->listWidgetImagesSelection->currentItem();
    item->setFlags( item->flags() | Qt::ItemIsEditable );

    ui->listWidgetImagesSelection->editItem( item );

    /* Contrôle 5 images = bouton suivant ok */
    controlNumberOfImages();
}

void Editeur::slotListWidgetImagesSelectionEditEnd(QWidget *w, QAbstractItemDelegate::EndEditHint)
{
    QListWidgetItem *item = ui->listWidgetImagesSelection->currentItem();
    const QString newFileName = static_cast<QLineEdit*>(w)->text();
    ABULEDU_LOG_DEBUG() << "FIN EDITION ITEM [" << item->data(4).toString()<<"] -> [" << newFileName << "]";

    QFileInfo fi(item->data(4).toString());
    QFile xml(fi.absoluteFilePath().replace(fi.suffix(), "xml").remove("/images"));

    /* Petite protect° -> #3735 si fichier déjà portant ce nom, on sort avec une petite pop-up */
    if(QFile::exists(fi.absoluteFilePath().replace(fi.baseName(), newFileName))){

        /* On remet l'ancien nom sur l'item */
        item->setText(fi.baseName());

        /* Petit message */
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Modification du nom de l'image"),
                                                              trUtf8("Une image se nomme déjà %1.\n Veuillez choisir un autre nom pour cette image").arg(newFileName), true, this);
        alertBox->setWindowModality(Qt::ApplicationModal);
        alertBox->setWink(false);
        alertBox->show();
        return;
    }

    if(xml.exists()){
        qDebug() <<"Le fichier XML existe" << xml.fileName();
        QFileInfo fi_xml(xml);
        if(QFile::rename(fi_xml.absoluteFilePath(),fi_xml.absoluteFilePath().replace(fi_xml.baseName(), newFileName))){
            ABULEDU_LOG_DEBUG() << "Renommage du fichier XML accompagnant la ressource... [OK]";
        }
        else
            ABULEDU_LOG_DEBUG() << "Renommage du fichier XML accompagnant la ressource... [KO]";
    }

    if(QFile::rename(fi.absoluteFilePath(), fi.absoluteFilePath().replace(fi.baseName(), newFileName))){
        ABULEDU_LOG_DEBUG() << "Renommage fichier... [OK]";
        /* Pour changement ou exercice */
        item->setData(4, fi.absoluteFilePath().replace(fi.baseName(), newFileName));
    }
    else
        ABULEDU_LOG_DEBUG() << "Renommage fichier... [KO]";

}

void Editeur::ajouterImage(QFileInfo monFichier)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << " Chemin :" << monFichier.absoluteFilePath() << "Nom : " << monFichier.fileName();

    /* Controle des insertions (éviter les doublons) */
    if (m_listeFichiersImages.contains(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg"))
    {
        ABULEDU_LOG_DEBUG() << "Fichier deja présent";
        return;
    }

    if(m_abuleduFile->resizeImage(&monFichier, 1024, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/"))
    {
        /* je range le chemin de l'image dans ma liste (celui du fichier temp) */
        m_listeFichiersImages << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg";
        /* Insertion dans mon listWidget */
        QListWidgetItem *item = new QListWidgetItem(QIcon(monFichier.absoluteFilePath()), monFichier.baseName());
        item->setData(4, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() + ".jpg");
        ui->listWidgetImagesSelection->insertItem(0, item);
        item->setFlags( item->flags() | Qt::ItemIsEditable ); /* pour modifier le titre */
    }

    /* Contrôle 5 images = bouton suivant ok */
    controlNumberOfImages();
}

void Editeur::slotImportImageMediatheque(QSharedPointer<AbulEduFileV1> fichierABB, const int& success)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    if(success > -1)
    {
        ajouterImage(fichierABB->abeFileGetContent(0));
        /* Ajout des metadata */
        m_abuleduFile->abeFileAddMetaDataFromABB(fichierABB->abeFileGetLOM(), fichierABB->abeFileGetContent(0).baseName());
        ABULEDU_LOG_DEBUG() << "Importation image depuis mediatheque... [OK]";
    }
    else
        ABULEDU_LOG_DEBUG() << "Importation image depuis mediatheque... [KO]";
}

void Editeur::on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    /* Si j'ai un item à cet endroit, j'appelle mon menu */
    if (ui->listWidgetImagesSelection->itemAt(pos) != NULL)
    {
        m_menuListWidget->exec(ui->listWidgetImagesSelection->mapToGlobal(pos));
    }
}

void Editeur::on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    AbulEduVisionneuseImageV1 *visio = new AbulEduVisionneuseImageV1(ui->tabWidgetImages);
    connect(visio, SIGNAL(destroyed()),this,SLOT(slotSortieVisionneuse()), Qt::UniqueConnection);
    visio->setAttribute(Qt::WA_DeleteOnClose);
    visio->fixeTempsAffichageMax(0);
    QGraphicsProxyWidget* prox = ui->gvPageVisio->scene()->addWidget(visio);
    prox->grabMouse();
    visio->ouvrirFicher(item->data(4).toString());
    visio->ajusterFenetreAuMieux();

    slotEditorChangePageRequested(PageVisio);
}

void Editeur::createAbe()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    if (preparerSauvegarde())
    {
        emit editorChooseOrSave(AbulEduBoxFileManagerV1::abeSave);
    }
}

/***********************************************************************************************************************************************************
                                                               PARCOURS
**********************************************************************************************************************************************************/
void Editeur::slotBtnParcours_clicked(const int &numBtn)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ <<" "<< numBtn;

    m_numeroParcours = numBtn;
    slotEditorChangePageRequested(PageParcours);

    if(remplirGvParcours(m_numeroParcours)){
        ABULEDU_LOG_DEBUG() << "Remplissage... [OK], Affichage editeur de parcours";
    }
    else {
        ABULEDU_LOG_DEBUG()<< "Remplissage... [KO], Réinitialisation parcours";
    }

    /* Petite protection, le nb de masque choisi ne peut pas etre superieur au nb total de masque */
    ui->sbParcoursMasque->setMaximum(ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value());
}

bool Editeur::remplirGvParcours(const int &numeroParcours)
{
    ABULEDU_LOG_DEBUG() <<__PRETTY_FUNCTION__ << " "<< numeroParcours;

    m_OPT_nbMasquesChoisisParcours = ui->sbParcoursMasque->value();

    m_opt_nbMasquesLargeur = ui->spinBoxParcoursMasquesLargeur->value();
    m_opt_nbMasquesHauteur = ui->spinBoxParcoursMasqueHauteur->value();

    /** @todo SCROLLBAR */
    float largeurGv = ui->gv_editeurParcours->width()  /*- ui->gv_editeurParcours->verticalScrollBar()->width()*/;
    float hauteurGv = ui->gv_editeurParcours->height() /*- ui->gv_editeurParcours->horizontalScrollBar()->height()*/;

    const qreal largeurMasque = largeurGv / m_opt_nbMasquesLargeur;
    const qreal hauteurMasque = hauteurGv / m_opt_nbMasquesHauteur;

    qreal xMasque, yMasque = 0.00;

    int numeroMasque = 0;
    /* Pose de masque normaux pour remplissage */
    for(float i=0; i < m_opt_nbMasquesHauteur; i++)
    {
        for (int j=0; j < m_opt_nbMasquesLargeur; j++)
        {
            m_masque = new MasqueDeplaceSouris(0, numeroMasque);
            m_masque->setSize(largeurMasque, hauteurMasque);
            m_masque->setPos(xMasque, yMasque);

            m_masque->setColor(QColor::fromRgb(255,255,255));
            m_masque->setProperty("Role", trUtf8("Fixe"));

            m_masque->setHideOnMouseOver(false);
            m_masque->setIsEditable(true);

            connect(m_masque, SIGNAL(signalPoseSurParcours(MasqueDeplaceSouris*)),
                    this, SLOT(masquePoseParcours(MasqueDeplaceSouris*)), Qt::UniqueConnection);

            xMasque+=largeurMasque;
            ui->gv_editeurParcours->scene()->addItem(m_masque);
            m_listeMasques << m_masque;
            numeroMasque++;
        }
        xMasque = 0;
        yMasque += hauteurMasque;
    }

    /* Gestion de la taille de la Scene */
    /* @todo soustraire la taille des scrollBar */
    //        m_editeurParcoursWidget->setGeometry((m_editeurParcoursWidget->x()), (m_editeurParcoursWidget->y()),
    //                                             (largeurMasque * m_opt_nbMasquesLargeur) + m_editeurParcoursWidget->getGraphicsView()->verticalScrollBar()->width(),
    //                                             (hauteurMasque * m_opt_nbMasquesHauteur) + m_editeurParcoursWidget->getGraphicsView()->horizontalScrollBar()->height() + (m_editeurParcoursWidget->getBoutonHeight()) *2);

    if(chargerPositionMasque(numeroParcours)){
        m_numeroParcours = numeroParcours;

        ui->sbParcoursMasque->setValue(m_positionMasquesParcours.count());

        /* Parcours de la QMap positionMasqueParcours */
        int positionDepart = 0, positionArrivee = 0;
        QList<int> positionParcours;

        QMap<QString, int>::const_iterator i = m_positionMasquesParcours.constBegin();
        if(!m_positionMasquesParcours.isEmpty()) /* Condition de garde si on reinitialise un parcours en mode modification */
        {
            while (i != m_positionMasquesParcours.constEnd())
            {
                ABULEDU_LOG_DEBUG() << i.key() << " " << i.value();
                if (i.key() == "MasqueDepart")
                    positionDepart = i.value();
                else if (i.key() == "MasqueArrivee")
                    positionArrivee = i.value();
                i ++;
            }

            QList<int> listePosition;
            QString masque;
            QRegExp masqueP("MasqueParcours");
            i = m_positionMasquesParcours.constBegin();
            while(i != m_positionMasquesParcours.constEnd())
            {
                masque = i.key();
                qDebug() << "Masque == "  << masque;
                if(masque.contains(masqueP)){
                    listePosition << masque.split(masqueP).last().toInt();
                }
                i++;
            }

            /* Tout les numéros des masques sont dans la liste, il faut effectuer un tri croissant */
            qSort(listePosition.begin(), listePosition.end(), qLess<int>());

            int pos;
            while(!listePosition.isEmpty()){
                pos = listePosition.takeFirst();
                foreach (const QString var, m_positionMasquesParcours.keys()) {
                    if(var.contains("MasqueParcours")){
                        if(pos == var.split("MasqueParcours").at(1).toInt()){
                            positionParcours << m_positionMasquesParcours.value(var);
                        }
                    }
                }
            }

            ABULEDU_LOG_DEBUG() << "La liste des positions normales : " << m_positionMasquesParcours;
            ABULEDU_LOG_DEBUG() << "Position Depart/Arrivee         : " << positionDepart << "/" << positionArrivee;
            ABULEDU_LOG_DEBUG() << "Position Parcours               : " << positionParcours;


            /*Ici on a toutes les positions necessaires, plus qu'à les mettre dans l'ordre : depart, parcours, arrivee */
            /* depart */
            m_listeMasques.at(positionDepart)->setColor(QColor(Qt::green));
            m_listeMasques.at(positionDepart)->setProperty("Role", trUtf8("Depart"));
            m_listeMasquesParcours << m_listeMasques.at(positionDepart);

            /* parcours */
            while(!positionParcours.isEmpty())
            {
                if(AbulEduTools::masquesVoisins(m_listeMasquesParcours.last()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur).contains(positionParcours.first()))
                {
                    ABULEDU_LOG_DEBUG() << "Voisin ok ";
                    m_listeMasques.at(positionParcours.first())->setColor(QColor(Qt::black));
                    m_listeMasques.at(positionParcours.first())->setProperty("Role", trUtf8("Parcours"));
                    m_listeMasquesParcours << m_listeMasques.at(positionParcours.first());
                    positionParcours.removeFirst();
                }
                else{
                    ABULEDU_LOG_DEBUG() << "PROBLEME VOISINAGE";

                    /* Utilisation d'une QEventLoop avec le message sinon le btnReset n'était plus corrélé au numéro du parcours.*/
                    AbulEduMessageBoxV1 *m_messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),
                                                                                trUtf8("Le chargement du parcours numéro %1 a detecté une anomalie dans les paramètres. \r" \
                                                                                       "Le parcours va être réinitialisé.").arg(numeroParcours), true, this);
                    connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()),ui->btnResetParcours, SLOT(click()), Qt::UniqueConnection);
                    QEventLoop *eL = new QEventLoop(this);
                    connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), eL, SLOT(quit()), Qt::UniqueConnection);

                    m_messageBox->show();
                    eL->exec();

                    return false;
                }
            }
            /* arrivee */
            m_listeMasques.at(positionArrivee)->setColor(QColor(Qt::red));
            m_listeMasques.at(positionArrivee)->setProperty("Role", trUtf8("Arrivee"));
            m_listeMasquesParcours << m_listeMasques.at(positionArrivee);

            /* Et j'active le menu Sauvegarder */
            ui->btnSaveParcours->setEnabled(true);
            ui->sbParcoursMasque->setEnabled(false);
        }
        return true;
    }
    else{
        ui->btnSaveParcours->setEnabled(false);
        ui->sbParcoursMasque->setEnabled(true);
        return false;
    }
}

void Editeur::masquePoseParcours(MasqueDeplaceSouris* masque)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    ui->sbParcoursMasque->setEnabled(false);

    QList<int> listeVoisins;
    /* Si mon masque n'a pas de role (= Fixe) = création de masque */
    if (masque->property("Role") == "Fixe")
    {
        /* Liste Vide = Depart */
        if (m_listeMasquesParcours.isEmpty())
        {
            masque->setColor(QColor(Qt::green));
            masque->setProperty("Role", trUtf8("Depart"));
            masque->update();
            m_listeMasquesParcours << masque;

            /* ...& je coloris en gris les voisins */
            listeVoisins = AbulEduTools::masquesVoisins(masque->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
            foreach(MasqueDeplaceSouris* var_masque, m_listeMasques)
            {
                for (int i =0 ; i < listeVoisins.count(); i++)
                {
                    if(var_masque->getNumero() == listeVoisins.at(i))
                    {
                        var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                        var_masque->update();
                    }
                }
            }
        }
        /* Liste avec un masque = Parcours */
        else if(m_listeMasquesParcours.count() < (m_OPT_nbMasquesChoisisParcours - 1))
        {
            listeVoisins.clear();
            /* Je vais chercher les voisins du dernier masque posé */
            listeVoisins = AbulEduTools::masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);

            if (listeVoisins.contains(masque->getNumero())) /* Si le numero de ce masque est contenu ds mas liste de voisins = Ok */
            {
                /* S'il y a des masques de couleur grise... les remettre en blanc */
                foreach(MasqueDeplaceSouris* var_masque,m_listeMasques)
                {
                    if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
                    {
                        var_masque->setColor(QColor(Qt::white));
                        var_masque->update();
                    }
                }

                masque->setColor(QColor(Qt::black));
                masque->setProperty("Role", trUtf8("Parcours"));
                masque->update();
                m_listeMasquesParcours << masque;

                /* ...& je coloris en gris les voisins */
                listeVoisins = AbulEduTools::masquesVoisins(masque->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
                foreach(MasqueDeplaceSouris* var_masque, m_listeMasques)
                {
                    for (int i =0 ; i < listeVoisins.count(); i++)
                    {
                        if((var_masque->getNumero() == listeVoisins.at(i)) && (var_masque->property("Role") == "Fixe"))
                        {
                            var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                            var_masque->update();
                        }
                    }
                }
            }
        }
        else if ( m_listeMasquesParcours.count() == (m_OPT_nbMasquesChoisisParcours-1))
        {
            listeVoisins = AbulEduTools::masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);

            if (listeVoisins.contains(masque->getNumero())) // Si le numero de ce masque est contenu ds mas liste de voisins = Ok
            {
                masque->setColor(QColor(Qt::red));
                masque->setProperty("Role", trUtf8("Arrivee"));
                masque->update();
                m_listeMasquesParcours << masque;
                /* S'il y a des masques de couleur grise... les remettre en blanc */
                foreach(MasqueDeplaceSouris* var_masque,m_listeMasques)
                {
                    if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
                    {
                        var_masque->setColor(QColor(Qt::white));
                        var_masque->update();
                    }
                }
                // Et j'active le menu Sauvegarder
                ui->btnSaveParcours->setEnabled(true);
            }
        }
    } // Fin si mon masque n'a pas de role = creation de masque

    /* Si mon masque à un role (different de Fixe) = destruction du masque */
    // Je n'enleve que le dernier masque
    else if (masque->getNumero() == m_listeMasquesParcours.back()->getNumero())
    {
        /* Remise à défaut */
        masque->setProperty("Role", trUtf8("Fixe"));
        masque->setColor(QColor(Qt::white));
        masque->update();

        /* je remets les masques blanc */
        foreach(MasqueDeplaceSouris* var_masque,m_listeMasques)
        {
            if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
            {
                var_masque->setColor(QColor(Qt::white));
                var_masque->update();
            }
        }
        /* Et je desactive le menu Sauvegarder */
        ui->btnSaveParcours->setEnabled(false);
        /* et j'enleve ce masque de ma liste parcours */
        m_listeMasquesParcours.removeLast();
        /* Remise des masques gris pour les voisins du dernier masque (je viens d'enlever CE masque juste avant)
             * ...& je coloris en gris les voisins sauf si j'enleve depart donc pas de colriage en gris
             */
        if (!m_listeMasquesParcours.isEmpty())
        {
            listeVoisins = AbulEduTools::masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
            foreach(MasqueDeplaceSouris* var_masque, m_listeMasques)
            {
                for (int i =0 ; i < listeVoisins.count(); i++)
                {
                    if((var_masque->getNumero() == listeVoisins.at(i)) && (var_masque->property("Role") == "Fixe"))
                    {
                        var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                        var_masque->update();
                    }
                }
            }
        }

    }
    ABULEDU_LOG_DEBUG() << m_listeMasquesParcours.count();
}

void Editeur::mapSignalCheckBoxParametres()
{
    QSignalMapper *signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), SLOT(slotCheckBoxParametres_clicked(int)), Qt::UniqueConnection);

    signalMapper->setMapping(ui->groupBoxSurvol,        1);
    signalMapper->setMapping(ui->groupBoxClic,          2);
    signalMapper->setMapping(ui->groupBoxDoubleClic,    3);
    signalMapper->setMapping(ui->groupBoxParcours,      4);

    connect(ui->groupBoxSurvol, SIGNAL(clicked(bool)), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->groupBoxClic, SIGNAL(clicked(bool)), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->groupBoxDoubleClic, SIGNAL(clicked(bool)), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->groupBoxParcours, SIGNAL(clicked(bool)), signalMapper, SLOT(map()), Qt::UniqueConnection);
}

bool Editeur::slotCheckBoxParametres_clicked(int)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ ;

    /* Si n'importe lequel des exercices est checké sauf parcours == on dit oui */
    if((ui->groupBoxSurvol->isChecked() || ui->groupBoxClic->isChecked() || ui->groupBoxDoubleClic->isChecked())
            && !ui->groupBoxParcours->isChecked())
    {
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(true);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(true);
        m_assistantEtapes->abeWidgetAssistantEnableClick(true);
        return true;
    }
    else if(ui->groupBoxParcours->isChecked()){
        if(chargerPositionMasque(1)
                && chargerPositionMasque(2)
                && chargerPositionMasque(3)
                && chargerPositionMasque(4)
                && chargerPositionMasque(5))
        {
            ABULEDU_LOG_DEBUG() << "Réglages parcours... [OK]";
            m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(true);
            m_assistantEtapes->abeWidgetAssistantEnableSuivant(true);
            m_assistantEtapes->abeWidgetAssistantEnableClick(true);
            return true;
        }
        else{
            ABULEDU_LOG_DEBUG() << "Réglages parcours... [KO]";
            return false;
        }
    }
    m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(false);
    m_assistantEtapes->abeWidgetAssistantEnableSuivant(false);
    m_assistantEtapes->abeWidgetAssistantEnableClick(false);
    return false;
}

bool Editeur::slotParcoursSave()
{
    /*  Il faut que _nombreParcoursSave soit égale à 5 pour que les données soient viables.
      * Permet également d'activé l'écran de sauvegarde. */
    ABULEDU_LOG_DEBUG()<< __PRETTY_FUNCTION__;
    m_nombreParcoursSave ++;
    if(m_nombreParcoursSave == 5)
    {
        ABULEDU_LOG_DEBUG() << "On peut activer la page de Fin";
        m_assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(true);
        m_assistantEtapes->abeWidgetAssistantEnableSuivant(true);
        m_assistantEtapes->abeWidgetAssistantEnableClick(true);
        return true;
    }
    return false;
}

void Editeur::slotSetNombreMasquesParcours(int value)
{
    m_OPT_nbMasquesChoisisParcours = value;
}

void Editeur::mapSignalBtnParcours()
{
    QSignalMapper *signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(slotBtnParcours_clicked(int)), Qt::UniqueConnection);

    signalMapper->setMapping(ui->btnParcours1, 1);
    signalMapper->setMapping(ui->btnParcours2, 2);
    signalMapper->setMapping(ui->btnParcours3, 3);
    signalMapper->setMapping(ui->btnParcours4, 4);
    signalMapper->setMapping(ui->btnParcours5, 5);

    connect(ui->btnParcours1, SIGNAL(clicked()), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btnParcours2, SIGNAL(clicked()), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btnParcours3, SIGNAL(clicked()), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btnParcours4, SIGNAL(clicked()), signalMapper, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btnParcours5, SIGNAL(clicked()), signalMapper, SLOT(map()), Qt::UniqueConnection);
}

bool Editeur::chargerPositionMasque(const int &numeroParcours)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << " " << numeroParcours;

    m_positionMasquesParcours.clear();

    ABULEDU_LOG_DEBUG() << "Parametres charges depuis : " << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    ABULEDU_LOG_DEBUG() << "Existence Fichier Conf :: " << QFile::exists(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf");

    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.beginGroup("parcours");
    parametres.beginGroup("parcours"+QString::number(numeroParcours));
    for (int i =0 ; i < parametres.childKeys().count(); i++)
    {
        m_positionMasquesParcours.insert(parametres.childKeys().at(i),parametres.value(parametres.childKeys().at(i)).toInt());
    }
    parametres.endGroup();
    if(!m_positionMasquesParcours.isEmpty())
        return true;
    else
    {
        ABULEDU_LOG_DEBUG() << " Aucune entree dans le QSettings. ";
        return false;
    }
}

/* Gestion reinitialisation parcours */
void Editeur::on_btnResetParcours_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << m_numeroParcours;
    /* Remettre tout mes masques d'origine ! */
    foreach (MasqueDeplaceSouris* var, m_listeMasques) {
        var->setColor(QColor(Qt::white));
        var->setProperty("Role", trUtf8("Fixe"));
        var->update();
    }

    /* Vider listeMasquesParcours */
    m_listeMasquesParcours.clear();

    /* Supprimer les entrées correspondantes dans le QSettings */
    ABULEDU_LOG_DEBUG() << "Suppression Entrées dans le QSettings";
    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.beginGroup("parcours");
    parametres.beginGroup("parcours"+QString::number(m_numeroParcours));
    if(parametres.childKeys().count() > 0){ /* Si on a des entrées relatives à ce parcours, on les supprime */
        parametres.remove("");
        parametres.endGroup();
        parametres.sync();
        ABULEDU_LOG_DEBUG() << "Parcours num : " <<m_numeroParcours<< " -> efface du conf";
    }

    /* bouton parcours correspondant en rouge */
    switch(m_numeroParcours){
    case 1:
        ui->btnParcours1->setStyleSheet("color : red;");
        break;
    case 2:
        ui->btnParcours2->setStyleSheet("color : red;");
        break;
    case 3:
        ui->btnParcours3->setStyleSheet("color : red;");
        break;
    case 4:
        ui->btnParcours4->setStyleSheet("color : red;");
        break;
    case 5:
        ui->btnParcours5->setStyleSheet("color : red;");
        break;
    }

    ui->btnSaveParcours->setEnabled(false);
    ui->sbParcoursMasque->setEnabled(true);
}

/* Gestion sauvegarde parcours */
void Editeur::on_btnSaveParcours_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << " parcours :" << m_numeroParcours;

    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/parametres.conf", QSettings::IniFormat);
    switch (m_numeroParcours)
    {
    /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
    case 1:
        parametres.beginGroup("parcours");
        parametres.setValue("parcours1/MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        parametres.setValue("parcours1/MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        for (int i =0; i < m_listeMasquesParcours.count(); i++ ){
            parametres.setValue("parcours1/MasqueParcours"+QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours1->setStyleSheet("color : green;");
        parametres.endGroup();
        parametres.sync();
        break;
    case 2:
        parametres.beginGroup("parcours");
        parametres.setValue("parcours2/MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        parametres.setValue("parcours2/MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        for (int i =0; i < m_listeMasquesParcours.count(); i++ ){
            parametres.setValue("parcours2/MasqueParcours"+QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours2->setStyleSheet("color : green;");
        parametres.endGroup();
        parametres.sync();
        break;
    case 3:
        parametres.beginGroup("parcours");
        parametres.setValue("parcours3/MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        parametres.setValue("parcours3/MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        for (int i =0; i < m_listeMasquesParcours.count(); i++ ){
            parametres.setValue("parcours3/MasqueParcours"+QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours3->setStyleSheet("color : green;");
        parametres.endGroup();
        parametres.sync();
        break;
    case 4:
        parametres.beginGroup("parcours");
        parametres.setValue("parcours4/MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        parametres.setValue("parcours4/MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        for (int i =0; i < m_listeMasquesParcours.count(); i++ ){
            parametres.setValue("parcours4/MasqueParcours"+QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours4->setStyleSheet("color : green;");
        parametres.endGroup();
        parametres.sync();
        break;
    case 5:
        parametres.beginGroup("parcours");
        parametres.setValue("parcours5/MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        parametres.setValue("parcours5/MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        for (int i =0; i < m_listeMasquesParcours.count(); i++ ){
            parametres.setValue("parcours5/MasqueParcours"+QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours5->setStyleSheet("color : green;");
        parametres.endGroup();
        parametres.sync();
        break;
    }
    /***************************
        Commun apres sauvegarde
    ****************************/
    m_listeMasquesFixes.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    /* Nettoyage scene */
    ui->gv_editeurParcours->scene()->clear();
    ui->sbParcoursMasque->setEnabled(true);

    ui->gv_editeurParcours->scene()->clear();

    /* Petit message */
    AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"), true, this);
    alertBox->setWindowModality(Qt::ApplicationModal);
    alertBox->setWink();
    alertBox->show();
    /* Emission du signal pour gestion d'intégrité (et de navigation)*/
    emit signalParcoursSave();

    /* Revenir sur l'écran des parametres */
    slotEditorChangePageRequested(PageEtapeParametres);
}

void Editeur::on_sbParcoursMasque_valueChanged(int val)
{
    /** Gestion changement de valeur du nombre de masques */
    m_OPT_nbMasquesChoisisParcours = val;
}

/**********************************************************************************************************************************************************
                                                    GESTION MODE MODIFIER ABE
**********************************************************************************************************************************************************/

void Editeur::setModeModificationAbe(const bool &yesNo)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << yesNo;

    m_modeModificationAbe = yesNo;
}

/** Bouton création ABE
    Probleme :
    Il faut également dire au logiciel = Attention l'Abe est vide (ce qui permettrait en sortant de l'éditeur après avoir cliqué sur nouveau de repasser par la box)
*/
void Editeur::on_btnCreationAbe_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* Etape 1 : Le dossier temporaire doit être vide sinon problème d'enregistrement */
    if(!m_abuleduFile->abeFileGetFileList().isEmpty()){
        ABULEDU_LOG_DEBUG() << "Dossier Temporaire vide... [KO]";

        /* Vidange du dossier */
        if(AbulEduTools::supprimerDir(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath())){
            /* La vidange du dossier s'est bien passée */
            ABULEDU_LOG_DEBUG() << "Vidange Dossier Temporaire... [OK]";
        }
    }

    /* Il faut recréer l'arborescence data/images */
    if(m_abuleduFile->abeFileGetDirectoryTemp().mkpath("data/images")) {
        ABULEDU_LOG_DEBUG() << "Creation dossier data/images... [OK]";
    }
    else {
        ABULEDU_LOG_DEBUG() << "Creation dossier data/images... [KO]";
        return;
    }

    ABULEDU_LOG_DEBUG()  << "Dossier temp abuleduFile : " << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath();

    /* On vide les différentes listes */
    m_listeFichiersImages.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->listWidgetImagesSelection->clear();

    /* Style des Boutons */
    ui->btnParcours1->setStyleSheet(styleSheet());
    ui->btnParcours2->setStyleSheet(styleSheet());
    ui->btnParcours3->setStyleSheet(styleSheet());
    ui->btnParcours4->setStyleSheet(styleSheet());
    ui->btnParcours5->setStyleSheet(styleSheet());

    /* CheckBox des différents exercices */
    ui->groupBoxSurvol->setChecked(false);
    ui->groupBoxClic->setChecked(false);
    ui->groupBoxDoubleClic->setChecked(false);
    ui->groupBoxParcours->setChecked(false);

    /* On dit que le fichier abe n'a pas de nom maintenant (important pr la MW) */
    m_abuleduFile->abeFileSetFilename("");
    m_abuleduFile->abeFileSetTitle("");

    /* Et on lance un signal qui changera le titre de la mainwindows */
    emit editorNewAbe(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

    setModeModificationAbe(false);

    /* Si modification abe disponible */
    ui->btnModificationCourant->setText(trUtf8("Editer le module en cours"));
    ui->btnModificationCourant->setEnabled(false);

    /* Passer à la fenetre suivante */
    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
    m_assistantEtapes->abeWidgetAssistantSetEtapeCourante(PageEtapeGestionImages);

    if(ui->cbLangueRessource->count() == 0)
        ui->cbLangueRessource->addItems(m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().values());
}

void Editeur::modificationAbe()
{
    setModeModificationAbe(true);
    slotEditorChangePageRequested(PageEtapeGestionImages);
    slotLoadUnit();
}

/** Bouton modification courant ABE */
void Editeur::on_btnModificationCourant_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    modificationAbe();
}

/** Bouton modification autre (ouverture box) */
void Editeur::on_btnModificationAutre_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    emit editorChooseOrSave(AbulEduBoxFileManagerV1::abeOpen);
    setModeModificationAbe(true);
}

void Editeur::slotLoadUnit()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ <<m_abuleduFile->abeFileGetFileName().fileName();
    m_listeFichiersImages.clear();
    ui->listWidgetImagesSelection->clear();

    /* Remettre tout par défaut */
    foreach (QObject* var, ui->pageParametres->children()) {
        if(var->inherits("QGroupBox"))
            ((QGroupBox*)var)->setChecked(false);
    }
    ui->btnParcours1->setStyleSheet("color : gray;");
    ui->btnParcours2->setStyleSheet("color : gray;");
    ui->btnParcours3->setStyleSheet("color : gray;");
    ui->btnParcours4->setStyleSheet("color : gray;");
    ui->btnParcours5->setStyleSheet("color : gray;");

    m_listeMasques.clear();
    m_listeMasquesFixes.clear();
    m_listeMasquesParcours.clear();

    QDir folder(QString(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/images"));
    folder.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(const QFileInfo fileInfo, folder.entryInfoList())
    {
        ajouterImage(fileInfo.absoluteFilePath());
        m_lastOpenDir = fileInfo.absolutePath();
    }

    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/parametres.conf", QSettings::IniFormat);
    ui->cbNiveauDifficulte->setCurrentIndex(parametres.value("niveauDifficulte",0).toInt());

    /* Exercice Clic */
    parametres.beginGroup("clic");
    ui->groupBoxClic->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxClicSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxClicMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
    parametres.endGroup();
    /* Exercice Double-Clic */
    parametres.beginGroup("doubleClic");
    ui->groupBoxDoubleClic->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxDoubleClicSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxDoubleClicMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
    parametres.endGroup();
    /* Exercice Survol */
    parametres.beginGroup("survol");
    ui->groupBoxSurvol->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxSurvolSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxSurvolMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
    parametres.endGroup();
    /* Exercice Parcours */
    parametres.beginGroup("parcours");
    ui->groupBoxParcours->setChecked(parametres.value("exerciceActive",false).toBool());
    if(parametres.value("exerciceActive",true).toBool())
    {
        ui->spinBoxParcoursSuivant->setValue(parametres.value("timerSuivant",7).toInt());
        ui->spinBoxParcoursMasquesLargeur->setValue(parametres.value("nbMasquesLargeur",7).toInt());
        ui->spinBoxParcoursMasqueHauteur->setValue(parametres.value("nbMasquesHauteur",7).toInt());

        /* Recupération nb masque pour remplissage ComboBox */
        /* Parcours 1 */
        parametres.beginGroup("parcours1");
        if(chargerPositionMasque(1) && remplirGvParcours(1)){
                ABULEDU_LOG_DEBUG() << "Le parcours 1...[OK]";
                ui->btnParcours1->setStyleSheet("color : green;");
                emit signalParcoursSave();
        }
        else{
            ABULEDU_LOG_DEBUG() << "Le parcours 1...[KO]";
            ui->btnParcours1->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        /* Parcours 2 */
        parametres.beginGroup("parcours2");
        if(chargerPositionMasque(2) && remplirGvParcours(2)){
            ABULEDU_LOG_DEBUG() << "Le parcours 2...[OK]";
            ui->btnParcours2->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            ABULEDU_LOG_DEBUG() << "Le parcours 2...[KO]";
            ui->btnParcours2->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        /* Parcours 3 */
        parametres.beginGroup("parcours3");
        if(chargerPositionMasque(3) && remplirGvParcours(3)){
            ABULEDU_LOG_DEBUG() << "Le parcours 3...[OK]";
            ui->btnParcours3->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            ABULEDU_LOG_DEBUG() << "Le parcours 3...[KO]";
            ui->btnParcours3->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        /* Parcours 4 */
        parametres.beginGroup("parcours4");
        if(chargerPositionMasque(4) && remplirGvParcours(4)){
            ABULEDU_LOG_DEBUG() << "Le parcours 4...[OK]";
            ui->btnParcours4->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            ABULEDU_LOG_DEBUG() << "Le parcours 4...[KO]";
            ui->btnParcours4->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        /* Parcours 5 */
        parametres.beginGroup("parcours5");
        if(chargerPositionMasque(5) && remplirGvParcours(5)){
            ABULEDU_LOG_DEBUG() << "Le parcours 5...[OK]";
            ui->btnParcours5->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            ABULEDU_LOG_DEBUG() << "Le parcours 5...[KO]";
            ui->btnParcours5->setStyleSheet("color : red;");
        }
        parametres.endGroup();
    }
    else
    {
        ABULEDU_LOG_DEBUG() << "PAS DE PARCOURS MAIS ON SUPPRIME AU CAS OU ";

        ABULEDU_LOG_DEBUG() << parametres.allKeys();
        parametres.remove("");
        ABULEDU_LOG_DEBUG() << parametres.allKeys();
        parametres.sync();
        /* On a supprimé toutes les entrees parcours
         * On Sauvegarde sans impunité, correction à la volée =)
         * Petite combine pour avoir le nom
         */
        QString nameModule = ((MainWindow *) parentWidget()->topLevelWidget())->abeGetMyAbulEduFile()->abeFileGetFileName().absoluteFilePath();
        ABULEDU_LOG_DEBUG() << nameModule;

        ABULEDU_LOG_DEBUG() << m_abuleduFile->abeFileExportPrepare(AbulEduTools::parcoursRecursif(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()),
                                                        m_abuleduFile->abeFileGetDirectoryTemp().absolutePath(), "abe");
        ABULEDU_LOG_DEBUG() << m_abuleduFile->abeFileExportFile(nameModule.remove(".abe"));
    }

    ui->leTitre->setText(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralTitle(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first()));
    if (abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() == 1)
    {
        ui->leAuteur->setText(abeApp->getAbeIdentite()->abeGetPreNom()+" "+abeApp->getAbeIdentite()->abeGetNom());
    }

    if(ui->cbLangueRessource->count() == 0)
        ui->cbLangueRessource->addItems(m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().values());

    ui->teDescription->setPlainText(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralDescription(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first()).first());
    QString langueRessource = m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().value(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first());

    ui->cbLangueRessource->setCurrentIndex(ui->cbLangueRessource->findText(langueRessource));
//    QString licence = _abuleduFile->abeFileGetLOM()->abeLOMgetRightsDescription(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first());
//    ui->cbLicence->setCurrentIndex(ui->cbLicence->findText(licence));

    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
}

/**********************************************************************************************************************************************************
                                                     GESTION DRAG & DROP
**********************************************************************************************************************************************************/

void Editeur::dropEvent(QDropEvent *event)
{
    ABULEDU_LOG_DEBUG() <<  __PRETTY_FUNCTION__;
    ABULEDU_LOG_DEBUG() <<  event->source() << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());

    if(event->source()->objectName() == "treeViewArborescence" && ui->listWidgetImagesSelection->geometry().contains(event->pos())){
        ABULEDU_LOG_DEBUG() << "SOURCE == treeViewArbo";
        /* Controle que c'est bien une image */
        if(event->mimeData()->hasImage()){
            ABULEDU_LOG_DEBUG() << "C'est une image";
        }
    }
    else if(event->source()->objectName() == "lwSimple" && ui->listWidgetImagesSelection->geometry().contains(event->pos())){
        ABULEDU_LOG_DEBUG() << "SOURCE == mediathequeGet";

        ui->abuleduMediathequeGet->abeStartDownload();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void Editeur::dragEnterEvent(QDragEnterEvent *event)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << event->source() << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());
    event->accept();
}

void Editeur::on_btnAjouterImageQFileDialog_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Choisir une image"), m_lastOpenDir, trUtf8("Images (*.png *.jpg *.jpeg *.svg *.bmp *.ppm *.xpm *.xbm)"));
    QFileInfo fi(fileName);
    if(fi.exists()) {
        ajouterImage(fi.absoluteFilePath());
        m_lastOpenDir = fi.absolutePath();
    }
}

bool Editeur::preparerSauvegarde()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    if(ui->leTitre->text().trimmed().isEmpty() || ui->leAuteur->text().trimmed().isEmpty())
    {
        if(ui->leTitre->text().trimmed().isEmpty()){
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas de titre !"),trUtf8("Vous n'avez pas renseigné le champ titre !"), true, this);
            alertBox->setWindowModality(Qt::ApplicationModal);
            alertBox->show();
            ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeFin);
            ui->lblTitreModule->setStyleSheet("color:red");
            ui->leTitre->setStyleSheet("border:1px solid red;border-radius:3px");
        }
        else{
            ui->lblTitreModule->setStyleSheet("color:black");
            ui->leTitre->setStyleSheet("border:1px solid grey;border-radius:3px");
            /* Le fichier abe porte le titre que l'utilisateur lui donne */
            m_abuleduFile->abeFileSetTitle(ui->lblTitreModule->text());
        }
        if(ui->leAuteur->text().trimmed().isEmpty())
        {
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas d'auteur !"),trUtf8("Vous n'avez pas renseigné le champ auteur !"), true, this);
            alertBox->setWindowModality(Qt::ApplicationModal);
            alertBox->show();
            ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeFin);
            ui->lblNom->setStyleSheet("color:red");
            ui->leAuteur->setStyleSheet("border:1px solid red;border-radius:3px");
        }
        else
        {
            ui->lblNom->setStyleSheet("color:black");
            ui->leAuteur->setStyleSheet("border:1px solid grey;border-radius:3px");
        }
        return false;
    }
    else{
        ui->lblNom->setStyleSheet("color:black");
        ui->leAuteur->setStyleSheet("border:1px solid grey;border-radius:3px");
        ui->lblTitreModule->setStyleSheet("color:black");
        ui->leTitre->setStyleSheet("border:1px solid grey;border-radius:3px");
    }

    QString codeLangue = m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().key(ui->cbLangueRessource->currentText());

    /* Les informations pour LOM */
    m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralTitle(codeLangue,ui->leTitre->text());
    m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralDescription(codeLangue,ui->teDescription->document()->toPlainText());

    vCard vcard;
    vCardProperty name_prop  = vCardProperty::createName(ui->leAuteur->text(), "");
    vcard.addProperty(name_prop);
    vCardProperty fn_prop    = vCardProperty(VC_FORMATTED_NAME, ui->leAuteur->text());
    vcard.addProperty(fn_prop);
    vCardProperty note_prop  = vCardProperty(VC_NOTE, "");
    vcard.addProperty(note_prop);
    vCardProperty org_prop   = vCardProperty::createOrganization("");
    vcard.addProperty(org_prop);
    vCardProperty url_prop   = vCardProperty(VC_URL, "");
    vcard.addProperty(url_prop);

    m_abuleduFile->abeFileGetLOM()->abeLOMaddLifeCycleContributionRole("author", vcard, QDate::currentDate());
    m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsCost("no");
    m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsCopyrightAndOtherRestrictions("yes");
    m_abuleduFile->abeFileGetLOM()->abeLOMsetEducationalDifficulty(ui->cbNiveauDifficulte->currentText());

    m_abuleduFile->abeFileGetLOM()->abeLOMsetAnnotationDate(QDate::currentDate());
    m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralLanguage(codeLangue);

    /* Creation fichier Conf [@note les timers sont convertis en millisecondes] */
    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.setValue("version",abeApp->getAbeApplicationUnitVersion());
    parametres.setValue("niveauDifficulte",ui->cbNiveauDifficulte->currentIndex());

    /* Parametres Survol */
    parametres.beginGroup("survol");
    ABULEDU_LOG_DEBUG()<< "(survol) : "  << ui->groupBoxSurvol->isChecked();
    if (ui->groupBoxSurvol->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxSurvolSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    }
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Parametres Clic */
    parametres.beginGroup("clic");
    if (ui->groupBoxClic->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxClicSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxClicMasque->value()));
    }
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Parametres Double-Clic */
    parametres.beginGroup("doubleClic");
    if (ui->groupBoxDoubleClic->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxDoubleClicSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxDoubleClicMasque->value()));
    }
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Paramètres Parcours */
    /** @todo toutes les sauvegardes de parcours ont été déplacées.
        Reflechir à ce qu'il faut faire ici (contrôle ou rien)
    */
    parametres.beginGroup("parcours");
    if (ui->groupBoxParcours->isChecked()){
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxParcoursSuivant->value()));
        parametres.setValue("nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
        parametres.setValue("nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    }
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Creation .abe */
    parametres.sync(); //pour forcer l'écriture du .conf

    return m_abuleduFile->abeFileExportPrepare(AbulEduTools::parcoursRecursif(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()), m_abuleduFile->abeFileGetDirectoryTemp().absolutePath(), "abe");
}

void Editeur::releaseAbe()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    ui->listWidgetImagesSelection->clear();
    if (preparerSauvegarde())
    {
        ui->stPageMediathequePush->abeSetFile(m_abuleduFile);
        ui->stackedWidgetEditeur->setCurrentWidget(ui->stPageMediathequePush);
    }
}

void Editeur::on_btnEssayer_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    if(preparerSauvegarde()){
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Passage en mode essai..."),
                                                              trUtf8("Votre module n'est pas enregistré. Si les paramètres vous conviennent, revenez dans l'éditeur pour enregistrer ou publier."), true, this);
        alertBox->setWindowModality(Qt::ApplicationModal);
        alertBox->show();
        emit editorTest();
    }
    else
        ABULEDU_LOG_DEBUG() << "TEST Impossible ! ";
}

void Editeur::on_btnEnregistrer_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    createAbe();
}

void Editeur::on_btnPublier_clicked()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    releaseAbe();
}

void Editeur::slotSortieVisionneuse()
{
    slotEditorChangePageRequested(PageEtapeGestionImages);
    /* Activation de la naviguation en fonction */
    controlNumberOfImages();
}

void Editeur::slotAfficheEtatPublication(const int &code)
{
    if(code > 0)
    {
        AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"),
                                                                         trUtf8("Votre module a bien été publié sur AbulÉdu-Médiathèque..."), true, this);
        msgEnregistrement->setWindowModality(Qt::ApplicationModal);
        msgEnregistrement->setWink();
        msgEnregistrement->show();
    }
    else
    {
        AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Problème"),
                                                                         trUtf8("Un problème a empêché la publication de votre module sur AbulÉdu-Médiathèque..."), true, this);
        msgEnregistrement->setWindowModality(Qt::ApplicationModal);
        msgEnregistrement->show();
    }
}
