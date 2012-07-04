/** Classe ExerciceSurvol
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

#include "exercicesurvol.h"
#include <QtCore/qmath.h>


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

    // Demarrage de la machine à états
    sequenceMachine->start();

    // Assignation des propriétés de la télécommande
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnNiveau  , "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnNiveau  , "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnNiveau  , "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);

    // Pour les appuis automatiques sur les touches
    connect(this, SIGNAL(appuiVerifier()),getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()));
    connect(this, SIGNAL(appuiSuivant()),getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()));
}

ExerciceSurvol::~ExerciceSurvol()
{
    emit exerciceExited(); // Permet à la MainWindow de savoir que l'exercice est terminé
}

void ExerciceSurvol::slotSequenceEntered() // en cours
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotSequenceEntered()";

    if(!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        abeEnableAnimationsConsigneBilan(false);
        setAbeNbExercices(1);      // a instancier avant appel du slot SequenceEntered !
        setAbeNbTotalQuestions(5); // a instancier avant appel du slot SequenceEntered !

        AbulEduCommonStatesV1::slotSequenceEntered();
        presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);

        setAbeLevel("1"); // a instancier après le slot sinon niveau 0 par def.
    }
}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotPresenteSequenceEntered()";

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

    // Appui auto sur bouton suivant
    if (m_localDebug) qDebug() << "Passage à l'exercice automatique";
    QTimer::singleShot(3000,this,SLOT(slotPassageAutoSuivant()));     // Click auto du bouton suivant avec un timer
}

// Mettre tout ce qui est commun à chaque question
// aller chercher le pack image
// choisir 5 images au hasard dans le pack
void ExerciceSurvol::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotRealisationExerciceEntered()";

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
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotInitQuestionEntered()";
    AbulEduCommonStatesV1::slotInitQuestionEntered();
    if (!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setVisible(false);
        m_itemImage = new QGraphicsPixmapItem(0, gv_AireDeJeu->scene());
        m_itemImage->setPixmap(m_listeImage.takeAt(0));
        redimensionnerImage2();

        gv_AireDeJeu->show();
        getAbeExerciceTelecommandeV1()->ui->pbarQuestion->setValue(m_numQuestion);

        //mise en place du masque
        qreal largeurMasque = 0.00;
        qreal hauteurMasque = 0.00;

        switch (m_numQuestion){
        case 1:
            largeurMasque = gv_AireDeJeu->width()/4.00;
            hauteurMasque = gv_AireDeJeu->height()/2.00;
            break;
        case 2:
            largeurMasque = gv_AireDeJeu->width()/8.00;
            hauteurMasque = gv_AireDeJeu->height()/4.00;
            break;
        case 3:
            largeurMasque = gv_AireDeJeu->width()/16.00;
            hauteurMasque = gv_AireDeJeu->height()/8.00;
            break;
        case 4:
            largeurMasque = gv_AireDeJeu->width()/32.00;
            hauteurMasque = gv_AireDeJeu->height()/16.00;
            break;
        case 5:
            largeurMasque = gv_AireDeJeu->width()/64.00;
            hauteurMasque = gv_AireDeJeu->height()/32.00;
            break;
        default :
            largeurMasque = hauteurMasque = 10.00;
            break;
        }

//        if (m_localDebug)
//        {
//            qDebug() <<"LARGEUR & HAUTEUR AIRE DE JEU"  << gv_AireDeJeu->width() << gv_AireDeJeu->height();
//            qDebug() << "LARGEUR MASQUE"<<largeurMasque<< "HAUTEUR MASQUE" << hauteurMasque;
//            qDebug() <<"LARGEUR IMAGE" << m_image.width();
//            qDebug() <<"TAILLE ITEM IMAGE" <<m_itemImage->boundingRect();
//            qDebug() << "TAILLE GV_PRINCIPALE"<< getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
//            qDebug() << "TAILLE PROXY"<<proxy->geometry().width();
//        }

        int nbTotalPieces = 0;
        int num = 0;
        //        Calcul du nombre de lignes et de colonnes necessaires
        for(float i = 0; i < gv_AireDeJeu->height() ; i+=hauteurMasque) {
            for(float j = 0; j < gv_AireDeJeu->width() ; j+=largeurMasque) {
//                if (m_localDebug) qDebug() << "ajout d'une piece ... " << nbTotalPieces << " haut : " << i << " : " << hauteurMasque << " larg " << j << " : " << largeurMasque;
                nbTotalPieces++;

                m_masque = new masqueDeplaceSouris();
                m_masque->setToolTip(QString("%1").arg(num));

                m_masque->setSize(largeurMasque, hauteurMasque);
                m_masque->moveBy(j,i);
                m_masque->setColor(QColor::fromRgb(255,255,255));
                m_masque->setHideOnMouseOver(false);
                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
                num ++;
            }
//            if (m_localDebug) qDebug()<<"Nombre de masques fixes :" << m_listeMasquesFixes.count();
        }
    }

}

// Choix aléatoire des masques intéractifs
// Connexion du slotCacheMasque
void ExerciceSurvol::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotQuestionEntered()";

    // Lancement du chrono
    // Instanciation du chronometre
    m_chronometre = new QTime();
    m_chronometre->start();

    // Seuls 7 masques soient actifs et "survolables", les autres ne bougent pas quand mais disparaissent quand il n'y a plus de masques sensibles (see slotCacheMasque)
    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        m_nbMasquesInteractifs = 0;
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

        while (m_nbMasquesInteractifs < 7)
        {
            int alea = (qrand() % (m_listeMasquesFixes.count()));
            if (m_localDebug) qDebug() << "alea = " << alea;
            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueInteractif->setColor(QColor::fromRgb(150,150,150));
            m_masqueInteractif->setHideOnMouseOver(true);

            m_nbMasquesInteractifs++;
//            if (m_localDebug) qDebug()<< "Nombre de masques survolables : " << m_nbMasquesInteractifs;
        }
    }

    m_exerciceEnCours = true;
}

// Appeler lors de l'appui sur le bouton suivant
void ExerciceSurvol::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceSurvol::slotAfficheVerificationQuestionEntered()";
    // Je me sers de ce slot pour appuyer automatiquement sur le bouton suivant de la télécommande,
    if (m_localDebug) qDebug()<< "Click bouton suivant automatique !";

    if (m_exerciceEnCours)
    {
        QTimer::singleShot(2000,this,SLOT(slotPassageAutoSuivant()));     // Click auto du bouton suivant avec un timer
    }
}



// Appeler lors de l'appui sur le bouton suivant
void ExerciceSurvol::slotFinVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceSurvol::slotFinVerificationQuestionEntered()";
    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    m_listeMasquesFixes.clear();
    m_nbMasquesInteractifs = 0;

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
    if (m_localDebug)qDebug()<< "##########################  ExerciceSurvol::slotFinQuestionEntered()";

    if (m_localDebug)qDebug()<< "*******************BRAVO !!!!!!!!!!!!!!!!!";

    // Affichage du temps passé Total
    if (m_localDebug) qDebug("Temps écoulé en millisecondes: %d ms", m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5);
    // On ne veut pas un chronometre précis au millième près =)
    if (m_localDebug) qDebug("Temps écoulé en secondes: %d sec", (m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);


    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void ExerciceSurvol::slotBilanExerciceEntered() // todo boucle pour les têtes
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceSurvol::slotBilanExerciceEntered()";

    gv_AireDeJeu->scene()->removeItem(m_itemImage);

    gv_AireDeJeu->scene()->clear();
    gv_AireDeJeu->show();
    m_listeImage.clear();

    // Variables locales
    int m_minute;
    int m_seconde;
    QString debutTableau;
    QString imagetete;
    QString consigne;
    QString finTableau;

    // Les heures ne sont pas gérées
    // Arrondi à l'entier supérieur
    m_tempsTotal = qCeil((m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);
    // Cas ou le temps total (exprimé en secondes) est supérieur à une minute => conversion
    if(m_tempsTotal > 59)
    {
        m_seconde = m_tempsTotal %60;
        m_minute  = m_tempsTotal /60;
    }



    AbulEduCommonStatesV1::slotBilanExerciceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Bilan");
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Survol"));

    debutTableau = "<tr>";
    imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");

    //-------------------------------------------
    // Boucle pour l'affichage de la consigne !
    //-------------------------------------------
    if (m_tempsTotal <= 59) // Affichage en secondes
    {
        consigne = "<td> " + QString("Tu as fini l'exercice en ") + QString::number(m_tempsTotal) +" secondes" +" </td>" ;
    }
    else if((m_tempsTotal%60) == 0) // Affichage que en minutes (pas besoin d'afficher "en 0 secondes" :p)
    {
        consigne = "<td> " + QString("Tu as fini l'exercice en ") + QString::number(m_minute) +" minute(s)" +" </td>" ;
    }
    else if (m_tempsTotal > 59) // Affichage en minutes et secondes
    {
        consigne = "<td> " + QString("Tu as fini l'exercice en ") + QString::number(m_minute) +" minute(s)" + " et "+ QString::number(m_seconde) +" secondes" + " </td>" ;
    }
    finTableau = "</tr>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);


    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
    getAbeExerciceMessageV1()->setVisible(true);

    redimensionnerConsigne();

}


void ExerciceSurvol::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotQuitter";

    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceSurvol::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::setDimensionsWidgets()---start";

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
    //        gv_AireDeJeu->move((170 * ratio) / 2,50 * ratio);
    gv_AireDeJeu->move(80 * ratio, 64 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes
    //  redimensionnerConsigne();
    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::setDimensionsWidgets()---end";
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
    if (m_localDebug) qDebug()<< m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
    int largeurImage = m_itemImage->pixmap().width();
    int hauteurImage = m_itemImage->pixmap().height();

    //Redimensionner l'image avec condition de garde car peut lever une exception donc bug
    if(!m_itemImage->pixmap().isNull())
    {
        //Si l'image est plus large que haute
        if (largeurImage > hauteurImage)
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToHeight(gv_AireDeJeu->height(), Qt::SmoothTransformation));
            if (m_localDebug)  qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            // Centrage de l'image
            m_itemImage->moveBy(differenceLargeur/2,0);

        }
        // Si l'image est plus haute que large
        else // largeurImage < hauteurImage
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToWidth(gv_AireDeJeu->width(), Qt::SmoothTransformation));
            if (m_localDebug)  qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
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
    if (m_localDebug) qDebug() << "##########################  ExerciceSurvol::slotCacheMasque : " << m_nbMasquesInteractifs;
    m_nbMasquesInteractifs--;

    if(m_nbMasquesInteractifs == 0)
    {
        //On affiche l'image en entier
        for(int i = 0; i < m_listeMasquesFixes.count(); i++)
        {
            m_listeMasquesFixes.at(i)->setVisible(false);
//            if (m_localDebug) qDebug()<< "Nb de masques fixes: " <<m_listeMasquesFixes.count();
        }
        if (m_localDebug) qDebug() << "Appui sur le bouton Verifier";
        emit appuiVerifier();

        boiteTetes->setEtatTete(m_numQuestion-1, abe::evalA );
        m_listeMasquesFixes.clear();

        // Affichage du temps passé
        if (m_localDebug)
        {
            qDebug("Temps écoulé: %d ms",     m_chronometre->elapsed());
            qDebug("Temps écoulé: %d sec",     (m_chronometre->elapsed())/1000);
        }

        // Enregistrement du temps passé pour chaque question
        switch (m_numQuestion){
        case 1:
            m_tempsQuestion1 = m_chronometre->elapsed();
            break;
        case 2:
            m_tempsQuestion2 = m_chronometre->elapsed();
            break;
        case 3:
            m_tempsQuestion3 = m_chronometre->elapsed();
            break;
        case 4:
            m_tempsQuestion4 = m_chronometre->elapsed();
            break;
        case 5:
            m_tempsQuestion5 = m_chronometre->elapsed();
            break;
        default :
            if (m_localDebug) qDebug("!!!!!!!! Case Default !!!!!! Temps écoulé: %d ms",     m_chronometre->elapsed());
            break;
        }
    }
}


// Méthode qui appuie sur le bouton suivant
void ExerciceSurvol::slotPassageAutoSuivant()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceSurvol::slotPassageAutoSuivant()";
    emit appuiSuivant();
}


