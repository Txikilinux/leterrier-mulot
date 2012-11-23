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

#include "editeur.h"


/// **********************************************************************************************************************************************************
///                                             EDITEUR
///
/// **********************************************************************************************************************************************************


Editeur::Editeur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    m_parent = (MainWindow*) parent;
    m_lastOpenDir = QDir::homePath();
//    m_threadRechercheImages = threadRechercheImage;
//    connect(m_threadRechercheImages, SIGNAL(finished()), this, SLOT(testThread())); // OK ça Fonctionne !!
//    connect(m_threadRechercheImages, SIGNAL(signalFichierTrouve(QString, QString)), this, SLOT(slotTestImportImage(QString, QString)));
//    // Cela fonctionne, mais si l'éditeur est appelé et que le thread est fini, on a pas de signal =)


//    if(m_threadRechercheImages->isRechercheTerminee())
//    {
//        qDebug() << "RECHERCHE TERMINE";
//        testThread();
//    }

    ui->abuleduMediathequeGet->abeSetSource("data");
    ui->abuleduMediathequeGet->abeSetCustomBouton1(trUtf8("Importer l'image"));
    ui->abuleduMediathequeGet->abeHideBoutonTelecharger();
    ui->abuleduMediathequeGet->abeCustomBouton1SetDownload(true);


    connect(ui->abuleduMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(int)), this, SLOT(slotImportImageMediatheque()));

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);

    m_localDebug = true;

    m_opt_nbMasquesChoisisParcours = 0;
    m_opt_nbMasquesLargeur = 0;
    m_opt_nbMasquesHauteur = 0;
    m_numeroParcours = 0;

    m_listeMasques.clear();
    m_listeMasquesParcours.clear();
    m_parametresParcours1.clear();
    m_parametresParcours2.clear();
    m_parametresParcours3.clear();
    m_parametresParcours4.clear();
    m_parametresParcours5.clear();
    m_listeFichiersImages.clear();

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(majBarreNavigation(int)));

    ui->stackedWidget->setCurrentIndex(0);
    majBarreNavigation(0);
    remplirArborescence();

    //    ui->listWidgetImagesSelection->installEventFilter(this);
    setAcceptDrops(true);

    //    ui->abuleduMediathequeGet->abeHideBoutonTelecharger();
    ui->abuleduMediathequeGet->abeHideInfoPanel(true);
    ui->abuleduMediathequeGet->abeSetDefaultView(AbulEduMediathequeGetV1::abeMediathequeThumbnails);

    creationMenu();

    // Affichage de la mediatheque par defaut
    ui->tabWidgetImages->setCurrentWidget(ui->pageMediatheque);
}

void Editeur::initCheminTemp()
{
    // Initialisation des chemins temporaires Mode Création
    m_abuledufilev1 = new AbulEduFileV1();
    m_destinationIdUnique = m_abuledufilev1->abeFileGetDirectoryTemp().absolutePath(); //je récupère mon Id unique
    qDebug() << m_destinationIdUnique;
    m_cheminImage = m_destinationIdUnique + "/data/images";
    m_cheminConf = m_destinationIdUnique + "/conf";

    qDebug() << "Dossier Temporaire de l'Editeur (doit etre celui de la MWindow" << m_abuledufilev1->abeFileGetDirectoryTemp().absolutePath();

    m_destImage = new QDir(m_cheminImage); // creation dossier temporaire pour les images
    if(m_destImage->mkpath(m_cheminImage)) // tentative de création du fichier temp avec un id unique + sous dossier au nom du theme
    {
        if (m_localDebug)
        {
            qDebug() << "Creation ok : " << m_destImage->absolutePath();
        }
        else { return; } // si echec pas la peine d'aller plus loin
    }
}

Editeur::~Editeur()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::~Editeur()";
    delete ui;
}


void Editeur::remplirArborescence()
{

    /// Ce qui marche est commenté
//        QFileSystemModel *model = new QFileSystemModel;
//        model->setRootPath(QDir::homePath());


//        QStringList filters;
//        QDir dir;
//        filters << "*.jpg" << "*.bmp"<< "*.png" << "*.svg"; //Choix des extensions
//        dir.setNameFilters(filters);

//        ui->treeViewArborescence->setModel(model);
//        model->setNameFilters(filters); //Filtrage des photos

//        connect(ui->treeViewArborescence, SIGNAL(clicked(const QModelIndex&)),this, SLOT(slotResizeColumn(const QModelIndex&)));
//        //    connect(ui->treeViewArborescence, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotMenuContextuel(const QPoint &)));

//        ui->treeViewArborescence->setHeaderHidden(true);
//        ui->treeViewArborescence->hideColumn(1);
//        ui->treeViewArborescence->hideColumn(2);
//        ui->treeViewArborescence->hideColumn(3);

//        ui->treeViewArborescence->setUniformRowHeights(true); // met toutes les lignes à la meme taille, ce qui permet d'optimiser le temps de réponse (ne recalcule pas la taille de chaque ligne)
//        ui->treeViewArborescence->setAnimated(true);          // a mettre a false pour les petites configurations (hein JLF !)
//        ui->treeViewArborescence->setSortingEnabled(true);
//        ui->listWidgetImagesSelection->setIconSize(QSize(50, 50));

//        creationMenu();

    /// Tentative d'une nouvelle facon

//        ThreadRechercheImage *toto;
//        toto = new ThreadRechercheImage();
//        toto->run(ui->listWidgetImagesSelection);
//        QStringList listeFichiersThread;

//        if (toto->getListeFichiers().count() < 0)
//        {
//            qDebug() << "OK pour la liste";
//        }

}

void Editeur::creationMenu()
{
    /// COMMUN
    QStyle* style =  QApplication::style(); // récupération du style systeme
    /// MENU LISTWIDGET (Supprimer)
    m_menuListWidget = new QMenu(ui->listWidgetImagesSelection);
    QIcon iconSupprimer = style->standardIcon(QStyle::SP_DialogResetButton); //On récupère l'icône désiré
    QAction *a_supprimer = new QAction(trUtf8("&Supprimer de la selection"),m_menuListWidget);
    a_supprimer->setIcon(iconSupprimer);
    a_supprimer->setIconVisibleInMenu(true);
    a_supprimer->connect(a_supprimer, SIGNAL(triggered()), this, SLOT(slotSupprimerImage()));
    m_menuListWidget->addAction(a_supprimer);
}

/** Ce slot redimensionne la colonne au contenu de celle-ci
  * Il est connecté au slot clicked() du treeView -> donc appelé à chaque fois qu'on descend dans l'arborescence
  */
//void Editeur::slotResizeColumn(const QModelIndex& index)
//{
//    ui->treeViewArborescence->resizeColumnToContents(index.column());
//    ui->treeViewArborescence->model()->sort(0, Qt::AscendingOrder);
//}

void Editeur::slotSupprimerImage()// Test OK
{
    if (m_listeFichiersImages.isEmpty()) // condition garde meme si j'appelle ce slot que si j'ai un item ds ma listView, donc une liste avec au moins 1 éléments =)
    {return;}

    if (ui->listWidgetImagesSelection->selectedItems().isEmpty()) // Garde
    { return;}

    qDebug() << "Suppression ItemImage -> liste images avant : ";
    for (int i = 0; i < m_listeFichiersImages.count(); i++)
    {
        qDebug() << i <<" "<<m_listeFichiersImages.at(i);
    }

    for (int i = 0; i < ui->listWidgetImagesSelection->selectedItems().count(); i++)// Suppression de ma liste d'images
    {
        m_listeFichiersImages.removeOne(ui->listWidgetImagesSelection->selectedItems().at(i)->data(4).toString());
        // suppression du fichier image dans temp
        if (QFile::remove(ui->listWidgetImagesSelection->selectedItems().at(i)->data(4).toString()))
            qDebug() << "suppression image du fichier temp ok";
    }

    foreach(QListWidgetItem *i, ui->listWidgetImagesSelection->selectedItems())// Suppression des items selectionné
    {
        delete i;
    }

    qDebug() << "Suppression ItemImage -> liste images apres : ";
    for (int i = 0; i < m_listeFichiersImages.count(); i++)
    {
        qDebug() << i <<" "<<m_listeFichiersImages.at(i);
    }
}

void Editeur::ajouterImage(QFileInfo monFichier) // pour les fichiers provenant de mediathequeGet
{
    qDebug() << "ajouterImage -> Chemin du fichier selectionné : " << monFichier.absoluteFilePath() <<" Nom du fichier : " << monFichier.fileName();

    if (m_listeFichiersImages.contains(monFichier.fileName())) // Controle des insertions (éviter les doublons)
    {
        qDebug() << "Fichier deja présent";
    }
    else
    {
        if(copierImageDansTemp(monFichier, m_parent->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath())) // Si cette fonction a fonctionnée
        {
            m_listeFichiersImages << m_parent->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath() + QDir::separator() + monFichier.fileName(); // je range le chemin de l'image dans ma liste (celui du fichier temp)
            // Insertion dans mon listWidget
            QListWidgetItem *item = new QListWidgetItem();
            QIcon icone(m_parent->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath() + QDir::separator() + monFichier.fileName());//pour la mettre  à coté de l'item
            item->setIcon(icone); // ajout de la petite icone sur l'item
            item->setText(monFichier.fileName());
            item->setData(4, m_parent->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath() + QDir::separator() + monFichier.fileName());
            ui->listWidgetImagesSelection->insertItem(0, item);
        }
    }
}

void Editeur::slotImportImageMediatheque(){
    ajouterImage(ui->abuleduMediathequeGet->abeGetFile()->abeFileGetContent(0));
}

bool Editeur::copierImageDansTemp(QFileInfo cheminOriginal, QString dossierDestination)
{
    if (m_localDebug) // Affichage chemin originale & destination des images
    {
        qDebug() << "Chemin de l'image a copier      " << cheminOriginal.absoluteFilePath();
        qDebug() << "Chemin ou l'image va etre copiee" << dossierDestination + QDir::separator() + cheminOriginal.fileName();
    }
    if(QFile::copy(cheminOriginal.absoluteFilePath(), dossierDestination + QDir::separator() + cheminOriginal.fileName()))
    {
        if (m_localDebug) qDebug() << "Copie image ok";
        return true;
    }
    else // Si la copie échoue, pas la peine d'aller plus loin
    {
        if (m_localDebug) qDebug() << "Copie impossible";
        return false;
    }
    return false;
}

void Editeur::on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos)
{
    if (ui->listWidgetImagesSelection->itemAt(pos) != NULL) // j'ai un item à cet endroit, j'appelle mon menu
    {
        m_menuListWidget->exec(ui->listWidgetImagesSelection->mapToGlobal(pos));
    }
    else // sinon je fais rien
    {
        qDebug() << "Pas d'item";
    }
}

void Editeur::on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item){
    if (m_localDebug) qDebug() << "##########################  Editeur::on_listWidget_itemDoubleClicked(QListWidgetItem *item)";

    item = ui->listWidgetImagesSelection->currentItem();
    m_visionneuseImage = new VisionneuseImage(this);
    m_visionneuseImage->ouvrirFicher(item->data(4).toString());
    m_visionneuseImage->setWindowModality(Qt::WindowModal);
#ifdef __ABULEDUTABLETTEV1__MODE__
    m_visionneuseImage->showFullScreen();
#else
    m_visionneuseImage->show();
#endif
}

/**
  * Creation du .abe
  */
void Editeur::createAbe()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btnCreationtheme_clicked()";

    ui->listWidgetImagesSelection->clear();

    /// Creation fichier Conf (note les timers sont convertis en millisecondes)
    QSettings parametres(m_cheminConf+"/parametres.conf", QSettings::IniFormat);
    parametres.setValue("version",abeApp->applicationVersion());
    /// Parametres Survol
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
    /// Parametres Clic
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
    /// Parametres Double-Clic
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
    /// Paramètres Parcours
    parametres.beginGroup("parcours");
        if (ui->groupBoxParcours->isChecked())
        {
            parametres.setValue("exerciceActive",true);
            parametres.setValue("timerSuivant", (ui->spinBoxParcoursSuivant->value()));
            parametres.setValue("nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
            parametres.setValue("nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
            parametres.setValue("nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
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

    /// Creation .abe
    parametres.sync(); //pour forcer l'écriture du .conf

    if (m_localDebug) qDebug() << m_destinationIdUnique<< " " << parametres.fileName();
    if (m_localDebug) qDebug() << parcoursRecursif(m_destinationIdUnique);

//    QString nomtheme = ui->lineEditNomTheme->text();
//    QDir temp(m_destinationIdUnique); // récupération du chemin du fichier temp
//    QString m_fileBase = temp.absolutePath();

//    QString destination = m_dirAbe->absolutePath() + QDir::separator() + nomtheme ;
    m_abuledufilev1->abeFileExportPrepare(parcoursRecursif(m_destinationIdUnique), m_destinationIdUnique, "abe");

    AbulEduBoxFileManagerV1 *SaveAbuleduFileManager = new AbulEduBoxFileManagerV1(0,m_abuledufilev1,AbulEduBoxFileManagerV1::abeSave);
    SaveAbuleduFileManager->abeSetFile(m_abuledufilev1);
    SaveAbuleduFileManager->show();


    if (m_localDebug) qDebug() << "Création abe OK";

    /// Supprimer le dossier temporaire
//    if(supprimerDir(temp.absolutePath())) qDebug() << "Effacement dossier temp ok";
//    else qDebug() << "Suppression impossible";

    /// Arrangement graphique
//    ui->lineEditNomTheme->clear();
    //    ui->listWidget->clear();
    //    ui->listWidgetSelection->clear();
    //    ui->treeWidget->clear();
//    QMessageBox::information(this, trUtf8("Sauvegarder Thème"), trUtf8("Le theme a été sauvegardé avec succès"));
}

/** Supprime un répertoire et tout son contenu
  * Le répertoire passé en paramètre est aussi supprimé
  * @param const QString& dirPath, le chemin du répertoire à supprimer (ex : "/home/user/monRepertoire")
  * @return bool, true si suppression ok, false sinon
  */
bool Editeur::supprimerDir(const QString& dirPath)
{
    QDir folder(dirPath);
    //On va lister dans ce répertoire tous les éléments différents de "." et ".."
    //(désignant respectivement le répertoire en cours et le répertoire parent)
    folder.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    foreach(QFileInfo fileInfo, folder.entryInfoList())
    {
        //Si l'élément est un répertoire, on applique la méthode courante à ce répertoire, c'est un appel récursif
        if(fileInfo.isDir())
        {
            if(!supprimerDir(fileInfo.filePath())) //Si une erreur survient, on retourne false
                return false;
        }
        //Si l'élément est un fichier, on le supprime
        else if(fileInfo.isFile())
        {
            if(!QFile::remove(fileInfo.filePath()))
            {
                //Si une erreur survient, on retourne false
                return false;
            }
        }
    }
    //Le dossier est maintenant vide, on le supprime
    if(!folder.rmdir(dirPath))
    {
        //Si une erreur survient, on retourne false
        return false;
    }
    //Sinon, on retourne true
    return true;
}

QStringList Editeur::parcoursRecursif(QString dossier)
{
    QStringList resultat;
    QDir dir(dossier);
    //Attention a ne pas prendre le repertoire "." et ".."
    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        if(fileInfo.isDir()) {
            //C'est ici que le parcours est récursif
            resultat << parcoursRecursif(fileInfo.absoluteFilePath());
        }
        else {
            resultat << fileInfo.absoluteFilePath();
        }
    }
    return resultat;
}


/// **********************************************************************************************************************************************************
///                             PARCOURS
/// **********************************************************************************************************************************************************

void Editeur::remplirGvParcours()
{
    m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque->value();
    m_opt_nbMasquesLargeur = ui->spinBoxParcoursMasquesLargeur->value();
    m_opt_nbMasquesHauteur = ui->spinBoxParcoursMasqueHauteur->value();

    float largeurMasque = 0.00;
    float hauteurMasque = 0.00;

    float largeurGv = static_cast<float>(gv_AireParcours->width())-1;
    float hauteurGv = static_cast<float>(gv_AireParcours->height())-1;

    largeurMasque = largeurGv / m_opt_nbMasquesLargeur;
    hauteurMasque = hauteurGv / m_opt_nbMasquesHauteur;

    int nbMasques = m_opt_nbMasquesLargeur * m_opt_nbMasquesHauteur;
    qreal xMasque = 0.00;
    qreal yMasque = 0.00;

    if (m_localDebug)
        qDebug()<<" -------------------------- Début boucle d'affichage : "<<nbMasques;

    int numeroMasque = 0;
    for (float i=0; i<m_opt_nbMasquesHauteur;i++)
    {
        for (int j =0; j < m_opt_nbMasquesLargeur;j++)
        {
            m_masque = new masqueDeplaceSouris(0, numeroMasque);
            m_masque->setSize(largeurMasque, hauteurMasque);
            m_masque->setPos(xMasque, yMasque);
            m_masque->setColor(QColor::fromRgb(255,255,255));
            m_masque->setHideOnMouseOver(false);
            m_masque->setIsEditable(true);
            m_masque->setProperty("Role", trUtf8("Fixe"));

            connect(m_masque, SIGNAL(signalReinitialisationMasque()), this, SLOT(reinitialiserGvParcours()));                       // Reinitialisation
            connect(m_masque, SIGNAL(signalSauvegarderParcours()), this, SLOT(sauvegarderParcours()));                              // Sauvegarde du parcours
            connect(m_masque, SIGNAL(signalPoseSurParcours(masqueDeplaceSouris*)), this, SLOT(masquePoseParcours(masqueDeplaceSouris*)));

            xMasque+=largeurMasque;

            m_masque->setMenuSauvegarderEnabled(false);
            gv_AireParcours->scene()->addItem(m_masque);
            m_listeMasques << m_masque;
            numeroMasque++;
        }
        xMasque = 0;
        yMasque += hauteurMasque;
    }
}

void Editeur::masquePoseParcours(masqueDeplaceSouris* masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masquePoseParcours(masqueDeplaceSouris* masque)";

    QList<int> listeVoisins;

    /// Si mon masque n'a pas de role (= Fixe) = création de masque
    if (masque->property("Role") == "Fixe")
    {
        /// Liste Vide = Depart
        if (m_listeMasquesParcours.isEmpty())
        {
            masque->setColor(QColor(Qt::green));
            masque->setProperty("Role", trUtf8("Depart"));
            masque->update();
            m_listeMasquesParcours << masque;

            // ...& je coloris en gris les voisins
            listeVoisins = masquesVoisins(masque->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
            foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
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
        /// Liste avec un masque = Parcours
        else if(m_listeMasquesParcours.count() < (m_opt_nbMasquesChoisisParcours - 1))
        {
            listeVoisins.clear();
            // Je vais chercher les voisins du dernier masque posé
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);

            if (listeVoisins.contains(masque->getNumero())) // Si le numero de ce masque est contenu ds mas liste de voisins = Ok
            {
                // S'il y a des masques de couleur grise... les remettre en blanc
                foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
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

                // ...& je coloris en gris les voisins
                listeVoisins = masquesVoisins(masque->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
                foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
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
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);

            if (listeVoisins.contains(masque->getNumero())) // Si le numero de ce masque est contenu ds mas liste de voisins = Ok
            {
                masque->setColor(QColor(Qt::red));
                masque->setProperty("Role", trUtf8("Arrivee"));
                masque->update();
                m_listeMasquesParcours << masque;
                // S'il y a des masques de couleur grise... les remettre en blanc
                foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
                {
                    if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
                    {
                        var_masque->setColor(QColor(Qt::white));
                        var_masque->update();
                    }
                }
                // Et j'active le menu Sauvegarder
                foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
                {
                    var_masque->setMenuSauvegarderEnabled(true);
                    var_masque->update();
                }

            }
        }
    } // Fin si mon masque n'a pas de role = creation de masque
    /// Si mon masque à un role (different de Fixe) = destruction du masque
    // Je n'enleve que le dernier masque
    else if (masque->getNumero() == m_listeMasquesParcours.back()->getNumero())
    {
        // Remise à défaut
        masque->setProperty("Role", trUtf8("Fixe"));
        masque->setColor(QColor(Qt::white));
        masque->update();

        // je remets les masques blanc
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
            {
                var_masque->setColor(QColor(Qt::white));
                var_masque->update();
            }
        }
        // Et je desactive le menu Sauvegarder
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            var_masque->setMenuSauvegarderEnabled(false);
            var_masque->update();
        }
        // et j'enleve ce masque de ma liste parcours
        m_listeMasquesParcours.removeLast();
        // Remise des masques gris pour les voisins du dernier masque (je viens d'enlever CE masque juste avant)
        // ...& je coloris en gris les voisins sauf si j'enleve depart donc pas de colriage en gris
        if (!m_listeMasquesParcours.isEmpty())
        {
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
            foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
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
    qDebug() << m_listeMasquesParcours.count();
}

/** Réinitialise l'éditeur de Parcours.
  * Redéfinit tous les masques comme fixe, et la couleur à blanc
  * (à l'appui sur le menu contextuel Reinitialiser)
  */
void Editeur::reinitialiserGvParcours()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::reinitialiserGvParcours()";

    // Remettre tout mes masques d'origine !
    for (int i = 0; i < m_listeMasques.count(); i++)
    {
        m_listeMasques.at(i)->setColor(QColor(Qt::white));
        m_listeMasques.at(i)->setProperty("Role", trUtf8("Fixe"));
        m_listeMasques.at(i)->update();
    }
    // vider ma listeMasquesParcours
    m_listeMasquesParcours.clear();
    gv_AireParcours->update();
}

/** Sauvegarde le parcours (à l'appui sur le menu contextuel Sauvegarde)
  */
void Editeur::sauvegarderParcours()
{
    qDebug() << "Numero de parcours :" << m_numeroParcours;
    if (m_localDebug) qDebug() << "##########################  Editeur::sauvegarderParcours()";

    switch (m_numeroParcours)
    {
    case 1: // c'est le parcours 1
        // Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste
        m_parametresParcours1.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours1.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        // Il reste que des masques "Parcours" dans la liste
        qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours1.insert("MasqueParcours" + QString::number('0'+i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours1->setStyleSheet("color : green;");
        break;
    case 2: // c'est le parcours 2
        // Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste
        m_parametresParcours2.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours2.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        // Il reste que des masques "Parcours" dans la liste
        qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours2.insert("MasqueParcours"+ QString::number('0'+i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours2->setStyleSheet("color : green;");
        break;
    case 3: // c'est le parcours 3
        // Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste
        m_parametresParcours3.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours3.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        // Il reste que des masques "Parcours" dans la liste
        qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours3.insert("MasqueParcours"+ QString::number('0'+i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours3->setStyleSheet("color : green;");
        break;
    case 4: // c'est le parcours 4
        // Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste
        m_parametresParcours4.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours4.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        // Il reste que des masques "Parcours" dans la liste
        qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours4.insert("MasqueParcours"+ QString::number('0'+i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours4->setStyleSheet("color : green;");
        break;
    case 5: // c'est le parcours 5
        // Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste
        m_parametresParcours5.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours5.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        // Il reste que des masques "Parcours" dans la liste
        qDebug() << m_listeMasquesParcours.count();
        for (int i =0; i < m_listeMasquesParcours.count(); i++ )
        {
            m_parametresParcours5.insert("MasqueParcours"+ QString::number('0'+i), m_listeMasquesParcours.at(i)->getNumero());
        }
        ui->btnParcours5->setStyleSheet("color : green;");
        break;
    default:
        return;
    }

    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    QMessageBox::information(this,"Editeur de Parcours", trUtf8("Le parcours a bien été sauvegardé"),0,0);

    gv_AireParcours->close();
}

/** Retourne la liste des voisins possibles d'un masque
  * @param numeroMasque, int, le numero du masque courant
  * @param largeur, int, le nombre de masques dans la largeur du parcours
  * @param hauteur, int, le nombre de masques dans la hauteur du parcours
  * @return voisinsMasques, QList<Int>, la liste des voisins possibles (gauche, droite, haut, bas)
  */
QList<int> Editeur::masquesVoisins(int numeroMasque, int largeur, int hauteur)
{
    QList<int> voisinsMasques;
    int nbTotal = largeur * hauteur;

    int gauche;
    int droite;
    int bas;
    int haut;
    gauche = droite = bas = haut = 0;

    // calcul des différentes directions possibles
    gauche = numeroMasque - 1;
    droite = numeroMasque + 1;
    haut = numeroMasque - largeur;
    bas  = numeroMasque + largeur;

    // Controle gauche et droite
    if ( ((gauche/largeur)%hauteur) == ((numeroMasque/largeur)%hauteur) && ( gauche >= 0) && (gauche <= nbTotal) ) // c'est la meme ligne
    {
        voisinsMasques << gauche;
    }
    if ( ((droite/largeur)%hauteur) == ((numeroMasque/largeur)%hauteur) && ( droite >= 0) && (droite <= nbTotal)) // c'est la meme ligne
    {
        voisinsMasques << droite;
    }

    // Controle haut & bas
    if (haut >= 0 && haut <= nbTotal)
        voisinsMasques << haut;
    if (bas >= 0 && bas <= nbTotal)
        voisinsMasques << bas;

    return voisinsMasques;
}

void Editeur::on_btnParcours1_clicked()
{
    m_numeroParcours = 1;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        QMessageBox::information(this,trUtf8("Nombre masques Parcours"),
                                 trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur") ,0,0);
        return;
    }

    gv_AireParcours = new AbulEduEtiquettesV1(QPoint(0,0));
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 1"));
    //    gv_AireParcours->setGeometry(0,0,800,600);
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours();
    gv_AireParcours->show();
}

void Editeur::on_btnParcours2_clicked()
{
    m_numeroParcours = 2;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        QMessageBox::information(this,trUtf8("Nombre masques Parcours"),
                                 trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur") ,0,0);
        return;
    }

    gv_AireParcours = new AbulEduEtiquettesV1(QPoint(0,0));
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 2"));
    //    gv_AireParcours->setGeometry(0,0,800,600);
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours();
    gv_AireParcours->show();
}

void Editeur::on_btnParcours3_clicked()
{
    m_numeroParcours = 3;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        QMessageBox::information(this,trUtf8("Nombre masques Parcours"),
                                 trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur") ,0,0);
        return;
    }

    gv_AireParcours = new AbulEduEtiquettesV1(QPoint(0,0));
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 3"));
    //    gv_AireParcours->setGeometry(0,0,800,600);
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours();
    gv_AireParcours->show();
}

void Editeur::on_btnParcours4_clicked()
{
    m_numeroParcours = 4;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        QMessageBox::information(this,trUtf8("Nombre masques Parcours"),
                                 trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur") ,0,0);
        return;
    }

    gv_AireParcours = new AbulEduEtiquettesV1(QPoint(0,0));
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 4"));
    //    gv_AireParcours->setGeometry(0,0,800,600);
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours();
    gv_AireParcours->show();
}

void Editeur::on_btnParcours5_clicked()
{
    m_numeroParcours = 5;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        QMessageBox::information(this,trUtf8("Nombre masques Parcours"),
                                 trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur") ,0,0);
        return;
    }

    gv_AireParcours = new AbulEduEtiquettesV1(QPoint(0,0));
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 5"));
    //    gv_AireParcours->setGeometry(0,0,800,600);
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours();
    gv_AireParcours->show();
}

//void Editeur::on_treeViewArborescence_doubleClicked(const QModelIndex &index)
//{
//    qDebug() << "DOUBLE CLIC";
//    QItemSelectionModel *selection = ui->treeViewArborescence->selectionModel();
//    QModelIndexList listeSelection;
//    listeSelection = selection->selectedRows(0); // je recupere tous les lignes selectionnées (sans controle dir/file)

//    // Je construis un QFileInfo pour chaque ligne selectionnée afin de déduire si c'est un dossier ou un fichier
//    for (int i = 0; i < listeSelection.count(); i++)
//    {
//        QFileSystemModel *monModel;
//        monModel = new QFileSystemModel(ui->treeViewArborescence->model()); // je recupere mon modele (je n'arrive pas à le caster...)
//        QFileInfo monFichier(monModel->filePath(listeSelection.at(i)));

//        if (monFichier.isDir()) // Controle sur la monFichier si fichier -> dans listeImages sinon nothing
//        {
//            qDebug() << "c'est un dossier";
//        }
//        else if (monFichier.isFile())
//        {
//            ajouterImage(monFichier);
//        }
//    }
//    selection->clearSelection(); //nettoyage de la selection

//    /// QDebug de ma liste
//    qDebug() << "Ma liste d'Images en sortie";
//    for (int i = 0; i < m_listeFichiersImages.count(); i++)
//    {
//        qDebug() << i <<" "<<m_listeFichiersImages.at(i);
//    }
//}


/// **********************************************************************************************************************************************************
///                             GESTION MODE MODIFIER ABE
/// **********************************************************************************************************************************************************

/** TODO : Gestion des chemins lors d'une modification abe
  * Focus OK
  * Problème de récupération du chemin de dezippage de l'abe choisi getFile ne renvoie pas le bon -> OK
  */
void Editeur::setModeModificationAbe(bool yesNo)
{
    m_modeModificationAbe = yesNo;
}

void Editeur::on_btnCreationAbe_clicked()
{
    //    remplirArborescence();

    m_listeDossiers.clear();
    m_listeFichiersImages.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->listWidgetImagesSelection->clear();
    initCheminTemp();

    qDebug() << trUtf8("Mode Création sélectionné");
    setModeModificationAbe(false);
    qDebug() << m_modeModificationAbe;

    ui->btnSuivant->click(); // Clic sur le bouton suivant
}

void Editeur::on_btnModificationAbe_clicked()
{
    m_listeDossiers.clear();
    m_listeFichiersImages.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->listWidgetImagesSelection->clear();
    initCheminTemp();

    qDebug() << trUtf8("Mode Modification sélectionné");
    setModeModificationAbe(true);
    qDebug() << m_modeModificationAbe;

    //Récupération de l'abe à modifier
    m_abuledufilev1 = new AbulEduFileV1();
    m_abuleduFileManager = new AbulEduBoxFileManagerV1();
    m_abuleduFileManager->abeSetFile(m_abuledufilev1);

    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected()),this, SLOT(slotOpenFile()));
#ifdef __ABULEDUTABLETTEV1__MODE__
    m_abuleduFileManager->showFullScreen();
#else
    m_abuleduFileManager->show();
#endif
    ui->btnSuivant->click(); // Clic sur le bouton suivant
}

/** Lors de la modification d'un .abe, permet de le choisir et de faire ce qui faut en conséquence
  * C'est encore en construction...
  */
void Editeur::slotOpenFile()
{
    if (m_localDebug) qDebug() << trUtf8("Nom du fichier passé :") << m_abuleduFileManager->abeGetFile()->abeFileGetFileName().absoluteFilePath();
    m_abuledufilev1 = m_abuleduFileManager->abeGetFile();

    qDebug() << trUtf8("Fichier temporaire de dézippage de l'ABE choisi")  << m_abuledufilev1->abeFileGetDirectoryTemp().absolutePath();
    m_abuleduFileManager->hide();

    m_destImageABE = new QDir(m_cheminImage) ;

    qDebug() << "CHEMIN IMAGE " << m_destImageABE->absolutePath();
    // Jusqu'ici ok pour les images

    /// OK
    // Copier les images dans un dossier temporaire
    // les mettre dans le listeWidget de Selection
    //    destImageABE->setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    //    QFileInfoList list = destImageABE->entryInfoList();
    //    for(int i = 0; i < list.count(); i++)
    //    {
    //        qDebug() << "Liste du dossier image " << QString::number(i)+" "+ list.at(i).absoluteFilePath();
    //        ajouterImage(list.at(i));
    //    }

    /// Autre approche -> copier tous le dossier dezipper dans le fichier temporaire
    /// TODO -> Comment rassembler des fichiers LOMFr et tout ce qui suit ?

}

void Editeur::slotLoadUnit()
{
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    m_listeFichiersImages.clear();
    ui->listWidgetImagesSelection->clear();
    QDir folder(QString(m_parent->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath()+"/data/images"));
    folder.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(QFileInfo fileInfo, folder.entryInfoList())
    {
        ajouterImage(fileInfo.absoluteFilePath());
        m_lastOpenDir = fileInfo.absolutePath();
    }
    QSettings parametres(m_parent->abeGetMyAbulEduFile()->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
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
        ui->spinBoxParcoursMasque->setValue(parametres.value("nbMasquesChoisis",7).toInt());
        ui->spinBoxParcoursMasquesLargeur->setValue(parametres.value("nbMasquesLargeur",7).toInt());
        ui->spinBoxParcoursMasqueHauteur->setValue(parametres.value("nbMasquesHauteur",7).toInt());
    parametres.endGroup();
    ui->stackedWidget->setCurrentWidget(ui->pageGestionImages);
}

//void Editeur::testThread()
//{
//    qDebug() << __PRETTY_FUNCTION__ << " L'editeur vient de voir que la recherche est terminé";
//    // Recupération de la liste des fichiers listee par le thread
//    qDebug() << ui->listWidgetImage->item(0)->text();
//    ui->listWidgetImage->removeItemWidget(ui->listWidgetImage->item(0));
//    ui->listWidgetImage->item(0)->setText("OK");
////    QStringList m_fichiersImagesLocales = m_threadRechercheImages->getListeFichiers();
////    qDebug() << m_fichiersImagesLocales.count();

////    for (int i =0; i < 1000/*m_fichiersImagesLocales.count()*/; i++)
////    {
//        //        QListWidgetItem *item = new QListWidgetItem();
//        //        QIcon icone(m_fichiersImagesLocales.at(i));//pour la mettre  à coté de l'item
//        //        item->setIcon(icone); // ajout de la petite icone sur l'item
//        //        item->setText(m_fichiersImagesLocales.at(i));
//        ////        item->setData(4, destImage->absolutePath() + QDir::separator() + monFichier.fileName());
//        //        ui->listWidgetImage->insertItem(0, item);
//        //        ui->listWidgetImage->show();
////    }

//    //    ui->listWidgetImage->show();
////    qDebug() << "Item Terminer";
//    //    QListWidgetItem *item = new QListWidgetItem();
//    //    QIcon icone(destImage->absolutePath() + QDir::separator() + monFichier.fileName());//pour la mettre  à coté de l'item
//    //    item->setIcon(icone); // ajout de la petite icone sur l'item
//    //    item->setText(monFichier.fileName());
//    //    item->setData(4, destImage->absolutePath() + QDir::separator() + monFichier.fileName());
//    //    ui->listWidgetImagesSelection->insertItem(0, item);

//    ui->widgetDisqueLocal->setListeFichiers(m_threadRechercheImages->getListeFichiers());
//}

void Editeur::slotTestImportImage(QString cheminFichier, QString nomFichier)
{
    qDebug() << "Fichier Recu" << cheminFichier <<" "<< nomFichier;

    /// La creation d'item pose probleme




}


/// **********************************************************************************************************************************************************
///                             WIDGET BARRE NAVIGATION
/// **********************************************************************************************************************************************************

void Editeur::on_btnPrecedent_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
}

void Editeur::on_btnSuivant_clicked()
{
    if(ui->stackedWidget->currentWidget()->objectName() == "pageGestionImages") {
        /// Aller chercher les images et les enregistrer dans le fichier temporaire
        // Condition de garde = m_listeFichiersImages < 5
        if ( m_listeFichiersImages.count() < 5)
        {
            QMessageBox::warning(this, trUtf8("Sauvegarder Thème"), trUtf8("Veuillez sélectionner au minimum 5 images"));
            return;
        }
    }
    if(ui->stackedWidget->currentWidget()->objectName() == "pageParametres") {
        createAbe();
        close();
        return;
    }
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
}

void Editeur::on_btnQuitter_clicked()
{
    qDebug() << __PRETTY_FUNCTION__ ;
    // Controle que l'utilisateur veut quitter sans sauvegarder
    this->close();
}

void Editeur::majBarreNavigation(int numPage)
{
    qDebug() << "Index de la page courante" << ui->stackedWidget->currentIndex();

    if (numPage == 0) // page d'accueil
    {
        // On cache le bouton précèdent
        ui->btnPrecedent->setVisible(false);
        ui->btnSuivant->setVisible(false);
    }
    else if(numPage != 0)
    {
        ui->btnPrecedent->setVisible(true);
        ui->btnSuivant->setVisible(true);

        if (numPage == 2) // derniere page
        {
            ui->btnSuivant->setIcon(QIcon(":/bouton/disque"));
        }
        else if(numPage != 2)
        {
            ui->btnSuivant->setIcon(QIcon(":/bouton/flecheD"));
        }
    }
}

/// **********************************************************************************************************************************************************
///                             GESTION DRAG & DROP
/// **********************************************************************************************************************************************************

void Editeur::dropEvent(QDropEvent *event)
{


    qDebug() << __PRETTY_FUNCTION__ << " " << event->source() << " " << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());

    if (event->source()->objectName() == "treeViewArborescence" && ui->listWidgetImagesSelection->geometry().contains(event->pos()))
    {
        qDebug() << "SOURCE == treeViewArbo";
        // Controle que c'est bien une image
        if(event->mimeData()->hasImage())
        {
            qDebug() << "C'est une image";
        }
    }
    else if(event->source()->objectName() == "lwSimple" && ui->listWidgetImagesSelection->geometry().contains(event->pos()))
    {
        qDebug() << "SOURCE == mediathequeGet";

        ui->abuleduMediathequeGet->abeStartDownload();
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }

}

void Editeur::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << __PRETTY_FUNCTION__ << " " << event->source() << " " << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());
    event->accept();

    //    if (event->mimeData()->hasImage())
    //        event->acceptProposedAction();

    //    if (obj->objectName() == ui->listWidgetImagesSelection->objectName())
    //    {
    //        qDebug() << "OK";
    //    }
}

//bool Editeur::eventFilter(QObject *obj, QEvent *ev)
//{
//    //    qDebug() << "** " << ev->type();

//    QDragEnterEvent *event = static_cast<QDragEnterEvent*>(ev);
//    if(event->type() == QEvent::Drop /*&& obj == ui->listWidgetImagesSelection*/) {
//        qDebug() << "======ENTER ======================";
//        qDebug() << obj->objectName();
//        qDebug() << "============================";
//        //        event->accept();

//    }
//    if(ev->type() == QEvent::DragLeave && obj == ui->listWidgetImagesSelection) {
//        qDebug() << "============DROP ================";
//        qDebug() << obj->objectName();
//        qDebug() << "============================";
//        QDropEvent *event = static_cast<QDropEvent*>(ev);
//        event->acceptProposedAction();
//    }
//    if(ev->type() == MouseEvent::MouseButtonRelease)
//    {
//        qDebug() << "QMouseEvent::Release";
//    }
//}

void Editeur::on_btnAjouterImageQFileDialog_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         trUtf8("Choisir une image"), m_lastOpenDir, trUtf8("Images (*.png *.jpg *.jpeg)"));
    QFileInfo fi(fileName);
    if(fi.exists()) {
        ajouterImage(fi.absoluteFilePath());
        m_lastOpenDir = fi.absolutePath();
    }
}

void Editeur::on_btnModificationCourant_clicked()
{
    slotLoadUnit();
}
