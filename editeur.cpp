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


/**   @todo

    Suppression des structures de données volatiles
    Utilisation des QSettings à la sauvegarde directement.
    Enregistrement du 0 si < 10 (pour le QSetting)

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

    ui->abuleduMediathequeGet->abeHideCloseBouton(true);

    connect(ui->abuleduMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(QSharedPointer<AbulEduFileV1>, int)), this,
            SLOT(slotImportImageMediatheque(QSharedPointer<AbulEduFileV1>,int)), Qt::UniqueConnection);

    connect(ui->stPageMediathequePush, SIGNAL(signalMediathequePushFileUploaded(int)),this,
            SLOT(slotAfficheEtatPublication(int)), Qt::UniqueConnection);

    QShortcut *shortcutSupprimeChoix = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidgetImagesSelection, 0, 0, Qt::WidgetShortcut);
    connect(shortcutSupprimeChoix, SIGNAL(activated()), this, SLOT(slotSupprimerImage()), Qt::UniqueConnection);

    _OPT_nbMasquesChoisisParcours = m_opt_nbMasquesLargeur = m_opt_nbMasquesHauteur = m_numeroParcours = _nombreParcoursSave = 0;

    /* Vidange des différentes listes */
    m_listeMasques.clear();
    m_listeMasquesParcours.clear();
    _listeFichiersImages.clear();
    //    m_parametresParcours1.clear();
    //    m_parametresParcours2.clear();
    //    m_parametresParcours3.clear();
    //    m_parametresParcours4.clear();
    //    m_parametresParcours5.clear();

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

    _assistantEtapes = new AbulEduWidgetAssistantEtapesV1(listeEtapes, 12, 16, 20, this);
    _assistantEtapes->abeWidgetAssistantEnableClick(false);
    ui->hlFilAriane->addWidget(_assistantEtapes);

    connect(_assistantEtapes, SIGNAL(signalEtapeHasChanged(int)), this, SLOT(slotEditorChangePageRequested(int)), Qt::UniqueConnection);
    connect(_assistantEtapes, SIGNAL(signalQuitterRequested()), this, SLOT(slotCloseEditor()), Qt::UniqueConnection);

    /* Ceci sert à mettre la page d'accueil par dféfaut au démarrage, donc initialisation du 1er message d'aide */
    slotEditorChangePageRequested(PageEtapeAccueil);

    /* Je connecte les méthodes qui vont gérées l'intégrité des parcours */
    connect(this, SIGNAL(signalParcoursSave()), SLOT(slotParcoursSave()), Qt::UniqueConnection);

    ui->gv_editeurParcours->setScene(new QGraphicsScene(this));
}

Editeur::~Editeur()
{
    /// @todo delete les objets sans parent
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << _abuleduFile->abeFileGetFileName().baseName();
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
    case PageEtapeAccueil:
        if(m_localDebug) qDebug() << "PAGE Accueil";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeAccueil);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(false);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeParametres)->setEnabled(false);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(false);
        ui->lbAide->setText(_messageAidePageAccueil);
        break;
    case PageEtapeGestionImages:
        if(m_localDebug) qDebug() << "PAGE Images";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(true);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeAccueil)->setEnabled(false);
        ui->lbAide->setText(_messageAidePageGestionImages);
        /* Contrôle 5 images = bouton suivant ok */
        controlNumberOfImages();
        break;
    case PageEtapeParametres:
        if(m_localDebug) qDebug() << "PAGE Parametres";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeParametres);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        /* Obligé d'avoir un int pour le connect mais je ne m'en sers pas */
        slotCheckBoxParametres_clicked(0);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(true);
        ui->lbAide->setText(_messageAidePageParametres);
        break;
    case PageEtapeFin:
        if(m_localDebug) qDebug() << "PAGE Fin";
        ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeFin);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        ui->lbAide->setText(_messageAidePageFin);
        break;
    case PageParcours:
        if(m_localDebug) qDebug() << "PAGE Parcours";
        ui->lbAide->setText(_messageAidePageParcours);
        ui->stackedWidgetEditeur->setCurrentIndex(PageParcours);
        ui->gv_editeurParcours->setSceneRect(ui->gv_editeurParcours->rect());
        ui->gv_editeurParcours->scene()->setSceneRect(ui->gv_editeurParcours->rect());
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(true);
        /* Obligé d'avoir un int pour le connect mais je ne m'en sers pas */
        slotCheckBoxParametres_clicked(0);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->setEnabled(false);
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(false);
        _assistantEtapes->abeWidgetAssistantEnableClick(false);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        break;
    case PageVisio:
        if(m_localDebug) qDebug() << "PAGE Visio";
        ui->stackedWidgetEditeur->setCurrentIndex(PageVisio);
        ui->lbAide->setText(_messageAidePageVisio);
        _assistantEtapes->abeWidgetAssistantEnablePrecedent(false);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
        _assistantEtapes->abeWidgetAssistantEnableClick(false);
        break;
    default:
        break;
    }

}

void Editeur::slotCloseEditor()
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    /* Remettre le titre par defaut du boutonModifier courant */
    ui->btnModificationCourant->setText(trUtf8("Editer le module en cours"));
    /* Remettre la page d'Accueil par defaut */
    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeAccueil);
    _assistantEtapes->abeWidgetAssistantSetEtapeCourante(PageEtapeAccueil);
    /* Vider les listes */
    //    m_parametresParcours1.clear();
    //    m_parametresParcours2.clear();
    //    m_parametresParcours3.clear();
    //    m_parametresParcours4.clear();
    //    m_parametresParcours5.clear();

    /** @todo Quitter oui/non ? */
    /* Pour l'instant, en quittant on supprime tout */
    qDebug() << "NETTOYAGE TEMPORAIRE DE : " << _abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    _abuleduFile->abeCleanDirectoryRecursively(_abuleduFile->abeFileGetDirectoryTemp().absolutePath());

    /* On dit que le fichier abe n'a pas de nom maintenant (important pr la MW) */
    _abuleduFile->abeFileSetFilename("");
    _abuleduFile->abeFileSetTitle("");


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

    _messageAidePageParcours = trUtf8("Message d'aide à venir");

    _messageAidePageFin = trUtf8("Message d'aide à venir");

    _messageAidePageVisio = trUtf8("Pour revenir à l'écran précédent, cliquez sur l'image.");
}

void Editeur::controlNumberOfImages()
{
    if(_listeFichiersImages.count() >= 5)
    {
        /* Il faut activer seulement le bouton suivant */
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeParametres)->setEnabled(true);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(true);
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        /* Afin d'afficher le bouton comme il faut dans la riviere */
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeGestionImages)->click();
    }
    else
    {
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeParametres)->setEnabled(false);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
    }
}

void Editeur::creationMenu()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;

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
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    /* condition garde meme si j'appelle ce slot que si j'ai un item ds ma listView, donc une liste avec au moins 1 éléments =) */
    if (_listeFichiersImages.isEmpty()){return;}
    if (ui->listWidgetImagesSelection->selectedItems().isEmpty()){ return;}

    /* Debug avant suppression*/
    if (m_localDebug){
        for (int i = 0; i < _listeFichiersImages.count(); i++)
        {
            qDebug() << i <<" "<<_listeFichiersImages.at(i);
        }
    }

    /* Suppression des items selectionnés */
    foreach(QListWidgetItem *i, ui->listWidgetImagesSelection->selectedItems())
    {
        _listeFichiersImages.removeOne(i->data(4).toString());
        QFileInfo fi(i->data(4).toString());
        if(QFile::remove(fi.absoluteFilePath().replace(fi.suffix(), "xml").remove("/images"))){
            if(m_localDebug) qDebug() << "Suppression du fichier XML accompagnant la ressource... [OK]";
        }
        if (QFile::remove(fi.absoluteFilePath()))
        {
            if (m_localDebug) qDebug() << "Suppression de l'image du fichier temporaire... [OK]";
        }
        delete i;
    }

    /* Debug apres suppression*/
    if (m_localDebug){
        for (int i = 0; i < _listeFichiersImages.count(); i++)
        {
            if (m_localDebug) qDebug() << i <<" "<<_listeFichiersImages.at(i);
        }
    }

    /* Contrôle 5 images = bouton suivant ok */
    controlNumberOfImages();
}

void Editeur::ajouterImage(QFileInfo monFichier)
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << " Chemin : " << monFichier.absoluteFilePath() << "Nom : " << monFichier.fileName();

    /* Controle des insertions (éviter les doublons) */
    if (_listeFichiersImages.contains(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg"))
    {
        if(m_localDebug) qDebug() << "Fichier deja présent";
        return;
    }

    if(_abuleduFile->resizeImage(&monFichier, 1024, _abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/"))
    {
        /* je range le chemin de l'image dans ma liste (celui du fichier temp) */
        _listeFichiersImages << _abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg";
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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    /* Si j'ai un item à cet endroit, j'appelle mon menu */
    if (ui->listWidgetImagesSelection->itemAt(pos) != NULL)
    {
        m_menuListWidget->exec(ui->listWidgetImagesSelection->mapToGlobal(pos));
    }
}

void Editeur::on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item)
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;

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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;

    ui->listWidgetImagesSelection->clear();

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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__ <<" "<< numBtn;

    m_numeroParcours = numBtn;
    slotEditorChangePageRequested(PageParcours);

    if(remplirGvParcours(m_numeroParcours)){
        if(m_localDebug)
            qDebug() << "Remplissage... [OK], Affichage editeur de parcours";
    }
    else {
        if(m_localDebug)
            qDebug() << "Remplissage... [KO], Réinitialisation parcours";
        //        reinitialiserGvParcours();
    }

    /* Petite protection, le nb de masque choisi ne peut pas etre superieur au nb total de masque */
    ui->sbParcoursMasque->setMaximum(ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value());
}

bool Editeur::remplirGvParcours(const int &numeroParcours)
{
    if(m_localDebug) qDebug() <<__PRETTY_FUNCTION__ << " "<< numeroParcours;

    _OPT_nbMasquesChoisisParcours = ui->sbParcoursMasque->value();

    m_opt_nbMasquesLargeur = ui->spinBoxParcoursMasquesLargeur->value();
    m_opt_nbMasquesHauteur = ui->spinBoxParcoursMasqueHauteur->value();

    float largeurGv = ui->gv_editeurParcours->rect().width() -1;
    float hauteurGv = ui->gv_editeurParcours->rect().height()-1;

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

        ui->sbParcoursMasque->setValue(positionMasquesParcours.count());

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
                i ++;
            }

            QList<int> listePosition;
            QString masque;
            QRegExp masqueP("MasqueParcours");
            i = positionMasquesParcours.constBegin();
            while(i != positionMasquesParcours.constEnd())
            {
                masque = i.key();
                qDebug() << "Masque == "  << masque;
                if(masque.contains(masqueP)){
                    listePosition << masque.split(masqueP).last().toInt();
                }
                    i++;
            }

            /* Ici, j'ai tout les numéros des masques sont dans la liste, il faut effectuer un tri croissant */
            qSort(listePosition.begin(), listePosition.end(), qLess<int>());

            int pos;
            while(!listePosition.isEmpty()){
                pos = listePosition.takeFirst();
                foreach (const QString var, positionMasquesParcours.keys()) {
                    if(var.contains("MasqueParcours")){
                        if(pos == var.split("MasqueParcours").at(1).toInt()){
                            positionParcours << positionMasquesParcours.value(var);
                        }
                    }
                }
            }


            if (m_localDebug) {
                qDebug() << "La liste des positions normales : " << positionMasquesParcours;
                qDebug() << "Position Depart/Arrivee         : " << positionDepart << "/" << positionArrivee;
                qDebug() << "Position Parcours               : " << positionParcours;
            }

            /*Ici on a toutes les positions necessaires, plus qu'à les mettre dans l'ordre : depart, parcours, arrivee */
            /* depart */
            m_listeMasques.at(positionDepart)->setColor(QColor(Qt::green));
            m_listeMasques.at(positionDepart)->setProperty("Role", trUtf8("Depart"));
            m_listeMasquesParcours << m_listeMasques.at(positionDepart);

            /* parcours */
            while(!positionParcours.isEmpty())
            {
                if(AbulEduTools::masquesVoisins(m_listeMasquesParcours.last()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur).contains(positionParcours.first()))
                    qDebug() << "Voisin ok ";
                else
                    qDebug() << "PROBLEME VOISINAGE";
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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;

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
        else if(m_listeMasquesParcours.count() < (_OPT_nbMasquesChoisisParcours - 1))
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
        else if ( m_listeMasquesParcours.count() == (_OPT_nbMasquesChoisisParcours-1))
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
    if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__ ;

    /* Si n'importe lequel des exercices est checké sauf parcours == on dit oui */
    if((ui->groupBoxSurvol->isChecked() || ui->groupBoxClic->isChecked() || ui->groupBoxDoubleClic->isChecked())
            && !ui->groupBoxParcours->isChecked())
    {
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(true);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(true);
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        return true;
    }
    else if(ui->groupBoxParcours->isChecked()){
        if(chargerPositionMasque(1)
                && chargerPositionMasque(2)
                && chargerPositionMasque(3)
                && chargerPositionMasque(4)
                && chargerPositionMasque(5))
        {
            if(m_localDebug) qDebug() << "Réglages parcours... [OK]";
            _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(true);
            _assistantEtapes->abeWidgetAssistantEnableSuivant(true);
            _assistantEtapes->abeWidgetAssistantEnableClick(true);
            return true;
        }
        else{
            if(m_localDebug) qDebug() << "Réglages parcours... [KO]";
            return false;
        }
    }
    _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(false);
    _assistantEtapes->abeWidgetAssistantEnableSuivant(false);
    _assistantEtapes->abeWidgetAssistantEnableClick(false);
    return false;
}

bool Editeur::slotParcoursSave()
{
    /*  Il faut que _nombreParcoursSave soit égale à 5 pour que les données soient viables.
      * Permet également d'activé l'écran de sauvegarde. */
    qDebug() << __PRETTY_FUNCTION__;
    _nombreParcoursSave ++;
    if(_nombreParcoursSave == 5)
    {
        if(m_localDebug) qDebug() << "On peut activer la page de Fin";
        _assistantEtapes->abeWidgetAssistantGetButton(PageEtapeFin)->setEnabled(true);
        _assistantEtapes->abeWidgetAssistantEnableSuivant(true);
        _assistantEtapes->abeWidgetAssistantEnableClick(true);
        return true;
    }
    return false;
}

void Editeur::slotSetNombreMasquesParcours(int value)
{
    _OPT_nbMasquesChoisisParcours = value;
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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__ << " " << numeroParcours;

    positionMasquesParcours.clear();

    qDebug() << "Parametres charges depuis : " << _abuleduFile->abeFileGetDirectoryTemp().absolutePath();

    qDebug() << "Existence Fichier Conf :: " << QFile::exists(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf");

    QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    qDebug() << "CHARGEMENT : " << parametres.childKeys() ;

    parametres.beginGroup("parcours");
    parametres.beginGroup("parcours"+QString::number(numeroParcours));
    for (int i =0 ; i < parametres.childKeys().count(); i++)
    {
        positionMasquesParcours.insert(parametres.childKeys().at(i),parametres.value(parametres.childKeys().at(i)).toInt());
    }
    parametres.endGroup();
    qDebug() << "NOUS AVONS : " << positionMasquesParcours.count() << " MASQUES !";

    if(!positionMasquesParcours.isEmpty())
        return true;
    else
    {
        qDebug() << __FUNCTION__ << " Aucune entree dans le QSettings. ";
        return false;
    }

}

/* Gestion reinitialisation parcours */
void Editeur::on_btnResetParcours_clicked()
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;

    ui->sbParcoursMasque->setEnabled(true);

    /* Remettre tout mes masques d'origine ! */
    foreach (MasqueDeplaceSouris* var, m_listeMasques) {
        var->setColor(QColor(Qt::white));
        var->setProperty("Role", trUtf8("Fixe"));
        var->update();
    }

    /* Vider ma listeMasquesParcours */
    m_listeMasquesParcours.clear();

    /* Supprimer les entrées correspondantes dans le QSettings */
    if(m_localDebug) qDebug() << "Suppression Entrées dans le QSettings";
    switch(m_numeroParcours){
    case 1:
        QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
        parametres.beginGroup("parcours");
        parametres.beginGroup("parcours"+QString::number(m_numeroParcours));

        if(parametres.childKeys().count() > 0){ /* Si on a des entrées relatives à ce parcours, on les supprime */
            parametres.remove("");
            parametres.endGroup();
            parametres.sync();
            if(m_localDebug) qDebug() << "Parcours num : " <<m_numeroParcours<< " -> efface du conf";
        }
        /* bouton parcours correspondant en rouge */
        ui->btnParcours1->setStyleSheet("color : red;");

        break;

    }

    //    ui->gv_editeurParcours->update();

    //    /* Supprimer le parcours dans le .conf si on est en mode modification */
    //    if(m_modeModificationAbe){
    //        if(m_localDebug) qDebug() << "Nous sommes en modification, on recherche les entrées dans le QSettings";

    //        QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    //        parametres.beginGroup("parcours");
    //        parametres.beginGroup("parcours"+QString::number(m_numeroParcours));

    //        if(parametres.childKeys().count() > 0){ /* Si on a des entrées relatives à ce parcours, on les supprime */
    //            parametres.remove("");
    //            parametres.endGroup();
    //            parametres.sync();
    //            if(m_localDebug) qDebug() << "Parcours num : " <<m_numeroParcours<< " -> efface du conf";
    //        }

    //        /* Le bouton relatif au parcours ne doit plus etre vert*/
    //        switch(m_numeroParcours){
    //        case 1:
    //            ui->btnParcours1->setStyleSheet("color : red;");
    //            break;
    //        case 2:
    //            ui->btnParcours2->setStyleSheet("color : red;");
    //            break;
    //        case 3:
    //            ui->btnParcours3->setStyleSheet("color : red;");
    //            break;
    //        case 4:
    //            ui->btnParcours4->setStyleSheet("color : red;");
    //            break;
    //        case 5:
    //            ui->btnParcours5->setStyleSheet("color : red;");
    //            break;
    //        }
    //    }
}

/* Gestion sauvegarde parcours */
void Editeur::on_btnSaveParcours_clicked()
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__ << " parcours :" << m_numeroParcours;

    QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/parametres.conf", QSettings::IniFormat);
//    qDebug() << "Parametres enregistres dans : " << _abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    switch (m_numeroParcours)
    {
    case 1:
        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
        parametres.beginGroup("parcours");
        parametres.setValue("parcours1/MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        parametres.setValue("parcours1/MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            parametres.setValue("parcours1/MasqueParcours"+QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours1->setStyleSheet("color : green;");
//        qDebug() << "SAUVEGARDE : " << parametres.childKeys();

        parametres.endGroup();
        parametres.sync();
        break;
    }

    //    case 2:
    //        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
    //        m_parametresParcours2.clear();
    //        m_parametresParcours2.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
    //        m_parametresParcours2.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
    //        /* Il reste que des masques "Parcours" dans la liste */
    //        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
    //        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
    //        {
    //            m_parametresParcours2.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
    //        }
    //        ui->btnParcours2->setStyleSheet("color : green;");
    //        break;

    //    case 3:
    //        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
    //        m_parametresParcours3.clear();
    //        m_parametresParcours3.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
    //        m_parametresParcours3.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
    //        /* Il reste que des masques "Parcours" dans la liste */
    //        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
    //        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
    //        {
    //            m_parametresParcours3.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
    //        }
    //        ui->btnParcours3->setStyleSheet("color : green;");
    //        break;

    //    case 4:
    //        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
    //        m_parametresParcours4.clear();
    //        m_parametresParcours4.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
    //        m_parametresParcours4.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
    //        /* Il reste que des masques "Parcours" dans la liste */
    //        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
    //        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
    //        {
    //            m_parametresParcours4.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
    //        }
    //        ui->btnParcours4->setStyleSheet("color : green;");
    //        break;

    //    case 5:
    //        /* Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste */
    //        m_parametresParcours5.clear();
    //        m_parametresParcours5.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
    //        m_parametresParcours5.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
    //        /* Il reste que des masques "Parcours" dans la liste */
    //        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
    //        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
    //        {
    //            m_parametresParcours5.insert("MasqueParcours"+ QString::number(i), m_listeMasquesParcours.at(i)->getNumero());
    //        }
    //        ui->btnParcours5->setStyleSheet("color : green;");
    //        break;
    //    default:
    //        return;
    //    }

    /***************************
            Commun apres sauvegarde
      **************************/
    m_listeMasquesFixes.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->gv_editeurParcours->scene()->clear();

    /* Petit message */
    AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"));
    alertBox->setWindowModality(Qt::ApplicationModal);
    alertBox->setWink();
    alertBox->show();
    /* Emission du signal pour gestion d'intégrité (et de navigation)*/
    emit signalParcoursSave();

    /* Nettoyage scene */
    ui->gv_editeurParcours->scene()->clear();
    ui->sbParcoursMasque->setEnabled(true);

    /* Revenir sur l'écran des parametres */
    slotEditorChangePageRequested(PageEtapeParametres);
}

void Editeur::on_sbParcoursMasque_valueChanged(int val)
{
    /** Gestion changement de valeur du nombre de masques */
    _OPT_nbMasquesChoisisParcours = val;
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
    _listeFichiersImages.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->listWidgetImagesSelection->clear();

    /* Style des Boutons */
    ui->btnParcours1->setStyleSheet("color : gray;");
    ui->btnParcours2->setStyleSheet("color : gray;");
    ui->btnParcours3->setStyleSheet("color : gray;");
    ui->btnParcours4->setStyleSheet("color : gray;");
    ui->btnParcours5->setStyleSheet("color : gray;");

    /* On dit que le fichier abe n'a pas de nom maintenant (important pr la MW) */
    _abuleduFile->abeFileSetFilename("");
    _abuleduFile->abeFileSetTitle("");

    /* Et on lance un signal qui changera le titre de la mainwindows */
    emit editorNewAbe(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

    setModeModificationAbe(false);

    /* Si modification abe disponible */
    qDebug() << "BTN MODIF EN COURS REINIT";
    ui->btnModificationCourant->setText(trUtf8("Editer le module en cours"));
    ui->btnModificationCourant->setEnabled(false);

    /* Passer à la fenetre suivante */
    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
    _assistantEtapes->abeWidgetAssistantSetEtapeCourante(PageEtapeGestionImages);
}

/** Bouton modification courant ABE */
void Editeur::on_btnModificationCourant_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    setModeModificationAbe(true);
    slotEditorChangePageRequested(PageEtapeGestionImages);
    slotLoadUnit();
}

/** Bouton modification autre (ouverture box) */
void Editeur::on_btnModificationAutre_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    emit editorChooseOrSave(AbulEduBoxFileManagerV1::abeOpen);
    setModeModificationAbe(true);
}

void Editeur::slotLoadUnit()
{
    if (m_localDebug) qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__<<" :: "<<_abuleduFile->abeFileGetFileName().fileName();
    _listeFichiersImages.clear();
    ui->listWidgetImagesSelection->clear();
    QDir folder(QString(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/images"));
    folder.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(const QFileInfo fileInfo, folder.entryInfoList())
    {
        ajouterImage(fileInfo.absoluteFilePath());
        m_lastOpenDir = fileInfo.absolutePath();
    }

    QSettings parametres(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/conf/parametres.conf", QSettings::IniFormat);
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
    if(parametres.value("exerciceActive",true).toBool())
    {
        ui->groupBoxParcours->setChecked(parametres.value("exerciceActive",true).toBool());
        ui->spinBoxParcoursSuivant->setValue(parametres.value("timerSuivant",7).toInt());
        ui->spinBoxParcoursMasquesLargeur->setValue(parametres.value("nbMasquesLargeur",7).toInt());
        ui->spinBoxParcoursMasqueHauteur->setValue(parametres.value("nbMasquesHauteur",7).toInt());

        /* Recupération nb masque pour remplissage ComboBox */
        parametres.beginGroup("parcours1");
        /** signal parcours save */
        if(chargerPositionMasque(1)){
            if(m_localDebug) qDebug() << "Le parcours 1...[OK]";
            ui->btnParcours1->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            if(m_localDebug) qDebug() << "Le parcours 1...[KO]";
            ui->btnParcours1->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        parametres.beginGroup("parcours2");
        /** Controle position + Bouton Vert + signal parcours save */
        if(chargerPositionMasque(2)){
            if(m_localDebug) qDebug() << "Le parcours 2...[OK]";
            ui->btnParcours2->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            if(m_localDebug) qDebug() << "Le parcours 2...[KO]";
            ui->btnParcours2->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        parametres.beginGroup("parcours3");
        /** Controle position + Bouton Vert + signal parcours save */
        if(chargerPositionMasque(3)){
            if(m_localDebug)  qDebug() << "Le parcours 3...[OK]";
            ui->btnParcours3->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            if(m_localDebug) qDebug() << "Le parcours 3...[KO]";
            ui->btnParcours3->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        parametres.beginGroup("parcours4");
        /** Controle position + Bouton Vert + signal parcours save */
        if(chargerPositionMasque(4)){
            if(m_localDebug) qDebug() << "Le parcours 4...[OK]";
            ui->btnParcours4->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            if(m_localDebug) qDebug() << "Le parcours 4...[KO]";
            ui->btnParcours4->setStyleSheet("color : red;");
        }
        parametres.endGroup();
        parametres.beginGroup("parcours5");
        /** Controle position + Bouton Vert + signal parcours save */
        if(chargerPositionMasque(5)){
            if(m_localDebug) qDebug() << "Le parcours 5...[OK]";
            ui->btnParcours5->setStyleSheet("color : green;");
            emit signalParcoursSave();
        }
        else{
            if(m_localDebug) qDebug() << "Le parcours 5...[KO]";
            ui->btnParcours5->setStyleSheet("color : red;");
        }
        parametres.endGroup();
    }

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

    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
}

/**********************************************************************************************************************************************************
                                                     GESTION DRAG & DROP
**********************************************************************************************************************************************************/

void Editeur::dropEvent(QDropEvent *event)
{
    if(m_localDebug){
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() <<  event->source() << " " << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());
    }
    if(event->source()->objectName() == "treeViewArborescence" && ui->listWidgetImagesSelection->geometry().contains(event->pos())){
        if(m_localDebug) qDebug() << "SOURCE == treeViewArbo";
        /* Controle que c'est bien une image */
        if(event->mimeData()->hasImage()){
            if (m_localDebug) qDebug() << "C'est une image";
        }
    }
    else if(event->source()->objectName() == "lwSimple" && ui->listWidgetImagesSelection->geometry().contains(event->pos())){
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
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;

    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Choisir une image"), m_lastOpenDir, trUtf8("Images (*.png *.jpg *.jpeg *.svg *.bmp *.ppm *.xpm *.xbm)"));
    QFileInfo fi(fileName);
    if(fi.exists()) {
        ajouterImage(fi.absoluteFilePath());
        m_lastOpenDir = fi.absolutePath();
    }
}

bool Editeur::preparerSauvegarde()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;

    if(ui->leTitre->text().trimmed().isEmpty() || ui->leAuteur->text().trimmed().isEmpty())
    {
        if(ui->leTitre->text().trimmed().isEmpty()){
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas de titre !"),trUtf8("Vous n'avez pas renseigné le champ titre !"));
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
            _abuleduFile->abeFileSetTitle(ui->lblTitreModule->text());
        }
        if(ui->leAuteur->text().trimmed().isEmpty())
        {
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas d'auteur !"),trUtf8("Vous n'avez pas renseigné le champ auteur !"));
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

//    QString destTemp = _abuleduFile->abeFileGetDirectoryTemp().absolutePath();

//    QDir().mkpath(destTemp + "/data/images");
//    QDir().mkpath(destTemp + "/conf");

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
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Parametres Clic */
    parametres.beginGroup("clic");
    if (ui->groupBoxClic->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxSurvolSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    }
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Parametres Double-Clic */
    parametres.beginGroup("doubleClic");
    if (ui->groupBoxClic->isChecked())
    {
        parametres.setValue("exerciceActive",true);
        parametres.setValue("timerSuivant", (ui->spinBoxSurvolSuivant->value()));
        parametres.setValue("nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
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
        //        if(!m_parametresParcours1.isEmpty())
        //        {
        //            QMapIterator<QString, QVariant> i(m_parametresParcours1);
        //            while (i.hasNext()) {
        //                i.next();
        //                parametres.setValue("parcours1/"+ i.key(), i.value());
        //            }
        //        }
        //        if(!m_parametresParcours2.isEmpty())
        //        {
        //            QMapIterator<QString, QVariant> i(m_parametresParcours2);
        //            while (i.hasNext()) {
        //                i.next();
        //                parametres.setValue("parcours2/"+ i.key(), i.value());
        //            }
        //        }
        //        if(!m_parametresParcours3.isEmpty())
        //        {
        //            QMapIterator<QString, QVariant> i(m_parametresParcours3);
        //            while (i.hasNext()) {
        //                i.next();
        //                parametres.setValue("parcours3/"+ i.key(), i.value());
        //            }
        //        }
        //        if(!m_parametresParcours4.isEmpty())
        //        {
        //            QMapIterator<QString, QVariant> i(m_parametresParcours4);
        //            while (i.hasNext()) {
        //                i.next();
        //                parametres.setValue("parcours4/"+ i.key(), i.value());
        //            }
        //        }
        //        if(!m_parametresParcours5.isEmpty())
        //        {
        //            QMapIterator<QString, QVariant> i(m_parametresParcours5);
        //            while (i.hasNext()) {
        //                i.next();
        //                parametres.setValue("parcours5/"+ i.key(), i.value());
        //            }
        //        }
    }
    else{parametres.setValue("exerciceActive",false);}
    parametres.endGroup();

    /* Creation .abe */
    parametres.sync(); //pour forcer l'écriture du .conf

/** @todo return booleen abuledufile */
//    _abuleduFile->abeFileExportPrepare(AbulEduTools::parcoursRecursif(_abuleduFile->abeFileGetDirectoryTemp().absolutePath()), _abuleduFile->abeFileGetDirectoryTemp().absolutePath(), "abe");
    return true;
}

void Editeur::releaseAbe()
{
    if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    ui->listWidgetImagesSelection->clear();
    if (preparerSauvegarde())
    {
        ui->stPageMediathequePush->abeSetFile(_abuleduFile);
        ui->stackedWidgetEditeur->setCurrentWidget(ui->stPageMediathequePush);

    }
}

void Editeur::on_btnEssayer_clicked()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;

    if(preparerSauvegarde()){
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Passage en mode essai..."),
                                                              trUtf8("Votre module n'est pas enregistré. Si les paramètres vous conviennent, revenez dans l'éditeur pour enregistrer ou publier."));
        alertBox->setWindowModality(Qt::ApplicationModal);
        alertBox->show();
        emit editorTest();
    }
    else
        qDebug() << "TEST Impossible ! ";
}

void Editeur::on_btnEnregistrer_clicked()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    createAbe();
}

void Editeur::on_btnPublier_clicked()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    releaseAbe();
}

void Editeur::slotSortieVisionneuse()
{
    ui->stackedWidgetEditeur->setCurrentIndex(PageEtapeGestionImages);
    /* Activation de la naviguation en fonction */
    controlNumberOfImages();
}

void Editeur::slotAfficheEtatPublication(const int &code)
{
    if(code > 0)
    {
        AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"),
                                                                         trUtf8("Votre module a bien été publié sur AbulÉdu-Médiathèque..."));
        msgEnregistrement->setWindowModality(Qt::ApplicationModal);
        msgEnregistrement->setWink();
        msgEnregistrement->show();
    }
    else
    {
        AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Problème"),
                                                                         trUtf8("Un problème a empêché la publication de votre module sur AbulÉdu-Médiathèque..."));
        msgEnregistrement->setWindowModality(Qt::ApplicationModal);
        msgEnregistrement->show();
    }
}

