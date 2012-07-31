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

    QSettings parametres(cheminConf, QSettings::IniFormat);
    opt_timerSuivant     = parametres.value("Parcours/timerSuivant", 7000).toInt();
    opt_timerVerifier    = parametres.value("Parcours/timerVerifier", 2000).toInt();
    opt_nbMasquesLargeur = parametres.value("Parcours/nbMasquesLargeur", 10).toInt();
    opt_nbMasquesHauteur = parametres.value("Parcours/nbMasquesHauteur", 5).toInt();
    opt_nbMasquesChoisis = parametres.value("Parcours/nbMasquesChoisis", 7).toInt();

    // aller dans le groupe Parcours1, et tout récupérer
    parametres.beginGroup("Parcours1");
    qDebug() << parametres.childKeys(); //retourne ("MasqueArrivee","MasqueDepart","MasqueParcours0","MasqueParcours1","MasqueParcours2","MasqueParcours3","MasqueParcours4")

    for (int i =0 ; i < parametres.childKeys().count(); i++)
    {
        positionMasquesParcours1 << parametres.value(parametres.childKeys().at(i)).toInt();
    }
    /// Ok les positions de parcours 1 sont dans ma liste

    if (m_localDebug)
    {
        qDebug() << positionMasquesParcours1;
        //        qDebug() << "Timer Suivant      :"   << opt_timerSuivant  << "\n"
        //                 << "Timer Verifier     :"   << opt_timerVerifier << "\n"
        //                 << "Nb Masques choisis :"   << opt_nbMasquesChoisis;
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

        int numeroMasque = 0;
        for (float i=0; i<opt_nbMasquesHauteur;i++)
        {

            for (int j =0; j < opt_nbMasquesLargeur;j++)
            {
                m_masque = new masqueDeplaceSouris(0, numeroMasque);
                m_masque->setSize(largeurMasque, hauteurMasque);
                m_masque->setPos(xMasque, yMasque);
                m_masque->setColor(QColor::fromRgb(255,255,255));
                m_masque->setHideOnMouseOver(false);

                xMasque+=largeurMasque;
                gv_AireDeJeu->scene()->addItem(m_masque);
                m_listeMasquesFixes << m_masque;
                numeroMasque ++;
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
        //        m_nbMasquesInteractifs = 0;
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

        //placer le l'arrivee et le depart (les deux premiers de la liste)
        // Controle de la liste
        if (positionMasquesParcours1.isEmpty() && positionMasquesParcours1.count() != opt_nbMasquesChoisis)
        {
            qDebug() << "PROBLEME Liste parcours 1";
            return;
        }

        /// Masque arrivee (1 de la liste positionMasque)
        m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours1.takeFirst());
        m_masqueArrivee->setColor(QColor(Qt::red));
;
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

void ExerciceParcours::slotCacheMasque()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::slotCacheMasque : " << m_nbMasquesInteractifs;

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
        foreach (masqueDeplaceSouris* var_masque, m_listeMasquesFixes) {
            var_masque->setVisible(false);
        }
    }
}
