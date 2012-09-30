/** Classe ExerciceParcours
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

#include "exerciceparcours.h"

ExerciceParcours::ExerciceParcours(QWidget *parent, QString theme):
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
    m_listeMasquesParcours.clear();
    m_nbImage = 0;
    m_nbMasquesInteractifs = 0;
    opt_nbMasquesLargeur = 0;
    opt_nbMasquesHauteur = 0;
    onPeutMettreEnPause = false;
    positionMasquesParcours1.clear();
    positionMasquesParcours2.clear();
    positionMasquesParcours3.clear();
    positionMasquesParcours4.clear();
    positionMasquesParcours5.clear();

    //pour l'affichage pause
    m_labelImagePause = new QLabel(m_parent);
    m_labelTextePause = new QLabel(m_parent);

    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    // 2012/09/29
    m_tailleAireDejeu = QSize(0,0);

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

    // jlf 2012/09/29 Réactivation du bouton commencer
    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);

    // Pour les appuis automatiques sur les touches
    connect(this, SIGNAL(appuiVerifier()),getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()));
    connect(this, SIGNAL(appuiSuivant()),getAbeExerciceTelecommandeV1()->ui->btnSuivant, SIGNAL(clicked()));

    if (m_localDebug)
    {
        qDebug() << "Chemin du fichier de configuration" << cheminConf;
        qDebug() << "Chemin des fichiers images" << cheminImage;
    }
}

ExerciceParcours::~ExerciceParcours()
{
    m_labelImagePause->deleteLater();
    m_labelTextePause->deleteLater();
    emit exerciceExited(); // Permet à la MainWindow de savoir que l'exercice est terminé
}

/** Charge les options contenues dans le fichier de configuration (parametres.ini)
  */
void ExerciceParcours::chargerOption()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::chargerOption()";

    switch (m_numQuestion+1)
    {
    case 1:
    {
        QSettings parametres(cheminConf, QSettings::IniFormat);
        opt_timerSuivant     = parametres.value("Parcours1/timerSuivant", 7000).toInt();
        opt_nbMasquesLargeur = parametres.value("Parcours1/nbMasquesLargeur", 10).toInt();
        opt_nbMasquesHauteur = parametres.value("Parcours1/nbMasquesHauteur", 5).toInt();
        opt_nbMasquesChoisis = parametres.value("Parcours1/nbMasquesChoisis", 7).toInt();
    }
        break;

    case 2:
    {
        QSettings parametres(cheminConf, QSettings::IniFormat);
        opt_timerSuivant     = parametres.value("Parcours2/timerSuivant", 7000).toInt();
        opt_nbMasquesLargeur = parametres.value("Parcours2/nbMasquesLargeur", 10).toInt();
        opt_nbMasquesHauteur = parametres.value("Parcours2/nbMasquesHauteur", 5).toInt();
        opt_nbMasquesChoisis = parametres.value("Parcours2/nbMasquesChoisis", 7).toInt();
    }
        break;
    case 3:
    {
        QSettings parametres(cheminConf, QSettings::IniFormat);
        opt_timerSuivant     = parametres.value("Parcours3/timerSuivant", 7000).toInt();
        opt_nbMasquesLargeur = parametres.value("Parcours3/nbMasquesLargeur", 10).toInt();
        opt_nbMasquesHauteur = parametres.value("Parcours3/nbMasquesHauteur", 5).toInt();
        opt_nbMasquesChoisis = parametres.value("Parcours3/nbMasquesChoisis", 7).toInt();
    }
        break;
    case 4:
    {
        QSettings parametres(cheminConf, QSettings::IniFormat);
        opt_timerSuivant     = parametres.value("Parcours4/timerSuivant", 7000).toInt();
        opt_nbMasquesLargeur = parametres.value("Parcours4/nbMasquesLargeur", 10).toInt();
        opt_nbMasquesHauteur = parametres.value("Parcours4/nbMasquesHauteur", 5).toInt();
        opt_nbMasquesChoisis = parametres.value("Parcours4/nbMasquesChoisis", 7).toInt();
    }
        break;
    case 5:
    {
        QSettings parametres(cheminConf, QSettings::IniFormat);
        opt_timerSuivant     = parametres.value("Parcours5/timerSuivant", 7000).toInt();
        opt_nbMasquesLargeur = parametres.value("Parcours5/nbMasquesLargeur", 10).toInt();
        opt_nbMasquesHauteur = parametres.value("Parcours5/nbMasquesHauteur", 5).toInt();
        opt_nbMasquesChoisis = parametres.value("Parcours5/nbMasquesChoisis", 7).toInt();
    }
        break;
    default:
        return;
        break;
    } // fin switch
}

void ExerciceParcours::chargerPositionMasque(int numeroQuestion)
{
    QSettings parametres(cheminConf, QSettings::IniFormat);
    if(numeroQuestion == 1)
    {
        // aller dans le groupe Parcours1, et tout récupérer
        parametres.beginGroup("position1");
        for (int i =0 ; i < parametres.childKeys().count(); i++)
        {
            positionMasquesParcours1 << parametres.value(parametres.childKeys().at(i)).toInt();
        }
    }
    else if (numeroQuestion == 2)
    {
        // aller dans le groupe Parcours2, et tout récupérer
        parametres.beginGroup("position2");
        for (int i =0 ; i < parametres.childKeys().count(); i++)
        {
            positionMasquesParcours2 << parametres.value(parametres.childKeys().at(i)).toInt();
        }
    }
    else if (numeroQuestion == 3)
    {
        // aller dans le groupe Parcours3, et tout récupérer
        parametres.beginGroup("position3");
        for (int i =0 ; i < parametres.childKeys().count(); i++)
        {
            positionMasquesParcours3 << parametres.value(parametres.childKeys().at(i)).toInt();
        }
    }
    else if (numeroQuestion == 4)
    {
        // aller dans le groupe Parcours4, et tout récupérer
        parametres.beginGroup("position4");
        for (int i =0 ; i < parametres.childKeys().count(); i++)
        {
            positionMasquesParcours4 << parametres.value(parametres.childKeys().at(i)).toInt();
        }
    }
    else if (numeroQuestion == 5)
    {
        // aller dans le groupe Parcours5, et tout récupérer
        parametres.beginGroup("position5");
        for (int i =0 ; i < parametres.childKeys().count(); i++)
        {
            positionMasquesParcours5 << parametres.value(parametres.childKeys().at(i)).toInt();
        }
    }
}

void ExerciceParcours::slotSequenceEntered() // en cours
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotSequenceEntered()";

    if(!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        abeEnableAnimationsConsigneBilan(false);
        setAbeNbExercices(1);      // a instancier avant appel du slot SequenceEntered !
        setAbeNbTotalQuestions(5); // a instancier avant appel du slot SequenceEntered !

        AbulEduCommonStatesV1::slotSequenceEntered();

        // jlf 2012/09/29 Réactivation du bouton commencer
        presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);

        setAbeLevel("1"); // a instancier après le slot sinon niveau 0 par def.
    }
}

/** Affichage de la consigne
  */
void ExerciceParcours::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotPresenteSequenceEntered()";

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Ma consigne qui presentera la sequence (video souhaitee)");
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Parcours"));

    QString debutTableau = "<tr>";
    QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
    QString consigne = "<td> " + trUtf8("Ma consigne qui presentera la sequence (video souhaitee). Ma consigne est tres longue car je veux tester son comportement") +" </td>" ;
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
void ExerciceParcours::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotRealisationExerciceEntered()";

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
        }
    }
}

/** Affichage de l'image
  * Calcul et mise en place des masques
  */
void ExerciceParcours::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotInitQuestionEntered()";

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
            qDebug() << "nb masques largeur :" << opt_nbMasquesLargeur;
            qDebug() << "nb masques hauteur :" << opt_nbMasquesHauteur;
        }

        float largeurAireJeu = static_cast<float>(gv_AireDeJeu->width())-1;
        float hauteurAireJeu = static_cast<float>(gv_AireDeJeu->height())-1;

        largeurMasque = largeurAireJeu / opt_nbMasquesLargeur;
        hauteurMasque = hauteurAireJeu / opt_nbMasquesHauteur;

        int nbMasques = opt_nbMasquesLargeur * opt_nbMasquesHauteur;
        qreal xMasque = 0.00;
        qreal yMasque = 0.00;

        qDebug()<<" -------------------------- Début boucle d'affichage : "<<nbMasques;

        int numeroMasque = 0;
        for (float i=0; i<opt_nbMasquesHauteur;i++)
        {

            for (int j =0; j < opt_nbMasquesLargeur;j++)
            {
                m_masque = new masqueDeplaceSouris(0, numeroMasque);
                m_masque->setSize(largeurMasque, hauteurMasque);
                m_masque->setPos(xMasque, yMasque);
                m_masque->setColor(QColor::fromRgb(255,255,255));

                xMasque+=largeurMasque;
                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
                numeroMasque ++;
            }
            xMasque = 0;
            yMasque += hauteurMasque;
        }
    }
}

/** Choix aléatoire du positionnement des masques interactifs
  * Connexion du slot cacheMasque sur chaque masque interactif
  */
void ExerciceParcours::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotQuestionEntered()";

    // Instanciation & Demarrage du chronometre
    m_chronometre = new QTime();
    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        qDebug() << m_numQuestion;
        switch (m_numQuestion)
        {
        case 1:
        {
            // Chargement et Controle de la liste
            chargerPositionMasque(1);
            if (positionMasquesParcours1.isEmpty() && positionMasquesParcours1.count() != opt_nbMasquesChoisis)
            {
                qDebug() << "PROBLEME Liste parcours 1";
                return;
            }
            /// Masque arrivee (1 de la liste positionMasque)
            m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours1.takeFirst());
            m_masqueArrivee->setColor(QColor(Qt::red));
            connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            /// Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee)
            m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours1.takeFirst());
            m_masqueDepart->setColor(QColor(Qt::green));
            connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueDepart->setHideOnMouseOver(false);
            m_masqueDepart->setHideOnClick(true);
            m_listeMasquesParcours << m_masqueDepart; // en premier
            /// Masque parcours (le reste de la liste)
            while (!positionMasquesParcours1.isEmpty())
            {
                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours1.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
                m_listeMasquesParcours << m_masqueParcours;
            }
            m_listeMasquesParcours << m_masqueArrivee; // en dernier
        }
            break;
        case 2:
        {
            // Chargement et Controle de la liste
            chargerPositionMasque(2);
            if (positionMasquesParcours2.isEmpty() && positionMasquesParcours2.count() != opt_nbMasquesChoisis)
            {
                qDebug() << "PROBLEME Liste parcours 2";
                return;
            }
            /// Masque arrivee (1 de la liste positionMasque)
            m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours2.takeFirst());
            m_masqueArrivee->setColor(QColor(Qt::red));
            connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            /// Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee)
            m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours2.takeFirst());
            m_masqueDepart->setColor(QColor(Qt::green));
            connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueDepart->setHideOnMouseOver(false);
            m_masqueDepart->setHideOnClick(true);
            m_listeMasquesParcours << m_masqueDepart; // en premier
            /// Masque parcours (le reste de la liste)
            while (!positionMasquesParcours2.isEmpty())
            {
                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours2.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
                m_listeMasquesParcours << m_masqueParcours;
            }
            m_listeMasquesParcours << m_masqueArrivee; // en dernier

        }
            break;
        case 3:
        {
            // Chargement et Controle de la liste
            chargerPositionMasque(3);
            if (positionMasquesParcours3.isEmpty() && positionMasquesParcours3.count() != opt_nbMasquesChoisis)
            {
                qDebug() << "PROBLEME Liste parcours 3";
                return;
            }
            /// Masque arrivee (1 de la liste positionMasque)
            m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours3.takeFirst());
            m_masqueArrivee->setColor(QColor(Qt::red));
            connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            /// Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee)
            m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours3.takeFirst());
            m_masqueDepart->setColor(QColor(Qt::green));
            connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueDepart->setHideOnMouseOver(false);
            m_masqueDepart->setHideOnClick(true);
            m_listeMasquesParcours << m_masqueDepart; // en premier
            /// Masque parcours (le reste de la liste)
            while (!positionMasquesParcours3.isEmpty())
            {
                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours3.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
                m_listeMasquesParcours << m_masqueParcours;
            }
            m_listeMasquesParcours << m_masqueArrivee; // en dernier
        }
            break;
        case 4:
        {
            // Chargement et Controle de la liste
            chargerPositionMasque(4);
            if (positionMasquesParcours4.isEmpty() && positionMasquesParcours4.count() != opt_nbMasquesChoisis)
            {
                qDebug() << "PROBLEME Liste parcours 4";
                return;
            }
            /// Masque arrivee (1 de la liste positionMasque)
            m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours4.takeFirst());
            m_masqueArrivee->setColor(QColor(Qt::red));
            connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            /// Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee)
            m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours4.takeFirst());
            m_masqueDepart->setColor(QColor(Qt::green));
            connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueDepart->setHideOnMouseOver(false);
            m_masqueDepart->setHideOnClick(true);
            m_listeMasquesParcours << m_masqueDepart; // en premier
            /// Masque parcours (le reste de la liste)
            while (!positionMasquesParcours4.isEmpty())
            {
                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours4.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
                m_listeMasquesParcours << m_masqueParcours;
            }
            m_listeMasquesParcours << m_masqueArrivee; // en dernier
        }
            break;
        case 5:
        {
            // Chargement et Controle de la liste
            chargerPositionMasque(5);
            if (positionMasquesParcours5.isEmpty() && positionMasquesParcours5.count() != opt_nbMasquesChoisis)
            {
                qDebug() << "PROBLEME Liste parcours 5";
                return;
            }
            /// Masque arrivee (1 de la liste positionMasque)
            m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours5.takeFirst());
            m_masqueArrivee->setColor(QColor(Qt::red));
            connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            /// Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee)
            m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours5.takeFirst());
            m_masqueDepart->setColor(QColor(Qt::green));
            connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
            m_masqueDepart->setHideOnMouseOver(false);
            m_masqueDepart->setHideOnClick(true);
            m_listeMasquesParcours << m_masqueDepart; // en premier
            /// Masque parcours (le reste de la liste)
            while (!positionMasquesParcours5.isEmpty())
            {
                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours5.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
                m_listeMasquesParcours << m_masqueParcours;
            }
            m_listeMasquesParcours << m_masqueArrivee; // en dernier
        }
            break;
        default:
        {
            qDebug() << "Defaut switch ";
        }
            break;
        }

    }
    m_exerciceEnCours = true;
}

/**
  * Appeler pour appuyer automatiquement sur le bouton suivant
  */
void ExerciceParcours::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceParcours::slotAfficheVerificationQuestionEntered()";

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

void ExerciceParcours::slotFinVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceParcours::slotFinVerificationQuestionEntered()";
    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    m_listeMasquesFixes.clear();

    boiteTetes->setEtatTete(m_numQuestion-1, abe::evalA );

    // Vider itemImage
    gv_AireDeJeu->scene()->removeItem(m_itemImage);
    gv_AireDeJeu->scene()->clear();
    gv_AireDeJeu->show();
    m_exerciceEnCours = false;
}

void ExerciceParcours::slotFinQuestionEntered()
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceParcours::slotFinQuestionEntered()";

    // Affichage du temps passé Total
    if (m_localDebug) qDebug("Temps écoulé en millisecondes: %d ms", m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5);
    // On ne veut pas un chronometre précis au millième près =)
    if (m_localDebug) qDebug("Temps écoulé en secondes: %d sec", (m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void ExerciceParcours::slotBilanExerciceEntered()
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceParcours::slotBilanExerciceEntered()";

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

void ExerciceParcours::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotQuitter";

    AbulEduCommonStatesV1::slotQuitter();
}


/** Cette méthode emet le signal appuiSuivant
  * Permet donc d'activer (de simuler) l'appui sur le bouton suivant de la telecommande
  */
void ExerciceParcours::slotAppuiAutoSuivant()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::slotAppuiAutoSuivant()";
    emit appuiSuivant();
}

/** Cette méthode emet le signal appuiVerifier
  * Permet donc d'activer (de simuler) l'appui sur le bouton verifier de la telecommande
  */
void ExerciceParcours::slotAppuiAutoVerifier()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::slotAppuiAutoVerifier()";
    emit appuiVerifier();
}


//------------------------------------------------------------------
//                 Méthodes propres à la classe
//------------------------------------------------------------------
/** Redimensionne la consigne
  */
void ExerciceParcours::redimensionnerConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

/** Redimensionne l'image (2e méthode)
  */
void ExerciceParcours::redimensionnerImage2()
{
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(m_tailleAireDejeu, Qt::KeepAspectRatio));
    gv_AireDeJeu->setFixedSize(m_itemImage->boundingRect().size().toSize());
   // positionner l'aire de jeu au centre */
    float ratio = abeApp->getAbeApplicationDecorRatio();
    gv_AireDeJeu->move((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width()
                        -gv_AireDeJeu->width())/2 + 40 * ratio,
                       (getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height()
                        - 60 * ratio -gv_AireDeJeu->height())/2 + 32 * ratio);

}

void ExerciceParcours::setDimensionsWidgets()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::setDimensionsWidgets()---start";

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

    m_tailleAireDejeu = gv_AireDeJeu->size();
    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes
    //  redimensionnerConsigne();
    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::setDimensionsWidgets()---end";
}

void ExerciceParcours::slotCacheMasque()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::slotCacheMasque : ";

    //j'enleve le premier masque de la ma liste de parcours
    if(!m_listeMasquesParcours.isEmpty())
    {
        m_listeMasquesParcours.removeFirst();
        if (m_listeMasquesParcours.count() != 0)
        {
            if (m_listeMasquesParcours.count() == 1)
                m_listeMasquesParcours.first()->setHideOnClick(true);
            else
                m_listeMasquesParcours.first()->setHideOnMouseOver(true);
        }
    }

    if (m_listeMasquesParcours.isEmpty())
    {
        foreach (masqueDeplaceSouris* var_masque, m_listeMasquesFixes)
        {
            var_masque->setVisible(false);
        }

        // Faire tout ce qu'il faut qd j'ai plus de masque
        // On peut mettre en pause
        onPeutMettreEnPause = true;

        // Appui sur le bouton vérifier
        QTimer::singleShot(0,this,SLOT(slotAppuiAutoVerifier()));

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

/** Event Filter pour la pause.
  * Capture l'appui sur la barre espace lorsque le booléen "onPeutMettreEnPause" est à "true".
  * C'est le cas lorsque tous les masques sont découvert.
  */
bool ExerciceParcours::eventFilter(QObject *obj, QEvent *event)
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
