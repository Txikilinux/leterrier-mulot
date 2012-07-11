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


    QList<QAction *> actions;

// ------- Action Menu Contextuel Tree Widget ------------------------------------------------------------
    m_menuTreeWidget = new QMenu(ui->treeWidget);
    QAction *a_supprimer = new QAction(trUtf8("&Supprimer..."),m_menuTreeWidget);
    QStyle* style =  QApplication::style();
    QIcon iconSupprimer = style->standardIcon(QStyle::SP_DialogResetButton); //On récupère l'icône désiré
    a_supprimer->setIcon(iconSupprimer);
    a_supprimer->setIconVisibleInMenu(true);
    a_supprimer->connect(a_supprimer, SIGNAL(triggered()), this, SLOT(on_action_Supprimer_album_triggered()));

    actions << /*a_nouveau << a_renommer <<*/ a_supprimer;
    m_menuTreeWidget->addActions(actions);
//----------------------------------------------------------------------------------------------------------

// ------- Action Menu Contextuel Tree Widget --------------------------------------------------------------
   m_menuListWidget = new QMenu(ui->listWidget);
   QAction *a_supprimer2 = new QAction(trUtf8("&Supprimer..."),m_menuListWidget);
//   QStyle* style =  QApplication::style();
//   QIcon iconSupprimer = style->standardIcon(QStyle::SP_DialogResetButton); //On récupère l'icône désiré
   a_supprimer2->setIcon(iconSupprimer);
   a_supprimer2->setIconVisibleInMenu(true);
   a_supprimer2->connect(a_supprimer2, SIGNAL(triggered()), this, SLOT(on_action_Supprimer_photo_triggered()));

   actions << /*a_nouveau << a_renommer <<*/ a_supprimer;
   m_menuTreeWidget->addActions(actions);

}

Editeur::~Editeur()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::~Editeur()";
    delete ui;
}

void Editeur::on_action_Supprimer_album_triggered()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_action_Supprimer_album_triggered()";

    QTreeWidgetItem *item = ui->treeWidget->currentItem(); // je recupere l'item

    // Le supprimer de la liste de dossier
    QString nom = item->data(1,0).toString();
    qDebug() << nom;
    m_listeDossiers.removeOne(nom);

    if (m_localDebug)
    {
        for (int i=0; i< m_listeDossiers.count(); i++)
            qDebug() << m_listeDossiers.at(i);
    }

    // Supprimer l'item du TreeWidget
    int index = ui->treeWidget->indexOfTopLevelItem(item);
    ui->treeWidget->takeTopLevelItem(index);
    ui->treeWidget->update();
    ui->listWidget->clear(); // nettoyage de la listeWidget

    // Supprimer la liste de fichier correspondante
    m_listeFichiers.clear();
}

void Editeur::on_action_Supprimer_photo_triggered()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_action_Supprimer_photo_triggered()";

    QListWidgetItem *item = ui->listWidget->currentItem();

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
        m_listeFichiers << listeFichiers.at(i).absoluteFilePath();
        if (m_localDebug)
            qDebug() << "chemin des fichiers contenus dans le dossier" << m_listeFichiers[i];
        rafraichirListeImages();
    }
    m_listeFichiers.clear();
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
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
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
        qDebug()<< "pas d'item";
    }
}

void Editeur::on_listWidget_itemDoubleClicked(QListWidgetItem *item) // Ouverture de la visionneuse
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_listWidget_itemDoubleClicked(QListWidgetItem *item)";

    item = ui->listWidget->currentItem();
    qDebug() << item->data(0) << "" << item->data(4);

    if (m_localDebug) qDebug() << "Ouverture de l'image";
    m_visionneuseImage = new VisionneuseImage(this);
    m_visionneuseImage->ouvrirFicher(item->data(4).toString());

    m_visionneuseImage->setWindowModality(Qt::WindowModal);
    m_visionneuseImage->show();
}

void Editeur::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    if (ui->listWidget->itemAt(pos) != NULL)
    {
        qDebug() << "J'appelle mon menu ";
    }
    else
    {
        qDebug() << "J'ai cliqué lààààà !!!!";
    }
}

void Editeur::on_btSelection_clicked()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_btSelection_clicked()";

    for (int i =0; i<ui->listWidget->count();i++)
    {

        if(ui->listWidget->item(i)->checkState() == 2)
        {
            qDebug() << "cet item est selectionné";
            QListWidgetItem *item = ui->listWidget->item(i)->clone();

            ui->listWidgetSelection->insertItem(0, item );
        }
        else if (ui->listWidget->item(i)->checkState() == 0)
        {
            qDebug() << "item non selectionné";
        }

    }
}
