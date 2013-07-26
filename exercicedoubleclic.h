#ifndef EXERCICEDOUBLECLIC_H
#define EXERCICEDOUBLECLIC_H

#include "exerciceabstract.h"

class ExerciceDoubleClic : public ExerciceAbstract
{
    Q_INTERFACES(ExerciceAbstract)

public:
    ExerciceDoubleClic(QWidget *parent, QString theme);
    ~ExerciceDoubleClic();

private slots:

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotAide();
};

#endif // EXERCICEDOUBLECLIC_H
