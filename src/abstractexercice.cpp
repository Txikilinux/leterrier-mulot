/** Classe AbstractExercice
  * @file abstractexercice.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2013-2014 Icham Sirat <icham.sirat@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
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

#include "abstractexercice.h"

AbstractExercice::AbstractExercice(QWidget *parent, const QString &theme, const ExerciceType &exerciceType):
    AbulEduCommonStatesV1(parent)
{
    m_parent         = parent;
    m_theme          = theme;
    m_exerciceType   = exerciceType;

    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << m_parent  << m_theme << exerciceType;

    /* Ici sera defini tout ce qui est commun au 5 exercices */
    m_exerciceEnCours    = false;

    /* Création de l'aire de travail + propriétés */
    m_aireTravail = new QGraphicsView();
    m_aireTravail->setAttribute(Qt::WA_AcceptTouchEvents);
    m_aireTravail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_aireTravail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *scene = new QGraphicsScene(this);
    m_aireTravail->setScene(scene);
    m_aireTravail->setFocusPolicy(Qt::NoFocus);

    /* Pour que l'aire de jeu ne bouge pas avec le doigt (tab) */
    m_aireTravail->setDragMode(QGraphicsView::NoDrag);

    /* On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget */
    m_proxyGraphique = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(m_aireTravail);
    m_proxyGraphique->setGeometry(m_aireTravail->rect());
    m_proxyGraphique->setZValue(-1) ;
    m_proxyGraphique->setFocusPolicy(Qt::NoFocus);

    /* Instanciation des variables membres */
    m_listeImage.clear();
    m_listeFichiers.clear();
    m_listeMasquesFixes.clear();
    m_nbImage = m_nbMasquesInteractifs = m_OPT_nbMasquesLargeur = m_OPT_nbMasquesHauteur = m_OPT_timerSuivant = m_OPT_nbMasquesChoisis = 0;
    m_onPeutMettreEnPause = false;
    m_messageBox =  0;
    m_masque = m_masqueInteractif = 0;
    m_labelImagePause  = new QLabel(m_parent);
    m_labelTextePause  = new QLabel(m_parent);
    m_pixmapPause = new QPixmap(":/bouton/pause");
    m_labelTextePause->setText(trUtf8("En Pause ..."));
    m_labelImagePause->setStyleSheet("background-color: transparent");
    m_labelTextePause->setStyleSheet("background-color: transparent");
    m_tailleAireTravail = QSize(0,0);
    m_cheminConf  = m_theme + QDir::separator() + QString("conf") + QDir::separator() + QString("parametres.conf");
    m_cheminImage = m_theme + QDir::separator() + QString("data") + QDir::separator() + QString("images") + QDir::separator();
    m_chronometre = new QTime();
    m_isPaused = false;

    /* Gestion Consignes & Aide */
    onPeutPresenterExercice = false;
    onPeutPresenterSequence = false;

    m_timer = new QTimer(this);

    m_keySpace = new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1);

    /* Gestion graphiques de l'aire de travail */
    m_aireTravail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_aireTravail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_aireTravail->setStyleSheet("background-color: rgba(0,0,0,0)");
    m_aireTravail->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(m_aireTravail);

    chargerOption();

    /* Demarrage de la machine à états */
    sequenceMachine->start();
    /* Assignation des propriétés de la télécommande */
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", true);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    question->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", true);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    initQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnVerifier, "enabled", false);
    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnRefaire , "enabled", false);
//    bilanExercice->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant , "enabled", false);

    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnAide    , "enabled", true);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnCorriger, "enabled", false);
    afficheCorrectionQuestion->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", false);
    /* Pour les appuis automatiques sur les touches */
    connect(this, SIGNAL(appuiVerifier()), getAbeExerciceTelecommandeV1()->ui->btnVerifier, SIGNAL(clicked()), Qt::UniqueConnection);
    connect(this, SIGNAL(appuiSuivant()),  getAbeExerciceTelecommandeV1()->ui->btnSuivant,  SIGNAL(clicked()), Qt::UniqueConnection);

    ABULEDU_LOG_DEBUG() << "Chemin du fichier de configuration [" << m_cheminConf  << "]";
    ABULEDU_LOG_DEBUG() << "Chemin des fichiers images ["         << m_cheminImage << "]";

    /* Pour l'affichage du titre des images #3101 */
    m_labelTitreImage = new QLabel(m_aireTravail);
    m_labelTitreImage->setStyleSheet("background-color: rgba(14, 160, 234, 1.0); color: white;");
    m_labelTitreImage->hide();
}

AbstractExercice::~AbstractExercice()
{
    //! @todo à tester dans toutes les conditions de fermeture.
    //! rien à signaler pour l'instant
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    m_labelImagePause->deleteLater();
    m_labelTextePause->deleteLater();

    /* Petit test car parfois j'ai un warning (p-ê dû au fait que j'utilise cet objet dans les classes dérivées)*/
    if(m_messageBox != 0)
        m_messageBox->deleteLater();

    m_timer->deleteLater();
    m_aireTravail->deleteLater();
    m_proxyGraphique->deleteLater();

    foreach (MasqueDeplaceSouris* var, m_listeMasquesFixes) {
        var->deleteLater();
    }

    m_parametres->deleteLater();

    /* Permet à la MainWindow de savoir que l'exercice est terminé */
    emit exerciceExited();
}

void AbstractExercice::slotSequenceEntered(){
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    if(!m_exerciceEnCours)
    {
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        setAbeNbExercices(5);      /* a instancier avant appel du slot SequenceEntered ! */
        setAbeNbTotalQuestions(1); /* a instancier avant appel du slot SequenceEntered ! */

        AbulEduCommonStatesV1::slotSequenceEntered();
        setAbeLevel("1"); /* a instancier après le slot sinon niveau 0 par def. */

        /* Mettre tout ce qui est commun à chaque question */
        m_nbImage = getAbeNbTotalQuestions(); // le nb image = le nb de question
        m_nbMasquesInteractifs = 0;

        /* aller chercher le pack image */
        QDir dir(m_cheminImage);
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        /* Si la dir est vide */
        if (dir.entryInfoList().count() <= 0){
            ABULEDU_LOG_DEBUG() << "1. Verification des images <= 0";
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
    }

    if(m_exerciceType != Parcours){
        ABULEDU_LOG_DEBUG() << "Tous les exercices sauf Parcours";
        const QPair<int, int> divisionEcran = AbulEduTools::plusPetiteDivision(m_OPT_nbMasquesChoisis);
        if (divisionEcran.first > divisionEcran.second)
        {
            m_OPT_nbMasquesLargeur = divisionEcran.first;
            m_OPT_nbMasquesHauteur = divisionEcran.second;
        }
        else
        {
            m_OPT_nbMasquesLargeur = divisionEcran.second;
            m_OPT_nbMasquesHauteur = divisionEcran.first;
        }
    }
}

void AbstractExercice::slotRealisationExerciceEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    m_aireTravail->scene()->clear();
    m_aireTravail->show();

    /* On cache le titre de l'image #3101 */
    m_labelTitreImage->hide();
    m_labelTitreImage->clear();

    /* Gestion graphique de la pause si on appuie sur suivant alors que la pause est active @see https://redmine.ryxeo.com/issues/3537 */
    if(m_isPaused){
        pause();
    }

    if(!m_exerciceEnCours)
    {
        /* Si la liste de fichier est vide */
        if (m_listeFichiers.size() <= 0 )
        {
            ABULEDU_LOG_DEBUG() << "2. Verification de listeFichiers <= 0";
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
                /* #3101 gestion du titre de l'image pour affichage */
                m_labelTitreImage->setText(fileInfo.baseName());
                m_image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);
                m_listeImage << m_image;
            }
            AbulEduCommonStatesV1::slotRealisationExerciceEntered();
        }
    }
}

void AbstractExercice::slotInitQuestionEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    m_onPeutMettreEnPause = false;

    /* Petit nettoyage suite #3127 */
    m_aireTravail->scene()->clear();
    m_aireTravail->scene()->update();

    if(m_listeImage.isEmpty()) {return;}

    AbulEduCommonStatesV1::slotInitQuestionEntered();
    if (!m_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setVisible(false);
        m_itemImage = new QGraphicsPixmapItem();
        m_aireTravail->scene()->addItem(m_itemImage);
        m_itemImage->setPixmap(m_listeImage.takeAt(0));
        m_itemImage->setVisible(false);
    }
    redimensionnerImage();

    m_aireTravail->show();

    /* Calcul de la taille des masques */
    float largeurMasque, hauteurMasque = 0.00;

    ABULEDU_LOG_DEBUG() << "nb masques largeur :" << m_OPT_nbMasquesLargeur << "hauteur :" << m_OPT_nbMasquesHauteur;

    const float largeurAireJeu = static_cast<float>(m_aireTravail->width())-1;
    const float hauteurAireJeu = static_cast<float>(m_aireTravail->height())-1;

    largeurMasque = largeurAireJeu / m_OPT_nbMasquesLargeur;
    hauteurMasque = hauteurAireJeu / m_OPT_nbMasquesHauteur;

    qreal xMasque, yMasque = 0.00;

    /* Petite difference de pose entre les exercices */
    switch(m_exerciceType){
        case Parcours:
        {
            ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << " :: Parcours";

            int numeroMasque = 0;
            for (float i = 0; i < m_OPT_nbMasquesHauteur ; i++)
            {
                for (int j = 0; j < m_OPT_nbMasquesLargeur ; j++)
                {
                    m_masque = new MasqueDeplaceSouris(0, numeroMasque);
                    m_masque->setSize(largeurMasque, hauteurMasque);

                    m_masque->setPos(xMasque, yMasque);
                    m_masque->setColor(QColor::fromRgb(255,255,255));

                    xMasque += largeurMasque;
                    m_aireTravail->scene()->addItem(m_masque);
                    m_listeMasquesFixes << m_masque;
                    numeroMasque ++;
                }
                xMasque = 0;
                yMasque += hauteurMasque;
            }
            break;
        }
        default: /* Tous les exercices sauf Parcours */
        {
            ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << " :: Tous les exercices sauf Parcours";
            for (float i = 0 ; i < m_OPT_nbMasquesHauteur ; i++)
            {
                for (int j = 0 ; j < m_OPT_nbMasquesLargeur ; j++)
                {
                    m_masque = new MasqueDeplaceSouris();
                    m_masque->setSize(largeurMasque, hauteurMasque);
                    m_masque->setPos(xMasque, yMasque);
                    m_masque->setColor(QColor::fromRgb(255,255,255));

                    xMasque += largeurMasque;
                    m_aireTravail->scene()->addItem(m_masque);
                    m_listeMasquesFixes << m_masque;
                }
                xMasque = 0;
                yMasque += hauteurMasque;
            }

            m_OPT_nbMasquesLargeur += 2;
            m_OPT_nbMasquesHauteur += 1;
            break;
        }
    }
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
}

void AbstractExercice::slotQuestionEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ ;

    /* Demarrage du chronometre */
    m_chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if(!m_exerciceEnCours)
    {
        m_itemImage->setVisible(true);
        m_nbMasquesInteractifs = 0;

        while(m_nbMasquesInteractifs < m_OPT_nbMasquesChoisis)
        {
            int alea = (qrand() % (m_listeMasquesFixes.count()));
            ABULEDU_LOG_DEBUG() << "alea = " << alea;
            m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
            connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);

            switch(m_exerciceType){
            case Survol:
                m_masqueInteractif->setHideOnMouseOver(true);
                break;
            case Clic:
                m_masqueInteractif->setHideOnClick(true);
                break;
            case DoubleClic:
                m_masqueInteractif->setHideOnDoubleClick(true);
                break;
            default:/* rien pour parcours (géré dans sa classe)*/
                break;

            }

            m_masqueInteractif->setColor(QColor::fromRgb(0,0,0));

            m_nbMasquesInteractifs++;
            ABULEDU_LOG_DEBUG() << "Nombre de masques Cliquables : " << m_nbMasquesInteractifs;
        }
    }
    m_exerciceEnCours = true;
}

void AbstractExercice::slotAfficheVerificationQuestionEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    if(m_exerciceEnCours)
    {
        ABULEDU_LOG_DEBUG() << "Click bouton suivant automatique " << m_OPT_timerSuivant;

        slotAppuiAutoSuivant();
    }
    abeStateMachineSetVerifieReponse(verifieReponse());

    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();

    /* Desactivation delai auto de la stateMachine */
    if(sequenceMachine->cancelDelayedEvent(id_temporisation)){
        ABULEDU_LOG_DEBUG()<< "desactivation delai sequence machine ok";
    }
}


void AbstractExercice::slotFinQuestionEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* Affichage du temps passé Total */
    // ABULEDU_LOG_DEBUG()  << "Temps écoulé en millisecondes : " << m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5;
    /* On ne veut pas un chronometre précis au millième près =) */
    ABULEDU_LOG_DEBUG() << "Temps écoulé en secondes      : " << (m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000;

    /* On affiche le titre de l'image #3101 et on centre le texte #460 */
    if(!m_labelTitreImage->text().isEmpty()){
        m_labelTitreImage->show();
        m_labelTitreImage->updateGeometry();
        m_labelTitreImage->move((m_labelTitreImage->parentWidget()->width() - m_labelTitreImage->width())/2, m_labelTitreImage->y());
    }

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void AbstractExercice::slotFinVerificationQuestionEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    m_listeMasquesFixes.clear();

    m_exerciceEnCours = false;
}

void AbstractExercice::slotBilanExerciceEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* Click automatique bouton suivant */
    m_timer->start();
    boiteTetes->setEtatTete(abeStateMachineGetNumExercice(), abe::evalA,false);
}

void AbstractExercice::slotBilanSequenceEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    m_aireTravail->scene()->removeItem(m_itemImage);
    m_exerciceEnCours = false;

    /* Variables locales */
    int minute ,seconde;
    minute = seconde = 0;

    /* Les heures ne sont pas gérées, Arrondi à l'entier supérieur */
    m_tempsTotal = qCeil((m_tempsQuestion1 + m_tempsQuestion2 + m_tempsQuestion3 + m_tempsQuestion4 + m_tempsQuestion5)/1000);
    /* Cas ou le temps total (exprimé en secondes) est supérieur à une minute => conversion */
    if(m_tempsTotal > 59)
    {
        seconde = m_tempsTotal % 60;
        minute  = m_tempsTotal / 60;
    }

    /* Gestion affichage barre de titre */
    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Bilan");
    switch(m_exerciceType)
    {
    case Survol:
        getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Survol"));
        break;
    case Clic:
        getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Clic"));
        break;
    case DoubleClic:
        getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Double Clic"));
        break;
    case Parcours:
        getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Parcours"));
        break;
    }

    /* Affichage Bilan */
    QString debutTableau = "<tr>";
    QString imagetete    = "<td><img src=\":/evaluation/neutre\"></td>";
    QString finTableau   = "</tr>";
    QString consigne;
    if (m_tempsTotal <= 59) // Affichage en secondes
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 secondes.").arg( QString::number(m_tempsTotal)) +" </td>" ;
    }
    else if((m_tempsTotal % 60) == 0) // Affichage que en minutes (pas besoin d'afficher "en 0 secondes" :p)
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 minute(s).").arg(QString::number(minute))+" </td>" ;
    }
    else if (m_tempsTotal > 59) // Affichage en minutes et secondes
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 minute(s) et %2 secondes").arg(QString::number(minute)).arg(QString::number(seconde)) + " </td>" ;
    }
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);

    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(false);
    getAbeExerciceMessageV1()->setVisible(true);

    redimensionnerConsigne();
}


/********************************************************************************************************************************************
                                                        METHODES DE CLASSE
*********************************************************************************************************************************************/
void AbstractExercice::chargerOption()
{
    m_parametres = new QSettings(m_cheminConf, QSettings::IniFormat);
    QString exercice;

    switch(m_exerciceType){
    case Survol:
        exercice = trUtf8("Survol");
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << exercice;
        m_parametres->beginGroup("survol");
        break;
    case Clic:
        exercice = trUtf8("Clic");
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__  << exercice;
        m_parametres->beginGroup("clic");
        break;
    case DoubleClic:
        exercice = trUtf8("Doucle Clic");
        ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << " " << exercice;
        m_parametres->beginGroup("doubleClic");
        break;
    case Parcours:
        exercice = trUtf8("Parcours");
        m_parametres->beginGroup("parcours");
        m_OPT_nbMasquesLargeur   = m_parametres->value("nbMasquesLargeur", 10).toInt();
        m_OPT_nbMasquesHauteur   = m_parametres->value("nbMasquesHauteur", 5).toInt();
        ABULEDU_LOG_DEBUG()  << __PRETTY_FUNCTION__ << " " << exercice;
        ABULEDU_LOG_DEBUG()  << "Nombre masque largeur : " << m_OPT_nbMasquesLargeur  << "/ hauteur : " << m_OPT_nbMasquesHauteur;
        break;
    }

    if (m_parametres->value("exerciceActive",false) == false)
    {
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Exercice absent du module"),
                                                                  trUtf8("Ce module ne contient pas de paramètres pour l'exercice <b>%1</b>").arg(exercice), true, m_parent);
        messageBox->show();
        slotQuitterAccueil();
    }

    m_OPT_timerSuivant     = m_parametres->value("timerSuivant", 7).toInt();
    m_OPT_nbMasquesChoisis = m_parametres->value("nbMasquesChoisis", 7).toInt();

    ABULEDU_LOG_DEBUG() << "Timer suivant : "   << m_OPT_timerSuivant;
    ABULEDU_LOG_DEBUG() << "Nb masques choisis : " << m_OPT_nbMasquesChoisis;

    m_timer->setInterval(m_OPT_timerSuivant*1000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(slotAppuiAutoSuivant()), Qt::UniqueConnection);

    /* Important sinon le changement de groupe pour la lecture des positions de masque de parcours ne fonctionne pas*/
    m_parametres->endGroup();
}

void AbstractExercice::redimensionnerImage()
{
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    m_itemImage->setPixmap(m_itemImage->pixmap().scaled(m_tailleAireTravail, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_aireTravail->setFixedSize(m_itemImage->boundingRect().size().toSize());

    /* positionner l'aire de jeu au centre */
   m_aireTravail->move((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width()- m_aireTravail->width())/2 + 40 * ratio,
                       (getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height()- 60 * ratio - m_aireTravail->height())/2 + 32 * ratio);

}

void AbstractExercice::slotCacheMasque()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    m_nbMasquesInteractifs --;

    if(m_nbMasquesInteractifs == 0)
    {
        /* On peut mettre en pause */
        m_onPeutMettreEnPause = true;

        /* On affiche l'image en entier */
        for(int i = 0; i < m_listeMasquesFixes.count(); i++){
            m_listeMasquesFixes.at(i)->setVisible(false);
        }

        /* Appui sur le bouton vérifier */
        QTimer::singleShot(0, this, SLOT(slotAppuiAutoVerifier()));

        m_listeMasquesFixes.clear();

        /* Affichage du temps passé */
        ABULEDU_LOG_DEBUG() << "Temps écoulé: " << m_chronometre->elapsed() << "ms";
        ABULEDU_LOG_DEBUG() << "Temps écoulé: " << (m_chronometre->elapsed())/1000  << "sec";

        /* Enregistrement du temps passé pour chaque question */
        switch(abeStateMachineGetNumExercice()+1){
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
            ABULEDU_LOG_DEBUG() << "Case Default -- Temps écoulé: " << m_chronometre->elapsed() <<"ms";
            break;
        }
    }
}

void AbstractExercice::setDimensionsWidgets()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* Placement de AireDeTravailV1 */
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    /* Placement de l'AireDeJeu */
    const int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    const int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;

    m_aireTravail->setFixedSize(QSize(large-125 * ratio, haut - 50 * ratio));
    m_aireTravail->setSceneRect(0,0,(large-125 * ratio), (haut - 50 * ratio));

    m_tailleAireTravail = m_aireTravail->size();

    /* Placement des têtes */
    boiteTetes->resizeTetes(115*ratio);
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);
}

void AbstractExercice::redimensionnerConsigne()
{
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2)  /*200**/ * abeApp->getAbeApplicationDecorRatio());
}

void AbstractExercice::slotAppuiAutoSuivant()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    emit appuiSuivant();
}

void AbstractExercice::slotAppuiAutoVerifier()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    emit appuiVerifier();
}

bool AbstractExercice::eventFilter(QObject *obj, QEvent *ev)
{
    /* #3960 -> filtre barre espace et clic utilisé poour passer automatiquement à la question suivante */
    if(obj->objectName() == "AbulEduExerciceWidgetTelecommandeV1"){
        return abeApp->eventFilter(obj, ev);
    }

    if(!m_onPeutMettreEnPause) /* #3960 Evite de chopper l'event si Exercice Clic... */
        return abeApp->eventFilter(obj, ev);

    switch (ev->type()) {
    /* Si on clic sur la boiteTete ou l'image pause  */
    case QEvent::MouseButtonPress:{
        QMouseEvent *castMouseEvent;
        castMouseEvent = static_cast<QMouseEvent *>(ev);

        if(!castMouseEvent)
            return false; /* petite protection */

        if((boiteTetes->geometry().contains(castMouseEvent->pos()) ||
            m_labelImagePause->geometry().contains(castMouseEvent->pos()) ||
            m_labelTextePause->geometry().contains(castMouseEvent->pos() )))
        {
            ABULEDU_LOG_DEBUG() << "CLIC Pause OK";
            pause();
            ev->accept();
            return true;
        }
    }
        break;

    /* Si appui sur touche "Espace" */
    case QEvent::KeyRelease: {
        QKeyEvent *c = static_cast<QKeyEvent *>(ev);
        if(c && c->key() == Qt::Key_Space )
        {
            ABULEDU_LOG_DEBUG() << "BARRE_ESPACE Pause OK";
            pause();
            ev->accept();
            return true;
        }
    }

        break;
    /* Afin de bloquer les wheelEvents qui bouge l'aire de jeu */
    case QEvent::Wheel:
        return true;
    default:
        break;
    }
    return abeApp->eventFilter(obj, ev);
}

void AbstractExercice::slotFermetureAide()
{
    eventFilter(this, m_keySpace);
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(true);
}

void AbstractExercice::pause()
{
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    m_labelImagePause->setPixmap(m_pixmapPause->scaled((m_pixmapPause->width() * ratio),(m_pixmapPause->height() * ratio),Qt::KeepAspectRatio));

    if(m_timer->isActive() && m_onPeutMettreEnPause)
    {
        m_timer->stop();
        ABULEDU_LOG_DEBUG() << "Pause !";

        m_isPaused = true;
        boiteTetes->setVisible(false);
        m_labelImagePause->show();
        m_labelTextePause->show();

        int W     = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
        int w1    = m_labelImagePause->width();
        int w2    = m_labelTextePause->width();
        int x1    = (W-(w1+w2))/2;
        int x2    = (W-(w2-w1))/2;

        m_labelImagePause->move(x1, m_aireTravail->height() + m_labelImagePause->height() - 60 *ratio);
        m_labelTextePause->move(x2, m_aireTravail->height() + m_labelImagePause->height());

    }
    else if(!m_timer->isActive()  && m_onPeutMettreEnPause)
    {
        m_timer->start();
        ABULEDU_LOG_DEBUG()<< "Le timer repart";
        m_isPaused = false;
        m_labelImagePause->setVisible(false);
        m_labelTextePause->setVisible(false);
        boiteTetes->setVisible(true);
    }
}

int AbstractExercice::verifieReponse()
{
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation());
    return 0;
}
