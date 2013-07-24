/** Classe ExerciceClic
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
  * @author 2013 Eric Seigne <eric.seigne@ryxeo.com>
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

ExerciceClic::ExerciceClic(QWidget *parent, QString theme):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = false;
    m_exerciceEnCours = false;

    m_parent = parent;
    m_theme = theme;

    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()), Qt::UniqueConnection);

    /* Création de l'aire de travail + propriétés */
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));

    /* On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget */
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu) ;
    proxy->setGeometry(gv_AireDeJeu->rect());
    proxy->setZValue(-1) ;

    /* Instanciation des variables membres */
    m_listeImage.clear();
    m_listeFichiers.clear();
    m_listeMasquesFixes.clear();
    m_nbImage = m_nbMasquesInteractifs = nbMasquesLargeur = nbMasquesHauteur = 0;
    onPeutMettreEnPause = false;

    /* Pour l'affichage de la pause */
    m_labelImagePause = new QLabel(m_parent);
    m_labelTextePause = new QLabel(m_parent);


    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    /* Fond transparent */
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)");
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    // 2012/09/29
    m_tailleAireDejeu = QSize(0,0);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);

    cheminConf  = m_theme + QDir::separator()+QString("conf")+QDir::separator() + QString("parametres.conf");
    cheminImage = m_theme + QDir::separator() + QString("data") +QDir::separator() + QString("images") + QDir::separator()  ;

    chargerOption();

    /* Demarrage de la machine à états */
    sequenceMachine->start();
    /* Assignation des propriétés de la télécommande */
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", true);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnNiveau  , "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", true);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnNiveau  , "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", true);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnNiveau  , "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    afficheVerificationQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);

    // jlf 2012/09/29 Réactivation du bouton commencer
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);

    /* Pour les appuis automatiques sur les touches */
    connect(this, SIGNAL(appuiVerifier()),getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()), Qt::UniqueConnection);
    connect(this, SIGNAL(appuiSuivant()),getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()), Qt::UniqueConnection);

    if (m_localDebug)
    {
        qDebug() << "Chemin du fichier de configuration" << cheminConf;
        qDebug() << "Chemin des fichiers images" << cheminImage;
    }

    /* Gestion Consignes & Aide */
    onPeutPresenterExercice = false;
    onPeutPresenterSequence = false;

    m_timer = new QTimer(this);
    m_timer->setInterval(opt_timerSuivant*1000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(slotAppuiAutoSuivant()), Qt::UniqueConnection);

    keySpace = new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1);
}

ExerciceClic::~ExerciceClic()
{
    m_labelImagePause->deleteLater();
    m_labelTextePause->deleteLater();
    /* Permet à la MainWindow de savoir que l'exercice est terminé */
    emit exerciceExited();
}

void ExerciceClic::chargerOption()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    QSettings parametres(cheminConf, QSettings::IniFormat);
    parametres.beginGroup("clic");
    if (parametres.value("exerciceActive",false) == false)
    {
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Exercice absent du module"),trUtf8("Ce module ne contient pas de paramètres pour l'exercice <b>Clic</b>"));
        messageBox->show();
        slotQuitterAccueil();
    }
    opt_timerSuivant     = parametres.value("timerSuivant", 7).toInt();
    opt_nbMasquesChoisis = parametres.value("nbMasquesChoisis", 7).toInt();

    if (m_localDebug)
    {
        qDebug() << "Timer Suivant      :"   << opt_timerSuivant  << "\n"<< "Nb Masques choisis :"   << opt_nbMasquesChoisis;
    }
}

void ExerciceClic::slotSequenceEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    if(!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        setAbeNbExercices(1);      // a instancier avant appel du slot SequenceEntered !
        setAbeNbTotalQuestions(5); // a instancier avant appel du slot SequenceEntered !

        AbulEduCommonStatesV1::slotSequenceEntered();

        // 2012/09/29 Réactivation du bouton commencer jlf
        presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);

        setAbeLevel("1"); // a instancier après le slot sinon niveau 0 par def.
    }

}

void ExerciceClic::slotAide()
{
    eventFilter(this, keySpace);
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Clique sur les rectangles noirs pour faire apparaitre l'image.")+"<br />"
            + trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.") +" </td>" ;

    m_messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,false, 0);
    connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    m_messageBox->setWink();
    m_messageBox->show();
}

void ExerciceClic::slotFermetureAide()
{
    eventFilter(this, keySpace);
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(true);
}

void ExerciceClic::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    if (!m_exerciceEnCours)
    {
        /* Mettre tout ce qui est commun à chaque question */
        m_nbImage = getAbeNbTotalQuestions(); // le nb image = le nb de question
        m_nbMasquesInteractifs = 0;
        boiteTetes->resetTetes(getAbeNbTotalQuestions());

        /* aller chercher le pack image */
        QDir dir(cheminImage);
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        /* Si la dir est vide */
        if (dir.entryInfoList().count() <= 0){
            if (m_localDebug) qDebug() << "1. Verification des images <= 0";
            QMessageBox::critical(m_parent,trUtf8("Erreur"), trUtf8("L'exercice a rencontré un problème.")+
                                  QString("<strong>")+trUtf8("Veuillez vérifier votre fichier thème (.abe)")+ QString("</strong>"), 0,0);
            slotQuitterAccueil();
            return;
        }
        else{
            const QFileInfoList list = dir.entryInfoList();
            for(int i = 0; i < list.count(); i++)
            {
                m_listeFichiers << list.at(i).absoluteFilePath();
            }
        }
        /* Si la liste de fichier est vide */
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
            /* choisir 5 images au hasard dans le pack */
            for(int i = 0; i < m_nbImage; i++)
            {
                qsrand(QDateTime::currentDateTime().toTime_t());
                int n = (qrand() % (m_listeFichiers.size()));
                QFileInfo fileInfo = m_listeFichiers.takeAt(n);
                m_image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);
                m_listeImage << m_image;
            }
            AbulEduCommonStatesV1::slotRealisationExerciceEntered();

            QPair<int, int> divisionEcran = AbulEduTools::plusPetiteDivision(opt_nbMasquesChoisis);
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

void ExerciceClic::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

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
        m_itemImage->setVisible(false);
        redimensionnerImage2();

        gv_AireDeJeu->show();
        getAbeExerciceTelecommandeV1()->ui->pbarQuestion->setValue(getAbeNumQuestion());

        //--------- Calcul de la taille des masques
        float largeurMasque, hauteurMasque = 0.00;

        if (m_localDebug)
        {
            qDebug() << "nb masques largeur :" << nbMasquesLargeur ;
            qDebug() << "nb masques hauteur :" << nbMasquesHauteur ;
        }

        const float largeurAireJeu = static_cast<float>(gv_AireDeJeu->width())-1;
        const float hauteurAireJeu = static_cast<float>(gv_AireDeJeu->height())-1;

        largeurMasque = largeurAireJeu / nbMasquesLargeur;
        hauteurMasque = hauteurAireJeu / nbMasquesHauteur;

        const int nbMasques = nbMasquesLargeur * nbMasquesHauteur;
        qreal xMasque, yMasque = 0.00;

        if (m_localDebug) qDebug()<<"Début boucle d'affichage : "<<nbMasques;

        for (float i=0; i<nbMasquesHauteur;i++)
        {
            for (int j =0; j < nbMasquesLargeur;j++)
            {
                m_masque = new MasqueDeplaceSouris();
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

void ExerciceClic::slotQuestionEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    /* Instanciation & Demarrage du chronometre */
    m_chronometre = new QTime();
    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        m_itemImage->setVisible(true);
        m_nbMasquesInteractifs = 0;
        int alea = 0;

        while (m_nbMasquesInteractifs < opt_nbMasquesChoisis)
        {
            alea = (qrand() % (m_listeMasquesFixes.count()));
            if (m_localDebug) qDebug() << "alea = " << alea;
            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
            m_masqueInteractif->setHideOnClick(true);
            m_masqueInteractif->setColor(QColor::fromRgb(0,0,0));

            m_nbMasquesInteractifs++;
            if (m_localDebug) qDebug()<< "Nombre de masques Clicables : " << m_nbMasquesInteractifs;
        }
    }
    m_exerciceEnCours = true;
}

void ExerciceClic::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    if (m_exerciceEnCours)
    {
        if (m_localDebug) qDebug()<< "Click bouton suivant automatique ! " << opt_timerSuivant;
        m_timer->start();
    }
    abeStateMachineSetVerifieReponse(verifieReponse());
    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();

    /* Desactivation delai auto de la stateMachine */
    if(sequenceMachine->cancelDelayedEvent(id_temporisation))
        if(m_localDebug) qDebug() << "DESACTIVATION DELAI OK";

}

void ExerciceClic::slotFinVerificationQuestionEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    m_listeMasquesFixes.clear();
    boiteTetes->setEtatTete(getAbeNumQuestion()-1, abe::evalA );

    /* Vider itemImage */
    gv_AireDeJeu->scene()->removeItem(m_itemImage);
    gv_AireDeJeu->scene()->clear();
    gv_AireDeJeu->show();
    m_exerciceEnCours = false;
}

void ExerciceClic::slotFinQuestionEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    /* Affichage du temps passé Total */
    if (m_localDebug) qDebug() << "Temps écoulé en millisecondes: " << m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5;
    /* On ne veut pas un chronometre précis au millième près =) */
    if (m_localDebug) qDebug() << "Temps écoulé en secondes: " << (m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000;

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void ExerciceClic::slotBilanExerciceEntered()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    /* Variables locales */
    int m_minute, m_seconde;
    QString debutTableau, imagetete, consigne, finTableau;

    /* Les heures ne sont pas gérées ; Arrondi à l'entier supérieur */
    m_tempsTotal = qCeil((m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);
    /* Cas ou le temps total (exprimé en secondes) est supérieur à une minute => conversion */
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

    /*-------------------------------------------
       Boucle pour l'affichage de la consigne !
     -------------------------------------------*/
    if (m_tempsTotal <= 59) // Affichage en secondes
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 secondes.").arg( QString::number(m_tempsTotal)) +" </td>" ;
    }
    else if((m_tempsTotal%60) == 0) // Affichage que en minutes (pas besoin d'afficher "en 0 secondes" :p)
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 minute(s).").arg(QString::number(m_minute))+" </td>" ;
    }
    else if (m_tempsTotal > 59) // Affichage en minutes et secondes
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 minute(s) et %2 secondes").arg(QString::number(m_minute)).arg(QString::number(m_seconde)) + " </td>" ;
    }
    finTableau = "</tr>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);

    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
    getAbeExerciceMessageV1()->setVisible(true);

    redimensionnerConsigne();
}

void ExerciceClic::slotQuitter()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceClic::setDimensionsWidgets()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;

    /* Placement de AireDeTravailV1 */
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    /* Placement de WidgetTelecommandeV1 */
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    /* Placement de l'AireDeJeu */
    const int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    const int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));

    gv_AireDeJeu->move(80 * ratio, 64 * ratio);
    m_tailleAireDejeu = gv_AireDeJeu->size();

    /* Placement des têtes */
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    AbulEduCommonStatesV1::setDimensionsWidgets();
}


//------------------------------------------------------------------
//                 Méthodes propres à la classe
//------------------------------------------------------------------
void ExerciceClic::redimensionnerConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

void ExerciceClic::redimensionnerImage()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    if (m_localDebug) qDebug()<< m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
    int largeurImage = m_itemImage->pixmap().width();
    int hauteurImage = m_itemImage->pixmap().height();

    /* Redimensionner l'image avec condition de garde car peut lever une exception donc bug */
    if(!m_itemImage->pixmap().isNull())
    {
        /* Si l'image est plus large que haute */
        if (largeurImage > hauteurImage)
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToHeight(gv_AireDeJeu->height(), Qt::SmoothTransformation));
            if (m_localDebug)  qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            // Centrage de l'image
            m_itemImage->moveBy(differenceLargeur/2,0);

        }
        /* Si l'image est plus haute que large */
        else
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToWidth(gv_AireDeJeu->width(), Qt::SmoothTransformation));
            if (m_localDebug)  qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            /* Centrage de l'image */
            m_itemImage->moveBy(differenceLargeur/2,0);
        }
    }
}

void ExerciceClic::redimensionnerImage2()
{
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(m_tailleAireDejeu, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    gv_AireDeJeu->setFixedSize(m_itemImage->boundingRect().size().toSize());
    /* positionner l'aire de jeu au centre */
    float ratio = abeApp->getAbeApplicationDecorRatio();
    gv_AireDeJeu->move((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width()
                        -gv_AireDeJeu->width())/2 + 40 * ratio,
                       (getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height()
                        - 60 * ratio -gv_AireDeJeu->height())/2 + 32 * ratio);
}

void ExerciceClic::slotCacheMasque()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    m_nbMasquesInteractifs --;

    if(m_nbMasquesInteractifs == 0)
    {
        /* On affiche l'image en entier */
        for(int i = 0; i < m_listeMasquesFixes.count(); i++)
        {
            m_listeMasquesFixes.at(i)->setVisible(false);
        }

        /* On peut mettre en pause */
        onPeutMettreEnPause = true;
        /* Appui sur le bouton vérifier */
        QTimer::singleShot(0, this, SLOT(slotAppuiAutoVerifier()));

        boiteTetes->setEtatTete(getAbeNumQuestion()-1, abe::evalA );
        m_listeMasquesFixes.clear();

        /* Affichage du temps passé */
        if (m_localDebug)
        {
            qDebug("Temps écoulé: %d ms",     m_chronometre->elapsed());
            qDebug("Temps écoulé: %d sec",     (m_chronometre->elapsed())/1000);
        }

        /* Enregistrement du temps passé pour chaque question */
        switch (getAbeNumQuestion()){
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

void ExerciceClic::slotAppuiAutoSuivant()
{
    emit appuiSuivant();
}

void ExerciceClic::slotAppuiAutoVerifier()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << __FILE__ << __LINE__;
    emit appuiVerifier();
}

bool ExerciceClic::eventFilter(QObject *obj, QEvent *event)
{
    obj = this;

    if(event->type() == QEvent::KeyRelease && onPeutMettreEnPause)
    {
        QKeyEvent *c = dynamic_cast<QKeyEvent *>(event);
        if(c && c->key() == Qt::Key_Space )
        {
            pause();
        }
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *castMouseEvent;
        castMouseEvent = dynamic_cast<QMouseEvent *>(event);
        if(boiteTetes->geometry().contains(castMouseEvent->pos()) || m_labelImagePause->geometry().contains(castMouseEvent->pos()))
        {
            pause();
        }
    }
    else {
        event->ignore();
    }
    return false;
}

void ExerciceClic::pause()
{
    QPixmap pixPause(":/bouton/pause");
    float ratio = abeApp->getAbeApplicationDecorRatio();
    m_labelImagePause->setPixmap(pixPause.scaled((pixPause.width() * ratio),(pixPause.height() * ratio),Qt::KeepAspectRatio));
    m_labelTextePause->setText(trUtf8("En Pause ..."));
    m_labelImagePause->setStyleSheet("background-color: transparent");
    m_labelTextePause->setStyleSheet("background-color: transparent");

    if( m_timer->isActive())
    {
        m_timer->stop();

        qDebug() << m_timer->isActive();
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
        if(m_localDebug) qDebug() << "Le timer repart";
        m_labelImagePause->setVisible(false);
        m_labelTextePause->setVisible(false);
        boiteTetes->setVisible(true);
    }
}
