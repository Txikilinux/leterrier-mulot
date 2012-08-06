/** Classe ExerciceClic
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

#include "exerciceclic.h"

#include <QMessageBox>
#include <QtCore/qmath.h>

ExerciceClic::ExerciceClic(QWidget *parent, QString theme):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = true;
    m_exerciceEnCours = false;

    m_parent = parent;
    m_theme = theme;

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
    onPeutMettreEnPause = false;

    //pour l'affichage pause
    m_labelImagePause =  new QLabel(m_parent);
    m_labelTextePause = new QLabel(m_parent);


    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);

    cheminConf  = m_theme + QDir::separator()+QString("conf")+QDir::separator() + QString("parametres.conf");
    cheminImage = m_theme + QDir::separator() + QString("data") +QDir::separator() + QString("images") + QDir::separator()  ;

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

    if (m_localDebug)
    {
        qDebug() << "Chemin du fichier de configuration" << cheminConf;
        qDebug() << "Chemin des fichiers images" << cheminImage;
    }

}

ExerciceClic::~ExerciceClic()
{
    m_labelImagePause->deleteLater();
    m_labelTextePause->deleteLater();
    emit exerciceExited(); // Permet à la MainWindow de savoir que l'exercice est terminé
}

/** Charge les options contenues dans le fichier de configuration (parametres.ini)
  */
void ExerciceClic::chargerOption()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceClic::chargerOption()";

    QSettings parametres(cheminConf, QSettings::IniFormat);
    opt_timerSuivant     = parametres.value("Clic/timerSuivant", 7000).toInt();
    opt_nbMasquesChoisis = parametres.value("Clic/nbMasquesChoisis", 7).toInt();

    if (m_localDebug)
    {
        qDebug() << "Timer Suivant      :"   << opt_timerSuivant  << "\n"
                 << "Nb Masques choisis :"   << opt_nbMasquesChoisis;
    }
}

void ExerciceClic::slotSequenceEntered() // en cours
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::slotSequenceEntered()";

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
void ExerciceClic::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::slotPresenteSequenceEntered()";

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Ma consigne qui presentera la sequence (video souhaitee)");
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Clic"));

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
    QTimer::singleShot(opt_timerSuivant,this,SLOT(slotAppuiAutoSuivant()));     // Clic auto du bouton suivant avec un timer
}

/** Mettre tout ce qui est commun à chaque question
  * Aller chercher le pack image
  * Choisir 5 images au hasard dans le pack
  * Condition de garde .abe
  */
void ExerciceClic::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::slotRealisationExerciceEntered()";

    if (!m_exerciceEnCours)
    {
        // Mettre tout ce qui est commun à chaque question
        m_nbImage = m_nbTotalQuestions; // le nb image = le nb de question
        m_nbMasquesInteractifs = 0;
        boiteTetes->resetTetes(m_nbTotalQuestions);

        // aller chercher le pack image
        QDir dir(cheminImage);
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        // Si la dir est vide
        if (dir.entryInfoList().count() <= 0)
        {
            if (m_localDebug) qDebug() << "1. Verification des images <= 0";
            QMessageBox::critical(m_parent,trUtf8("Erreur"), trUtf8("L'exercice a rencontré un problème.")+
                                  QString("<strong>")+trUtf8("Veuillez vérifier votre fichier thème (.abe)")+ QString("</strong>"), 0,0);
            slotQuitterAccueil();
            return;
        }
        else
        {
            QFileInfoList list = dir.entryInfoList();
            for(int i = 0; i < list.count(); i++)
            {
                m_listeFichiers << list.at(i).absoluteFilePath();
            }
        }
        // Si la liste de fichier est vide
        if (m_listeFichiers.size() <= 0 )
        {
            if (m_localDebug) qDebug() << "2. Verification de listeFichiers <= 0";
            QMessageBox::critical(m_parent,trUtf8("Erreur"), trUtf8("L'excercice a rencontré un problème.")+
                                  QString("<strong>")+trUtf8("Veuillez vérifier votre fichier thème (.abe)")+ QString("</strong>"), 0,0);
            slotQuitterAccueil();
            return;
        }
        else
        {
            for(int i = 0; i < m_nbImage; i++) // choisir 5 images au hasard dans le pack
            {
                qsrand(QDateTime::currentDateTime ().toTime_t ());
                int n = (qrand() % (m_listeFichiers.size()));
                QFileInfo fileInfo = m_listeFichiers.takeAt(n);
                m_image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);
                m_listeImage << m_image;
            }
            AbulEduCommonStatesV1::slotRealisationExerciceEntered();

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
    }
}

/** Affichage de l'image
  * Calcul et mise en place des masques
  */
void ExerciceClic::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::slotInitQuestionEntered()";

    onPeutMettreEnPause = false;

    if(m_listeImage.count() <= 0) {
        return;
    }

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
        float largeurMasque = 0.00;
        float hauteurMasque = 0.00;

        if (m_localDebug)
        {
            qDebug() << "nb masques largeur :" << nbMasquesLargeur ;
            qDebug() << "nb masques hauteur :" << nbMasquesHauteur ;
        }

        float largeurAireJeu = static_cast<float>(gv_AireDeJeu->width())-1;
        float hauteurAireJeu = static_cast<float>(gv_AireDeJeu->height())-1;

        largeurMasque = largeurAireJeu / nbMasquesLargeur;
        hauteurMasque = hauteurAireJeu / nbMasquesHauteur;

        int nbMasques = nbMasquesLargeur * nbMasquesHauteur;
        qreal xMasque = 0.00;
        qreal yMasque = 0.00;

        qDebug()<<" -------------------------- Début boucle d'affichage : "<<nbMasques;

        for (float i=0; i<nbMasquesHauteur;i++)
        {
            for (int j =0; j < nbMasquesLargeur;j++)
            {
                m_masque = new masqueDeplaceSouris();
                m_masque->setSize(largeurMasque, hauteurMasque);
                m_masque->setPos(xMasque, yMasque);
                m_masque->setColor(QColor::fromRgb(255,255,255));

                xMasque+=largeurMasque;
                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
            }
            xMasque = 0;
            yMasque += hauteurMasque;
        }
        nbMasquesLargeur += 2;
        nbMasquesHauteur += 1;
    }
}

/** Choix aléatoire du positionnement des masques interactifs
  * Connexion du slot cacheMasque sur chaque masque interactif
  */
void ExerciceClic::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::slotQuestionEntered()";

    // Instanciation & Demarrage du chronometre
    m_chronometre = new QTime();
    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        m_nbMasquesInteractifs = 0;
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Clicables ";

        while (m_nbMasquesInteractifs < opt_nbMasquesChoisis)
        {
            int alea = (qrand() % (m_listeMasquesFixes.count()));
            if (m_localDebug) qDebug() << "alea = " << alea;
            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
//            m_masqueInteractif->setHideOnMouseOver(true);
            m_masqueInteractif->setHideOnClick(true);
            m_masqueInteractif->setColor(QColor::fromRgb(0,0,0));

            m_nbMasquesInteractifs++;
            if (m_localDebug) qDebug()<< "Nombre de masques Clicables : " << m_nbMasquesInteractifs;
        }
    }
    m_exerciceEnCours = true;
}

/**
  * Appeler pour appuyer automatiquement sur le bouton suivant
  */
void ExerciceClic::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceClic::slotAfficheVerificationQuestionEntered()";

    if (m_exerciceEnCours)
    {
        if (m_localDebug) qDebug()<< "Click bouton suivant automatique ! " << opt_timerSuivant;
        m_timer = new QTimer(this);
        m_timer->setInterval(opt_timerSuivant);
        m_timer->setSingleShot(true);
        connect(m_timer, SIGNAL(timeout()), SLOT(slotAppuiAutoSuivant()));
        m_timer->start();
    }
}

void ExerciceClic::slotFinVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceClic::slotFinVerificationQuestionEntered()";
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
void ExerciceClic::slotFinQuestionEntered()
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceClic::slotFinQuestionEntered()";

    // Affichage du temps passé Total
    if (m_localDebug) qDebug("Temps écoulé en millisecondes: %d ms", m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5);
    // On ne veut pas un chronometre précis au millième près =)
    if (m_localDebug) qDebug("Temps écoulé en secondes: %d sec", (m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void ExerciceClic::slotBilanExerciceEntered() // todo boucle pour les têtes
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceClic::slotBilanExerciceEntered()";

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
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Clic"));

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

void ExerciceClic::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::slotQuitter";

    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceClic::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    // Placement de l'AireDeJeu
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    //        gv_AireDeJeu->move((170 * ratio) / 2,50 * ratio);
    gv_AireDeJeu->move(80 * ratio, 64 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes
    //  redimensionnerConsigne();
    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"##########################  ExerciceClic::setDimensionsWidgets()---end";
}


//------------------------------------------------------------------
//                 Méthodes propres à la classe
//------------------------------------------------------------------
/** Redimensionne la consigne
  */
void ExerciceClic::redimensionnerConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

/** Redimensionne l'image par rapport à sa largeur ou sa hauteur.
  * Obsolète mais conserver au cas ou !
  */
void ExerciceClic::redimensionnerImage()
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

/** Redimensionne l'image (2e méthode)
  */
void ExerciceClic::redimensionnerImage2()
{
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(gv_AireDeJeu->maximumViewportSize(), Qt::IgnoreAspectRatio));
}

/** A chaque passsage sur un masque interactif, on décremente le nombre de masques interactifs.
  * Dès que les masques interactifs sont tous cliqués, on affiche l'image.
  * On affiche la tête dans la boiteTete.
  * On vide m_listeMasquesFixes
  */
void ExerciceClic::slotCacheMasque()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceClic::slotCacheMasque : " << m_nbMasquesInteractifs;
    m_nbMasquesInteractifs --;

    if(m_nbMasquesInteractifs == 0)
    {
        //On affiche l'image en entier
        for(int i = 0; i < m_listeMasquesFixes.count(); i++)
        {
            m_listeMasquesFixes.at(i)->setVisible(false);
        }

        // On peut mettre en pause
        onPeutMettreEnPause = true;

        // Appui sur le bouton vérifier
        QTimer::singleShot(0, this, SLOT(slotAppuiAutoVerifier()));

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
void ExerciceClic::slotAppuiAutoSuivant()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceClic::slotAppuiAutoSuivant()";
    emit appuiSuivant();
}

/** Cette méthode emet le signal appuiVerifier
  * Permet donc d'activer (de simuler) l'appui sur le bouton verifier de la telecommande
  */
void ExerciceClic::slotAppuiAutoVerifier()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceClic::slotAppuiAutoVerifier()";
    emit appuiVerifier();
}

/** Cette méthode retourne la plus petite division d'entiers dont le résultat est supérieur à monChiffre
  * Elle sert au calcul de la taille des masques.
  * exemple: pour 11 masques, les divisions possibles sont 2*6 ou 3*4.
  * le QPair retourné sera 3*4, ce qui nous donne 12.
  * Donc à la première question, l'image sera divisé par 12 petits masques de taille identique (4 dans la largeur et 3 dans la hauteur)
  */
QPair<int, int> ExerciceClic::plusPetiteDivision(int monChiffre)
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

/** Event Filter pour la pause.
  * Capture l'appui sur la barre espace lorsque le booléen "onPeutMettreEnPause" est à "true".
  * C'est le cas lorsque tous les masques sont découvert.
  */
bool ExerciceClic::eventFilter(QObject *obj, QEvent *event)
{
    obj = this;
    if(event->type() == QEvent::KeyRelease && onPeutMettreEnPause)
    {
        QPixmap pixPause(":/bouton/pause");
        float ratio = abeApp->getAbeApplicationDecorRatio();
        m_labelImagePause->setPixmap(pixPause.scaled((pixPause.width() * ratio),(pixPause.height() * ratio),Qt::KeepAspectRatio));
        m_labelTextePause->setText(trUtf8("En Pause ..."));
        //        m_labelImagePause->setStyleSheet("border:2px solid black");
        //        m_labelTextePause->setStyleSheet("border:2px solid black");

        QKeyEvent *c = dynamic_cast<QKeyEvent *>(event);
        if(c && c->key() == Qt::Key_Space )
        {
            if (m_timer->isActive())
            {
                m_timer->stop();
                qDebug() << "Le timer est actif est vient d'etre stoppé";

                boiteTetes->setVisible(false);
                m_labelImagePause->show();
                m_labelTextePause->show();

                int W = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
                int w1 = m_labelImagePause->width();
                int w2 = m_labelTextePause->width();
                int x1 = (W-(w1+w2))/2;
                int x2 = (W-(w2-w1))/2;

                m_labelImagePause->move(x1, gv_AireDeJeu->height() + m_labelImagePause->height() - 60 *ratio);
                m_labelTextePause->move(x2, gv_AireDeJeu->height() + m_labelImagePause->height());
            }
            else
            {
                m_timer->start();
                qDebug() << "Le timer repart   ";
                m_labelImagePause->setVisible(false);
                m_labelTextePause->setVisible(false);
                boiteTetes->setVisible(true);
            }

            qDebug() << "Pause !";
        }
    }
    return false;
}

