#include "editeur.h"
#include "ui_editeur.h"

Editeur::Editeur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );

    m_localDebug = true;

    ui->treeWidget->setAlternatingRowColors(true);
    ui->treeWidget->setColumnCount(1);

    m_menuTreeWidget = new QMenu(ui->treeWidget);
    QList<QAction *> actions;


    QAction *a_supprimer = new QAction(trUtf8("&Supprimer..."),m_menuTreeWidget);
    QStyle* style =  QApplication::style();
    QIcon iconSupprimer = style->standardIcon(QStyle::SP_DialogResetButton); //On récupère l'icône désiré
    a_supprimer->setIcon(iconSupprimer);
    a_supprimer->setIconVisibleInMenu(true);
    a_supprimer->connect(a_supprimer, SIGNAL(triggered()), this, SLOT(on_action_Supprimer_album_triggered()));

    actions << /*a_nouveau << a_renommer <<*/ a_supprimer;

    m_menuTreeWidget->addActions(actions);
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
        ui->listWidget->insertItem(i,item);
    }

}

void Editeur::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_menuTreeWidget->exec(ui->treeWidget->mapToGlobal(pos));
}


