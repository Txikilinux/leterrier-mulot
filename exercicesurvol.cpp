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
    nbMasquesLargeur = 0;
    nbMasquesHauteur = 0;

    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);

    chargerOption();

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

/** Affichage de la consigne
  */
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
    QTimer::singleShot(opt_timerSuivant,this,SLOT(slotAppuiAutoSuivant()));     // Click auto du bouton suivant avec un timer
}

/** Mettre tout ce qui est commun à chaque question
  * Aller chercher le pack image
  * Choisir 5 images au hasard dans le pack
  */
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

    QPair<int, int> divisionEcran = plusPetiteDivision(opt_nbMasquesChoisis);
    if (divisionEcran.first > divisionEcran.second)
    {
        nbMasquesLargeur = divisionEcran.first;
        nbMasquesHauteur = divisionEcran.second;
    }
    else
    {
        nbMasquesLargeur = divisionEcran.second;
        nbMasquesHauteur = divisionEcran.first;
    }
}

/** Affichage de l'image
  * Calcul et mise en place des masques
  */
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

        //--------- Calcul de la taille des masques
        qreal largeurMasque = 0.00;
        qreal hauteurMasque = 0.00;

        if (m_localDebug)
        {
            qDebug() << "nb masques largeur :" << nbMasquesLargeur ;
            qDebug() << "nb masques hauteur :" << nbMasquesHauteur ;
        }

        largeurMasque =  qreal(gv_AireDeJeu->width() / nbMasquesLargeur);
        hauteurMasque = qreal(gv_AireDeJeu->height() / nbMasquesHauteur);

        int nbMasques = nbMasquesLargeur * nbMasquesHauteur;
        qreal xMasque = 0.00;
        qreal yMasque = 0.00;

        qDebug()<<" -------------------------- Début boucle d'affichage : "<<nbMasques;
        for (int i =0; i<nbMasquesHauteur; i++)
        {
            int j = 0;
            while (j < nbMasquesLargeur)
            {
                m_masque = new masqueDeplaceSouris();
                m_masque->setSize(largeurMasque, hauteurMasque);
                m_masque->setPos(xMasque, yMasque);
                j++;
                xMasque += largeurMasque;
                m_masque->setColor(QColor::fromRgb(255,255,255));
                m_masque->setHideOnMouseOver(false);

                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
                qDebug()<< "masque ajoute";
            }
            xMasque = 0.00;
            yMasque += hauteurMasque;
        }
        nbMasquesLargeur = nbMasquesLargeur*2;
        nbMasquesHauteur = nbMasquesHauteur*2;
    }
}

/** Choix aléatoire du positionnement des masques interactifs
  * Connexion du slot cacheMasque sur chaque masque interactif
  */
void ExerciceSurvol::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::slotQuestionEntered()";

    // Instanciation & Demarrage du chronometre
    m_chronometre = new QTime();
    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        m_nbMasquesInteractifs = 0;
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

        while (m_nbMasquesInteractifs < opt_nbMasquesChoisis)
        {
            int alea = (qrand() % (m_listeMasquesFixes.count()));
            if (m_localDebug) qDebug() << "alea = " << alea;
            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueInteractif->setHideOnMouseOver(true);
            m_masqueInteractif->setColor(QColor::fromRgb(0,0,0));

            m_nbMasquesInteractifs++;
            if (m_localDebug) qDebug()<< "Nombre de masques survolables : " << m_nbMasquesInteractifs;
        }
    }
    m_exerciceEnCours = true;
}

/**
  * Appeler pour appuyer automatiquement sur le bouton suivant
  */
void ExerciceSurvol::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceSurvol::slotAfficheVerificationQuestionEntered()";

    if (m_exerciceEnCours)
    {
        if (m_localDebug) qDebug()<< "Click bouton suivant automatique ! " << opt_timerSuivant;
        QTimer::singleShot(opt_timerSuivant ,this,SLOT(slotAppuiAutoSuivant()));     // Click auto du bouton suivant avec un timer
    }
}

void ExerciceSurvol::slotFinVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceSurvol::slotFinVerificationQuestionEntered()";
    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    m_listeMasquesFixes.clear();

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
/** Redimensionne la consigne
  */
void ExerciceSurvol::redimensionnerConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

/** Redimensionne l'image par rapport à sa largeur ou sa hauteur.
  * Obsolète mais conserver au cas ou !
  */
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

/** Redimmensionne l'image (2e méthode)
  */
void ExerciceSurvol::redimensionnerImage2()
{
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(gv_AireDeJeu->maximumViewportSize(), Qt::IgnoreAspectRatio));
}

/** A chaque passsage sur un masque interactif, on décremente le nombre de masques interactifs.
  * Dès que les masques interactifs sont tous survolés, on affiche l'image.
  * On affiche la tête dans la boiteTete.
  * On vide m_listeMasquesFixes
  */
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
        }

        // Appui sur le bouton vérifier
        if (m_localDebug) qDebug() << "Appui sur le bouton Verifier " << opt_timerVerifier ;
        QTimer::singleShot(opt_timerVerifier,this,SLOT(slotAppuiAutoVerifier()));

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

/** Cette méthode emet le signal appuiSuivant
  * Permet donc d'activer (de simuler) l'appui sur le bouton suivant de la telecommande
  */
void ExerciceSurvol::slotAppuiAutoSuivant()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceSurvol::slotAppuiAutoSuivant()";
    emit appuiSuivant();
}

void ExerciceSurvol::slotAppuiAutoVerifier()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceSurvol::slotAppuiAutoVerifier()";
    emit appuiVerifier();
}

/** Charge les options contenues dans le fichier de configuration (parametres.ini)
  */
void ExerciceSurvol::chargerOption()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceSurvol::chargerOption()";

    QSettings parametres(QDir::currentPath()+QDir::separator()+QString("data")+QDir::separator()+QString("parametres.ini"), QSettings::IniFormat);
    opt_timerSuivant     = parametres.value("Survol/timerSuivant", 7000).toInt();
    opt_timerVerifier    = parametres.value("Survol/timerVerifier", 2000).toInt();
    opt_nbMasquesChoisis = parametres.value("Survol/nbMasquesChoisis", 7).toInt();

    if (m_localDebug)
    {
        qDebug() << "Timer Suivant      :"   << opt_timerSuivant  << "\n"
                 << "Timer Verifier     :"   << opt_timerVerifier << "\n"
                 << "Nb Masques choisis :"   << opt_nbMasquesChoisis;
    }

}

/** Cette méthode retourne la plus petite division d'entiers dont le résultat est supérieur à monChiffre
  * Elle sert au calcul de la taille des masques.
  * exemple: pour 11 masques, les divisions possibles sont 2*6 ou 3*4.
  * le QPair retourné sera 3*4, ce qui nous donne 12.
  * Donc à la première question, l'image sera divisé par 12 petits masques de taille identique (4 dans la largeur et 3 dans la hauteur)
  */
QPair<int, int> ExerciceSurvol::plusPetiteDivision(int monChiffre)
{
    QList<QPair<int,int> > listeResultat;
    QPair<int,int> paire;

    for (int a = 2; a<10; a++){
        for (int b=2; b<10; b++)
        {
            if ((a*b) > monChiffre)
            {
                if (listeResultat.isEmpty())
                {
                    listeResultat << QPair<int,int>(a,b);
                }
                else
                {
                    if (a*b < listeResultat.first().first*listeResultat.first().second)
                    {
                        listeResultat.clear();
                        listeResultat << QPair<int,int>(a,b);
                    }
                    else
                    {
                        if(a*b == listeResultat.first().first*listeResultat.first().second)
                        {
                            listeResultat << QPair<int,int>(a,b);
                        }
                    }
                }
            }
        }
    }
    if(m_localDebug) qDebug()  << listeResultat;
    int mini = monChiffre;
    for(int k = 0 ; k<listeResultat.size();k++)
    {
        if(qAbs((listeResultat[k].first - listeResultat[k].second)) < mini)
        {
            mini = qAbs((listeResultat[k].first - listeResultat[k].second));
            paire = listeResultat[k];
        }
    }
    if(m_localDebug) qDebug()<<paire;
    return paire;
}
