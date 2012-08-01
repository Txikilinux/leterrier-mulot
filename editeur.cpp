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
    /// Parametres Survol
    parametres.setValue("Survol/timerSuivant", (ui->spinBoxSurvolSuivant->value()*1000));
    parametres.setValue("Survol/timerVerifier", (ui->spinBoxSurvolVerifier->value()*1000));
    parametres.setValue("Survol/nbMasquesChoisis", (ui->spinBoxSurvolMasque->value()));
    /// Parametres Clic
    parametres.setValue("Clic/timerSuivant", (ui->spinBoxClicSuivant->value()*1000));
    parametres.setValue("Clic/timerVerifier", (ui->spinBoxClicVerifier->value()*1000));
    parametres.setValue("Clic/nbMasquesChoisis", (ui->spinBoxClicMasque->value()));
    /// Parametres Double-Clic
    parametres.setValue("Double-Clic/timerSuivant", (ui->spinBoxDoubleClicSuivant->value()*1000));
    parametres.setValue("Double-Clic/timerVerifier", (ui->spinBoxDoubleClicVerifier->value()*1000));
    parametres.setValue("Double-Clic/nbMasquesChoisis", (ui->spinBoxDoubleClicMasque->value()));
    /// Parametres Parcours1
    parametres.setValue("Parcours1/timerSuivant", (ui->spinBoxParcoursSuivant->value()*1000));
    parametres.setValue("Parcours1/timerVerifier", (ui->spinBoxParcoursVerifier->value()*1000));
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
    parametres.setValue("Parcours2/timerVerifier", (ui->spinBoxParcoursVerifier->value()*1000));
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
    parametres.setValue("Parcours3/timerVerifier", (ui->spinBoxParcoursVerifier->value()*1000));
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
    parametres.setValue("Parcours4/timerVerifier", (ui->spinBoxParcoursVerifier->value()*1000));
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
    parametres.setValue("Parcours5/timerVerifier", (ui->spinBoxParcoursVerifier->value()*1000));
    parametres.setValue("Parcours5/nbMasquesLargeur", (ui->spinBoxParcoursMasquesLargeur->value()));
    parametres.setValue("Parcours5/nbMasquesHauteur", (ui->spinBoxParcoursMasqueHauteur->value()));
    parametres.setValue("Parcours5/nbMasquesChoisis", (ui->spinBoxParcoursMasque->value()));
    // Parcours
    QMapIterator<QString, QVariant> m(m_parametresParcours5);
    while (m.hasNext()) {
        m.next();
        parametres.setValue("position5/"+ m.key(), m.value());
    }

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
            connect(m_masque,SIGNAL(signalMasqueDepart(masqueDeplaceSouris*)), this, SLOT(masqueDepart(masqueDeplaceSouris*)));     // Masque de depart = vert
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

/** Definit le masque comme masque de départ
  */
void Editeur::masqueDepart(masqueDeplaceSouris* masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueDepart()";

    if (!m_listeMasquesParcours.isEmpty())
    {
        qDebug() << "la listeMasquesParcours contient qqchose !, impossible de mettre un départ";
    }
    else // ma liste est vide, je mets ce masque en depart et je le range ds listeMasquesParcours
    {
        masque->setColor(QColor(Qt::green));
        masque->setProperty("Role", trUtf8("Depart"));
        masque->setMenuDepartEnabled(false);
        masque->setMenuEnleverEnabled(true); // on peut enlever que les masques ayant un role
        masque->update();
        m_listeMasquesParcours << masque;

        // et je desactive le menu Depart pour les autres
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            var_masque->setMenuDepartEnabled(false);
            //            var_masque->setMenuParcoursEnabled(true);
        }

        // ...& j'active le menu parcours, enlever pour les masques voisins seulement
        QList<int> voisins = masquesVoisins(masque->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            for (int i =0 ; i < voisins.count(); i++)
            {
                if(var_masque->getNumero() == voisins.at(i))
                {
                    var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                    var_masque->setMenuParcoursEnabled(true); // j'active le menu parcours
                    var_masque->update();
                }
            }
        }
    }
}

/** Definit le masque comme masque de parcours
  */
void Editeur::masqueParcours(masqueDeplaceSouris *masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueParcours()";

    // S'il y a des masques de couleur grise... les remettre en blanc
    foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
    {
        if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
        {
            var_masque->setColor(QColor(Qt::white));
            var_masque->update();
            var_masque->setMenuParcoursEnabled(false); // et desactiver "Parcours" pour les autres
        }
    }

    if( m_listeMasquesParcours.count() < (opt_nbMasquesChoisisParcours-1) ) // -1 car dans ma liste, à ce moment j'ai deja un masque (depart)
    {
        masque->setColor(QColor(Qt::black));
        masque->setProperty("Role", trUtf8("Parcours"));
        masque->setMenuParcoursEnabled(false);
        masque->update();
    }

    // ...& j'active le menu parcours, enlever pour les masques voisins seulement
    QList<int> voisins = masquesVoisins(masque->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);

    foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
    {
        for (int i =0 ; i < voisins.count(); i++)
        {
            if(var_masque->getNumero() == voisins.at(i))
            {
                if(var_masque->property("Role") == "Fixe") // sauf mes masques deja mis
                {
                    var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                    var_masque->setMenuParcoursEnabled(true); // j'active le menu parcours
                    var_masque->update();
                }
            }
        }
    }

    // je desactive le menu "Enlever" pour les masques de parcours d'avant, on enleve les masques que dans l'ordre opposé à leur placement
    foreach (masqueDeplaceSouris* var_masque , m_listeMasquesParcours)
    {
        var_masque->setMenuEnleverEnabled(false);
    }

    m_listeMasquesParcours << masque;
    masque->setMenuEnleverEnabled(true); // on peut enlever que les masques ayant un role

    /// jusqu'ici je suis OK

    // Mise en place du menu Arrivee
    if (m_listeMasquesParcours.count() == (opt_nbMasquesChoisisParcours-1))
    {
        // J'active le menu arrivee que pour les voisins du dernier masque de parcours
        QList<int> voisins = masquesVoisins(masque->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);

        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            for (int i =0 ; i < voisins.count(); i++)
            {
                if(var_masque->getNumero() == voisins.at(i))
                {
                    if(var_masque->property("Role") == "Fixe") // sauf mes masques deja mis
                    {
                        var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                        var_masque->setMenuArriveeEnabled(true); // j'active le menu "arrivee"
                        var_masque->setMenuParcoursEnabled(false);
                        var_masque->update();
                    }
                }
            }
        }
    }
}

/** Definit le masque comme masque d'Arrivee
  */
void Editeur::masqueArrivee(masqueDeplaceSouris *masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueArrivee()";

    //controle de la listeMAsquesParcours et verifier qu'elle est égale au nb de masques choisis -1
    if (m_listeMasquesParcours.count() == (opt_nbMasquesChoisisParcours-1))
    {
        // S'il y a des masques de couleur grise... les remettre en blanc
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
            {
                var_masque->setColor(QColor(Qt::white));
                var_masque->update();
                var_masque->setMenuParcoursEnabled(false); // et desactiver "Parcours" pour les autres
            }
        }

        // PRopriete Arrivee
        masque->setColor(QColor(Qt::red));
        masque->setProperty("Role", trUtf8("Arrivee"));
        masque->setMenuArriveeEnabled(false);

        // je desactive le menu "Enlever" pour les masques de parcours d'avant, on enleve les masques que dans l'ordre opposé à leur placement
        foreach (masqueDeplaceSouris* var_masque , m_listeMasquesParcours)
        {
            var_masque->setMenuEnleverEnabled(false);
        }

        m_listeMasquesParcours << masque;
        masque->setMenuEnleverEnabled(true); // on peut enlever que le dernier masque mis
        masque->update();


        // m_listeMasquesParcours est égale au nb de masque choisi
        // je desactive le menu Arrivee
        // et j'active le menu sauvegarde
        if (m_listeMasquesParcours.count() == opt_nbMasquesChoisisParcours)
        {
            foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
            {
                var_masque->setMenuArriveeEnabled(false);
                var_masque->setMenuSauvegarderEnabled(true);
            }
        }
    }
}

void Editeur::masqueEnlever(masqueDeplaceSouris *masque)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::masqueEnlever()";

    qDebug() << "ma liste avant";
    for (int i = 0; i < m_listeMasquesParcours.count(); i++)
    {
        qDebug() << m_listeMasquesParcours.at(i)->property("Role");
    }

    // Je n'enleve que le dernier masque posé
    m_listeMasquesParcours.removeLast();

    /// Masque Depart
    if (masque->property("Role") == "Depart")
    {
        reinitialiserGvParcours();
    }

    /// Masque Parcours
    if (masque->property("Role") == "Parcours")
    {
        // Remise à défaut
        masque->setProperty("Role", trUtf8("Fixe"));
        masque->setColor(QColor(Qt::white));
        masque->update();

        // S'il y a des masques de couleur grise... les remettre en blanc
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
            {
                var_masque->setColor(QColor(Qt::white));
                var_masque->update();
                var_masque->setMenuParcoursEnabled(false); // et desactiver "Parcours" pour les autres
            }
        }

        // Reaffichage des masques possibles par rapport au dernier masque de ma listeParcours
        QList<int> voisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            for (int i =0 ; i < voisins.count(); i++)
            {
                if(var_masque->getNumero() == voisins.at(i))
                {
                    if(var_masque->property("Role") == "Fixe") // sauf mes masques deja mis
                    {
                        var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                        //                       var_masque->setMenuArriveeEnabled(false); // j'active le menu "arrivee"
                        var_masque->setMenuParcoursEnabled(true);
                        var_masque->update();
                    }
                }
            }
        }
        // Remise du menu "Enlever" au dernier masque de ma liste
        m_listeMasquesParcours.back()->setMenuEnleverEnabled(true);

    }

    /// Masque Arrivee
    if (masque->property("Role") == "Arrivee")
    {
        // Remise à défaut
        masque->setProperty("Role", trUtf8("Fixe"));
        masque->setColor(QColor(Qt::white));
        masque->update();

        // S'il y a des masques de couleur grise... les remettre en blanc
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
            {
                var_masque->setColor(QColor(Qt::white));
                var_masque->update();
                var_masque->setMenuParcoursEnabled(false); // et desactiver "Parcours" pour les autres
            }
        }

        // Reaffichage des masques possibles par rapport au dernier masque de ma listeParcours
        QList<int> voisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
        foreach(masqueDeplaceSouris* var_masque,m_listeMasques)
        {
            for (int i =0 ; i < voisins.count(); i++)
            {
                if(var_masque->getNumero() == voisins.at(i))
                {
                    if(var_masque->property("Role") == "Fixe") // sauf mes masques deja mis
                    {
                        var_masque->setColor(QColor(Qt::gray)); // je coloris les voisins en gris
                        var_masque->setMenuArriveeEnabled(true); // j'active le menu "arrivee"
                        var_masque->update();
                    }
                }
            }
        }
        // Remise du menu "Enlever" au dernier masque de ma liste
        m_listeMasquesParcours.back()->setMenuEnleverEnabled(true);
    }

    if (m_localDebug) qDebug() << "ma liste apres";
    for (int i = 0; i < m_listeMasquesParcours.count(); i++)
    {
        if(m_localDebug) qDebug() << m_listeMasquesParcours.at(i)->property("Role");
    }
}

void Editeur::reinitialiserGvParcours()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::reinitialiserGvParcours()";

    // Remettre tout mes masques d'origine !
    for (int i = 0; i < m_listeMasques.count(); i++)
    {
        m_listeMasques.at(i)->setColor(QColor(Qt::white));
        m_listeMasques.at(i)->setProperty("Role", trUtf8("Fixe"));

        // Reactiver que le menu depart
        m_listeMasques.at(i)->setMenuDepartEnabled(true);
        m_listeMasques.at(i)->setMenuArriveeEnabled(false);
        m_listeMasques.at(i)->setMenuParcoursEnabled(false);
        m_listeMasques.at(i)->setMenuSauvegarderEnabled(false);
        m_listeMasques.at(i)->setMenuEnleverEnabled(false);

        m_listeMasques.at(i)->update();
    }
    // vider ma listeMasquesParcours
    m_listeMasquesParcours.clear();
    gv_AireParcours->update();
}

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

    // Desactivation des menus parcours et arrivee tant qu'il n'y a pas de départ et du menu sauvegarde
    foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
    {
        var_masque->setMenuArriveeEnabled(false);
        var_masque->setMenuParcoursEnabled(false);
        var_masque->setMenuSauvegarderEnabled(false);
        var_masque->setMenuEnleverEnabled(false);
    }
}

void Editeur::on_btnParcours2_clicked()
{
    m_numeroParcours = 2;
    m_listeMasquesParcours.clear();

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

    // Desactivation des menus parcours et arrivee tant qu'il n'y a pas de départ et du menu sauvegarde
    foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
    {
        var_masque->setMenuArriveeEnabled(false);
        var_masque->setMenuParcoursEnabled(false);
        var_masque->setMenuSauvegarderEnabled(false);
        var_masque->setMenuEnleverEnabled(false);
    }
}

void Editeur::on_btnParcours3_clicked()
{
    m_numeroParcours = 3;
    m_listeMasquesParcours.clear();

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

    // Desactivation des menus parcours et arrivee tant qu'il n'y a pas de départ et du menu sauvegarde
    foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
    {
        var_masque->setMenuArriveeEnabled(false);
        var_masque->setMenuParcoursEnabled(false);
        var_masque->setMenuSauvegarderEnabled(false);
        var_masque->setMenuEnleverEnabled(false);
    }
}

void Editeur::on_btnParcours4_clicked()
{
    m_numeroParcours = 4;
    m_listeMasquesParcours.clear();

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

    // Desactivation des menus parcours et arrivee tant qu'il n'y a pas de départ et du menu sauvegarde
    foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
    {
        var_masque->setMenuArriveeEnabled(false);
        var_masque->setMenuParcoursEnabled(false);
        var_masque->setMenuSauvegarderEnabled(false);
        var_masque->setMenuEnleverEnabled(false);
    }
}

void Editeur::on_btnParcours5_clicked()
{
    m_numeroParcours = 5;
    m_listeMasquesParcours.clear();

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

    // Desactivation des menus parcours et arrivee tant qu'il n'y a pas de départ et du menu sauvegarde
    foreach(masqueDeplaceSouris* var_masque, m_listeMasques)
    {
        var_masque->setMenuArriveeEnabled(false);
        var_masque->setMenuParcoursEnabled(false);
        var_masque->setMenuSauvegarderEnabled(false);
        var_masque->setMenuEnleverEnabled(false);
    }
}
