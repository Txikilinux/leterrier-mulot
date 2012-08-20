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
#include "ui_editeur.h"


Editeur::Editeur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);

    ui->abuleduMediathequeGet->abeSetSource("data");
    ui->abuleduMediathequeGet->abeSetCustomBouton1(trUtf8("Importer l'image"));
    ui->abuleduMediathequeGet->abeHideBoutonTelecharger();
    ui->abuleduMediathequeGet->abeCustomBouton1SetDownload(true);

    // Initialisation des chemins temporaires
    m_abuledufilev1 = new AbulEduFileV1();
    destinationIdUnique = m_abuledufilev1->abeFileGetDirectoryTemp().absolutePath(); //je récupère mon Id unique
    arborescenceImage = QString("data") + QDir::separator() + QString("images");
    cheminImage = destinationIdUnique + QDir::separator() + arborescenceImage ;
    arborescenceConf = QString("conf");
    cheminConf = destinationIdUnique + QDir::separator() + arborescenceConf;

    connect(ui->abuleduMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(int)), this, SLOT(slotImportImageMediatheque()));

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);

    m_localDebug = true;

    opt_nbMasquesChoisisParcours = 0;
    opt_nbMasquesLargeur = 0;
    opt_nbMasquesHauteur = 0;
    m_numeroParcours = 0;

    m_listeMasques.clear();
    m_listeMasquesParcours.clear();
    m_parametresParcours1.clear();
    m_parametresParcours2.clear();
    m_parametresParcours3.clear();
    m_parametresParcours4.clear();
    m_parametresParcours5.clear();

    m_listeFichiersImages.clear();

    remplirArborescence();

    destImage = new QDir(cheminImage); // creation dossier temporaire pour les images
    if(destImage->mkpath(cheminImage)) // tentative de création du fichier temp avec un id unique + sous dossier au nom du theme
    {
        if (m_localDebug)
        {
            qDebug() << "Creation ok : " << destImage->absolutePath();
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
    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::homePath());

    QStringList filters;
    QDir dir;
    filters << "*.jpg" << "*.bmp"<< "*.png" << "*.svg"; //Choix des extensions
    dir.setNameFilters(filters);

    ui->treeViewArborescence->setModel(model);
    model->setNameFilters(filters); //Filtrage des photos

    connect(ui->treeViewArborescence, SIGNAL(clicked(const QModelIndex&)),this, SLOT(slotResizeColumn(const QModelIndex&)));
    connect(ui->treeViewArborescence, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(slotMenuContextuel(const QPoint &)));

    ui->treeViewArborescence->hideColumn(1);
    ui->treeViewArborescence->hideColumn(2);
    ui->treeViewArborescence->hideColumn(3);

    ui->treeViewArborescence->setUniformRowHeights(true); // met toutes les lignes à la meme taille, ce qui permet d'optimiser le temps de réponse (ne recalcule pas la taille de chaque ligne)
    ui->treeViewArborescence->setAnimated(true);          // a mettre a false pour les petites configurations (hein JLF !)
    ui->treeViewArborescence->setSortingEnabled(true);
    ui->listWidgetImagesSelection->setIconSize(QSize(50, 50));

    creationMenu();
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
void Editeur::slotResizeColumn(const QModelIndex& index)
{
    ui->treeViewArborescence->resizeColumnToContents(index.column());
}

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

/** Ce slot appelle le menu contextuel seulement si des fichiers sont selectionnés
  * et que ces fichiers soient des images
  */
void Editeur::slotMenuContextuel(const QPoint&)
{
    QItemSelectionModel *selection = ui->treeViewArborescence->selectionModel();
    QModelIndexList listeSelection;
    listeSelection = selection->selectedRows(0); // je recupere tous les lignes selectionnées (sans controle dir/file)

    // Je construis un QFileInfo pour chaque ligne selectionnée afin de déduire si c'est un dossier ou un fichier
    for (int i = 0; i < listeSelection.count(); i++)
    {
        QFileSystemModel *monModel;
        monModel = new QFileSystemModel(ui->treeViewArborescence->model()); // je recupere mon modele (je n'arrive pas à le caster...)
        QFileInfo monFichier(monModel->filePath(listeSelection.at(i)));

        if (monFichier.isDir()) // Controle sur la monFichier si fichier -> dans listeImages sinon nothing
        {
            qDebug() << "c'est un dossier";
        }
        else if (monFichier.isFile())
        {
            ajouterImage(monFichier);
        }
    }
    selection->clearSelection(); //nettoyage de la selection

    /// QDebug de ma liste
    qDebug() << "Ma liste d'Images en sortie";
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
        if(copierImageDansTemp(monFichier, destImage->absolutePath())) // Si cette fonction a fonctionnée
        {
            m_listeFichiersImages << destImage->absolutePath() + QDir::separator() + monFichier.fileName(); // je range le chemin de l'image dans ma liste (celui du fichier temp)
            // Insertion dans mon listWidget
            QListWidgetItem *item = new QListWidgetItem();
            QIcon icone(destImage->absolutePath() + QDir::separator() + monFichier.fileName());//pour la mettre  à coté de l'item
            item->setIcon(icone); // ajout de la petite icone sur l'item
            item->setText(monFichier.fileName());
            item->setData(4, destImage->absolutePath() + QDir::separator() + monFichier.fileName());
            ui->listWidgetImagesSelection->insertItem(0, item);
        }
    }
}

void Editeur::slotImportImageMediatheque()
{
    //    copierImageDansTemp(ui->abuleduMediathequeGet->abeGetFile()->abeFileGetContent(0), destImage->absolutePath());
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

void Editeur::on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_listWidget_itemDoubleClicked(QListWidgetItem *item)";

    item = ui->listWidgetImagesSelection->currentItem();
    m_visionneuseImage = new VisionneuseImage(this);
    m_visionneuseImage->ouvrirFicher(item->data(4).toString());
    m_visionneuseImage->setWindowModality(Qt::WindowModal);
    m_visionneuseImage->show();
}

/**
  * Creation du .abe
  */
void Editeur::on_btnCreerTheme_clicked()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btnCreationtheme_clicked()";

    //    qDebug() << "destinationIdUnique " << destinationIdUnique;
    //    qDebug() << "arborescenceImage " << arborescenceImage;
    //    qDebug() << "cheminImage " << cheminImage;
    //    qDebug() << "arborescenceConf" << arborescenceConf;
    //    qDebug() << "cheminConf" << cheminConf;

    // Condition de garde si le nom du theme est vide
    if (ui->lineEditNomTheme->text().isEmpty())
    {
        QMessageBox::warning(this, trUtf8("Sauvegarder Thème"), trUtf8("Veuillez remplir le champs \"Nom du thème\" "));
        return;
    }

    /// Aller chercher les images et les enregistrer dans le fichier temporaire
    // Condition de garde = m_listeFichiersImages < 5
    if ( m_listeFichiersImages.count() < 5)
    {
        QMessageBox::warning(this, trUtf8("Sauvegarder Thème"), trUtf8("Veuillez sélectionner au minimum 5 images"));
        return;
    }

    /// Choix destination .abe
    QString destAbe = QFileDialog::getExistingDirectory(this, trUtf8("Ouvrir un répertoire"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (m_localDebug)   qDebug() << destAbe;
    if (destAbe.isNull()) // dossier est nul, donc pas la peine d'aller plus loin
    {
        QMessageBox::warning(this, trUtf8("Sauvegarder Thème"), trUtf8("Veuillez choisir un emplacement de destination pour le thème"));
        qDebug() << "Appui sur le bouton annuler";
        return;
    }
    m_dirAbe = new QDir();
    m_dirAbe->setPath(destAbe);


    /// TTTTTTT
    ui->listWidgetImagesSelection->clear();
    /// TTTTTTT

    if (m_localDebug) qDebug() << "Copie Images dans fichier temp ok";
    //    }

    /// Aller chercher le fichier conf
    QDir confDir(cheminConf);//creation dossier temporaire pour .ini
    if(confDir.mkpath(cheminConf)) // tentative de création
    {
        if (m_localDebug) qDebug() << "Creation fichier temp/conf ok " << confDir.absolutePath();
        else { return; } // si echec pas la peine d'aller plus loin
    }

    /// Creation fichier Conf (note les timers sont convertis en millisecondes)
    QSettings parametres(cheminConf +QDir::separator()+"parametres.conf", QSettings::IniFormat);
    /// Parametres Survol
    parametres.setValue("Survol/timerSuivant", (ui->spinBoxSurvolSuivant->value()*1000));
    parametres.setValue("Survol/nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    /// Parametres Clic
    parametres.setValue("Clic/timerSuivant", (ui->spinBoxClicSuivant->value()*1000));
    parametres.setValue("Clic/nbMasquesChoisis", (ui->spinBoxClicMasque->value()));
    /// Parametres Double-Clic
    parametres.setValue("Double-Clic/timerSuivant", (ui->spinBoxDoubleClicSuivant->value()*1000));
    parametres.setValue("Double-Clic/nbMasquesChoisis", (ui->spinBoxDoubleClicMasque->value()));
    /// Parametres Parcours1
    parametres.setValue("Parcours1/timerSuivant", (ui->spinBoxParcoursSuivant->value()*1000));
    parametres.setValue("Parcours1/nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
    parametres.setValue("Parcours1/nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    parametres.setValue("Parcours1/nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
    // Parcours
    QMapIterator<QString, QVariant> i(m_parametresParcours1);
    while (i.hasNext()) {
        i.next();
        parametres.setValue("position1/"+ i.key(), i.value());
    }
    /// Parametres Parcours2
    parametres.setValue("Parcours2/timerSuivant", (ui->spinBoxParcoursSuivant->value()*1000));
    parametres.setValue("Parcours2/nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
    parametres.setValue("Parcours2/nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    parametres.setValue("Parcours2/nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
    // Parcours
    QMapIterator<QString, QVariant> j(m_parametresParcours2);
    while (j.hasNext()) {
        j.next();
        parametres.setValue("position2/"+ j.key(), j.value());
    }
    /// Parametres Parcours3
    parametres.setValue("Parcours3/timerSuivant", (ui->spinBoxParcoursSuivant->value()*1000));
    parametres.setValue("Parcours3/nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
    parametres.setValue("Parcours3/nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    parametres.setValue("Parcours3/nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
    // Parcours
    QMapIterator<QString, QVariant> k(m_parametresParcours3);
    while (k.hasNext()) {
        k.next();
        parametres.setValue("position3/"+ k.key(), k.value());
    }
    /// Parametres Parcours4
    parametres.setValue("Parcours4/timerSuivant", (ui->spinBoxParcoursSuivant->value()*1000));
    parametres.setValue("Parcours4/nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
    parametres.setValue("Parcours4/nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    parametres.setValue("Parcours4/nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
    // Parcours
    QMapIterator<QString, QVariant> l(m_parametresParcours4);
    while (l.hasNext()) {
        l.next();
        parametres.setValue("position4/"+ l.key(), l.value());
    }
    /// Parametres Parcours5
    parametres.setValue("Parcours5/timerSuivant", (ui->spinBoxParcoursSuivant->value()*1000));
    parametres.setValue("Parcours5/nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
    parametres.setValue("Parcours5/nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    parametres.setValue("Parcours5/nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
    // Parcours
    QMapIterator<QString, QVariant> m(m_parametresParcours5);
    while (m.hasNext()) {
        m.next();
        parametres.setValue("position5/"+ m.key(), m.value());
    }

    /// Creation .abe
    parametres.sync(); //pour forcer l'écriture du .conf

    if (m_localDebug) qDebug() << destinationIdUnique<< " " << parametres.fileName();
    if (m_localDebug) qDebug() << parcoursRecursif(destinationIdUnique);

    QString nomtheme = ui->lineEditNomTheme->text();
    QDir temp(destinationIdUnique); // récupération du chemin du fichier temp
    QString m_fileBase = temp.absolutePath();

    QString destination = m_dirAbe->absolutePath() + QDir::separator() + nomtheme ;
    m_abuledufilev1->abeFileSave(destination, parcoursRecursif(destinationIdUnique), m_fileBase, "abe");
    if (m_localDebug) qDebug() << "Création abe OK";

    /// Supprimer le dossier temporaire
    if(supprimerDir(temp.absolutePath())) qDebug() << "Effacement dossier temp ok";
    else qDebug() << "Suppression impossible";

    /// Arrangement graphique
    ui->lineEditNomTheme->clear();
    //    ui->listWidget->clear();
    //    ui->listWidgetSelection->clear();
    //    ui->treeWidget->clear();
    QMessageBox::information(this, trUtf8("Sauvegarder Thème"), trUtf8("Le theme a été sauvegardé avec succès"));
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

void Editeur::remplirGvParcours()
{
    opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque->value();
    opt_nbMasquesLargeur = ui->spinBoxParcoursMasquesLargeur->value();
    opt_nbMasquesHauteur = ui->spinBoxParcoursMasqueHauteur->value();

    float largeurMasque = 0.00;
    float hauteurMasque = 0.00;

    float largeurGv = static_cast<float>(gv_AireParcours->width())-1;
    float hauteurGv = static_cast<float>(gv_AireParcours->height())-1;

    largeurMasque = largeurGv / opt_nbMasquesLargeur;
    hauteurMasque = hauteurGv / opt_nbMasquesHauteur;

    int nbMasques = opt_nbMasquesLargeur * opt_nbMasquesHauteur;
    qreal xMasque = 0.00;
    qreal yMasque = 0.00;

    if (m_localDebug)
        qDebug()<<" -------------------------- Début boucle d'affichage : "<<nbMasques;

    int numeroMasque = 0;
    for (float i=0; i<opt_nbMasquesHauteur;i++)
    {
        for (int j =0; j < opt_nbMasquesLargeur;j++)
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
            listeVoisins = masquesVoisins(masque->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
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
        else if(m_listeMasquesParcours.count() < (opt_nbMasquesChoisisParcours - 1))
        {
            listeVoisins.clear();
            // Je vais chercher les voisins du dernier masque posé
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);

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
                listeVoisins = masquesVoisins(masque->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
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
        else if ( m_listeMasquesParcours.count() == (opt_nbMasquesChoisisParcours-1))
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
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
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
    int nbTotal = largeur * hauteur; // donc 50

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
