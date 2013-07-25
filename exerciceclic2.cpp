#include "exerciceclic2.h"

ExerciceClic2::ExerciceClic2(QWidget *parent, QString theme) : ExerciceAbstract(parent, theme, ExerciceAbstract::Clic)
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__ << " " <<  parent << " " << theme;

}

ExerciceClic2::~ExerciceClic2()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
}

void ExerciceClic2::slotAide()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
    eventFilter(this, new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1));
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Clique sur les rectangles noirs pour faire apparaitre l'image.")+"<br />"
            + trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.") +" </td>" ;

    _messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,false, 0);
    connect(_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    _messageBox->setWink();
    _messageBox->show();
}

