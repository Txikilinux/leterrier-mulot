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

//---------------------------------------------------------------------------------------------------------------------
//                            ONGLET CREATION THEME
//---------------------------------------------------------------------------------------------------------------------

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
    //    QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Images (*.png *.gif *.jpg *.jpeg)");

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
            qDebug() << "Ce dossier est déjà présent";
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

                // DEBUG
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

    // Ici, au clic sur l'item, j'ai son adresse =)
    // Plus qu'à remplir le widget list des miniatures !
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

/**
  * Cette méthode retourne un identifiant unique pour le fichier temporaire
  * Elle est implémentée pour pallier les risques de sécurité liés au fichier codé en "dur"
  * Utilisation d'un QTemporyFile afin d'avoir un nom unique
  * C'est un fichier créé avec un id imprévisible
  * Je me sers de cet id pour renommer le dossier temp dont j'ai besoin =)
  * note -> le file temp créé s'efface en même tps que la destruction de l'objet
  */
QString Editeur::uniqIDTemp()
{
    QTemporaryFile file;
    QString strArenvoyer ="";
    if (file.open()) {
        strArenvoyer = file.fileName();
        file.close();
    }
    return strArenvoyer;
}

void Editeur::on_btnCreationtheme_clicked()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btnCreationtheme_clicked()";

    //    // Condition de garde = listeWidgetSelection est inferieur à 5
    //    if (ui->listWidgetSelection->count() < 5)
    //    {
    //        QMessageBox::information(this, "Creation d'un Theme", "Veuillez selectionner au minimum 5 images");
    //        return;
    //    }

    //------------------------------------------------------------------------------------------------------------------
    //---------------------------------------- Ok jusque là Commenter juste pour tests ---------------------------------
    //------------------------------------------------------------------------------------------------------------------

    m_listeFichiers.clear(); // Cette liste contient les images choisies pour le thème avec les chemins du dossier temporaire
    QString nomtheme = "Animaux"; // Que l'utilisateur choisira

    //----------------------- Création du dossier Temporaire
    //--------------------------------------------------------

    if (m_localDebug) qDebug() << "tentative de creation de " << uniqIDTemp();

    QString destinationIdUnique = uniqIDTemp(); //je récupère mon Id unique
    QString arborescence = QString("data") + QDir::separator() + QString("images") + QDir::separator() + nomtheme;
    QString cheminImage = destinationIdUnique + QDir::separator() + arborescence ;

    QDir destDir(cheminImage);
    if(destDir.mkpath(cheminImage)) // tentative de création du fichier temp avec un id unique + sous dossier au nom du theme
    {
        if (m_localDebug)
        {
            qDebug() << "Creation ok "
                     << destDir.absolutePath();
        }

        else // si echec pas la peine d'aller plus loin
        {
            return;
        }



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
                if (m_localDebug) qDebug() << "Copie Ok ";
                m_listeFichiers << destDir.absolutePath() + QDir::separator() + originale.fileName();
                if (m_localDebug)
                {
                    qDebug() << "Enregistrement de l'image ds la liste " << m_listeFichiers.at(i);
                }
            }
            else // Si la copie échoue, pas la peine d'aller plus loin
            {
                if (m_localDebug) qDebug() << "Copie impossible";
                return;
            }
        }

        ui->listWidgetSelection->clear();

        //------------------------ Création du .abe
        //--------------------------------------------------------

//        QString m_fileBase = destDir.absolutePath();

//        AbulEduFileV1 *m_theme = new AbulEduFileV1();
//        m_theme->abeFileSave(nomtheme, m_listeFichiers, m_fileBase, "abe");


//        if (m_localDebug) qDebug() << "Création abe OK";





        //----------------------- Effacement du dossier Temporaire
        //--------------------------------------------------------
        // Effacer les fichiers contenus dans Temp
        //    destDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        //    QFileInfoList fichiersASupp = destDir.entryInfoList();
        //    foreach( QFileInfo fichier, fichiersASupp )
        //    {
        //        qDebug() << fichier.fileName();
        //        if(QFile::remove(fichier.absoluteFilePath()))
        //            qDebug() << "Suppression OK";
        //        else
        //            qDebug() << "Suppression impossible";
        //    }
        //    // Effacer le dossier
        //    if(destDir.rmdir(destDir.absolutePath()))
        //        qDebug() << "Effacement dossier temp ok";

        // De façon récursive maintenant
        QDir temp(destinationIdUnique);
        if(supprimerDir(temp.absolutePath()))
            qDebug() << "Effacement dossier temp ok";
        else
            qDebug() << "Suppression impossible";
    }
}

bool Editeur::supprimerDir(const QString& dirPath) //dirPath = le chemin du répertoire à supprimer, ex : "/home/user/monRepertoire")
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
