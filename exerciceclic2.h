#ifndef EXERCICECLIC2_H
#define EXERCICECLIC2_H

#include "exerciceabstract.h"


class ExerciceClic2 : public ExerciceAbstract
{
    Q_INTERFACES(ExerciceAbstract)

public:
    ExerciceClic2(QWidget *parent, QString theme);
    ~ExerciceClic2();

private slots:
    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotAide();

};

#endif // EXERCICECLIC2_H
