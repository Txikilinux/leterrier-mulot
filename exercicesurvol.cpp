#include "exercicesurvol.h"

ExerciceSurvol::ExerciceSurvol(QWidget *parent):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = true;

    m_parent = parent;
    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()));


    //Création de l'aire de travail + propriétés
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));

    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setStyleSheet("background-color: yellow");
    getAbeExerciceTelecommandeV1()->setStyleSheet("background-color: red");

    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu) ;
    proxy->setGeometry(gv_AireDeJeu->rect());
    proxy->setZValue(-1) ;

    // Instanciation de toutes les variables membres
    //    m_itemImage = new QGraphicsPixmapItem(0,getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene());

    m_listeImage.clear();
    m_listeFichiers.clear();
    m_listeMasquesFixes.clear();
    m_nbImage = 0;
    m_nbMasquesInteractifs = 0;

    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setStyleSheet("background-color: blue"); //Fond bleu
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);

    // Dans cet exercice, la correction et la vérification ne servent pas (pour l'instant)
    //    getAbeExerciceTelecommandeV1()->ui->btnCorriger->setVisible(false);
    //    getAbeExerciceTelecommandeV1()->ui->btnVerifier->setVisible(false);

    // Demarrage de la machine à états
    sequenceMachine->start();
}

ExerciceSurvol::~ExerciceSurvol()
{
    // Permet à la MainWindow de savoir que l'exercice est terminé
    emit exerciceExited();
}

void ExerciceSurvol::slotSequenceEntered() // en cours
{
    if (m_localDebug) qDebug()<<"--------------ExerciceSurvol::slotSequenceEntered()";

    setAbeNbExercices(1);
    setAbeNbTotalQuestions(5);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

    abeEnableAnimationsConsigneBilan(false);

    AbulEduCommonStatesV1::slotSequenceEntered();
    boiteTetes->resetTetes(m_nbTotalQuestions);
}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotPresenteSequenceEntered()";

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Survol"));
    QString debutTableau = "<tr valign=middle>";
    QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
    QString consigne = "<td align = right>  Ma consigne qui presentera la sequence (video souhaitee)    </td>" ;
    QString finTableau = "</tr>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);

    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionnerConsigne();
    onPeutPresenterExercice = false; // permet de "sauter" la présentation de l'exercice
}


void ExerciceSurvol::slotRealisationExerciceEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotRealisationExerciceEntered()";

    // Mettre tout ce qui est commun à chaque question
    m_nbImage = m_nbTotalQuestions; // le nb image = le nb de question
    m_nbMasquesInteractifs = 0;

    // aller chercher le pack image
    QDir dir("data/images/animaux/");
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++) {
        m_listeFichiers << list.at(i).absoluteFilePath();
    }
    // choisir 5 images au hasard dans le pack
    {
        for(int i = 0; i < m_nbImage; i++)
        {
            qsrand(QDateTime::currentDateTime ().toTime_t ());
            int n = (qrand() % (m_listeFichiers.size()));
            QFileInfo fileInfo = m_listeFichiers.takeAt(n);

            m_image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);

            m_listeImage << m_image;
        }
    }
    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
}


void ExerciceSurvol::slotInitQuestionEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotInitQuestionEntered()";
    AbulEduCommonStatesV1::slotInitQuestionEntered();

    getAbeExerciceMessageV1()->setVisible(false);

    qDebug()<< "Numero de la question : " << m_numQuestion;
    // Au départ m_numQuestion = 0 !

    m_itemImage = new QGraphicsPixmapItem(0, gv_AireDeJeu->scene());
    m_itemImage->setPixmap(m_listeImage.takeAt(0));
//    m_image = m_listeImage[0];
//    gv_AireDeJeu->scene()->addPixmap(m_image);
    redimensionnerImage();
    gv_AireDeJeu->show();
}

void ExerciceSurvol::slotQuestionEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotQuestionEntered()";

    //mise en place du masque
    qDebug()<< "Numero de la question : " << m_numQuestion;
    int largeur;
    int hauteur;

    switch (m_numQuestion){
    case 1:
        largeur = hauteur = 150;
        break;
    case 2:
        largeur = hauteur = 100;
        break;
    case 3:
        largeur = hauteur = 75;
        break;
    case 4:
        largeur =hauteur = 50;
        break;
    case 5:
        largeur = hauteur = 25;
        break;
    }

    qDebug()<< "Taille de l'Aire de jeu "  << gv_AireDeJeu->size();
    //qDebug()<< "Taille de la pixmap     " << m_itemImage->pixmap().size();
    qDebug()<< "Taille de la tele       " <<getAbeExerciceTelecommandeV1()->geometry().size();
    qDebug()<< "Taille de la boitetete  " <<boiteTetes->geometry().size();

    int nbTotalPieces = 0;
    //Calcul du nombre de lignes et de colonnes necessaires
    for(int i = 0; i < gv_AireDeJeu->geometry().height(); i+=hauteur) {
        for(int j = 0; j < gv_AireDeJeu->geometry().width(); j+=largeur) {
            qDebug() << "ajout d'une piece ... " << nbTotalPieces << " haut : " << i << " : " << hauteur << " larg " << j << " : " << largeur;
            nbTotalPieces++;

            m_masque = new masqueDeplaceSouris();
            m_masque->setSize(largeur,hauteur);
            m_masque->moveBy(j,i);
            m_masque->setColor(Qt::black);
            m_masque->setParent(gv_AireDeJeu->scene());
            m_masque->setHideOnMouseOver(false);
            gv_AireDeJeu->scene()->addItem(m_masque);
            m_listeMasquesFixes << m_masque;
        }
        qDebug()<<"Nombre de masques fixes :" << m_listeMasquesFixes.count();
    }

    //Et ensuite on fait en sorte que seuls 7 masques soient actifs
    //Seuls quelques masques sont "survolables", les autres ne bougent pas quand
    //on les survole mais disparaissent quand il n'y a plus de masques sensibles

    m_nbMasquesInteractifs = 0;
    if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

    while (m_nbMasquesInteractifs < 7)
    {
        int alea = (qrand() % (m_listeMasquesFixes.count()));
        qDebug() << "alea = " << alea;
        m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
        connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
        m_masqueInteractif->setColor(Qt::yellow);
        m_masqueInteractif->setHideOnMouseOver(true);

        m_nbMasquesInteractifs++;
        qDebug()<< "Nombre de masques survolables : " << m_nbMasquesInteractifs;
//        alea++;
    }

    AbulEduCommonStatesV1::slotQuestionEntered();
}

void ExerciceSurvol::slotFinVerificationQuestionEntered()
{
    qDebug()<< "*******************ExerciceSurvol::slotFinVerificationQuestionEntered()";
    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    qDebug()<< "Avant Nombre de masques fixes dans la liste" << m_listeMasquesFixes.count();
    m_listeMasquesFixes.clear();
    m_nbMasquesInteractifs = 0;
    qDebug()<< "Apres Nombre de masques fixes dans la liste" << m_listeMasquesFixes.count();


    boiteTetes->setEtatTete(m_numQuestion-1, abe::evalA );

}

void ExerciceSurvol::slotFinQuestionEntered()
{
    qDebug()<< "*******************ExerciceSurvol::slotFinQuestionEntered()";


    // Nettoyage de la liste des masques
    for (int i =0; i< m_listeMasquesFixes.count(); i++)
    {
        delete m_listeMasquesFixes.takeAt(i);
    }

    qDebug()<< "Nombre de masques fixes dans la liste" << m_listeMasquesFixes.count();
    m_listeMasquesFixes.clear();
    m_nbMasquesInteractifs = 0;



    boiteTetes->setEtatTete(m_numQuestion-1, abe::evalA );


    // Vider itemImage
    gv_AireDeJeu->scene()->removeItem(m_itemImage);
    gv_AireDeJeu->scene()->clear();

    gv_AireDeJeu->show();


    // Passer à l'image suivante

    AbulEduCommonStatesV1::slotFinQuestionEntered();

}

void ExerciceSurvol::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotQuitter";
    AbulEduCommonStatesV1::slotQuitter();
}

//void ExerciceSurvol::resizeEvent(QResizeEvent *event) //todo
//{
//    if (m_localDebug) qDebug()<<"               ExerciceSurvol::resizeEvent()";
//    setDimensionsWidgets();
//}

//void ExerciceSurvol::showEvent(QShowEvent *event) //todo
//{
//    if (m_localDebug) qDebug()<<"               ExerciceSurvol::showEvent()";
//    setDimensionsWidgets();
//}

void ExerciceSurvol::setDimensionsWidgets() //todo
{
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0, 0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    // Placement de l'AireDeJeu
    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    gv_AireDeJeu->move((100 * ratio) / 2, 50 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes et l'image
    redimensionnerConsigne();
    // redimensionnerImage();

    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---end";
}


////////////////////////////// Méthodes propres à la classe ///////////////////////////////////////

void ExerciceSurvol::redimensionnerConsigne()
{
    //    fondConsigne->setVisible(false); // Scorie liée à la compatibilité avec l'ancienne façon de faire
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

void ExerciceSurvol::redimensionnerImage()
{
    qDebug()<< m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
    int largeurImage = m_itemImage->pixmap().width();
    int hauteurImage = m_itemImage->pixmap().height();

    //Redimensionner l'image avec condition de garde car peut lever une exception donc bug
    if(!m_itemImage->pixmap().isNull())
    {
        //Si l'image est plus large que haute
        if (largeurImage > hauteurImage)
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToHeight(gv_AireDeJeu->height(), Qt::SmoothTransformation));
            qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            // Centrage de l'image
            m_itemImage->moveBy(differenceLargeur/2,0);

        }
        // Si l'image est plus haute que large
        else // largeurImage < hauteurImage
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToWidth(gv_AireDeJeu->width(), Qt::SmoothTransformation));
            qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            // Centrage de l'image
            m_itemImage->moveBy(differenceLargeur/2,0);
        }
    }
}

void ExerciceSurvol::slotCacheMasque()
{
    qDebug() << "ExerciceSurvol::slotCacheMasque : " << m_nbMasquesInteractifs;
    m_nbMasquesInteractifs--;

    if(m_nbMasquesInteractifs == 0) {
        //On affiche l'image en entier
        for(int i = 0; i < m_listeMasquesFixes.count(); i++)
        {
            m_listeMasquesFixes.at(i)->setVisible(false);

            qDebug()<< "Nb de masques fixes: " <<m_listeMasquesFixes.count();
        }

    }
}
