#include "exercicesurvol.h"

ExerciceSurvol::ExerciceSurvol(QWidget *parent):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = true;
    m_exerciceEnCours = false;


    m_parent = parent;
    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()));

    //Création de l'aire de travail + propriétés
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));

    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu) ;
    proxy->setGeometry(gv_AireDeJeu->rect());
    proxy->setZValue(-1) ;

    // Instanciation des variables membres
    m_listeImage.clear();
    m_listeFichiers.clear();
    m_listeMasquesFixes.clear();
    m_nbImage = 0;
    m_nbMasquesInteractifs = 0;

    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);

    // qDebug() << getAbeExerciceAireDeTravailV1()->resize(size);
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



    if(!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        abeEnableAnimationsConsigneBilan(false);
        setAbeNbExercices(1);      // a instancier avant appel du slot SequenceEntered !
        setAbeNbTotalQuestions(5); // a instancier avant appel du slot SequenceEntered !

        AbulEduCommonStatesV1::slotSequenceEntered();
        //    boiteTetes->resetTetes(m_nbTotalQuestions);

        setAbeLevel("1"); // a instancier après le slot sinon niveau 0 par def.
    }
}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotPresenteSequenceEntered()";

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Ma consigne qui presentera la sequence (video souhaitee)");
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Survol"));

    QString debutTableau = "<tr>";
    QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
    QString consigne = "<td> " + QString("Ma consigne qui presentera la sequence (video souhaitee). Ma consigne est tres longue car je veux tester son comportement") +" </td>" ;
    QString finTableau = "</tr>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);

    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);

    getAbeExerciceMessageV1()->setVisible(true);

    redimensionnerConsigne();
    onPeutPresenterExercice = false; // permet de "sauter" la présentation de l'exercice
}

// Mettre tout ce qui est commun à chaque question
// aller chercher le pack image
// choisir 5 images au hasard dans le pack
void ExerciceSurvol::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotRealisationExerciceEntered()";

    if (!m_exerciceEnCours)
    {
        // Mettre tout ce qui est commun à chaque question
        m_nbImage = m_nbTotalQuestions; // le nb image = le nb de question
        m_nbMasquesInteractifs = 0;
        boiteTetes->resetTetes(m_nbTotalQuestions);

        // aller chercher le pack image
        QDir dir("data/images/gourmandises/");
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

}

// Affichage de l'image
// Calcule et mise en place des masques
void ExerciceSurvol::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotInitQuestionEntered()";
    AbulEduCommonStatesV1::slotInitQuestionEntered();
    if (!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setVisible(false);
        //    qDebug()<< "Numero de la question : " << m_numQuestion;
        m_itemImage = new QGraphicsPixmapItem(0, gv_AireDeJeu->scene());
        m_itemImage->setPixmap(m_listeImage.takeAt(0));
        //    .scaled(500, 400,Qt::KeepAspectRatio, Qt::SmoothTransformation

        redimensionnerImage2();
        gv_AireDeJeu->show();

        getAbeExerciceTelecommandeV1()->ui->pbarQuestion->setValue(m_numQuestion);


        //mise en place du masque
        int largeurMasque;
        int hauteurMasque;

        switch (m_numQuestion){
        case 1:
            largeurMasque = hauteurMasque = 150;
            break;
        case 2:
            largeurMasque = hauteurMasque = 100;
            break;
        case 3:
            largeurMasque = hauteurMasque = 75;
            break;
        case 4:
            largeurMasque = hauteurMasque = 50;
            break;
        case 5:
            largeurMasque = hauteurMasque = 25;
            break;
        default :
            largeurMasque = hauteurMasque = 10;
            break;
        }

        int nbTotalPieces = 0;
        int num = 0;
        //Calcul du nombre de lignes et de colonnes necessaires
        for(int i = 0; i < gv_AireDeJeu->geometry().height(); i+=hauteurMasque) {
            for(int j = 0; j < gv_AireDeJeu->geometry().width(); j+=largeurMasque) {
                qDebug() << "ajout d'une piece ... " << nbTotalPieces << " haut : " << i << " : " << hauteurMasque << " larg " << j << " : " << largeurMasque;
                nbTotalPieces++;

                m_masque = new masqueDeplaceSouris();
                m_masque->setToolTip(QString("%1").arg(num));
                m_masque->setSize(largeurMasque,hauteurMasque);
                m_masque->moveBy(j,i);
                m_masque->setColor(QColor::fromRgb(255,255,255));
                m_masque->setParent(gv_AireDeJeu->scene());
                m_masque->setHideOnMouseOver(false);
                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
                num ++;
            }
            qDebug()<<"Nombre de masques fixes :" << m_listeMasquesFixes.count();
        }
    }

}

// Choix aléatoire des masques intéractifs
// Connexion du slotCacheMasque
void ExerciceSurvol::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotQuestionEntered()";

    // Seuls 7 masques soient actifs et "survolables", les autres ne bougent pas quand mais disparaissent quand il n'y a plus de masques sensibles (see slotCacheMasque)

    if (!m_exerciceEnCours)
    {
        m_nbMasquesInteractifs = 0;
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

        while (m_nbMasquesInteractifs < 7)
        {
            int alea = (qrand() % (m_listeMasquesFixes.count()));
            qDebug() << "alea = " << alea;
            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueInteractif->setColor(QColor::fromRgb(150,150,150));
            m_masqueInteractif->setHideOnMouseOver(true);

            m_nbMasquesInteractifs++;
            qDebug()<< "Nombre de masques survolables : " << m_nbMasquesInteractifs;
        }
    }

    m_exerciceEnCours = true;
    AbulEduCommonStatesV1::slotQuestionEntered();
}

// Appeler lors de l'appui sur le bouton suivant
void ExerciceSurvol::slotAfficheVerificationQuestionEntered()
{
    qDebug()<< "*******************ExerciceSurvol::slotAfficheVerificationQuestionEntered()";

    // Je me sers de ce slot pour appuyer automatiquement sur le bouton suivant de la télécommande,
    // et ainsi faciliter l'exercice de l'utilisateur =)
    qDebug()<< "Click bouton suivant automatique !";

    qDebug()<< m_exerciceEnCours;
    if (m_exerciceEnCours){
        QTimer::singleShot(2000,this,SLOT(slotPassageAutoImageSuivante()));     // Click auto du bouton suivant avec un timer
    }
}


// Appeler lors de l'appui sur le bouton suivant
void ExerciceSurvol::slotFinVerificationQuestionEntered()
{
    qDebug()<< "*******************ExerciceSurvol::slotFinVerificationQuestionEntered()";
    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    qDebug()<< "Avant Nombre de masques fixes dans la liste" << m_listeMasquesFixes.count();
    m_listeMasquesFixes.clear();
    m_nbMasquesInteractifs = 0;
    qDebug()<< "Apres Nombre de masques fixes dans la liste" << m_listeMasquesFixes.count();

    boiteTetes->setEtatTete(m_numQuestion-1, abe::evalA );

    // Vider itemImage
    gv_AireDeJeu->scene()->removeItem(m_itemImage);
    gv_AireDeJeu->scene()->clear();
    gv_AireDeJeu->show();
    m_exerciceEnCours = false;

}

// Ce slot est pour la derniere question !
void ExerciceSurvol::slotFinQuestionEntered()
{
    qDebug()<< "*******************ExerciceSurvol::slotFinQuestionEntered()";

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void ExerciceSurvol::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotQuitter";

    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceSurvol::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    // Placement de l'AireDeJeu
    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    //    gv_AireDeJeu->move((170 * ratio) / 2,50 * ratio);
    gv_AireDeJeu->move(80 * ratio, 64 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes
    //  redimensionnerConsigne();

    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---end";
}



//------------------------------------------------------------------
//                 Méthodes propres à la classe
//------------------------------------------------------------------


void ExerciceSurvol::redimensionnerConsigne()
{
    //        fondConsigne->setVisible(false); // Scorie liée à la compatibilité avec l'ancienne façon de faire
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

// Methode obsolete mais à garder (a voir plus tard avec l'editeur)
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

void ExerciceSurvol::redimensionnerImage2()
{
    //    m_itemImage->setPixmap(m_itemImage->pixmap().scaledToWidth(gv_AireDeJeu->width(), Qt::SmoothTransformation));
    //    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(gv_AireDeJeu->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    //    imageLabel->setPixmap(myPixmap->scaled(scrollArea->maximumViewportSize(), Qt::KeepAspectRatio));
    //    imageLabel->adjustSize();
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(gv_AireDeJeu->maximumViewportSize(), Qt::IgnoreAspectRatio));
}

// A chaque passsage sur un masque interactif, on décremente le nombre de masques interactifs.
// Dès que les masques interactifs sont tous survolés, on affiche l'image.
// On affiche la tête dans la boiteTete
// On vide m_listeMasquesFixes
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
        // Appui sur le bouton vérifier auto dès que le nb masques interactifs = 0
        getAbeExerciceTelecommandeV1()->ui->btnVerifier->click();

        boiteTetes->setEtatTete(m_numQuestion-1, abe::evalA );
        m_listeMasquesFixes.clear();
    }
}

// Méthode qui appuie sur le bouton suivant
void ExerciceSurvol::slotPassageAutoImageSuivante()
{
    qDebug() << "ExerciceSurvol;;slotPassageAutoImageSuivante()";
    getAbeExerciceTelecommandeV1()->ui->btnSuivant->click();
}
