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
//    m_listeMasquesFixes.clear();
    m_nbImage = 0;
    m_nbMasquesInteractifs = 0;
    opt_nbMasquesLargeur = 0;
    opt_nbMasquesHauteur = 0;
    onPeutMettreEnPause = false;

    //pour l'affichage pause
    m_labelImagePause = new QLabel(m_parent);
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

ExerciceParcours::~ExerciceParcours()
{
    m_labelImagePause->deleteLater();
    m_labelTextePause->deleteLater();
    emit exerciceExited(); // Permet à la MainWindow de savoir que l'exercice est terminé
}

void ExerciceParcours::chargerOption()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::chargerOption()";

//    QSettings parametres(cheminConf, QSettings::IniFormat);
//    opt_timerSuivant     = parametres.value("Survol/timerSuivant", 7000).toInt();
//    opt_timerVerifier    = parametres.value("Survol/timerVerifier", 2000).toInt();
//    opt_nbMasquesChoisis = parametres.value("Survol/nbMasquesChoisis", 7).toInt();

//    if (m_localDebug)
//    {
//        qDebug() << "Timer Suivant      :"   << opt_timerSuivant  << "\n"
//                 << "Timer Verifier     :"   << opt_timerVerifier << "\n"
//                 << "Nb Masques choisis :"   << opt_nbMasquesChoisis;
//    }
        opt_timerSuivant     = 1000;
        opt_timerVerifier    = 1000;
        opt_nbMasquesChoisis = 7;
        opt_nbMasquesLargeur = 10;
        opt_nbMasquesHauteur = 5;
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
        presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);

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

        for (float i=0; i<opt_nbMasquesHauteur;i++)
        {

            for (int j =0; j < opt_nbMasquesLargeur;j++)
            {
                m_masque = new masqueDeplaceSouris();
                m_masque->setSize(largeurMasque, hauteurMasque);
                m_masque->setPos(xMasque, yMasque);
                m_masque->setColor(QColor::fromRgb(255,255,255));
                m_masque->setHideOnMouseOver(false);

                xMasque+=largeurMasque;
                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
            }
            xMasque = 0;
            yMasque += hauteurMasque;
        }
//        nbMasquesLargeur += 2;
//        nbMasquesHauteur += 1;
    }
}

/** Choix aléatoire du positionnement des masques interactifs
  * Connexion du slot cacheMasque sur chaque masque interactif
  */
void ExerciceParcours::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotQuestionEntered()";

    // Instanciation & Demarrage du chronometre
//    m_chronometre = new QTime();
//    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        m_nbMasquesInteractifs = 0;
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

//        while (m_nbMasquesInteractifs < opt_nbMasquesChoisis)
//        {
//            int alea = (qrand() % (m_listeMasquesFixes.count()));
//            if (m_localDebug) qDebug() << "alea = " << alea;
//            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
//            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
//            m_masqueInteractif->setHideOnMouseOver(true);
//            m_masqueInteractif->setColor(QColor::fromRgb(0,0,0));

//            m_nbMasquesInteractifs++;
//            if (m_localDebug) qDebug()<< "Nombre de masques survolables : " << m_nbMasquesInteractifs;
//        }

//        // Choisir le premier masque de départ
//        int alea = (qrand() % (m_listeMasquesFixes.count()));
//        m_masqueDepart = m_listeMasquesFixes.takeAt(alea);
//        m_masqueDepart->setHideOnMouseOver(false);
//        m_masqueDepart->setColor(QColor(Qt::green));
//////        qDebug() << "masque depart" << alea;
//        m_nbMasquesInteractifs++;

//        while (m_nbMasquesInteractifs < opt_nbMasquesChoisis)

//        {
////            alea ++;
//            m_masqueParcours = m_listeMasquesFixes.takeAt(alea);
//            m_masqueParcours->setHideOnMouseOver(false);
//            m_masqueParcours->setColor(QColor(Qt::black));
//            m_nbMasquesInteractifs++;

//        }


//        qDebug() << m_listeMasquesFixes.count();



    }
    m_exerciceEnCours = true;
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
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(gv_AireDeJeu->maximumViewportSize(), Qt::IgnoreAspectRatio));
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

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes
    //  redimensionnerConsigne();
    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"##########################  ExerciceSurvol::setDimensionsWidgets()---end";
}
