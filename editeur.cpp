/** Classe Editeur
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
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

Editeur::Editeur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    //    setAttribute(Qt::WA_ShowModal);

    m_parent = (MainWindow*) parent;
    connect(m_parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),this,SLOT(show()));
    connect(m_parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),m_parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur(),SLOT(hide()));

    if(!m_parent->abeGetMyAbulEduFile()->abeFileGetFileName().baseName().isEmpty())
    {
        m_abuleduFile = m_parent->abeGetMyAbulEduFile();
        ui->btnModificationCourant->setText(trUtf8("Editer le module ")+"\n"+m_abuleduFile->abeFileGetFileName().fileName());
        ui->btnModificationCourant->setEnabled(true);
        ui->btnModificationCourant->setMinimumHeight(60);
        ui->btnModificationAutre->setMinimumHeight(60);
        ui->btnCreationAbe->setMinimumHeight(60);
    }
    else
    {
        m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
        ui->btnModificationCourant->setEnabled(false);
        m_parent->abeSetMyAbulEduFile(m_abuleduFile);
    }

    m_abuleduFileManager = new AbulEduBoxFileManagerV1();
    m_abuleduFileManager->abeSetFile(m_abuleduFile);
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected()),this, SLOT(slotOpenFile()));

    m_lastOpenDir = QDir::homePath();

    ui->abuleduMediathequeGet->abeSetSource("data");
    ui->abuleduMediathequeGet->abeSetCustomBouton1(trUtf8("Importer l'image"));
    ui->abuleduMediathequeGet->abeHideBoutonTelecharger();
    ui->abuleduMediathequeGet->abeCustomBouton1SetDownload(true);
    ui->abuleduMediathequeGet->abeHideInfoPanel(true);
    ui->abuleduMediathequeGet->abeSetDefaultView(AbulEduMediathequeGetV1::abeMediathequeThumbnails);

    connect(ui->abuleduMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(int)), this, SLOT(slotImportImageMediatheque()));

    QShortcut *shortcutSupprimeChoix = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listWidgetImagesSelection, 0, 0, Qt::WidgetShortcut);
    connect(shortcutSupprimeChoix, SIGNAL(activated()), this, SLOT(slotSupprimerImage()),Qt::UniqueConnection);

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
    on_cbChoixEnregistrement_currentIndexChanged(0);

    setAcceptDrops(true);

    creationMenu();

    ui->cbLangueRessource->addItems(m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().values());

    // Affichage de la mediatheque par defaut
    ui->tabWidgetImages->setCurrentWidget(ui->pageMediatheque);

    if(m_abuleduFile->abeFileGetDirectoryTemp().mkpath("data/images")) {
        if(m_localDebug) qDebug() << "Creation ok";
    }
    else {
        return;
    } // si echec pas la peine d'aller plus loin
}

Editeur::~Editeur()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__ << m_abuleduFile->abeFileGetFileName().baseName();
    delete ui;
    delete m_abuleduFileManager;
    m_parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur()->hide();
    emit editorExited();
}

void Editeur::creationMenu()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
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

void Editeur::slotSupprimerImage()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    if (m_listeFichiersImages.isEmpty()) // condition garde meme si j'appelle ce slot que si j'ai un item ds ma listView, donc une liste avec au moins 1 éléments =)
    {return;}

    if (ui->listWidgetImagesSelection->selectedItems().isEmpty()) // Garde
    { return;}

    if (m_localDebug) qDebug() << "Suppression ItemImage -> liste images avant : ";
    for (int i = 0; i < m_listeFichiersImages.count(); i++)
    {
        if (m_localDebug) qDebug() << i <<" "<<m_listeFichiersImages.at(i);
    }

    for (int i = 0; i < ui->listWidgetImagesSelection->selectedItems().count(); i++)// Suppression de ma liste d'images
    {
        m_listeFichiersImages.removeOne(ui->listWidgetImagesSelection->selectedItems().at(i)->data(4).toString());
        // suppression du fichier image dans temp
        if (QFile::remove(ui->listWidgetImagesSelection->selectedItems().at(i)->data(4).toString()))
        {
            if (m_localDebug)qDebug() << "suppression image du fichier temp ok";
        }
    }

    foreach(QListWidgetItem *i, ui->listWidgetImagesSelection->selectedItems())// Suppression des items selectionné
    {
        delete i;
    }

    if (m_localDebug) qDebug() << "Suppression ItemImage -> liste images apres : ";
    for (int i = 0; i < m_listeFichiersImages.count(); i++)
    {
        if (m_localDebug) qDebug() << i <<" "<<m_listeFichiersImages.at(i);
    }
}

void Editeur::ajouterImage(QFileInfo monFichier)
{
    if (m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() << "ajouterImage -> Chemin du fichier selectionné : " << monFichier.absoluteFilePath() <<" Nom du fichier : " << monFichier.fileName();
    }

    if (m_listeFichiersImages.contains(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg")) // Controle des insertions (éviter les doublons)
    {
        if(m_localDebug) qDebug() << "Fichier deja présent";
        return;
    }
    else
    {
        if (m_abuleduFile->resizeImage(&monFichier, 1024,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" ))
        {
            m_listeFichiersImages << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() +".jpg"; // je range le chemin de l'image dans ma liste (celui du fichier temp)
            // Insertion dans mon listWidget
            QListWidgetItem *item = new QListWidgetItem();
            QIcon icone(monFichier.absoluteFilePath());
            item->setIcon(icone);
            item->setText(monFichier.baseName());
            item->setData(4, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+ "/data/images/" + monFichier.baseName() + ".jpg");
            ui->listWidgetImagesSelection->insertItem(0, item);
            qDebug() << m_listeFichiersImages;
        }
    }
}

void Editeur::slotImportImageMediatheque()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    ajouterImage(ui->abuleduMediathequeGet->abeGetFile()->abeFileGetContent(0));

    //Ajout des metadata
    m_abuleduFile->abeFileAddMetaDataFromABB(ui->abuleduMediathequeGet->abeGetLOM(), ui->abuleduMediathequeGet->abeGetFile()->abeFileGetContent(0).baseName());
}

void Editeur::on_listWidgetImagesSelection_customContextMenuRequested(const QPoint &pos)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    if (ui->listWidgetImagesSelection->itemAt(pos) != NULL) // j'ai un item à cet endroit, j'appelle mon menu
    {
        m_menuListWidget->exec(ui->listWidgetImagesSelection->mapToGlobal(pos));
    }
    else // sinon je fais rien
    {
        if(m_localDebug) qDebug() << "Pas d'item";
    }
}

void Editeur::on_listWidgetImagesSelection_itemDoubleClicked(QListWidgetItem *item)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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

void Editeur::createAbe()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    ui->listWidgetImagesSelection->clear();

    if (preparerSauvegarde())
    {
        AbulEduBoxFileManagerV1 *SaveAbuleduFileManager = new AbulEduBoxFileManagerV1(0,AbulEduBoxFileManagerV1::abeSave);
        SaveAbuleduFileManager->abeSetFile(m_abuleduFile);
        SaveAbuleduFileManager->show();
        close();
    }
}

bool Editeur::supprimerDir(const QString& dirPath)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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

QStringList Editeur::parcoursRecursif(const QString dossier)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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

void Editeur::remplirGvParcours(int numeroParcours)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    switch(numeroParcours)
    {
    case 1:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_1->value();
        break;
    case 2:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_2->value();
        break;
    case 3:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_3->value();
        break;
    case 4:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_4->value();
        break;
    case 5:
        m_opt_nbMasquesChoisisParcours = ui->spinBoxParcoursMasque_5->value();
        break;
    }

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
    {
        qDebug()<<" -------------------------- Début boucle d'affichage : " << nbMasques;
        qDebug() << "Chargement position parcours : " << numeroParcours;
    }
    chargerPositionMasque(numeroParcours);

    int numeroMasque = 0;
    for (float i=0; i<m_opt_nbMasquesHauteur;i++)
    {
        for (int j =0; j < m_opt_nbMasquesLargeur;j++)
        {
            m_masque = new MasqueDeplaceSouris(0, numeroMasque);
            m_masque->setSize(largeurMasque, hauteurMasque);
            m_masque->setPos(xMasque, yMasque);

            m_masque->setColor(QColor::fromRgb(255,255,255));
            m_masque->setProperty("Role", trUtf8("Fixe"));

            m_masque->setHideOnMouseOver(false);
            m_masque->setIsEditable(true);

            connect(m_masque, SIGNAL(signalPoseSurParcours(MasqueDeplaceSouris*)), this, SLOT(masquePoseParcours(MasqueDeplaceSouris*)));

            xMasque+=largeurMasque;
            gv_AireParcours->getScene()->addItem(m_masque);

            m_listeMasques << m_masque;
            numeroMasque++;
        }
        xMasque = 0;
        yMasque += hauteurMasque;
    }

    /// Gestion de la taille de la Scene
    gv_AireParcours->setGeometry((gv_AireParcours->x()), (gv_AireParcours->y()),
                                 (largeurMasque * m_opt_nbMasquesLargeur) + gv_AireParcours->getGraphicsView()->verticalScrollBar()->width(),
                                 (hauteurMasque * m_opt_nbMasquesHauteur) + gv_AireParcours->getGraphicsView()->horizontalScrollBar()->height() + (gv_AireParcours->getBoutonHeight()) *2);

    /// Mode Modification
    if(m_modeModificationAbe)
    {
        /// Parcours de la QMap positionMasqueParcours
        int positionDepart  = 0;
        int positionArrivee = 0;
        QList<int> positionParcours;
        positionParcours.clear();
        QMap<QString, int>::const_iterator i = positionMasquesParcours.constBegin();
        while (i != positionMasquesParcours.constEnd())
        {
            if(m_localDebug) qDebug() << i.key() << " " << i.value();
            if (i.key() == "MasqueDepart")
                positionDepart = i.value();
            else if (i.key() == "MasqueArrivee")
                positionArrivee = i.value();
            else
                positionParcours << i.value();
            i ++;
        }
        if (m_localDebug)
        {
            qDebug() << "La liste des positions normales : " << positionMasquesParcours;
            qDebug() << "Position Depart/Arrivee         : " << positionDepart << "/" << positionArrivee;
            qDebug() << "Position Parcours               : " << positionParcours;
        }

        /// ****************************************************
        /// MODE MODIFICATION
        /// Ici on a toutes les positions necessaires, plus qu'à les mettre dans l'ordre : depart, parcours, arrivee
        //depart
        m_listeMasques.at(positionDepart)->setColor(QColor(Qt::green));
        m_listeMasques.at(positionDepart)->setProperty("Role", trUtf8("Depart"));
        m_listeMasquesParcours << m_listeMasques.at(positionDepart);

        // parcours
        while(!positionParcours.isEmpty())
        {
            m_listeMasques.at(positionParcours.first())->setColor(QColor(Qt::black));
            m_listeMasques.at(positionParcours.first())->setProperty("Role", trUtf8("Parcours"));
            m_listeMasquesParcours << m_listeMasques.at(positionParcours.first());
            positionParcours.removeFirst();
        }

        //arrivee
        m_listeMasques.at(positionArrivee)->setColor(QColor(Qt::red));
        m_listeMasques.at(positionArrivee)->setProperty("Role", trUtf8("Arrivee"));
        m_listeMasquesParcours << m_listeMasques.at(positionArrivee);

        // Et j'active le menu Sauvegarder
        gv_AireParcours->connectBtnSave(true);
    }
    /// Mode Creation
    else
    {
        gv_AireParcours->connectBtnSave(false);
    }
}

void Editeur::masquePoseParcours(MasqueDeplaceSouris* masque)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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
        /// Liste avec un masque = Parcours
        else if(m_listeMasquesParcours.count() < (m_opt_nbMasquesChoisisParcours - 1))
        {
            listeVoisins.clear();
            // Je vais chercher les voisins du dernier masque posé
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);

            if (listeVoisins.contains(masque->getNumero())) // Si le numero de ce masque est contenu ds mas liste de voisins = Ok
            {
                // S'il y a des masques de couleur grise... les remettre en blanc
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

                // ...& je coloris en gris les voisins
                listeVoisins = masquesVoisins(masque->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
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
                foreach(MasqueDeplaceSouris* var_masque,m_listeMasques)
                {
                    if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
                    {
                        var_masque->setColor(QColor(Qt::white));
                        var_masque->update();
                    }
                }
                // Et j'active le menu Sauvegarder
                gv_AireParcours->connectBtnSave(true);
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
        foreach(MasqueDeplaceSouris* var_masque,m_listeMasques)
        {
            if(var_masque->getColor().value() == QColor(Qt::gray).value()) //gris getColor().value = 164;
            {
                var_masque->setColor(QColor(Qt::white));
                var_masque->update();
            }
        }
        // Et je desactive le menu Sauvegarder
        gv_AireParcours->connectBtnSave(false);
        // et j'enleve ce masque de ma liste parcours
        m_listeMasquesParcours.removeLast();
        // Remise des masques gris pour les voisins du dernier masque (je viens d'enlever CE masque juste avant)
        // ...& je coloris en gris les voisins sauf si j'enleve depart donc pas de colriage en gris
        if (!m_listeMasquesParcours.isEmpty())
        {
            listeVoisins = masquesVoisins(m_listeMasquesParcours.back()->getNumero(), m_opt_nbMasquesLargeur, m_opt_nbMasquesHauteur);
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

void Editeur::reinitialiserGvParcours()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

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

void Editeur::sauvegarderParcours()
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() << "Numero de parcours :" << m_numeroParcours;
    }

    switch (m_numeroParcours)
    {
    case 1: // c'est le parcours 1
        // Depart = 1er de la liste; Arrivee = dernier de la liste; Parcours = tout le reste
        m_parametresParcours1.insert("MasqueDepart", m_listeMasquesParcours.takeFirst()->getNumero());
        m_parametresParcours1.insert("MasqueArrivee", m_listeMasquesParcours.takeLast()->getNumero());
        // Il reste que des masques "Parcours" dans la liste
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
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
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
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
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
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
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
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
        if(m_localDebug) qDebug() << m_listeMasquesParcours.count();
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

    AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"));
    alertBox->setWink();
    alertBox->show();

    gv_AireParcours->close();
}

QList<int> Editeur::masquesVoisins(int numeroMasque, int largeur, int hauteur)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    QList<int> voisinsMasques;
    int nbTotal = largeur * hauteur;

    int gauche, droite, bas, haut = 0;

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
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    m_numeroParcours = 1;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque_1->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Nombre masques Parcours"),trUtf8("Le nombre de masques de Parcours doit être inférieur ou égal au nombre de masques Largeur * nombre de masques Hauteur"));
        alertBox->show();
        return;
    }

    gv_AireParcours = new EditeurParcoursWidget();
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 1"));
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    connect(gv_AireParcours->getBtnReset(), SIGNAL(clicked()), this, SLOT(reinitialiserGvParcours()));
    connect(gv_AireParcours->getBtnSave(), SIGNAL(clicked()), this, SLOT(sauvegarderParcours()));
    connect(gv_AireParcours, SIGNAL(signalCloseEvent(QCloseEvent*)), this, SLOT(slotFermetureEditeurParcoursWidget(QCloseEvent*)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours(m_numeroParcours);
    gv_AireParcours->show();
}

void Editeur::slotFermetureEditeurParcoursWidget(QCloseEvent *)
{
    if(m_localDebug) qDebug() << "Fermeture EditeurParcoursWidget...";
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
}

void Editeur::on_btnParcours2_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    m_numeroParcours = 2;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque_2->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"));
        alertBox->setWink();
        alertBox->show();
        return;
    }

    gv_AireParcours = new EditeurParcoursWidget();
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 2"));
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    connect(gv_AireParcours->getBtnReset(), SIGNAL(clicked()), this, SLOT(reinitialiserGvParcours()));
    connect(gv_AireParcours->getBtnSave(), SIGNAL(clicked()), this, SLOT(sauvegarderParcours()));
    connect(gv_AireParcours, SIGNAL(signalCloseEvent(QCloseEvent*)), this, SLOT(slotFermetureEditeurParcoursWidget(QCloseEvent*)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours(m_numeroParcours);
    gv_AireParcours->show();
}

void Editeur::on_btnParcours3_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    m_numeroParcours = 3;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque_3->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"));
        alertBox->setWink();
        alertBox->show();
        return;
    }

    gv_AireParcours = new EditeurParcoursWidget();
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 3"));
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    connect(gv_AireParcours->getBtnReset(), SIGNAL(clicked()), this, SLOT(reinitialiserGvParcours()));
    connect(gv_AireParcours->getBtnSave(), SIGNAL(clicked()), this, SLOT(sauvegarderParcours()));
    connect(gv_AireParcours, SIGNAL(signalCloseEvent(QCloseEvent*)), this, SLOT(slotFermetureEditeurParcoursWidget(QCloseEvent*)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours(m_numeroParcours);
    gv_AireParcours->show();
}

void Editeur::on_btnParcours4_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    m_numeroParcours = 4;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque_4->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"));
        alertBox->setWink();
        alertBox->show();
        return;
    }

    gv_AireParcours = new EditeurParcoursWidget();
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 4"));
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    connect(gv_AireParcours->getBtnReset(), SIGNAL(clicked()), this, SLOT(reinitialiserGvParcours()));
    connect(gv_AireParcours->getBtnSave(), SIGNAL(clicked()), this, SLOT(sauvegarderParcours()));
    connect(gv_AireParcours, SIGNAL(signalCloseEvent(QCloseEvent*)), this, SLOT(slotFermetureEditeurParcoursWidget(QCloseEvent*)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours(m_numeroParcours);
    gv_AireParcours->show();
}

void Editeur::on_btnParcours5_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    m_numeroParcours = 5;
    m_listeMasquesParcours.clear();

    //Verification du nombre de masque
    if (ui->spinBoxParcoursMasque_5->value() >= (ui->spinBoxParcoursMasqueHauteur->value() * ui->spinBoxParcoursMasquesLargeur->value()))
    {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Editeur de Parcours"),trUtf8("Le parcours a bien été sauvegardé"));
        alertBox->setWink();
        alertBox->show();
        return;
    }

    gv_AireParcours = new EditeurParcoursWidget();
    gv_AireParcours->setWindowTitle(trUtf8("Parcours 5"));
    gv_AireParcours->setWindowModality(Qt::ApplicationModal);

    connect(gv_AireParcours->getBtnReset(), SIGNAL(clicked()), this, SLOT(reinitialiserGvParcours()));
    connect(gv_AireParcours->getBtnSave(), SIGNAL(clicked()), this, SLOT(sauvegarderParcours()));
    connect(gv_AireParcours, SIGNAL(signalCloseEvent(QCloseEvent*)), this, SLOT(slotFermetureEditeurParcoursWidget(QCloseEvent*)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    gv_AireParcours->move((desktop_width-gv_AireParcours->width())/2, (desktop_height-gv_AireParcours->height())/2);

    remplirGvParcours(m_numeroParcours);
    gv_AireParcours->show();
}

/// **********************************************************************************************************************************************************
///                             GESTION MODE MODIFIER ABE
/// **********************************************************************************************************************************************************

void Editeur::setModeModificationAbe(bool yesNo)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    m_modeModificationAbe = yesNo;
}

void Editeur::on_btnCreationAbe_clicked()
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() << trUtf8("Mode Création sélectionné");
    }

    m_listeDossiers.clear();
    m_listeFichiersImages.clear();
    m_listeMasquesParcours.clear();
    m_listeMasques.clear();
    ui->listWidgetImagesSelection->clear();

    setModeModificationAbe(false);

    ui->btnSuivant->click(); // Clic sur le bouton suivant
}

void Editeur::slotOpenFile()
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() << trUtf8("Nom du fichier passé :") << m_abuleduFileManager->abeGetFile()->abeFileGetFileName().absoluteFilePath();
    }
    slotLoadUnit();
    if (m_localDebug) qDebug() << trUtf8("Répertoire temporaire de dézippage de l'ABE choisi")  << m_abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    m_abuleduFileManager->hide();

    /// TODO -> Comment rassembler des fichiers LOMFr et tout ce qui suit ?
}

void Editeur::slotLoadUnit()
{
    if (m_localDebug) qDebug()<< __FILE__ <<  __LINE__ << __FUNCTION__<<" :: "<<m_abuleduFile->abeFileGetFileName().fileName();
    m_listeFichiersImages.clear();
    ui->listWidgetImagesSelection->clear();
    QDir folder(QString(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()+"/data/images"));
    folder.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    foreach(QFileInfo fileInfo, folder.entryInfoList())
    {
        ajouterImage(fileInfo.absoluteFilePath());
        m_lastOpenDir = fileInfo.absolutePath();
    }

    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
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
    ui->spinBoxParcoursMasque_1->setValue(parametres.value("parcours1/nbMasquesChoisis",7).toInt());
    ui->spinBoxParcoursMasque_2->setValue(parametres.value("parcours2/nbMasquesChoisis",7).toInt());
    ui->spinBoxParcoursMasque_3->setValue(parametres.value("parcours3/nbMasquesChoisis",7).toInt());
    ui->spinBoxParcoursMasque_4->setValue(parametres.value("parcours4/nbMasquesChoisis",7).toInt());
    ui->spinBoxParcoursMasque_5->setValue(parametres.value("parcours5/nbMasquesChoisis",7).toInt());
    ui->spinBoxParcoursMasquesLargeur->setValue(parametres.value("nbMasquesLargeur",7).toInt());
    ui->spinBoxParcoursMasqueHauteur->setValue(parametres.value("nbMasquesHauteur",7).toInt());
    parametres.endGroup();

    ui->leTitre->setText(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralTitle(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first()));
    if (abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus() == 1)
    {
        ui->leAuteur->setText(abeApp->getAbeIdentite()->abeGetPreNom()+" "+abeApp->getAbeIdentite()->abeGetNom());
    }
    ui->teDescription->setPlainText(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralDescription(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first()).first());
    QString langueRessource = m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().value(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first());
    ui->cbLangueRessource->setCurrentIndex(ui->cbLangueRessource->findText(langueRessource));
    QString licence = m_abuleduFile->abeFileGetLOM()->abeLOMgetRightsDescription(m_abuleduFile->abeFileGetLOM()->abeLOMgetGeneralLanguage().first());
    ui->cbLicence->setCurrentIndex(ui->cbLicence->findText(licence));

    ui->stackedWidget->setCurrentWidget(ui->pageGestionImages);
}

void Editeur::chargerPositionMasque(int numeroParcours)
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    positionMasquesParcours.clear();
    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
    parametres.beginGroup("parcours");
    parametres.beginGroup("parcours"+QString::number(numeroParcours));
    for (int i =0 ; i < parametres.childKeys().count(); i++)
    {
        positionMasquesParcours.insert(parametres.childKeys().at(i),parametres.value(parametres.childKeys().at(i)).toInt());
    }
}

/// **********************************************************************************************************************************************************
///                             WIDGET BARRE NAVIGATION
/// **********************************************************************************************************************************************************

void Editeur::on_btnPrecedent_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
}

void Editeur::on_btnSuivant_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    if(ui->stackedWidget->currentWidget()->objectName() == "pageGestionImages") {
        // Condition de garde = m_listeFichiersImages < 5
        if ( m_listeFichiersImages.count() < 5)
        {
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Sauvegarder Thème"),trUtf8("Veuillez sélectionner au minimum 5 images"));
            alertBox->show();
            return;
        }
    }
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
}

void Editeur::on_btnQuitter_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    /// @todo Controle que l'utilisateur veut quitter sans sauvegarder
    this->close();
}

void Editeur::majBarreNavigation(int numPage)
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() << "Index de la page courante" << ui->stackedWidget->currentIndex();
    }
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

        if (numPage == ui->stackedWidget->count()-1) // derniere page
        {
            ui->btnSuivant->setVisible(false);
        }
        else if(numPage != ui->stackedWidget->count()-1)
        {
            ui->btnSuivant->setVisible(true);
            ui->btnSuivant->setIcon(QIcon(":/bouton/flecheD"));
        }
    }
}

/// **********************************************************************************************************************************************************
///                             GESTION DRAG & DROP
/// **********************************************************************************************************************************************************

void Editeur::dropEvent(QDropEvent *event)
{
    if(m_localDebug)
    {
        qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
        qDebug() <<  event->source() << " " << event->pos() << ui->listWidgetImagesSelection->geometry().contains(event->pos());
    }

    if (event->source()->objectName() == "treeViewArborescence" && ui->listWidgetImagesSelection->geometry().contains(event->pos()))
    {
        if(m_localDebug) qDebug() << "SOURCE == treeViewArbo";
        // Controle que c'est bien une image
        if(event->mimeData()->hasImage())
        {
            if (m_localDebug) qDebug() << "C'est une image";
        }
    }
    else if(event->source()->objectName() == "lwSimple" && ui->listWidgetImagesSelection->geometry().contains(event->pos()))
    {
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
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    trUtf8("Choisir une image"), m_lastOpenDir, trUtf8("Images (*.png *.jpg *.jpeg *.svg *.bmp *.ppm *.xpm *.xbm)")); /* */
    QFileInfo fi(fileName);
    if(fi.exists()) {
        ajouterImage(fi.absoluteFilePath());
        m_lastOpenDir = fi.absolutePath();
    }
}

void Editeur::on_btnModificationCourant_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    setModeModificationAbe(true);
    slotLoadUnit();
}

void Editeur::on_btnModificationAutre_clicked()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    setModeModificationAbe(true);
    m_abuleduFileManager->show();
}

bool Editeur::preparerSauvegarde()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    if(ui->leTitre->text().isEmpty()) {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas de titre !"),trUtf8("Vous n'avez pas renseigné le champ titre !"));
        alertBox->show();
        return false;
    }

    if(ui->leAuteur->text().trimmed() == "") {
        AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Pas d'auteur !"),trUtf8("Vous n'avez pas renseigné le champ auteur !"));
        alertBox->show();
        return false;
    }

    QString codeLangue = m_abuleduFile->abeFileGetLOM()->abeLOMgetAvailableLanguages().key(ui->cbLangueRessource->currentText());

    //Les informations pour LOM
    m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralTitle(codeLangue,ui->leTitre->text());
    m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralDescription(codeLangue,ui->teDescription->document()->toPlainText());

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

    m_abuleduFile->abeFileGetLOM()->abeLOMaddLifeCycleContributionRole("author", vcard, QDate::currentDate());
    m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsCost("no");
    m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsCopyrightAndOtherRestrictions("yes");
    m_abuleduFile->abeFileGetLOM()->abeLOMsetRightsDescription(codeLangue,ui->cbLicence->currentText());

    m_abuleduFile->abeFileGetLOM()->abeLOMsetAnnotationDate(QDate::currentDate());
    m_abuleduFile->abeFileGetLOM()->abeLOMsetGeneralLanguage(codeLangue);
    qDebug()<<" /////////////////////////////////////////// j'ai écrit pour la langue "<<codeLangue;

    QString destTemp = m_abuleduFile->abeFileGetDirectoryTemp().absolutePath();
    QDir().mkpath(destTemp + "/data/images");
    QDir().mkpath(destTemp + "/conf");
    QStringList listeFichiers;

    m_abuleduFile->abeFileExportPrepare(listeFichiers,destTemp,QString("abe"));
    /// Creation fichier Conf (note les timers sont convertis en millisecondes)
    QSettings parametres(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath() + "/conf/parametres.conf", QSettings::IniFormat);
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
        if(!m_parametresParcours1.isEmpty())
        {
            //            parametres.setValue("parcours1/nbMasquesChoisis", (ui->spinBoxParcoursMasque_1->value()));
            QMapIterator<QString, QVariant> i(m_parametresParcours1);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours1/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours2.isEmpty())
        {
            //            parametres.setValue("parcours2/nbMasquesChoisis", (ui->spinBoxParcoursMasque_2->value()));
            QMapIterator<QString, QVariant> i(m_parametresParcours2);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours2/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours3.isEmpty())
        {
            //            parametres.setValue("parcours3/nbMasquesChoisis", (ui->spinBoxParcoursMasque_3->value()));
            QMapIterator<QString, QVariant> i(m_parametresParcours3);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours3/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours4.isEmpty())
        {
            //            parametres.setValue("parcours4/nbMasquesChoisis", (ui->spinBoxParcoursMasque_4->value()));
            QMapIterator<QString, QVariant> i(m_parametresParcours4);
            while (i.hasNext()) {
                i.next();
                parametres.setValue("parcours4/"+ i.key(), i.value());
            }
        }
        if(!m_parametresParcours5.isEmpty())
        {
            //            parametres.setValue("parcours5/nbMasquesChoisis", (ui->spinBoxParcoursMasque_5->value()));
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
    m_abuleduFile->abeFileExportPrepare(parcoursRecursif(m_abuleduFile->abeFileGetDirectoryTemp().absolutePath()), m_abuleduFile->abeFileGetDirectoryTemp().absolutePath(), "abe");
    return true;
}

void Editeur::releaseAbe()
{
    if(m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;
    ui->listWidgetImagesSelection->clear();
    if (preparerSauvegarde())
    {
        AbulEduMediathequePushV1* medPush = new AbulEduMediathequePushV1(0,"mediatheque");
        medPush->abeSetFile(m_abuleduFile);
        medPush->setAttribute(Qt::WA_DeleteOnClose);
        medPush->show();
        close();
    }
}

void Editeur::on_btnEnregistrementOK_clicked()
{
    if (m_localDebug) qDebug() << __FILE__ <<  __LINE__ << __FUNCTION__;

    if (ui->cbChoixEnregistrement->currentIndex() == 0)
    {
        if (preparerSauvegarde())
        {
            hide();
            m_parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur()->show();
            AbulEduMessageBoxV1 *alertBox=new AbulEduMessageBoxV1(trUtf8("Passage en mode essai..."),trUtf8("Votre module n'est pas enregistré. Si les paramètres vous conviennent, revenez dans l'éditeur pour enregistrer ou publier."));
            alertBox->show();        }
    }
    else if(ui->cbChoixEnregistrement->currentIndex() == 1)
    {
        createAbe();
    }
    else
    {
        releaseAbe();
    }
}

void Editeur::on_cbChoixEnregistrement_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->btnEnregistrementOK->setIcon(QIcon(":/bouton/essai"));
        m_parent->abeGetMyAbulEduAccueil()->abePageAccueilGetBtnRevenirEditeur()->setVisible(true);
    }
    else if(index == 1)
    {
        ui->btnEnregistrementOK->setIcon(QIcon(":/bouton/disque"));
    }
    else
    {
        ui->btnEnregistrementOK->setIcon(QIcon(":/bouton/mediatheque-up"));
    }
}
