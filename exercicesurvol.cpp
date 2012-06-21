#include "exercicesurvol.h"

ExerciceSurvol::ExerciceSurvol(QWidget *parent):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = true;

    m_parent = parent;
    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()));

    //Création de l'aire de travail + propriétés
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));
    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu) ;
    proxy->setZValue(-1) ;


    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(m_parent);

    // Demarrage de la machine à états
    sequenceMachine->start();
}

ExerciceSurvol::~ExerciceSurvol()
{
    // Permet à la MainWindow de savoir que l'exercice est terminé
    emit exerciceExited();
}

void ExerciceSurvol::slotSequenceEntered() // en cours
{
    if (m_localDebug) qDebug()<<"--------------ExerciceSurvol::slotSequenceEntered()";

    AbulEduCommonStatesV1::slotSequenceEntered();

    setAbeNbExercices(5);
    setAbeNbTotalQuestions(1);
    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

    abeEnableAnimationsConsigneBilan(false);

    boiteTetes->resetTetes(m_nbExercices);

}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotPresenteSequenceEntered()";

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Survol"));
    QString debutTableau = "<tr valign=middle>";
    QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
    QString consigne = "<td align = right>  Ma consigne qui presentera la sequence (video souhaitee)    </td>" ;
    QString finTableau = "</tr>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);

    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionnerConsigne();
    onPeutPresenterExercice = false; // permet de "sauter" la présentation de l'exercice
}

// A ce stade j'ai n'ai pas besoin de presentation de question
// Les questions portent  sur le même exercice, juste les images qui changent
//void ExerciceSurvol::slotPresentationExerciceEntered() //todo
//{
//    AbulEduCommonStatesV1::slotPresentationExerciceEntered();
//}

void ExerciceSurvol::slotRealisationExerciceEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotRealisationExerciceEntered()";

    // Mettre tout ce qui est commun à chaque question
    m_nbImage = m_nbExercices; // le nb image = le nb exercice
    m_nbTotalMasques = 7; //toujours 7 masques sur chaque image

    // aller chercher le pack image
    QDir dir("data/images/animaux/");
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++) {
        m_ListeFichiers << list.at(i).absoluteFilePath();
    }
    // choisir 5 images au hasard dans le pack


    for(int i = 0; i < m_nbImage; i++)
    {
        qsrand(QDateTime::currentDateTime ().toTime_t ());
        int n = (qrand() % (m_ListeFichiers.size()));
        QFileInfo fileInfo = m_ListeFichiers.takeAt(n);

        image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);
        listeImage << image;

    }

    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    setDimensionsWidgets();
    // Test d'affichage d'une image (temporaire !)

    // Redimensionnement de l'image à la taille de l'aire de traivail ==> OK
    QSize size(getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width(), getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height());
    item = gv_AireDeJeu->scene()->addPixmap(listeImage[0].scaled(size));

}

void ExerciceSurvol::slotInitQuestionEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotInitQuestionEntered()";
    getAbeExerciceMessageV1()->setVisible(false);
    gv_AireDeJeu->show();

    // Choix de l'image dans la liste
    if (m_localDebug) qDebug()<<"Methode mon exercice !!!!!";

    AbulEduCommonStatesV1::slotInitQuestionEntered();
    setDimensionsWidgets();
}

void ExerciceSurvol::slotQuestionEntered() //todo
{
     //mise en place du masque
    AbulEduCommonStatesV1::slotQuestionEntered();
}

//void ExerciceSurvol::slotVerificationQuestionEntered() //todo
//{
//    AbulEduCommonStatesV1::slotAfficheVerificationQuestionEntered();
//}

//void ExerciceSurvol::slotBilanExerciceEntered() //todo
//{
//    AbulEduCommonStatesV1::slotBilanExerciceEntered();
//}

//void ExerciceSurvol::slotBilanSequenceEntered() //todo
//{
//    AbulEduCommonStatesV1::slotBilanSequenceEntered();
//}

//void ExerciceSurvol::slotBilanSequenceExited() //todo
//{
//    AbulEduCommonStatesV1::slotBilanSequenceExited();
//}


void ExerciceSurvol::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotQuitter";
    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceSurvol::resizeEvent(QResizeEvent *event) //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::resizeEvent()";
    setDimensionsWidgets();
}

void ExerciceSurvol::showEvent(QShowEvent *event) //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::showEvent()";
    setDimensionsWidgets();
}

void ExerciceSurvol::setDimensionsWidgets() //todo
{
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0, 0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();

    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    gv_AireDeJeu->move((150 * ratio) / 2, 50 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes
    redimensionnerConsigne();
    //TODO Redimensionner l'image avec condition de garde car peut lever une exception donc bug


    AbulEduCommonStatesV1::setDimensionsWidgets();

    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---end";
}


////////////////////////////// Méthodes propres à la classe ///////////////////////////////////////

void ExerciceSurvol::redimensionnerConsigne()
{
    //    fondConsigne->setVisible(false); // Scorie liée à la compatibilité avec l'ancienne façon de faire
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}
