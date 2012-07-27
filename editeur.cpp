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
    //    ui->widget->abeSetSource("data");
    setAttribute(Qt::WA_DeleteOnClose);

    m_localDebug = true;

    ui->treeWidget->setAlternatingRowColors(true);
    ui->treeWidget->setColumnCount(1);

    // ------- Action Menu Contextuel Tree Widget ------------------------------------------------------------
    QList<QAction *> actionsMenuTreeWidget;

    m_menuTreeWidget = new QMenu(ui->treeWidget);

    QAction *a_supprimer = new QAction(trUtf8("&Supprimer..."),m_menuTreeWidget);
    QStyle* style =  QApplication::style();
    QIcon iconSupprimer = style->standardIcon(QStyle::SP_DialogResetButton); //On récupère l'icône désiré
    a_supprimer->setIcon(iconSupprimer);
    a_supprimer->setIconVisibleInMenu(true);
    a_supprimer->connect(a_supprimer, SIGNAL(triggered()), this, SLOT(on_action_Supprimer_dossier_triggered()));

    actionsMenuTreeWidget << a_supprimer /*a_nouveau << a_renommer <<*/ ;
    m_menuTreeWidget->addActions(actionsMenuTreeWidget);

    // ------- Action Menu Contextuel Liste Widget Selection ---------------------------------------------------
    QList<QAction *> actionsMenuListWidgetSelection;

    m_menuListWidgetSelection = new QMenu(ui->listWidgetSelection);

    QAction *a_supprimer2 = new QAction(trUtf8("&Supprimer..."),m_menuListWidgetSelection);
    a_supprimer2->setIcon(iconSupprimer);
    a_supprimer2->setIconVisibleInMenu(true);
    a_supprimer2->connect(a_supprimer2, SIGNAL(triggered()), this, SLOT(on_action_Supprimer_photo_triggered()));

    actionsMenuListWidgetSelection << /*a_nouveau << a_renommer <<*/ a_supprimer2;
    m_menuListWidgetSelection->addActions(actionsMenuListWidgetSelection);

}

Editeur::~Editeur()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::~Editeur()";
    delete ui;
}

void Editeur::on_action_Supprimer_dossier_triggered()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_action_Supprimer_dossier_triggered()";

    // Le supprimer de la liste de dossier
    m_listeDossiers.removeOne(ui->treeWidget->currentItem()->data(1,0).toString());
    // Supprimer l'item du TreeWidget
    ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem()));
    ui->listWidget->clear(); // nettoyage de la listeWidget
}

void Editeur::on_action_Supprimer_photo_triggered()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_action_Supprimer_photo_triggered()";
    // Suppression de l'item
    ui->listWidgetSelection->takeItem(ui->listWidgetSelection->row(ui->listWidgetSelection->currentItem()));
}

void Editeur::on_btnImporterDossierImage_clicked()
{
    if(m_localDebug) qDebug() << "##########################  Appuie sur le bouton d'Importation des images";

    // Icone standard de dossier
    QStyle* style =  QApplication::style();
    QIcon iconDossier = style->standardIcon(QStyle::SP_DirIcon);

    QString dossier = QFileDialog::getExistingDirectory(this, trUtf8("Ouvrir un répertoire"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (m_localDebug)   qDebug() << dossier;

    if (dossier.isNull()) // dossier est nul, donc pas la peine d'aller plus loin
    {
        qDebug() << "Appui sur le bouton annuler";
        return;
    }

    m_dir = new QDir();
    m_dir->setPath(dossier);

    m_dir->setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );
    QFileInfoList list = m_dir->entryInfoList();

    if (m_localDebug)
    {
        for (int i=0; i< list.count(); i++)
            qDebug() << "Entree enregistree ds le QFileInfoList"<< list.at(i).filePath();
    }

    if (list.isEmpty()) // il n'y a pas de ss dossiers, donc le nom du dossier courant est affiché dans item
    {
        if (m_listeDossiers.contains(m_dir->absolutePath()))
        {
            qDebug() << trUtf8("Ce dossier est déjà présent");
        }
        else
        {
            QFileInfo file(m_dir->absolutePath()); // pour l'affichage du nom de dossier uniquement dans le tree widget
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, file.fileName());
            item->setIcon(0,iconDossier);
            item->setData(1, 0, m_dir->absolutePath());
            ui->treeWidget->show();
            m_listeDossiers << m_dir->absolutePath();
        }
    }
    else // on parcourt les ss dossiers et on les affiche
    {
        for(int i = 0; i < list.count(); i++)
        {
            if (m_listeDossiers.contains(list.at(i).filePath()))
            {
                qDebug() << "Ce dossier est déjà présent";
            }
            else
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
                item->setText(0, list.at(i).fileName());
                item->setIcon(0,iconDossier);
                item->setData(1, 0, list.at(i).absoluteFilePath());
                ui->treeWidget->addTopLevelItem(item);
                ui->treeWidget->show();

                m_listeDossiers << list.at(i).filePath();

                if (m_localDebug) qDebug() << item->data(1,0) << "" << list.at(i).fileName() << "" << list.at(i).absoluteFilePath();
            }
        }
    }

}

void Editeur::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)";
    // Quand je clique sur un item (dossier), parcours du dossier et affichage de toutes les images dans la listWidget

    QFileInfo fi (item->data(1,0).toString());
    m_dir = new QDir(fi.absoluteFilePath());

    if (m_localDebug) qDebug() << "Lecture de " << fi.absoluteFilePath() << " " << m_dir->absolutePath();

    // Ici, au clic sur l'item, j'ai son adresse =). Plus qu'à remplir le widget list des miniatures !
    m_dir->refresh();
    m_dir->setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QFileInfoList listeFichiers = m_dir->entryInfoList();
    for(int i = 0; i < listeFichiers.count(); i++)
    {
        rafraichirListeImages();
    }
}

void Editeur::rafraichirListeImages()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    ui->listWidget->clear();
    m_dir = new QDir(item->data(1,0).toString());

    m_dir->setFilter(QDir::Files);
    m_dir->setSorting(QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);
    QFileInfoList list = m_dir->entryInfoList();
    for(int i = 0; i < list.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem();
        QIcon icone(list.at(i).absoluteFilePath());//pour la mettre  à coté de l'item
        item->setIcon(icone); // ajout de la petite icone sur l'item
        item->setText(list.at(i).fileName());
        item->setData(4,list.at(i).absoluteFilePath());
        item->setCheckState(Qt::Unchecked);
        ui->listWidget->insertItem(i,item);
    }
}

void Editeur::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->treeWidget->itemAt(pos)!=NULL) // j'ai un item à cet endroit, j'appelle mon menu
    {
        m_menuTreeWidget->exec(ui->treeWidget->mapToGlobal(pos));
    }
    else // sinon je fais rien
    {
        if (m_localDebug) qDebug()<< "pas d'item";
    }
}

void Editeur::on_listWidgetSelection_customContextMenuRequested(const QPoint &pos)
{
    if (ui->listWidgetSelection->itemAt(pos) != NULL) // j'ai un item à cet endroit, j'appelle mon menu
    {
        m_menuListWidgetSelection->exec(ui->listWidgetSelection->mapToGlobal(pos));
    }
    else // sinon je fais rien
    {
        if (m_localDebug) qDebug() << "Pas d'item";
    }
}

void Editeur::on_listWidget_itemDoubleClicked(QListWidgetItem *item) // Ouverture de la visionneuse
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_listWidget_itemDoubleClicked(QListWidgetItem *item)";

    item = ui->listWidget->currentItem();
    if (m_localDebug) qDebug() << "Ouverture de l'image";
    m_visionneuseImage = new VisionneuseImage(this);
    m_visionneuseImage->ouvrirFicher(item->data(4).toString());
    m_visionneuseImage->setWindowModality(Qt::WindowModal);
    m_visionneuseImage->show();
}

void Editeur::on_btSelection_clicked()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btSelection_clicked()";

    for (int i =0; i< ui->listWidget->count(); i++ ) // Parcours de tout les items de la ListWidget
    {
        if(ui->listWidget->item(i)->checkState() == 2) // Si l'etat de mon item est "checked"
        {
            if (m_localDebug) qDebug() << trUtf8("cet item est selectionné");

            QListWidgetItem *item = new QListWidgetItem();
            QIcon icone(ui->listWidget->item(i)->icon());//pour la mettre  à coté de l'item
            item->setIcon(icone); // ajout de la petite icone sur l'item
            item->setText(ui->listWidget->item(i)->text());
            item->setData(4,ui->listWidget->item(i)->data(4));

            if (ui->listWidgetSelection->count() == 0)
            {
                if (m_localDebug) qDebug() << "Liste des selection vide >>>> Insertion de l'item";
                ui->listWidgetSelection->insertItem(0, item ); //... et on l'insere dans les listWidget des selectionnés
            }

            else if (ui->listWidgetSelection->count() > 0)
            {
                if (m_localDebug) qDebug() << "Liste des selection non vide";

                for(int i =0; i < ui->listWidgetSelection->count(); i++)
                {
                    if (controleDoublonsSelection(ui->listWidgetSelection, item->data(4).toString())) // Si true, j'insere
                        ui->listWidgetSelection->insertItem(0, item );
                    else
                        if (m_localDebug) qDebug() << "item deja Present";
                }
            }
        }

        else if (ui->listWidget->item(i)->checkState() == 0) // Si l'etat n'est pas "checked", on ne fait rien
        {
            if (m_localDebug) qDebug() << trUtf8("item non selectionné");
        }
    }
}

/**
  * Creation du .abe
  */
void Editeur::on_btnCreerTheme_clicked()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btnCreationtheme_clicked()";

    AbulEduFileV1 *m_abuledufilev1 = new AbulEduFileV1();
    // Initialisation des chemins temporaires
    destinationIdUnique = m_abuledufilev1->abeFileGetDirectoryTemp().absolutePath(); //je récupère mon Id unique
    arborescenceImage = QString("data") + QDir::separator() + QString("images");
    cheminImage = destinationIdUnique + QDir::separator() + arborescenceImage ;
    arborescenceConf = QString("conf");
    cheminConf = destinationIdUnique + QDir::separator() + arborescenceConf;

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
    /** Aller chercher les images et les enregistrer dans le fichier temporaire */

    // Condition de garde = listeWidgetSelection est inferieur à 5
    if (ui->listWidgetSelection->count() < 5)
    {
        QMessageBox::warning(this, trUtf8("Sauvegarder Thème"), trUtf8("Veuillez sélectionner au minimum 5 images"));
        return;
    }

    // Choix destination .abe
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

    QDir destDir(cheminImage); // creation dossier temporaire pour les images
    if(destDir.mkpath(cheminImage)) // tentative de création du fichier temp avec un id unique + sous dossier au nom du theme
    {
        if (m_localDebug)
        {
            qDebug() << "Creation ok "
                     << destDir.absolutePath();
        }
        else { return; } // si echec pas la peine d'aller plus loin

        // Copie des images selectionnées dans le fichier temporaire
        for (int i =0; i< ui->listWidgetSelection->count(); i++)
        {
            QFileInfo originale(ui->listWidgetSelection->item(i)->data(4).toString());
            if (m_localDebug) // Affichage chemin originale & destination des images
            {
                qDebug() << "Chemin de l'image a copier      " << originale.absoluteFilePath();
                qDebug() << "Chemin ou l'image va etre copiee" << destDir.absolutePath() + QDir::separator() + originale.fileName();
            }
            if( QFile::copy(originale.absoluteFilePath(), destDir.absolutePath() + QDir::separator() + originale.fileName()) )
            {
                if (m_localDebug) qDebug() << "Copie image ok";
            }
            else // Si la copie échoue, pas la peine d'aller plus loin
            {
                if (m_localDebug) qDebug() << "Copie impossible";
                return;
            }
        }
        ui->listWidgetSelection->clear();
        if (m_localDebug) qDebug() << "Copie Images dans fichier temp ok";
    }
    /** Aller chercher le fichier conf*/
    QDir confDir(cheminConf);//creation dossier temporaire pour .ini
    if(confDir.mkpath(cheminConf)) // tentative de création
    {
        if (m_localDebug) qDebug() << "Creation fichier temp/conf ok " << confDir.absolutePath();
        else { return; } // si echec pas la peine d'aller plus loin
    }

    // Creation fichier Conf (note les timers sont convertis en millisecondes)
    QSettings parametres(cheminConf +QDir::separator()+"parametres.conf", QSettings::IniFormat);
    // Parametres Survol
    parametres.setValue("Survol/timerSuivant", (ui->spinBoxSurvolSuivant->value()*1000));
    parametres.setValue("Survol/timerVerifier", (ui->spinBoxSurvolVerifier->value()*1000));
    parametres.setValue("Survol/nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    // Parametres Clic
    parametres.setValue("Clic/timerSuivant", (ui->spinBoxClicSuivant->value()*1000));
    parametres.setValue("Clic/timerVerifier", (ui->spinBoxClicVerifier->value()*1000));
    parametres.setValue("Clic/nbMasquesChoisis", (ui->spinBoxClicMasque->value()));
    // Parametres Double-Clic
    parametres.setValue("Double-Clic/timerSuivant", (ui->spinBoxDoubleClicSuivant->value()*1000));
    parametres.setValue("Double-Clic/timerVerifier", (ui->spinBoxDoubleClicVerifier->value()*1000));
    parametres.setValue("Double-Clic/nbMasquesChoisis", (ui->spinBoxDoubleClicMasque->value()));

    //------------------------ Création du .abe
    //--------------------------------------------------------
    // remplir ma liste de fichiers (parcours recursif)
    parametres.sync(); //pour forcer l'écriture du .conf

    if (m_localDebug) qDebug() << destinationIdUnique<< " " << parametres.fileName();
    if (m_localDebug) qDebug() << parcoursRecursif(destinationIdUnique);

    QString nomtheme = ui->lineEditNomTheme->text();
    QDir temp(destinationIdUnique); // récupération du chemin du fichier temp
    QString m_fileBase = temp.absolutePath();

    QString destination = m_dirAbe->absolutePath() + QDir::separator() + nomtheme ;
    m_abuledufilev1->abeFileSave(destination, parcoursRecursif(destinationIdUnique), m_fileBase, "abe");
    if (m_localDebug) qDebug() << "Création abe OK";

    /** Supprimer le dossier temporaire*/

    //----------------------- Effacement du dossier Temporaire
    //--------------------------------------------------------

    if(supprimerDir(temp.absolutePath())) qDebug() << "Effacement dossier temp ok";
    else qDebug() << "Suppression impossible";

    // Arrangement graphique
    ui->lineEditNomTheme->clear();
    ui->listWidget->clear();
    ui->listWidgetSelection->clear();
    ui->treeWidget->clear();
    QMessageBox::information(this, trUtf8("Sauvegarder Thème"), trUtf8("Le theme a été sauvegardé avec succès"));
}

/** Contrôle l'insertion d'un item dans un QListWidget
  * Le controle s'effectue sur le parametre data(4) de l'item
  * qui sert à contenir le chemin de l'image
  * @param QListWidget *listWidget, un pointeur sur l'objet QListWidget où l'on veut contrôler l'insertion
  * @param QString dataItem, la chaîne de caractere à controler
  * @return bool, true si non existant, false sinon
  */
bool Editeur::controleDoublonsSelection(QListWidget *listWidget, QString dataItem)
{
    if (listWidget->count() == 0)
        return true;
    else
    {
        for (int i = 0; i < listWidget->count(); i++)
        {
            if (listWidget->item(i)->data(4) == dataItem)
                return false;
        }
    }
    return true;
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
//    int opt_nbMasquesChoisis = 7;
    int opt_nbMasquesLargeur = 10;
    int opt_nbMasquesHauteur = 5;

    float largeurMasque = 0.00;
    float hauteurMasque = 0.00;

    float largeurGv = static_cast<float>(gv_AireParcours->width())-1;
    float hauteurGv = static_cast<float>(gv_AireParcours->height())-1;

    largeurMasque = largeurGv / opt_nbMasquesLargeur;
    hauteurMasque = hauteurGv / opt_nbMasquesHauteur;

    int nbMasques = opt_nbMasquesLargeur * opt_nbMasquesHauteur;
    qreal xMasque = 0.00;
    qreal yMasque = 0.00;

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
            connect(m_masque, SIGNAL(signalSauvegarderParcours()), this, SLOT(sauvegarderParcours()));  // Sauvegarde du parcours
            if(!controlePropertyDepart(m_listeMasques))
            {
                connect(m_masque,SIGNAL(signalMasqueDepart(masqueDeplaceSouris*)), this, SLOT(masqueDepart(masqueDeplaceSouris*)));     // Masque de depart = vert
            }
            connect(m_masque, SIGNAL(signalMasqueArrivee(masqueDeplaceSouris*)), this, SLOT(masqueArrivee(masqueDeplaceSouris*)));  // Masque Arrivee = rouge
            connect(m_masque, SIGNAL(signalMasqueParcours(masqueDeplaceSouris*)), this, SLOT(masqueParcours(masqueDeplaceSouris*)));// Masque Pärcours = noir
            connect(m_masque, SIGNAL(signalMasqueEnlever(masqueDeplaceSouris*)), this, SLOT(masqueEnlever(masqueDeplaceSouris*)));  // Masque Enlever = Fixe = blanc

            xMasque+=largeurMasque;
            gv_AireParcours->scene()->addItem(m_masque);
            m_listeMasques << m_masque;
            numeroMasque++;
        }
        xMasque = 0;
        yMasque += hauteurMasque;
    }
}

void Editeur::on_btnParcours1_clicked()
{
    gv_AireParcours = new AbulEduEtiquettesV1(QPoint(0,0));
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 1"));
//    gv_AireParcours->setGeometry(0,0,800,600);
    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours();
    gv_AireParcours->show();
}

/** Definit le masque comme masque de départ
  */
void Editeur::masqueDepart(masqueDeplaceSouris* masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueDepart()";

    qDebug() << masque->getNumero();

    if (!controlePropertyDepart(m_listeMasques))
    {
        masque->setColor(QColor(Qt::green));
        masque->setProperty("Role", trUtf8("Depart"));
        masque->setMenuDepartEnabled(false);
        foreach(masqueDeplaceSouris* masque,m_listeMasques)
        {
            masque->setMenuDepartEnabled(false);
        }
        masque->update();
    }
}

/** Controle si un masque depart est deja present
  */
bool Editeur::controlePropertyDepart(QList<masqueDeplaceSouris *> maListeMasques)
{
    bool dejaDepart = false;
    int i = 0;
    while (i < maListeMasques.count() && !dejaDepart)
    {
        if (maListeMasques.at(i)->property("Role") == trUtf8("Depart"))
        {
            dejaDepart = true;
        }
        i++;
    }
    return dejaDepart;
}

void Editeur::masqueArrivee(masqueDeplaceSouris *masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueArrivee()";
    qDebug() << masque->getNumero();

    if (!controlePropertyArrivee(m_listeMasques))
    {
        masque->setColor(QColor(Qt::red));
        masque->setProperty("Role", trUtf8("Arrivée"));
        masque->setMenuArriveeEnabled(false);

        foreach(masqueDeplaceSouris* masque,m_listeMasques)
        {
            masque->setMenuArriveeEnabled(false);
        }
        masque->update();
    }
}

bool Editeur::controlePropertyArrivee(QList<masqueDeplaceSouris *> maListeMasques)
{
    bool dejaArrivee = false;
    int i = 0;
    while (i < maListeMasques.count() && !dejaArrivee)
    {
        if (maListeMasques.at(i)->property("Role") == trUtf8("Arrivée"))
        {
            dejaArrivee = true;
        }
        i++;
    }
    return dejaArrivee;
}

void Editeur::masqueParcours(masqueDeplaceSouris *masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueParcours()";
}

void Editeur::masqueEnlever(masqueDeplaceSouris *masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueEnlever()";
}

void Editeur::reinitialiserGvParcours()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::reinitialiserGvParcours()";

    for (int i = 0; i < m_listeMasques.count(); i++)
    {
        m_listeMasques.at(i)->setColor(QColor(Qt::white));
        m_listeMasques.at(i)->setProperty("Role", trUtf8("Fixe"));
        m_listeMasques.at(i)->update();
        m_listeMasques.at(i)->setMenuDepartEnabled(true);
        m_listeMasques.at(i)->setMenuArriveeEnabled(true);
        qDebug() << m_listeMasques.at(i)->getNumero() <<" "<<m_listeMasques.at(i)->property("Role");
    }
    gv_AireParcours->update();
}

void Editeur::sauvegarderParcours()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::sauvegarderParcours()";
    for (int i = 0; i<m_listeMasques.count() ; i++)
    {
        qDebug() << trUtf8("masque n° ") << m_listeMasques.at(i)->getNumero()  <<  "Propriete" << m_listeMasques.at(i)->property("Role");
    }
}


bool Editeur::controleVoisinMasque(masqueDeplaceSouris *masque)
{
//    int numeroMasque = masque->getNumero();

//    foreach (masqueDeplaceSouris monMasque, m_listeMasques) {

//    }

//    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
//        if(fileInfo.isDir()) {
//            //C'est ici que le parcours est récursif
//            resultat << parcoursRecursif(fileInfo.absoluteFilePath());
//        }
//        else {
//            resultat << fileInfo.absoluteFilePath();
//        }
//    }

}
