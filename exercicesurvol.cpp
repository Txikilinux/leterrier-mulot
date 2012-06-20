#include "exercicesurvol.h"

ExerciceSurvol::ExerciceSurvol(QWidget *parent):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = true;

    m_parent = parent;
    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()));

   // getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setAcceptDrops(true);
   // setAbeTeteTelecommande("attente");

    //Création de l'aire de travail + propriétés
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));
    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
   // proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu); // le proxy va permettre de modifier l'ordre d'affichage des objets
   // proxy->setZValue(-1); // gv_AireDeJeu sera en dessous des autres objets
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

    setAbeNbExercices(1);
    setAbeNbTotalQuestions(5);
    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotPresenteSequenceEntered();";
    //    presentationSequence->assignProperty(getAbeExerciceTelecommandeV1()->ui->btnSuivant, "enabled", true); // Pour activer le bouton suivant

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
        redimensionneConsigne();
        onPeutPresenterExercice = false; // permet de "sauter" la présentation de l'exercice
}

//void ExerciceSurvol::slotPresentationExerciceEntered() //todo
//{
//    AbulEduCommonStatesV1::slotPresentationExerciceEntered();
//}

void ExerciceSurvol::slotRealisationExerciceEntered() //todo
{
    // Mettre tout ce qui est commun à chaque question
    // Dans notre cas, la liste d'images choisie aléatoirement

    AbulEduCommonStatesV1::slotRealisationExerciceEntered();

}

void ExerciceSurvol::slotInitQuestionEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotInitQuestionEntered();";
    getAbeExerciceMessageV1()->setVisible(false);
    gv_AireDeJeu->show();

    // Choix de l'image dans la liste
    if (m_localDebug) qDebug()<<"Methode mon exercice !!!!!";
    AbulEduCommonStatesV1::slotInitQuestionEntered();
}

//void ExerciceSurvol::slotQuestionEntered() //todo
//{
    // mise en place du masque
//    AbulEduCommonStatesV1::slotQuestionEntered();
//}

//void ExerciceSurvol::slotVerificationQuestionEntered() //todo
//{
//    AbulEduCommonStatesV1::slo
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

//void ExerciceSurvol::setDimensionsWidgets() //todo
//{
//}


////////////////////////////// Méthodes propres à la classe ///////////////////////////////////////

void ExerciceSurvol::redimensionneConsigne()
{
    //    fondConsigne->setVisible(false); // Scorie liée à la compatibilité avec l'ancienne façon de faire
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}
