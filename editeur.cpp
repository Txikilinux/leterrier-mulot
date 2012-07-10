#include "editeur.h"
#include "ui_editeur.h"

#include "QFileDialog"
#include <QMessageBox>




Editeur::Editeur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );

    m_localDebug = true;

    ui->treeWidget->setAlternatingRowColors(true);
    ui->treeWidget->setColumnCount(1);

}

Editeur::~Editeur()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::~Editeur()";
    delete ui;
}


void Editeur::on_btnImporterDossierImage_clicked()
{
    if(m_localDebug) qDebug() << "##########################  Appuie sur le bouton d'Importation des images";
    //    QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Images (*.png *.gif *.jpg *.jpeg)");

    QString dossier = QFileDialog::getExistingDirectory(this);
    if(m_localDebug) qDebug() << dossier;

    // Afficher le nom du dossier et de ses sous dossiers
    m_dir = new QDir(dossier);

    m_dir->setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList list = m_dir->entryInfoList();


    if (!m_listeDossiers.contains(m_dir->absolutePath(), Qt::CaseSensitive)) // si ma liste de dossiers ne contient pas ce dossier, je l'ajoute
    {
        m_listeDossiers << m_dir->absolutePath();

        if (list.isEmpty()) // il n'y a pas de ss dossiers, donc le nom du dossier courant est affiché dans item
        {
            QStringList m_nomDossier = m_dir->absolutePath().split("/"); // la liste des strings contenus dans le chemin absolu moins les "/"
            if(!m_nomDossier.isEmpty()) // on sait jamais, évitons les bugs
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
                item->setText(0, m_nomDossier.back());
                item->setData(1, 0, m_dir->absolutePath());
                ui->treeWidget->show();
                if (m_localDebug) qDebug() << item;

                m_listeDossiers << m_dir->absolutePath();
            }
            m_nomDossier.clear();

        }
        else // on parcourt les ss dossiers et on les affiche
        {
            for(int i = 0; i < list.count(); i++)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
                item->setText(0, list.at(i).fileName());
                item->setData(1, 0, list.at(i).absoluteFilePath());
                ui->treeWidget->addTopLevelItem(item);
                ui->treeWidget->show();

                m_listeDossiers << list.at(i).absoluteFilePath();
                // DEBUG
                if (m_localDebug) qDebug() << item->data(1,0) << "" << list.at(i).fileName() << "" << list.at(i).absoluteFilePath();
            }
        }
    } // FIN if contains

    else // ma liste de dossiers contient ce dossier, je ne l'ajoute pas
    {
        // Ce dossier est déjà présent
        qDebug() << "Ce dossier est déjà présent";
    }

    // DEBUG  Parcours de la liste des dossiers
    if (m_localDebug)
    {
        for (int i=0; i< m_listeDossiers.count(); i++)
            qDebug() << m_listeDossiers.at(i);
    }
}

void Editeur::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    if (m_localDebug) qDebug() << "##########################  Editeur::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)";
    // Quand je clique sur un item (dossier), parcours du dossier et affichage de toutes les images dans la listWidget

    //    item = ui->treeWidget->currentItem();
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
        ui->listWidget->insertItem(i,item);
    }

}
