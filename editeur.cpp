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
            QStringList m_nomDossier = m_dir->absolutePath().split("/"); // la liste des strings contenus dans le chemin absolu moins les "/"
            if(!m_nomDossier.isEmpty()) // on sait jamais, évitons les bugs
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
                item->setText(0, m_nomDossier.back());
                item->setIcon(0,iconDossier);
                item->setData(1, 0, m_dir->absolutePath());
                ui->treeWidget->show();

                m_listeDossiers << m_dir->absolutePath();
            }
            m_nomDossier.clear();
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

void Editeur::on_btnCreationtheme_clicked()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btnCreationtheme_clicked()";

    // Condition de garde = listeWidgetSelection est inferieur à 5
    if (ui->listWidgetSelection->count() < 5)
    {
        QMessageBox::information(this, "Creation d'un Theme", "Veuillez selectionner au minimum 5 images");
        return;
    }

    // Remplissage de la liste de fichier
    for (int i=0; i< ui->listWidgetSelection->count(); i++)
    {
        m_listeFichiers << ui->listWidgetSelection->item(i)->data(4).toString();
    }
//    bool valid  = QFile::copy ("C:/copie.txt", "C:/copieNext.txt");

    m_dir = new QDir(QDir::currentPath());
    qDebug() << m_dir->absolutePath(); // test OK -> "/home/utilisateurs/icham.sirat/mulot/version-1.0"

    // Effacement de l'ancien fichier temp
    m_dir->setFilter(QDir::Dirs);
    if(m_dir->entryInfoList().contains(QString("temp")))
    {
        m_dir->rmdir("temp");
        qDebug() << "Effacement de l'ancien fichier temp";
    }

    // creation du repertoire temp
    if (!m_dir->mkdir("temp"))
    {
        return;
    }
    else // j'ai reussi à creer mon fichier temporaire
    {
        qDebug() << "Creation fichier temporaire ok";
        m_dir->setCurrent(QDir::currentPath()+"/temp");
        qDebug()<< m_dir->absolutePath();

    }

    //    QString nomTheme = "monThemeTest"; // Que l'utilisateur choisira
    //    QString fileBase = "/home/utilisateurs/icham.sirat/Images/";  // +nomTheme;

//    AbulEduFileV1 *toto = new AbulEduFileV1();
//    toto->abeFileSave(nomTheme, m_listeFichiers, fileBase, "abe");

}
























