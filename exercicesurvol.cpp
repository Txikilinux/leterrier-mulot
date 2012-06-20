#include "exercicesurvol.h"

ExerciceSurvol::ExerciceSurvol(QWidget *parent):
    AbulEduCommonStatesV1(parent)
{
    m_parent = parent;
    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()));

    //Création de l'aire de travail + propriétés
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));
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

    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
}

void ExerciceSurvol::slotPresentationExerciceEntered() //todo
{
}

void ExerciceSurvol::slotRealisationExerciceEntered() //todo
{
}

void ExerciceSurvol::slotInitQuestionEntered() //todo
{
}

void ExerciceSurvol::slotQuestionEntered() //todo
{
}

void ExerciceSurvol::slotVerificationQuestionEntered() //todo
{
}

void ExerciceSurvol::slotBilanExerciceEntered() //todo
{
}

void ExerciceSurvol::slotBilanSequenceEntered() //todo
{
}

void ExerciceSurvol::slotBilanSequenceExited() //todo
{
}


void ExerciceSurvol::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotQuitter";
    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceSurvol::resizeEvent(QResizeEvent *event) //todo
{
}

void ExerciceSurvol::showEvent(QShowEvent *event) //todo
{
}

void ExerciceSurvol::setDimensionsWidgets() //todo
{
}
