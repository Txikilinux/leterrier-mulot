/** Classe AbstractExercice
  * @file abstractexercice.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2013 Icham Sirat <icham.sirat@ryxeo.com>
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
    _parent         = parent;
    _theme          = theme;
    _exerciceType   = exerciceType;

    _localDebug     = true;
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__ << _parent <<" " << _theme << " " << exerciceType;

    /* Ici sera defini tout ce qui est commun au 5 exercices */
    _exerciceEnCours    = false;

    /* Création de l'aire de travail + propriétés */
    _aireTravail = new QGraphicsView();
    _aireTravail->setAttribute(Qt::WA_AcceptTouchEvents);
    _aireTravail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _aireTravail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *scene = new QGraphicsScene(this);
    _aireTravail->setScene(scene);
    _aireTravail->setFocusPolicy(Qt::NoFocus);

    /* Pour que l'aire de jeu ne bouge pas avec le doigt (tab) */
    _aireTravail->setDragMode(QGraphicsView::NoDrag);

    /* On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget */
    _proxyGraphique = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(_aireTravail);
    _proxyGraphique->setGeometry(_aireTravail->rect());
    _proxyGraphique->setZValue(-1) ;
    _proxyGraphique->setFocusPolicy(Qt::NoFocus);

    /* Instanciation des variables membres */
    _listeImage.clear();
    _listeFichiers.clear();
    _listeMasquesFixes.clear();
    _nbImage = _nbMasquesInteractifs = _OPT_nbMasquesLargeur = _OPT_nbMasquesHauteur = _OPT_timerSuivant = _OPT_nbMasquesChoisis = 0;
    _onPeutMettreEnPause = false;
    _messageBox =  0;
    _masque = _masqueInteractif = 0;
    _labelImagePause  = new QLabel(_parent);
    _labelTextePause  = new QLabel(_parent);
    _tailleAireTravail = QSize(0,0);
    _cheminConf  = _theme + QDir::separator() + QString("conf") + QDir::separator() + QString("parametres.conf");
    _cheminImage = _theme + QDir::separator() + QString("data") + QDir::separator() + QString("images") + QDir::separator();
    _chronometre = new QTime();
    m_isPaused = false;

    /* Gestion Consignes & Aide */
    onPeutPresenterExercice = false;
    onPeutPresenterSequence = false;

    _timer = new QTimer(this);

    _keySpace = new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1);

    /* Gestion graphiques de l'aire de travail */
    _aireTravail->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _aireTravail->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _aireTravail->setStyleSheet("background-color: rgba(0,0,0,0)");
    _aireTravail->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(_aireTravail);

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

    if (_localDebug){
        qDebug() << "Chemin du fichier de configuration" << _cheminConf;
        qDebug() << "Chemin des fichiers images"         << _cheminImage;
    }
}

AbstractExercice::~AbstractExercice()
{
    //! @todo à tester dans toutes les conditions de fermeture.
    //! rien à signaler pour l'instant
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    if(_localDebug) qDebug() << "delete label et texte image";
    _labelImagePause->deleteLater();
    _labelTextePause->deleteLater();

    /* Petit test car parfois j'ai un warning (p-ê dû au fait que j'utilise cet objet dans les classes dérivées)*/
    if(_localDebug) qDebug() << "delete messagBox";
    if(_messageBox != 0)
        _messageBox->deleteLater();

    if(_localDebug) qDebug() << "delete timer";
    _timer->deleteLater();
    if(_localDebug) qDebug() << "delete aireTravail";
    _aireTravail->deleteLater();
    if(_localDebug) qDebug() << "delete ProxyG";
    _proxyGraphique->deleteLater();

    if(_localDebug) qDebug() << "delete masque ds _listeMasqueFixes";
    foreach (MasqueDeplaceSouris* var, _listeMasquesFixes) {
        var->deleteLater();
    }

    if(_localDebug) qDebug() << "delete parametres";
    _parametres->deleteLater();

    if(_localDebug) qDebug() << "delete timer";
    _timer->deleteLater();

    /* Permet à la MainWindow de savoir que l'exercice est terminé */
    emit exerciceExited();
}

void AbstractExercice::slotSequenceEntered(){
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;
    if(!_exerciceEnCours)
    {
//        getAbeExerciceMessageV1()->setParent(_aireTravail);
        getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

        setAbeNbExercices(5);      /* a instancier avant appel du slot SequenceEntered ! */
        setAbeNbTotalQuestions(1); /* a instancier avant appel du slot SequenceEntered ! */

        AbulEduCommonStatesV1::slotSequenceEntered();
        setAbeLevel("1"); /* a instancier après le slot sinon niveau 0 par def. */

        /* Mettre tout ce qui est commun à chaque question */
        _nbImage = getAbeNbTotalQuestions(); // le nb image = le nb de question
        _nbMasquesInteractifs = 0;

        /* aller chercher le pack image */
        QDir dir(_cheminImage);
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        /* Si la dir est vide */
        if (dir.entryInfoList().count() <= 0){
            if(_localDebug) qDebug() << "1. Verification des images <= 0";
            QMessageBox::critical(_parent,trUtf8("Erreur"), trUtf8("L'exercice a rencontré un problème.")+
                                  QString("<strong>")+trUtf8("Veuillez vérifier votre fichier thème (.abe)")+ QString("</strong>"), 0,0);
            slotQuitterAccueil();
            return;
        }
        else{
            const QFileInfoList list = dir.entryInfoList();
            for(int i = 0; i < list.count(); i++)
            {
                _listeFichiers << list.at(i).absoluteFilePath();
            }
        }
    }

    if(_exerciceType != Parcours){
        if(_localDebug) qDebug() << "Tous les exercices sauf Parcours";
        const QPair<int, int> divisionEcran = AbulEduTools::plusPetiteDivision(_OPT_nbMasquesChoisis);
        if (divisionEcran.first > divisionEcran.second)
        {
            _OPT_nbMasquesLargeur = divisionEcran.first;
            _OPT_nbMasquesHauteur = divisionEcran.second;
        }
        else
        {
            _OPT_nbMasquesLargeur = divisionEcran.second;
            _OPT_nbMasquesHauteur = divisionEcran.first;
        }
    }
}

void AbstractExercice::slotRealisationExerciceEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    _aireTravail->scene()->clear();
    _aireTravail->show();

    /* Gestion graphique de la pause si on appuie sur suivant alors que la pause est active @see https://redmine.ryxeo.com/issues/3537 */
    if(m_isPaused){
        qDebug() << "On est en pause ! ERREUR";
        pause();
    }

    if(!_exerciceEnCours)
    {
        /* Si la liste de fichier est vide */
        if (_listeFichiers.size() <= 0 )
        {
            if(_localDebug) qDebug() << "2. Verification de listeFichiers <= 0";
            QMessageBox::critical(_parent,trUtf8("Erreur"), trUtf8("L'excercice a rencontré un problème.")+
                                  QString("<strong>")+trUtf8("Veuillez vérifier votre fichier thème (.abe)")+ QString("</strong>"), 0,0);
            slotQuitterAccueil();
            return;
        }
        else
        {
            /* choisir 5 images au hasard dans le pack */
            for(int i = 0; i < _nbImage; i++)
            {
                qsrand(QDateTime::currentDateTime().toTime_t());
                int n = (qrand() % (_listeFichiers.size()));
                QFileInfo fileInfo = _listeFichiers.takeAt(n);
                _image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);
                _listeImage << _image;
            }
            AbulEduCommonStatesV1::slotRealisationExerciceEntered();
        }
    }
}

void AbstractExercice::slotInitQuestionEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    _onPeutMettreEnPause = false;

    /* Petit nettoyage suite #3127 */
    _aireTravail->scene()->clear();
    _aireTravail->scene()->update();

    if(_listeImage.isEmpty()) {return;}

    AbulEduCommonStatesV1::slotInitQuestionEntered();
    if (!_exerciceEnCours)
    {
        getAbeExerciceMessageV1()->setVisible(false);
        _itemImage = new QGraphicsPixmapItem();
        _aireTravail->scene()->addItem(_itemImage);
        _itemImage->setPixmap(_listeImage.takeAt(0));
        _itemImage->setVisible(false);
    }
    redimensionnerImage();

    _aireTravail->show();

    /* Calcul de la taille des masques */
    float largeurMasque, hauteurMasque = 0.00;

    if (_localDebug){
        qDebug() << "nb masques largeur :" << _OPT_nbMasquesLargeur ;
        qDebug() << "nb masques hauteur :" << _OPT_nbMasquesHauteur ;
    }

    const float largeurAireJeu = static_cast<float>(_aireTravail->width())-1;
    const float hauteurAireJeu = static_cast<float>(_aireTravail->height())-1;

    largeurMasque = largeurAireJeu / _OPT_nbMasquesLargeur;
    hauteurMasque = hauteurAireJeu / _OPT_nbMasquesHauteur;

    qreal xMasque, yMasque = 0.00;

    /* Petite difference de pose entre les exercices */
    switch(_exerciceType){
        case Parcours:
        {
            if(_localDebug) qDebug() << __PRETTY_FUNCTION__ << " :: Parcours";
            int nbMasques = _OPT_nbMasquesLargeur * _OPT_nbMasquesHauteur;

            if (_localDebug) qDebug() << "Début boucle d'affichage : " << nbMasques;

            int numeroMasque = 0;
            for (float i = 0; i < _OPT_nbMasquesHauteur ; i++)
            {
                for (int j = 0; j < _OPT_nbMasquesLargeur ; j++)
                {
                    _masque = new MasqueDeplaceSouris(0, numeroMasque);
                    _masque->setSize(largeurMasque, hauteurMasque);

                    _masque->setPos(xMasque, yMasque);
                    _masque->setColor(QColor::fromRgb(255,255,255));

                    xMasque += largeurMasque;
                    _aireTravail->scene()->addItem(_masque);
                    _listeMasquesFixes << _masque;
                    numeroMasque ++;
                }
                xMasque = 0;
                yMasque += hauteurMasque;
            }
            break;
        }
        default: /* Tous les exercices sauf Parcours */
        {
            if(_localDebug) qDebug() << __PRETTY_FUNCTION__ << " :: Tous les exercices sauf Parcours";
            for (float i = 0 ; i < _OPT_nbMasquesHauteur ; i++)
            {
                for (int j = 0 ; j < _OPT_nbMasquesLargeur ; j++)
                {
                    _masque = new MasqueDeplaceSouris();
                    _masque->setSize(largeurMasque, hauteurMasque);
                    _masque->setPos(xMasque, yMasque);
                    _masque->setColor(QColor::fromRgb(255,255,255));

                    xMasque += largeurMasque;
                    _aireTravail->scene()->addItem(_masque);
                    _listeMasquesFixes << _masque;
                }
                xMasque = 0;
                yMasque += hauteurMasque;
            }

            _OPT_nbMasquesLargeur += 2;
            _OPT_nbMasquesHauteur += 1;
            break;
        }
    }
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation(),false);
}

void AbstractExercice::slotQuestionEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__ ;

    /* Demarrage du chronometre */
    _chronometre->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if(!_exerciceEnCours)
    {
        _itemImage->setVisible(true);
        _nbMasquesInteractifs = 0;

        while(_nbMasquesInteractifs < _OPT_nbMasquesChoisis)
        {
            int alea = (qrand() % (_listeMasquesFixes.count()));
            if(_localDebug) qDebug() << "alea = " << alea;
            _masqueInteractif = _listeMasquesFixes.takeAt(alea);
            connect(_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);

            switch(_exerciceType){
            case Survol:
                _masqueInteractif->setHideOnMouseOver(true);
            case Clic:
                _masqueInteractif->setHideOnClick(true);
                break;
            case DoubleClic:
                _masqueInteractif->setHideOnDoubleClick(true);
            default:/* rien pour parcours (géré dans sa classe)*/
                break;

            }

            _masqueInteractif->setColor(QColor::fromRgb(0,0,0));

            _nbMasquesInteractifs++;
            if(_localDebug) qDebug()<< "Nombre de masques Cliquables : " << _nbMasquesInteractifs;
        }
    }
    _exerciceEnCours = true;
}

void AbstractExercice::slotAfficheVerificationQuestionEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    if(_exerciceEnCours)
    {
        if(_localDebug) qDebug()<< "Click bouton suivant automatique " << _OPT_timerSuivant;

        slotAppuiAutoSuivant();
    }
    abeStateMachineSetVerifieReponse(verifieReponse());

    AbulEduStateMachineV1::slotAfficheVerificationQuestionEntered();

    /* Desactivation delai auto de la stateMachine */
    if(sequenceMachine->cancelDelayedEvent(id_temporisation))
        if(_localDebug) qDebug() << "desactivation delai sequence machine ok";
}


void AbstractExercice::slotFinQuestionEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    /* Affichage du temps passé Total */
    if(_localDebug) {
        qDebug() << "Temps écoulé en millisecondes : " << _tempsQuestion1 + _tempsQuestion2 + _tempsQuestion3 + _tempsQuestion4 + _tempsQuestion5;
        /* On ne veut pas un chronometre précis au millième près =) */
        qDebug() << "Temps écoulé en secondes      : " << (_tempsQuestion1 + _tempsQuestion2 + _tempsQuestion3 + _tempsQuestion4 + _tempsQuestion5)/1000;
    }

    AbulEduCommonStatesV1::slotFinQuestionEntered();
}

void AbstractExercice::slotFinVerificationQuestionEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    AbulEduCommonStatesV1::slotFinVerificationQuestionEntered();

    _listeMasquesFixes.clear();

    _exerciceEnCours = false;
}

void AbstractExercice::slotBilanExerciceEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    /* Click automatique bouton suivant */
    _timer->start();
    boiteTetes->setEtatTete(abeStateMachineGetNumExercice(), abe::evalA,false);
}

void AbstractExercice::slotBilanSequenceEntered()
{
    if(_localDebug) qDebug() << __PRETTY_FUNCTION__;

    _aireTravail->scene()->removeItem(_itemImage);
    _exerciceEnCours = false;

    /* Variables locales */
    int _minute ,_seconde;
    _minute = _seconde = 0;

    /* Les heures ne sont pas gérées, Arrondi à l'entier supérieur */
    _tempsTotal = qCeil((_tempsQuestion1 + _tempsQuestion2 + _tempsQuestion3 + _tempsQuestion4 + _tempsQuestion5)/1000);
    /* Cas ou le temps total (exprimé en secondes) est supérieur à une minute => conversion */
    if(_tempsTotal > 59)
    {
        _seconde = _tempsTotal %60;
        _minute  = _tempsTotal /60;
    }

    /* Gestion affichage barre de titre */
    getAbeExerciceMessageV1()->abeWidgetMessageSetTexteExercice("Bilan");
    switch(_exerciceType)
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
    if (_tempsTotal <= 59) // Affichage en secondes
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 secondes.").arg( QString::number(_tempsTotal)) +" </td>" ;
    }
    else if((_tempsTotal%60) == 0) // Affichage que en minutes (pas besoin d'afficher "en 0 secondes" :p)
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 minute(s).").arg(QString::number(_minute))+" </td>" ;
    }
    else if (_tempsTotal > 59) // Affichage en minutes et secondes
    {
        consigne = "<td> " + trUtf8("Tu as fini l'exercice en %1 minute(s) et %2 secondes").arg(QString::number(_minute)).arg(QString::number(_seconde)) + " </td>" ;
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
    _parametres = new QSettings(_cheminConf, QSettings::IniFormat);
    QString exercice;

    switch(_exerciceType){
    case Survol:
        exercice = trUtf8("Survol");
        if(_localDebug) qDebug() << __PRETTY_FUNCTION__ << " " << exercice;
        _parametres->beginGroup("survol");
        break;
    case Clic:
        exercice = trUtf8("Clic");
        if(_localDebug) qDebug() << __PRETTY_FUNCTION__ << " " << exercice;
        _parametres->beginGroup("clic");
        break;
    case DoubleClic:
        exercice = trUtf8("Doucle Clic");
        if(_localDebug) qDebug() << __PRETTY_FUNCTION__ << " " << exercice;
        _parametres->beginGroup("doubleClic");
        break;
    case Parcours:
        exercice = trUtf8("Parcours");
        _parametres->beginGroup("parcours");
        _OPT_nbMasquesLargeur   = _parametres->value("nbMasquesLargeur", 10).toInt();
        _OPT_nbMasquesHauteur   = _parametres->value("nbMasquesHauteur", 5).toInt();
        if(_localDebug){
            qDebug() << __PRETTY_FUNCTION__ << " " << exercice;
            qDebug() << "Nombre masque largeur : " << _OPT_nbMasquesLargeur;
            qDebug() << "Nombre masque hauteur : " << _OPT_nbMasquesHauteur;

        }
        break;
    }

    if (_parametres->value("exerciceActive",false) == false)
    {
        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Exercice absent du module"),
                                                                  trUtf8("Ce module ne contient pas de paramètres pour l'exercice <b>%1</b>").arg(exercice), true, _parent);
        messageBox->show();
        slotQuitterAccueil();
    }

    _OPT_timerSuivant     = _parametres->value("timerSuivant", 7).toInt();
    _OPT_nbMasquesChoisis = _parametres->value("nbMasquesChoisis", 7).toInt();

    if (_localDebug){
        qDebug() << "Timer suivant : "   << _OPT_timerSuivant;
        qDebug() << "Nb masques choisis : "   << _OPT_nbMasquesChoisis;
    }

    _timer->setInterval(_OPT_timerSuivant*1000);
    _timer->setSingleShot(true);
    connect(_timer, SIGNAL(timeout()), SLOT(slotAppuiAutoSuivant()), Qt::UniqueConnection);

    /* Important sinon le changement de groupe pour la lecture des positions de masque de parcours ne fonctionne pas*/
    _parametres->endGroup();
}

void AbstractExercice::redimensionnerImage()
{
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    _itemImage->setPixmap(_itemImage->pixmap().scaled(_tailleAireTravail, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    _aireTravail->setFixedSize(_itemImage->boundingRect().size().toSize());

    /* positionner l'aire de jeu au centre */
    _aireTravail->move((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width()- _aireTravail->width())/2 + 40 * ratio,
                       (getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height()- 60 * ratio - _aireTravail->height())/2 + 32 * ratio);

}

void AbstractExercice::slotCacheMasque()
{
    if (_localDebug) qDebug() << __PRETTY_FUNCTION__;
    _nbMasquesInteractifs --;

    if(_nbMasquesInteractifs == 0)
    {
        /* On peut mettre en pause */
        _onPeutMettreEnPause = true;

        /* On affiche l'image en entier */
        for(int i = 0; i < _listeMasquesFixes.count(); i++)
        {
            _listeMasquesFixes.at(i)->setVisible(false);
        }

        /* Appui sur le bouton vérifier */
        QTimer::singleShot(0, this, SLOT(slotAppuiAutoVerifier()));

        _listeMasquesFixes.clear();

        /* Affichage du temps passé */
        if(_localDebug){
            qDebug("Temps écoulé: %d ms", _chronometre->elapsed());
            qDebug("Temps écoulé: %d sec", (_chronometre->elapsed())/1000);
        }

        /* Enregistrement du temps passé pour chaque question */
        switch(abeStateMachineGetNumExercice()+1){
        case 1:
            _tempsQuestion1 = _chronometre->elapsed();
            break;
        case 2:
            _tempsQuestion2 = _chronometre->elapsed();
            break;
        case 3:
            _tempsQuestion3 = _chronometre->elapsed();
            break;
        case 4:
            _tempsQuestion4 = _chronometre->elapsed();
            break;
        case 5:
            _tempsQuestion5 = _chronometre->elapsed();
            break;
        default :
            if (_localDebug) qDebug("Case Default -- Temps écoulé: %d ms",_chronometre->elapsed());
            break;
        }
    }
}

void AbstractExercice::setDimensionsWidgets()
{
    if (_localDebug) qDebug() << __PRETTY_FUNCTION__;

    /* Placement de AireDeTravailV1 */
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0,0);

    /* Placement de WidgetTelecommandeV1 */
//    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
//    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    /* Placement de l'AireDeJeu */
    const int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    const int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;

//    _aireTravail->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    _aireTravail->setFixedSize(QSize(large-125 * ratio, haut - 50 * ratio));
    _aireTravail->setSceneRect(0,0,(large-125 * ratio), (haut - 50 * ratio));

    _tailleAireTravail = _aireTravail->size();

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
    if (_localDebug) qDebug() << __PRETTY_FUNCTION__;
    emit appuiSuivant();
}

void AbstractExercice::slotAppuiAutoVerifier()
{
    if (_localDebug) qDebug() << __PRETTY_FUNCTION__;
    emit appuiVerifier();
}

bool AbstractExercice::eventFilter(QObject *obj, QEvent *ev)
{
    obj = this;

    /* Afin de bloquer les wheelEvents qui bouge l'aire de jeu */
    if(ev->type() == QEvent::Wheel )
        return true;

    if(ev->type() == QEvent::KeyRelease)
    {
        QKeyEvent *c = dynamic_cast<QKeyEvent *>(ev);
        if(c && c->key() == Qt::Key_Space )
        {
            pause();
        }
    }
    else if(ev->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *castMouseEvent;
        castMouseEvent = dynamic_cast<QMouseEvent *>(ev);
        if(boiteTetes->geometry().contains(castMouseEvent->pos()) || _labelImagePause->geometry().contains(castMouseEvent->pos()))
        {
            pause();
        }
    }
    else {
        ev->ignore();
    }
    return false;
}

void AbstractExercice::slotFermetureAide()
{
    eventFilter(this, _keySpace);
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(true);
}

void AbstractExercice::pause()
{
    QPixmap pixPause(":/bouton/pause");
    const float ratio = abeApp->getAbeApplicationDecorRatio();
    _labelImagePause->setPixmap(pixPause.scaled((pixPause.width() * ratio),(pixPause.height() * ratio),Qt::KeepAspectRatio));
    _labelTextePause->setText(trUtf8("En Pause ..."));

    _labelImagePause->setStyleSheet("background-color: transparent");
    _labelTextePause->setStyleSheet("background-color: transparent");

    if(_timer->isActive() && _onPeutMettreEnPause)
    {
        _timer->stop();
        if(_localDebug) qDebug() << "Pause !";

        m_isPaused = true;
        boiteTetes->setVisible(false);
        _labelImagePause->show();
        _labelTextePause->show();

        int W     = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
        int w1    = _labelImagePause->width();
        int w2    = _labelTextePause->width();
        int x1    = (W-(w1+w2))/2;
        int x2    = (W-(w2-w1))/2;

        _labelImagePause->move(x1, _aireTravail->height() + _labelImagePause->height() - 60 *ratio);
        _labelTextePause->move(x2, _aireTravail->height() + _labelImagePause->height());

    }
    else if(!_timer->isActive()  && _onPeutMettreEnPause)
    {
        _timer->start();
        if(_localDebug) qDebug() << "Le timer repart";
        m_isPaused = false;
        _labelImagePause->setVisible(false);
        _labelTextePause->setVisible(false);
        boiteTetes->setVisible(true);
    }
}

int AbstractExercice::verifieReponse()
{
    boiteTetes->setEtatTete(m_numExercice, getAbeExerciceEvaluation());
    return 0;
}
