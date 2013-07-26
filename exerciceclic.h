#ifndef ExerciceClic_H
#define ExerciceClic_H

#include "exerciceabstract.h"


class ExerciceClic : public ExerciceAbstract
{
    Q_INTERFACES(ExerciceAbstract)

public:
    ExerciceClic(QWidget *parent, QString theme);
    ~ExerciceClic();

private slots:
    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotAide();

};

#endif // ExerciceClic_H
