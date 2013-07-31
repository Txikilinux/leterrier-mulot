/** Classe Editeur
  * @file editeur.cpp
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

#include "editeur.h"
/**
  * @todo :
  */


Editeur::Editeur(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    m_localDebug = true; /// @todo mettre a false
    setAttribute(Qt::WA_DeleteOnClose);

    m_lastOpenDir = QDir::homePath();

    /* Gestion AbulEduMediathequeGet */
    ui->abuleduMediathequeGet->abeSetSource("data");
    ui->abuleduMediathequeGet->abeSetCustomBouton1(trUtf8("Importer l'image"));
    ui->abuleduMediathequeGet->abeHideBoutonTelecharger();
    ui->abuleduMediathequeGet->abeCustomBouton1SetDownload(true);
    ui->abuleduMediathequeGet->abeHideInfoPanel(true);
    ui->abuleduMediathequeGet->abeSetDefaultView(AbulEduMediathequeGetV1::abeMediathequeThumbnails);

    //! @todo ne pas pousser si modif abuleduFile non acceptée
    ui->abuleduMediathequeGet->abeHideCloseBouton(true);

    connect(ui->abuleduMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(QSharedPointer<AbulEduFileV1>, int)), this,
            SLOT(slotImportImageMediatheque(QSharedPointer<AbulEduFileV1>,int)), Qt::UniqueConnection);

    connect(ui->stPageMediathequePush, SIGNAL(signalMediathequePushFileUploaded(int)),this,
            SLOT(slotAfficheEtatPublication(int)), Qt::UniqueConnection);


    QShortcut *shortcutSupprimeChoix = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidgetImagesSelection, 0, 0, Qt::WidgetShortcut);
    connect(shortcutSupprimeChoix, SIGNAL(activated()), this, SLOT(slotSupprimerImage()), Qt::UniqueConnection);

    m_opt_nbMasquesChoisisParcours = m_opt_nbMasquesLargeur = m_opt_nbMasquesHauteur = m_numeroParcours = 0;

    /* Vidange des différentes listes */
    m_listeMasques.clear();
    m_listeMasquesParcours.clear();
    m_listeFichiersImages.clear();

    m_parametresParcours1.clear();
    m_parametresParcours2.clear();
    m_parametresParcours3.clear();
    m_parametresParcours4.clear();
    m_parametresParcours5.clear();

    setAcceptDrops(true);

    creationMenu();

    /* Affichage de la mediatheque par defaut */
    ui->tabWidgetImages->setCurrentWidget(ui->pageMediatheque);

    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->gvPageVisio->setScene(scene);

    /* Mappage des signaux des 5 boutons de parcours */
    mapSignalBtnParcours();

    /* Gestion des messages d'aide [à effectuer avant le setCurrentIndex(...)] */
    initMessagesAide();
    ui->btnModificationCourant->setEnabled(false);

    /* Gestion Assistant Etape */
    QList<QPair<QString, QString> > listeEtapes;
    listeEtapes.append(qMakePair(trUtf8("Accueil"),                 trUtf8("Accueil","For short")));
    listeEtapes.append(qMakePair(trUtf8("Gérer les images"),        trUtf8("Images","For short")));
    listeEtapes.append(qMakePair(trUtf8("Régler les paramètres"),   trUtf8("Paramètres","For short")));
    listeEtapes.append(qMakePair(trUtf8("Sauvegarder"),             trUtf8("Sauvegarder","For short")));

    _assistantEtapes = new AbulEduWidgetAssistantEtapesV1(listeEtapes);
    _assistantEtapes->abeWidgetAssistantSetFontSize(12, 16, 20);
    _assistantEtapes->abeWidgetAssistantEnableClick(false);
    ui->hlFilAriane->addWidget(_assistantEtapes);

    connect(_assistantEtapes, SIGNAL(signalEtapeHasChanged(int)), this, SLOT(slotEditorChangePageRequested(int)), Qt::UniqueConnection);
    connect(_assistantEtapes, SIGNAL(signalQuitterRequested()), this, SLOT(slotCloseEditor()), Qt::UniqueConnection);

    slotEditorChangePageRequested(PageAccueil);
}

Editeur::~Editeur()
{
    /// @todo delete les objets sans parent
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__ << _abuleduFile->abeFileGetFileName().baseName();
    delete ui;
    emit editorExited();
}

/*****************************************************************************************************************
                                    GESTION NAVIGATION
  ****************************************************************************************************************/
void Editeur::slotEditorChangePageRequested(int page)
{
    if (m_localDebug) qDebug()<<__PRETTY_FUNCTION__ << " " << page ;

    switch(page)
    {
    case PageAccueil:
        if(m_localDebug) qDebug() << "PAGE Accueil";
        ui->stackedWidgetEditeur->setCurrentIndex(PageAccueil);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        _assistantEtapes->abeWidgetAssistantEnableClick(false);
        ui->lbAide->setText(_messageAidePageAccueil);
        break;
    case PageGestionImages:
        if(m_localDebug) qDebug() << "PAGE Images";
        ui->stackedWidgetEditeur->setCurrentIndex(PageGestionImages);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        ui->lbAide->setText(_messageAidePageGestionImages);
        /* Contrôle 5 images = bouton suivant ok */
        controlNumberOfImages();
        break;
    case PageParametres:
        if(m_localDebug) qDebug() << "PAGE Parametres";
        ui->stackedWidgetEditeur->setCurrentIndex(PageParametres);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        ui->lbAide->setText(_messageAidePageParametres);
        break;
    case PageFin:
        if(m_localDebug) qDebug() << "PAGE Fin";
        ui->stackedWidgetEditeur->setCurrentIndex(PageFin);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        ui->lbAide->setText(_messageAidePageFin);
        break;
    case PageVisio:
        if(m_localDebug) qDebug() << "PAGE Visio";
        ui->stackedWidgetEditeur->setCurrentIndex(PageVisio);
        ui->lbAide->setText(_messageAidePageVisio);
        break;
    default:
        break;
    }

}

void Editeur::slotCloseEditor()
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    /* Remettre le titre par defaut du boutonModifier courant */
    ui->btnModificationCourant->setText(trUtf8("&Editer le module en cours"));
    /* Remettre la page d'Accueil par defaut */
    ui->stackedWidgetEditeur->setCurrentIndex(PageAccueil);
    _assistantEtapes->abeWidgetAssistantSetEtapeCourante(PageAccueil);
    /* Vider les listes */

    emit editorExited();
}

/*********************************************************************************************************

  ********************************************************************************************************/
void Editeur::initMessagesAide()
{
    _messageAidePageAccueil = trUtf8("Je suis votre guide, je vous donnerai les consignes à chaque écran rencontré.\n")
            + trUtf8("Choississez ci-dessous un mode d'édition.\n")
            + trUtf8("Les étapes sont bloquantes tant que les paramètres ne sont convenables.");

    _messageAidePageGestionImages = trUtf8("Veuillez sélectionner 5 images. Il est possible de selectionner ces images sur votre ordinateur (onglet \"Disque Local\")\n")
            + trUtf8("ou sur la médiathèque d'AbulEdu (onglet \"AbulEdu Data\").\n")
            + trUtf8("Pour passer à l'ecran suivant, la liste \"Images sélectionnées\" doit comporter 5 images.");

    _messageAidePageParametres = trUtf8("Cochez les exercices désirés et réglez les différentes valeurs.\n")
            + trUtf8("Pour passer à l'ecran suivant, au moins un exercice doit être activé.\n")
            + trUtf8("Si l'exercice parcours est activé, tous les boutons \"Parcours\" doivent être vert.");

    _messageAidePageFin = "";

    _messageAidePageVisio = trUtf8("Pour revenir à l'écran précédent, cliquez sur l'image.");
}

void Editeur::controlNumberOfImages()
{
    if(m_listeFichiersImages.count() >= 5)
    {
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(true);
    }
    else
    {
        _assistantEtapes->abeWidgetAssistantEnableClick(false);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
    }
}


//void Editeur::abeEditeurSetMainWindow(QWidget *mw)
//{
//    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

//    MainWindow* parent = (MainWindow*) mw;
//    connect(parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),this, SLOT(show()), Qt::UniqueConnection);
//    connect(parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),
//            parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur(),SLOT(hide()), Qt::UniqueConnection);

//    /* Sauvegarde -> BoxManager en version complète pour afficher le champ "Titre du Module" */
//    parent->abeGetMyAbulEduFileManager()->abeSetDisplaySimpleOrCompleteEnum(AbulEduBoxFileManagerV1::abeDisplayComplete);

//    if(!parent->abeGetMyAbulEduFile()->abeFileGetFileName().baseName().isEmpty())
//    {
//        _abuleduFile = parent->abeGetMyAbulEduFile();
//        ui->btnModificationCourant->setText(trUtf8("Editer le module ")+ "\n" + _abuleduFile->abeFileGetFileName().fileName());
//        ui->btnModificationCourant->setEnabled(true);
//        ui->btnModificationCourant->setMinimumHeight(60);
//        ui->btnModificationAutre->setMinimumHeight(60);
//        ui->btnCreationAbe->setMinimumHeight(60);
//    }
//    else
//    {
//        _abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
//        ui->btnModificationCourant->setEnabled(false);
//        parent->abeSetMyAbulEduFile(_abuleduFile);
//    }

//    ui->cbLangueRessource->addItems(_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().values());

//    if(_abuleduFile->abeFileGetDirectoryTemp().mkpath("data/images")) {
//        if(m_localDebug) qDebug() << "Creation dossier data/images... [OK]";
//    }
//    else {
//        if(m_localDebug) qDebug() << "Creation dossier data/images... [KO]";
//        return;
//    }
//    ui->stackedWidgetEditeur->setCurrentWidget(ui->pageAccueil);
//}

void Editeur::creationMenu()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    QStyle* style =  QApplication::style(); /* récupération du style systeme */

    /* MENU LISTWIDGET (Supprimer) */
    m_menuListWidget = new QMenu(ui->listWidgetImagesSelection);
    QIcon iconSupprimer = style->standardIcon(QStyle::SP_DialogResetButton); /* On récupère l'icône désiré */
    QAction *a_supprimer = new QAction(trUtf8("&Supprimer de la selection"),m_menuListWidget);
    a_supprimer->setIcon(iconSupprimer);
    a_supprimer->setIconVisibleInMenu(true);
    a_supprimer->connect(a_supprimer, SIGNAL(triggered()), this, SLOT(slotSupprimerImage()), Qt::UniqueConnection);
    m_menuListWidget->addAction(a_supprimer);
}

void Editeur::slotSupprimerImage()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    /* condition garde meme si j'appelle ce slot que si j'ai un item ds ma listView, donc une liste avec au moins 1 éléments =) */
    if (m_listeFichiersImages.isEmpty()){return;}
    if (ui->listWidgetImagesSelection->selectedItems().isEmpty()){ return;}

    /* Debug avant suppression*/
    if (m_localDebug){
        qDebug() << "Suppression ItemImage -> liste images avant : ";
        for (int i = 0; i < m_listeFichiersImages.count(); i++)
        {
            qDebug() << i <<" "<<m_listeFichiersImages.at(i);
        }
    }

    /* Suppression des items selectionnés */
    foreach(QListWidgetItem *i, ui->listWidgetImagesSelection->selectedItems())
    {
        m_listeFichiersImages.removeOne(i->data(4).toString());
        QFileInfo fi(i->data(4).toString());
        if(QFile::remove(fi.absoluteFilePath().replace(fi.suffix(), "xml").remove("/images"))){
            if(m_localDebug) qDebug() << "Suppression du fichier XML accompagnant la ressource... [OK]";
        }
        if (QFile::remove(fi.absoluteFilePath()))
        {
            if (m_localDebug)qDebug() << "Suppression de l'image du fichier temporaire... [OK]";
        }
        delete i;
    }

    /* Debug apres suppression*/
    if (m_localDebug){
        qDebug() << "Suppression ItemImage -> liste images apres : ";
        for (int i = 0; i < m_listeFichiersImages.count(); i++)
        {
            if (m_localDebug) qDebug() << i <<" "<<m_listeFichiersImages.at(i);
        }
    }

    /* Contrôle 5 images = bouton suivant ok */
    controlNumberOfImages();
}

void Editeur::ajouterImage(QFileInfo monFichier)
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << " Chemin : " << monFichier.absoluteFilePath() << "Nom : " << monFichier.fileName();

    /* Controle des insertions (éviter les doublons) */
    if (m_listeFichiersImages.contains(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg"))
    {
        if(m_localDebug) qDebug() << "Fichier deja présent";
        return;
    }

    qDebug() << "Chemin d'enregistrement : " << _abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/data/images/" ;
    if(_abuleduFile->resizeImage(&monFichier, 1024, _abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" ))
    {
        /* je range le chemin de l'image dans ma liste (celui du fichier temp) */
        m_listeFichiersImages << _abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg";
        /* Insertion dans mon listWidget */
        QListWidgetItem *item = new QListWidgetItem(QIcon(monFichier.absoluteFilePath()), monFichier.baseName());
        item->setData(4, _abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() + ".jpg");
        ui->listWidgetImagesSelection->insertItem(0, item);
    }

    /* Contrôle 5 images = bouton suivant ok */
    controlNumberOfImages();
}

void Editeur::slotImportImageMediatheque(QSharedPointer<AbulEduFileV1> fichierABB, const int& success)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    if(success > -1)
    {
        ajouterImage(fichierABB->abeFileGetContent(0));
        /* Ajout des metadata */
        _abuleduFile->abeFileAddMetaDataFromABB(fichierABB->abeFileGetLOM(), fichierABB->abeFileGetContent(0).baseName());

        if(m_localDebug) qDebug() << "Importation image depuis mediatheque... [OK]";
    }
    else
    {
        if(m_localDebug) qDebug() << "Importation image depuis mediatheque... [KO]";
    }
}

void Editeur::on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    /* Si j'ai un item à cet endroit, j'appelle mon menu */
    if (ui->listWidgetImagesSelection->itemAt(pos) != NULL)
    {
        m_menuListWidget->exec(ui->listWidgetImagesSelection->mapToGlobal(pos));
    }
}

void Editeur::on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    ui->listWidgetImagesSelection->clear();

    if (preparerSauvegarde())
    {
        emit editorChooseOrSave(AbulEduBoxFileManagerV1::abeSave);
    }
}

/***********************************************************************************************************************************************************
                                                               PARCOURS
**********************************************************************************************************************************************************/

bool Editeur::remplirGvParcours(const int &numeroParcours)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    switch(numeroParcours)
    {
    case 1:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_1->value();
        break;
    case 2:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_2->value();
        break;
    case 3:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_3->value();
        break;
    case 4:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_4->value();
        break;
    case 5:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_5->value();
        break;
    }

    m_opt_nbMasquesLargeur = ui->spinBoxParcoursMasquesLargeur->value();
    m_opt_nbMasquesHauteur = ui->spinBoxParcoursMasqueHauteur->value();

    float largeurMasque, hauteurMasque = 0.00;

    float largeurGv = static_cast<float>(m_editeurParcoursWidget->width())-1;
    float hauteurGv = static_cast<float>(m_editeurParcoursWidget->height())-1;

    largeurMasque = largeurGv / m_opt_nbMasquesLargeur;
    hauteurMasque = hauteurGv / m_opt_nbMasquesHauteur;

    int nbMasques = m_opt_nbMasquesLargeur * m_opt_nbMasquesHauteur;
    qreal xMasque, yMasque = 0.00;

    if (m_localDebug)
    {
        qDebug()<<" -------------------------- Début boucle d'affichage : " << nbMasques;
        qDebug() << "Chargement position parcours : " << numeroParcours;
    }

    int numeroMasque = 0;
    for (float i=0; i < m_opt_nbMasquesHauteur; i++)
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
            m_editeurParcoursWidget->getScene()->addItem(m_masque);

            m_listeMasques << m_masque;
            numeroMasque++;
        }
        xMasque = 0;
        yMasque += hauteurMasque;
    }

    /* Gestion de la taille de la Scene */
    m_editeurParcoursWidget->setGeometry((m_editeurParcoursWidget->x()), (m_editeurParcoursWidget->y()),
                                         (largeurMasque * m_opt_nbMasquesLargeur) + m_editeurParcoursWidget->getGraphicsView()->verticalScrollBar()->width(),
                                         (hauteurMasque * m_opt_nbMasquesHauteur) + m_editeurParcoursWidget->getGraphicsView()->horizontalScrollBar()->height() + (m_editeurParcoursWidget->getBoutonHeight()) *2);

    if(chargerPositionMasque(numeroParcours) && m_modeModificationAbe){

        if(m_localDebug){
            qDebug() << "Nous avons des parcours dans le QSetting & Mode modification = true";
        }

        /* Parcours de la QMap positionMasqueParcours */
        int positionDepart, positionArrivee = 0;
        QList<int> positionParcours;

        QMap<QString, int>::const_iterator i = positionMasquesParcours.constBegin();
        if(!positionMasquesParcours.isEmpty()) /* Condition de garde si on reinitialise un parcours en mode modification */
        {
            while (i != positionMasquesParcours.constEnd())
            {
                if(m_localDebug) qDebug() << i.key() << " " << i.value();
                if (i.key() == "MasqueDepart")
                    positionDepart = i.value();
                else if (i.key() == "MasqueArrivee")
                    positionArrivee = i.value();
                else
                    positionParcours << i.value();

                i ++;
            }
            if (m_localDebug)
            {
                qDebug() << "La liste des positions normales : " << positionMasquesParcours;
                qDebug() << "Position Depart/Arrivee         : " << positionDepart << "/" << positionArrivee;
                qDebug() << "Position Parcours               : " << positionParcours;
            }

            /* MODE MODIFICATION
             * Ici on a toutes les positions necessaires, plus qu'à les mettre dans l'ordre : depart, parcours, arrivee
             */
            /* depart */
            m_listeMasques.at(positionDepart)->setColor(QColor(Qt::green));
            m_listeMasques.at(positionDepart)->setProperty("Role", trUtf8("Depart"));
            m_listeMasquesParcours << m_listeMasques.at(positionDepart);

            /* parcours */
            while(!positionParcours.isEmpty())
            {
                /* Contrôle d'intégrité du parcours */
                if(!AbulEduTools::masquesVoisins(m_listeMasquesParcours.last()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur).contains(positionParcours.first()))
                {
                    QString msg = "<td> " + trUtf8("Les paramètres du parcours ne sont pas valides.")+" <br />"
                            + trUtf8("Les données relatives à ce module vont être <b>effacés</b>,") +" <br />"
                            + trUtf8("veuillez éditer un nouveau parcours.") +" <br />"
                            //                            + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                            +" </td>" ;
                    AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, m_parent);
                    messageBox->show();
                    reinitialiserGvParcours();
                    return false;
                }

                m_listeMasques.at(positionParcours.first())->setColor(QColor(Qt::black));
                m_listeMasques.at(positionParcours.first())->setProperty("Role", trUtf8("Parcours"));
                m_listeMasquesParcours << m_listeMasques.at(positionParcours.first());
                positionParcours.removeFirst();
            }

            /* arrivee */
            m_listeMasques.at(positionArrivee)->setColor(QColor(Qt::red));
            m_listeMasques.at(positionArrivee)->setProperty("Role", trUtf8("Arrivee"));
            m_listeMasquesParcours << m_listeMasques.at(positionArrivee);

            /* Et j'active le menu Sauvegarder */
            m_editeurParcoursWidget->connectBtnSave(true);
        }
        return true;
    }
    else{
        if(m_localDebug) qDebug() << "Nous n'avons pas de parcours dans le QSetting";
        m_editeurParcoursWidget->connectBtnSave(false);
        return true;
    }
    return false;
}

void Editeur::masquePoseParcours(MasqueDeplaceSouris* masque)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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
        else if(m_listeMasquesParcours.count() < (m_opt_nbMasquesChoisisParcours - 1))
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
        else if ( m_listeMasquesParcours.count() == (m_opt_nbMasquesChoisisParcours-1))
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
                m_editeurParcoursWidget->connectBtnSave(true);
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
        m_editeurParcoursWidget->connectBtnSave(false);
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
    if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
}

void Editeur::reinitialiserGvParcours()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    /* Remettre tout mes masques d'origine ! */
    foreach (MasqueDeplaceSouris* var, m_listeMasques) {
        var->setColor(QColor(Qt::white));
        var->setProperty("Role", trUtf8("Fixe"));
        var->update();
    }

    /* Vider ma listeMasquesParcours */
    m_listeMasquesParcours.clear();
    m_editeurParcoursWidget->update();

    /* Supprimer le parcours dans le .conf si on est en mode modification */
    if(m_modeModificationAbe){
        qDebug() << "Nous sommes en modification, on recherche les entrées dans le QSettings";

        QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
        parametres.beginGroup("parcours");
        parametres.beginGroup("parcours"+QString::number(m_editeurParcoursWidget->getNumeroParcours()));

        if(parametres.childKeys().count() > 0){ /* Si on a des entrées relatives à ce parcours, on les supprime */
            parametres.remove("");
            parametres.endGroup();
            parametres.sync();
            if(m_localDebug) qDebug() << "Parcours num : " <<m_editeurParcoursWidget->getNumeroParcours()<< " -> efface du conf";
        }

        /* Le bouton relatif au parcours ne doit plus etre vert*/
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
    }
}

void Editeur::sauvegarderParcours()
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__ << "-- Numero de parcours :" << m_numeroParcours;
    }

    switch (m_numeroParcours)
    {
    case 1:
        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
        m_parametresParcours1.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours1.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        /* Il reste que des masques "Parcours" dans la liste */
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours1.insert("MasqueParcours" + QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours1->setStyleSheet("color : green;");
        break;
    case 2:
        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
        m_parametresParcours2.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours2.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        /* Il reste que des masques "Parcours" dans la liste */
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours2.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours2->setStyleSheet("color : green;");
        break;

    case 3:
        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
        m_parametresParcours3.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours3.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        /* Il reste que des masques "Parcours" dans la liste */
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours3.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours3->setStyleSheet("color : green;");
        break;

    case 4:
        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
        m_parametresParcours4.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours4.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        /* Il reste que des masques "Parcours" dans la liste */
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours4.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours4->setStyleSheet("color : green;");
        break;

    case 5:
        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
        m_parametresParcours5.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours5.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        /* Il reste que des masques "Parcours" dans la liste */
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours5.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours5->setStyleSheet("color : green;");
        break;
    default:
        return;
    }

    m_listeMasquesParcours.clear();
    m_listeMasques.clear();

    /* Petit message */
    AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"), true, this);
    alertBox->setWink();
    alertBox->show();
    /* Fermeture Aire de jeu */
    m_editeurParcoursWidget->close();
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

void Editeur::slotBtnParcours_clicked(const int &numBtn)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__ << numBtn;

    int nbMasque = 0;
    m_listeMasquesParcours.clear();
    m_numeroParcours = numBtn;

    switch(numBtn){
    case 1:
        nbMasque = ui->spinBoxParcoursMasque_1->value();
        break;
    case 2:
        nbMasque = ui->spinBoxParcoursMasque_2->value();
        break;
    case 3:
        nbMasque = ui->spinBoxParcoursMasque_3->value();
        break;
    case 4:
        nbMasque = ui->spinBoxParcoursMasque_4->value();
        break;
    case 5:
        nbMasque = ui->spinBoxParcoursMasque_5->value();
        break;
    default:
        AbulEduMessageBoxV1 *alertBox = new AbulEduMessageBoxV1(trUtf8("Une erreur est survenue"),trUtf8("Relancer l'editeur."), true, this);
        alertBox->show();
        return;
    }

    /* Verification du nombre de masque */
    if (nbMasque >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Nombre masques Parcours"),
                                                              trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur"), true, this);
        alertBox->show();
        return;
    }

    m_editeurParcoursWidget = new EditeurParcoursWidget(m_numeroParcours);
    m_editeurParcoursWidget->setWindowTitle(trUtf8("Parcours ")+QString::number(numBtn));
    m_editeurParcoursWidget->setWindowModality(Qt::ApplicationModal);

    connect(m_editeurParcoursWidget->getBtnReset(), SIGNAL(clicked()), this, SLOT(reinitialiserGvParcours()), Qt::UniqueConnection);
    connect(m_editeurParcoursWidget->getBtnSave(), SIGNAL(clicked()), this, SLOT(sauvegarderParcours()), Qt::UniqueConnection);

    /* On centre la fenetre sur l'ecran de l'utilisateur */
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    m_editeurParcoursWidget->move((desktop_width-m_editeurParcoursWidget->width())/2, (desktop_height-m_editeurParcoursWidget->height())/2);

    if(remplirGvParcours(m_numeroParcours)){
        if(m_localDebug)
            qDebug() << "Remplissage OK, Affichage editeur de parcours";
        m_editeurParcoursWidget->show();
    }
    else {
        if(m_localDebug)
            qDebug() << "Remplissage KO, Réinitialisation parcours";
        reinitialiserGvParcours();
    }
}

bool Editeur::chargerPositionMasque(const int &numeroParcours)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    positionMasquesParcours.clear();
    QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.beginGroup("parcours");
    parametres.beginGroup("parcours"+QString::number(numeroParcours));
    for (int i =0 ; i < parametres.childKeys().count(); i++)
    {
        positionMasquesParcours.insert(parametres.childKeys().at(i),parametres.value(parametres.childKeys().at(i)).toInt());
    }
    if(positionMasquesParcours.isEmpty())
        return false;
    else
        return true;
}

/**********************************************************************************************************************************************************
                                                    GESTION MODE MODIFIER ABE
**********************************************************************************************************************************************************/

void Editeur::setModeModificationAbe(const bool &yesNo)
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__ << yesNo;

    m_modeModificationAbe = yesNo;
}

/** Bouton création ABE
    Probleme :
    si je clique sur nouveau, je supprime le contenu du dossier temporaire (pour sauvegarde ultérieure et bug mix d'abe possible)
    Il faut aussi supprimer le nom du module ouvert avant dan la barre des titres de la Mainwindows
    Il faut également dire au logiciel = Attention l'Abe est vide (ce qui permettrait en sortant de l'éditeur après avoir cliqué sur nouveau de repasser par la box)

*/
void Editeur::on_btnCreationAbe_clicked()
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;

    /* Etape 1 : Le dossier temporaire doit être vide sinon problème d'enregistrement */
    if(!_abuleduFile->abeFileGetFileList().isEmpty()){
        if(m_localDebug) qDebug() << "Dossier Temporaire vide... [KO]";

        /* Vidange du dossier */
        if(AbulEduTools::supprimerDir(_abuleduFile->abeFileGetDirectoryTemp().absolutePath())){
            /* La vidange du dossier s'est bien passée */
            if(m_localDebug) qDebug() << "Vidange Dossier Temporaire... [OK]";
        }
    }

    /* Il faut recréer l'arborescence data/images */
    if(_abuleduFile->abeFileGetDirectoryTemp().mkpath("data/images")) {
        if(m_localDebug) qDebug() << "Creation dossier data/images... [OK]";
    }
    else {
        if(m_localDebug) qDebug() << "Creation dossier data/images... [KO]";
        return;
    }

    if(m_localDebug) qDebug() << "Dossier temp abuleduFile : " << _abuleduFile->abeFileGetDirectoryTemp().absolutePath();

    /* On vide les différentes listes */
    m_listeDossiers.clear();
    m_listeFichiersImages.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->listWidgetImagesSelection->clear();

    /* On dit que le fichier abe n'a pas de nom maintenant (important pr la MW) */
    _abuleduFile->abeFileSetFilename("");

    /* Et on lance un signal qui changera le titre de la mainwindows */
    emit editorNewAbe(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

    setModeModificationAbe(false);

    /* Passer à la fenetre suivante */
    ui->stackedWidgetEditeur->setCurrentIndex(PageGestionImages);
    _assistantEtapes->abeWidgetAssistantSetEtapeCourante(PageGestionImages);
}

/** Bouton modification courant ABE */
void Editeur::on_btnModificationCourant_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    setModeModificationAbe(true);
    slotLoadUnit();
}

/** Bouton modification autre (ouverture box) */
void Editeur::on_btnModificationAutre_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    emit editorChooseOrSave(AbulEduBoxFileManagerV1::abeOpen);
    setModeModificationAbe(true);
}


//void Editeur::slotOpenFile(QSharedPointer<AbulEduFileV1>)
//{
//    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
//    slotLoadUnit();
//    if (m_localDebug) qDebug() << "Répertoire Tmp dézippage de l'ABE : "  << _abuleduFile->abeFileGetDirectoryTemp().absolutePath();
//}

void Editeur::slotLoadUnit()
{
    if (m_localDebug) qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__<<" :: "<<_abuleduFile->abeFileGetFileName().fileName();
    m_listeFichiersImages.clear();
    ui->listWidgetImagesSelection->clear();
    QDir folder(QString(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/images"));
    folder.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(const QFileInfo fileInfo, folder.entryInfoList())
    {
        ajouterImage(fileInfo.absoluteFilePath());
        m_lastOpenDir = fileInfo.absolutePath();
    }

    QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.beginGroup("clic");
    ui->groupBoxClic->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxClicSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxClicMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
    parametres.endGroup();
    parametres.beginGroup("doubleClic");
    ui->groupBoxDoubleClic->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxDoubleClicSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxDoubleClicMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
    parametres.endGroup();
    parametres.beginGroup("survol");
    ui->groupBoxSurvol->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxSurvolSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxSurvolMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
    parametres.endGroup();
    parametres.beginGroup("parcours");
    ui->groupBoxParcours->setChecked(parametres.value("exerciceActive",true).toBool());
    ui->spinBoxParcoursSuivant->setValue(parametres.value("timerSuivant",7).toInt());
    ui->spinBoxParcoursMasquesLargeur->setValue(parametres.value("nbMasquesLargeur",7).toInt());
    ui->spinBoxParcoursMasqueHauteur->setValue(parametres.value("nbMasquesHauteur",7).toInt());

    /* Recupération nb masque pour remplissage ComboBox */
    parametres.beginGroup("parcours1");
    ui->spinBoxParcoursMasque_1->setValue(parametres.childKeys().count());
    parametres.endGroup();
    parametres.beginGroup("parcours2");
    ui->spinBoxParcoursMasque_2->setValue(parametres.childKeys().count());
    parametres.endGroup();
    parametres.beginGroup("parcours3");
    ui->spinBoxParcoursMasque_3->setValue(parametres.childKeys().count());
    parametres.endGroup();
    parametres.beginGroup("parcours4");
    ui->spinBoxParcoursMasque_4->setValue(parametres.childKeys().count());
    parametres.endGroup();
    parametres.beginGroup("parcours5");
    ui->spinBoxParcoursMasque_5->setValue(parametres.childKeys().count());
    parametres.endGroup();

    ui->leTitre->setText(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralTitle(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first()));
    if (abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() == 1)
    {
        ui->leAuteur->setText(abeApp->getAbeIdentite()->abeGetPreNom()+" "+abeApp->getAbeIdentite()->abeGetNom());
    }
    ui->teDescription->setPlainText(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralDescription(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first()).first());
    QString langueRessource = _abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().value(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first());
    ui->cbLangueRessource->setCurrentIndex(ui->cbLangueRessource->findText(langueRessource));
    QString licence = _abuleduFile->abeFileGetLOM()->abeLOMgetRightsDescription(_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first());
    ui->cbLicence->setCurrentIndex(ui->cbLicence->findText(licence));

    ui->stackedWidgetEditeur->setCurrentWidget(ui->pageGestionImages);
}


/**********************************************************************************************************************************************************
                                                     GESTION DRAG & DROP
**********************************************************************************************************************************************************/

void Editeur::dropEvent(QDropEvent *event)
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() <<  event->source() << " " << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());
    }

    if (event->source()->objectName() == "treeViewArborescence" && ui->listWidgetImagesSelection->geometry().contains(event->pos()))
    {
        if(m_localDebug) qDebug() << "SOURCE == treeViewArbo";
        /* Controle que c'est bien une image */
        if(event->mimeData()->hasImage())
        {
            if (m_localDebug) qDebug() << "C'est une image";
        }
    }
    else if(event->source()->objectName() == "lwSimple" && ui->listWidgetImagesSelection->geometry().contains(event->pos()))
    {
        if (m_localDebug) qDebug() << "SOURCE == mediathequeGet";

        ui->abuleduMediathequeGet->abeStartDownload();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void Editeur::dragEnterEvent(QDragEnterEvent *event)
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__ << " " << event->source() << " " << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());
    event->accept();
}

void Editeur::on_btnAjouterImageQFileDialog_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Choisir une image"), m_lastOpenDir, trUtf8("Images (*.png *.jpg *.jpeg *.svg *.bmp *.ppm *.xpm *.xbm)"));
    QFileInfo fi(fileName);
    if(fi.exists()) {
        ajouterImage(fi.absoluteFilePath());
        m_lastOpenDir = fi.absolutePath();
    }
}

bool Editeur::preparerSauvegarde()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    if(ui->leTitre->text().trimmed().isEmpty() || ui->leAuteur->text().trimmed().isEmpty())
    {
        if(ui->leTitre->text().trimmed().isEmpty())
        {
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas de titre !"),trUtf8("Vous n'avez pas renseigné le champ titre !"), true, this);
            alertBox->show();
            ui->stackedWidgetEditeur->setCurrentWidget(ui->pageFin);
            ui->lblTitreModule->setStyleSheet("color:red");
            ui->leTitre->setStyleSheet("border:1px solid red;border-radius:3px");
        }
        else
        {
            ui->lblTitreModule->setStyleSheet("color:black");
            ui->leTitre->setStyleSheet("border:1px solid grey;border-radius:3px");
        }
        if(ui->leAuteur->text().trimmed().isEmpty())
        {
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas d'auteur !"),trUtf8("Vous n'avez pas renseigné le champ auteur !"), true, this);
            alertBox->show();
            ui->stackedWidgetEditeur->setCurrentWidget(ui->pageFin);
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
    else
    {
        ui->lblNom->setStyleSheet("color:black");
        ui->leAuteur->setStyleSheet("border:1px solid grey;border-radius:3px");
        ui->lblTitreModule->setStyleSheet("color:black");
        ui->leTitre->setStyleSheet("border:1px solid grey;border-radius:3px");
    }

    QString codeLangue = _abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().key(ui->cbLangueRessource->currentText());

    /* Les informations pour LOM */
    _abuleduFile->abeFileGetLOM()->abeLOMsetGeneralTitle(codeLangue,ui->leTitre->text());
    _abuleduFile->abeFileGetLOM()->abeLOMsetGeneralDescription(codeLangue,ui->teDescription->document()->toPlainText());

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

    _abuleduFile->abeFileGetLOM()->abeLOMaddLifeCycleContributionRole("author", vcard, QDate::currentDate());
    _abuleduFile->abeFileGetLOM()->abeLOMsetRightsCost("no");
    _abuleduFile->abeFileGetLOM()->abeLOMsetRightsCopyrightAndOtherRestrictions("yes");
    _abuleduFile->abeFileGetLOM()->abeLOMsetRightsDescription(codeLangue,ui->cbLicence->currentText());

    _abuleduFile->abeFileGetLOM()->abeLOMsetAnnotationDate(QDate::currentDate());
    _abuleduFile->abeFileGetLOM()->abeLOMsetGeneralLanguage(codeLangue);

    QString destTemp = _abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    QDir().mkpath(destTemp + "/data/images");
    QDir().mkpath(destTemp + "/conf");

    /* Creation fichier Conf [@note les timers sont convertis en millisecondes] */
    QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.setValue("version",abeApp->applicationVersion());
    /* Parametres Survol */
    parametres.beginGroup("survol");
    if (ui->groupBoxSurvol->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxSurvolSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    }
    else
    {
        parametres.setValue("exerciceActive",false);
    }
    parametres.endGroup();
    /* Parametres Clic */
    parametres.beginGroup("clic");
    if (ui->groupBoxClic->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxSurvolSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    }
    else
    {
        parametres.setValue("exerciceActive",false);
    }
    parametres.endGroup();
    /* Parametres Double-Clic */
    parametres.beginGroup("doubleClic");
    if (ui->groupBoxClic->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxSurvolSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    }
    else
    {
        parametres.setValue("exerciceActive",false);
    }
    parametres.endGroup();
    /* Paramètres Parcours */
    parametres.beginGroup("parcours");
    if (ui->groupBoxParcours->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxParcoursSuivant->value()));
        parametres.setValue("nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
        parametres.setValue("nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
        if(!m_parametresParcours1.isEmpty())
        {
            QMapIterator<QString, QVariant> i(m_parametresParcours1);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours1/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours2.isEmpty())
        {
            QMapIterator<QString, QVariant> i(m_parametresParcours2);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours2/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours3.isEmpty())
        {
            QMapIterator<QString, QVariant> i(m_parametresParcours3);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours3/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours4.isEmpty())
        {
            QMapIterator<QString, QVariant> i(m_parametresParcours4);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours4/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours5.isEmpty())
        {
            QMapIterator<QString, QVariant> i(m_parametresParcours5);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours5/"+ i.key(), i.value());
            }
        }
    }
    else
    {
        parametres.setValue("exerciceActive",false);
    }
    parametres.endGroup();

    /* Creation .abe */
    parametres.sync(); //pour forcer l'écriture du .conf
    _abuleduFile->abeFileExportPrepare(AbulEduTools::parcoursRecursif(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()), _abuleduFile->abeFileGetDirectoryTemp().absolutePath(), "abe");

    return true;
}

void Editeur::releaseAbe()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    ui->listWidgetImagesSelection->clear();
    if (preparerSauvegarde())
    {
        ui->stPageMediathequePush->abeSetFile(_abuleduFile);
        ui->stackedWidgetEditeur->setCurrentWidget(ui->stPageMediathequePush);

    }
}

void Editeur::on_btnEssayer_clicked()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    if (preparerSauvegarde())
    {
        emit editorTest();
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Passage en mode essai..."),
                                                              trUtf8("Votre module n'est pas enregistré. Si les paramètres vous conviennent, revenez dans l'éditeur pour enregistrer ou publier."), true, this);
        alertBox->show();
    }
}

void Editeur::on_btnEnregistrer_clicked()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    createAbe();
}

void Editeur::on_btnPublier_clicked()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    releaseAbe();
}

void Editeur::slotSortieVisionneuse()
{
    ui->stackedWidgetEditeur->setCurrentWidget(ui->pageGestionImages);
}

void Editeur::slotAfficheEtatPublication(const int &code)
{
    if(code > 0)
    {
        AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"), trUtf8("Votre module a bien été publié sur AbulÉdu-Médiathèque..."), true, this);
        msgEnregistrement->setWink();
        msgEnregistrement->show();
    }
    else
    {
        AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Problème"), trUtf8("Un problème a empêché la publication de votre module sur AbulÉdu-Médiathèque..."), true, this);
        msgEnregistrement->show();
    }
}

