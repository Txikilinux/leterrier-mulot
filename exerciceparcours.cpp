/** Classe ExerciceParcours
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

#include "exerciceparcours.h"
#include <QVector>
#include "abuledutools.h"

ExerciceParcours::ExerciceParcours(QWidget *parent, QString theme):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = false;
    m_exerciceEnCours = false;

    m_parent = parent;
    m_theme = theme;

    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()), Qt::UniqueConnection);

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

    m_nbImage = m_nbMasquesInteractifs = opt_nbMasquesLargeur = opt_nbMasquesHauteur = 0;
    onPeutMettreEnPause = false;

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
    cheminImage = m_theme + QDir::separator() + QString("data") +QDir::separator() + QString("images") + QDir::separator();

    chargerOption();

    // Demarrage de la machine à états
    sequenceMachine->start();
    // Assignation des propriétés de la télécommande
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
    //    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);
    //    presentationExercices->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);

    // Pour les appuis automatiques sur les touches
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
    parametres.beginGroup("parcours");
    if (parametres.value("exerciceActive",false) == false)
    {
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Exercice absent du module"),trUtf8("Ce module ne contient pas de paramètres pour l'exercice <b>Parcours</b>"));
        messageBox->show();
        slotQuitterAccueil();
    }

    opt_timerSuivant     = parametres.value("timerSuivant", 7).toInt();
    opt_nbMasquesLargeur = parametres.value("nbMasquesLargeur", 10).toInt();
    opt_nbMasquesHauteur = parametres.value("nbMasquesHauteur", 5).toInt();

    if (m_localDebug)
    {
        qDebug() << "Timer Suivant      :"   << opt_timerSuivant  << "\n"
                 << "Nb Masques choisis :"   << opt_nbMasquesChoisis;
    }
}

void ExerciceParcours::chargerPositionMasque(const int numeroQuestion)
{
    positionMasquesParcours.clear();

    QList<int> listePosition;
    QSettings parametres(cheminConf, QSettings::IniFormat);
    parametres.beginGroup("parcours");
    parametres.beginGroup("parcours"+QString::number(numeroQuestion));
    opt_nbMasquesChoisis = parametres.childKeys().count();

    /* Recuperation des masques de départ et d'arrivée.. */
    int i = 0;
    qDebug() << "Nombre de masques pour le parcours = " << parametres.childKeys().count();

    qDebug() << "Recherche du départ et de l'arrivee";
    while(i < parametres.childKeys().count()){
        if(parametres.childKeys().at(i).contains("MasqueD") || parametres.childKeys().at(i).contains("MasqueA")){
            positionMasquesParcours << parametres.value(parametres.childKeys().at(i)).toInt();
            ++i;
        }
        else
        {
            i++;
        }
    }

    /* Si pas de Départ et pas d'Arrivée, on quitte */
    if(positionMasquesParcours.count() != 2){
        // Message Box Alerte
        QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
                + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
                + trUtf8("merci de nous le signaler.") +" <br />"
                + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                +" </td>" ;
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, m_parent);
        messageBox->show();
        slotQuitterAccueil();
    }

    i = 0;
    while(i < parametres.childKeys().count()){
        if(parametres.childKeys().at(i).contains("MasqueParcours")){
            qDebug() <<"Test : " << parametres.childKeys().at(i) ;
            /* Remplissage d'un numero de masque pour le tri */
            listePosition << parametres.childKeys().at(i).split("MasqueParcours").at(1).toInt();
            ++i;
        }
        else {
            i++;
        }
    }

    /* Ici, j'ai tout les numéros des masques sont dans la liste, il faut effectuer un tri croissant */
    qSort(listePosition.begin(), listePosition.end(), qLess<int>());

    /* La liste listePosition est triée
         * Il faut boucler dessus est recupéré les positions des masques corrspondantes
         */
    int pos;
    while(!listePosition.isEmpty()){
        pos = listePosition.takeFirst();
        foreach (const QString var, parametres.childKeys()) {
            if(var.contains("MasqueParcours")){
                if(pos == var.split("MasqueParcours").at(1).toInt()){
                    positionMasquesParcours << parametres.value(var).toInt();
                }
            }
        }
    }
    /* On enregistre le nombre de masques attendus */
    NB_MASQUESATTENDUS = positionMasquesParcours.count();
}

void ExerciceParcours::slotSequenceEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotSequenceEntered()";

    if(!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        setAbeNbExercices(1);      // a instancier avant appel du slot SequenceEntered !
        setAbeNbTotalQuestions(5); // a instancier avant appel du slot SequenceEntered !

        AbulEduCommonStatesV1::slotSequenceEntered();

        // jlf 2012/09/29 Réactivation du bouton commencer
        presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true);

        setAbeLevel("1"); // a instancier après le slot sinon niveau 0 par def.
    }
}

void ExerciceParcours::slotAide()
{
    eventFilter(this, keySpace);
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Suis le parcours.")+" <br />"
            + trUtf8("Clique sur le rectangle vert pour commencer.") +" <br />"
            + trUtf8("Survole les rectangles noirs.") +" <br />"
            + trUtf8("Clique sur le rectangle rouge pour terminer.") +" <br />"
            +trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.")
            +" </td>" ;

    m_messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,false, 0);
    connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    m_messageBox->setWink();
    m_messageBox->show();
}

void ExerciceParcours::slotFermetureAide()
{
    eventFilter(this, keySpace);
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(true);
}

void ExerciceParcours::slotRealisationExerciceEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotRealisationExerciceEntered()";

    if (!m_exerciceEnCours)
    {
        // Mettre tout ce qui est commun à chaque question
        m_nbImage = getAbeNbTotalQuestions(); // le nb image = le nb de question
        m_nbMasquesInteractifs = 0;
        boiteTetes->resetTetes(getAbeNbTotalQuestions());

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

void ExerciceParcours::slotInitQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotInitQuestionEntered()";

    onPeutMettreEnPause = false;

    if(m_listeImage.count() <= 0) {return;}

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
            qDebug() << "nb masques largeur : " << opt_nbMasquesLargeur << " , hauteur : " << opt_nbMasquesHauteur;
        }

        float largeurAireJeu = static_cast<float>(gv_AireDeJeu->width())-1;
        float hauteurAireJeu = static_cast<float>(gv_AireDeJeu->height())-1;

        largeurMasque = largeurAireJeu / opt_nbMasquesLargeur;
        hauteurMasque = hauteurAireJeu / opt_nbMasquesHauteur;

        int nbMasques = opt_nbMasquesLargeur * opt_nbMasquesHauteur;
        qreal xMasque,yMasque = 0.00;

        if (m_localDebug) qDebug()<<" -------------------------- Début boucle d'affichage : "<<nbMasques;

        int numeroMasque = 0;
        for (float i=0; i<opt_nbMasquesHauteur;i++)
        {
            for (int j =0; j < opt_nbMasquesLargeur;j++)
            {
                m_masque = new MasqueDeplaceSouris(0, numeroMasque);
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

void ExerciceParcours::slotQuestionEntered()
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotQuestionEntered()";

    // Instanciation & Demarrage du chronometre
    m_chronometre = new QTime();
    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if (!m_exerciceEnCours)
    {
        m_itemImage->setVisible(true);
        if (m_localDebug) qDebug() << getAbeNumQuestion();
        chargerPositionMasque(getAbeNumQuestion());
        // Chargement et Controle de la liste
        if (positionMasquesParcours.isEmpty() && positionMasquesParcours.count() != opt_nbMasquesChoisis)
        {
            if (m_localDebug) qDebug() << "PROBLEME Liste parcours " + QString::number(getAbeNumQuestion());
            return;
        }

        /* Masque arrivee (1 de la liste positionMasque) */
        m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours.takeFirst());
        m_masqueArrivee->setColor(QColor(Qt::red));
        connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);

        /* Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee) */
        m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours.takeFirst());
        m_masqueDepart->setColor(QColor(Qt::green));
        connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
        m_masqueDepart->setHideOnMouseOver(false);
        m_masqueDepart->setHideOnClick(true);

        /* Le premier de la liste est le départ */
        m_listeMasquesParcours << m_masqueDepart; // en premier

        /* Masque parcours (le reste de la liste) */
        /*  Il faut cependant effectuer quelques petits contrôles afin de ne pas se retrouver avec des labyrinthes bizarres. */
        QList<int> voisinsPossibles;
        while (!positionMasquesParcours.isEmpty())
        {
            voisinsPossibles = AbulEduTools::masquesVoisins(m_listeMasquesParcours.last()->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
            if(voisinsPossibles.contains(m_listeMasquesFixes.at(positionMasquesParcours.first())->getNumero()))
            {
                /* Ce masque est ok, on y va */
                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
                m_listeMasquesParcours << m_masqueParcours;

            }
            else{
                /* Ce masque n'est pas bon */
                positionMasquesParcours.removeFirst();
            }
        }
    }
    m_listeMasquesParcours << m_masqueArrivee; // en dernier
    m_exerciceEnCours = true;

    NB_MASQUESREELS = m_listeMasquesParcours.count();
    if(NB_MASQUESATTENDUS != NB_MASQUESREELS){
        // Message Box Alerte
        QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
                + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
                + trUtf8("merci de nous le signaler.") +" <br />"
                + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                +" </td>" ;
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, m_parent);

        messageBox->show();
        slotQuitterAccueil();
    }
}

void ExerciceParcours::slotAfficheVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceParcours::slotAfficheVerificationQuestionEntered()";

    if (m_exerciceEnCours)
    {
        if (m_localDebug) qDebug()<< "Click bouton suivant automatique ! " << opt_timerSuivant;
        m_timer->start();
    }
    abeStateMachineSetVerifieReponse(verifieReponse());
    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();

    /* Desactivation delai auto de la stateMachine */
    if(sequenceMachine->cancelDelayedEvent(id_temporisation))
        if(m_localDebug) qDebug() << "DESATIVATION DELAI OK";

}

void ExerciceParcours::slotFinVerificationQuestionEntered()
{
    if (m_localDebug) qDebug()<< "##########################  ExerciceParcours::slotFinVerificationQuestionEntered()";
    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    m_listeMasquesFixes.clear();

    boiteTetes->setEtatTete(getAbeNumQuestion()-1, abe::evalA );

    /* Vider itemImage */
    gv_AireDeJeu->scene()->removeItem(m_itemImage);
    gv_AireDeJeu->scene()->clear();
    gv_AireDeJeu->show();
    m_exerciceEnCours = false;
}

void ExerciceParcours::slotFinQuestionEntered()
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceParcours::slotFinQuestionEntered()";

    /* Affichage du temps passé Total */
    if (m_localDebug) qDebug() << "Temps écoulé en millisecondes: " << m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5;
    /* On ne veut pas un chronometre précis au millième près =) */
    if (m_localDebug) qDebug() << "Temps écoulé en secondes: " << (m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000;

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void ExerciceParcours::slotBilanExerciceEntered()
{
    if (m_localDebug)qDebug()<< "##########################  ExerciceParcours::slotBilanExerciceEntered()";

    /* Variables locales */
    int m_minute;
    int m_seconde;
    QString debutTableau;
    QString imagetete;
    QString consigne;
    QString finTableau;

    /* Les heures ne sont pas gérées, Arrondi à l'entier supérieur */
    m_tempsTotal = qCeil((m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);
    /* Cas ou le temps total (exprimé en secondes) est supérieur à une minute => conversion */
    if(m_tempsTotal > 59)
    {
        m_seconde = m_tempsTotal %60;
        m_minute  = m_tempsTotal /60;
    }

    AbulEduCommonStatesV1::slotBilanExerciceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Bilan");
    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Parcours"));

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
    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->setVisible(true);

    redimensionnerConsigne();
}

void ExerciceParcours::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"##########################  ExerciceParcours::slotQuitter";

    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceParcours::slotAppuiAutoSuivant()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::slotAppuiAutoSuivant()";
    emit appuiSuivant();
}

void ExerciceParcours::slotAppuiAutoVerifier()
{
    if (m_localDebug) qDebug() << "##########################  ExerciceParcours::slotAppuiAutoVerifier()";
    emit appuiVerifier();
}


//------------------------------------------------------------------
//                 Méthodes propres à la classe
//------------------------------------------------------------------
void ExerciceParcours::redimensionnerConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

void ExerciceParcours::redimensionnerImage2()
{
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(m_tailleAireDejeu, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

    /* Placement de AireDeTravailV1 */
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    /* Placement de WidgetTelecommandeV1 */
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    /* Placement de l'AireDeJeu */
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    //        gv_AireDeJeu->move((170 * ratio) / 2,50 * ratio);
    gv_AireDeJeu->move(80 * ratio, 64 * ratio);

    m_tailleAireDejeu = gv_AireDeJeu->size();

    /* Placement des têtes */
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    /* Redimensionne le widget de consignes */
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
        qDebug() << "La liste avant : " << m_listeMasquesParcours.count();
        m_listeMasquesParcours.removeFirst();

        /* On colorie le suivant en bleu Attention, je fais un removeFirst, donc ma liste doit être
         * strictement supérieure à 1 (sinon segfault)
         */
        if(m_listeMasquesParcours.count() > 1){
            m_listeMasquesParcours.first()->setColor(Qt::blue);
            m_listeMasquesParcours.first()->update();
        }
        if (m_listeMasquesParcours.count() != 0)
        {
            if (m_listeMasquesParcours.count() == 1)
                m_listeMasquesParcours.first()->setHideOnClick(true);
            else
                m_listeMasquesParcours.first()->setHideOnMouseOver(true);
        }
        qDebug() << "La liste apres : " << m_listeMasquesParcours.count();
    }

    if (m_listeMasquesParcours.isEmpty())
    {
        foreach (MasqueDeplaceSouris* var_masque, m_listeMasquesFixes)
        {
            var_masque->setVisible(false);
        }

        // Faire tout ce qu'il faut qd j'ai plus de masque
        // On peut mettre en pause
        onPeutMettreEnPause = true;

        // Appui sur le bouton vérifier
        QTimer::singleShot(0,this,SLOT(slotAppuiAutoVerifier()));

        boiteTetes->setEtatTete(getAbeNumQuestion()-1, abe::evalA );
        m_listeMasquesFixes.clear();

        // Affichage du temps passé
        if (m_localDebug)
        {
            qDebug("Temps écoulé: %d ms",     m_chronometre->elapsed());
            qDebug("Temps écoulé: %d sec",     (m_chronometre->elapsed())/1000);
        }

        // Enregistrement du temps passé pour chaque question
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

bool ExerciceParcours::eventFilter(QObject *obj, QEvent *event)
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
        qDebug() << "Position :: " <<  castMouseEvent->pos();
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

void ExerciceParcours::pause()
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
        if(m_localDebug) qDebug() << "Le timer est actif est vient d'etre stoppé";

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
        if(m_localDebug) qDebug() << "Le timer repart   ";
        m_labelImagePause->setVisible(false);
        m_labelTextePause->setVisible(false);
        boiteTetes->setVisible(true);
    }

    if(m_localDebug) qDebug() << "Pause !";
}
