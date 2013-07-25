#ifndef EXERCICEPARCOURS2_H
#define EXERCICEPARCOURS2_H

#include "exerciceabstract.h"

class ExerciceParcours2 : public ExerciceAbstract
{
    Q_INTERFACES(ExerciceAbstract)

public:
    ExerciceParcours2(QWidget *parent, QString theme);
    ~ExerciceParcours2();

private slots:
    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotQuestionEntered();

    /** Méthode héritée @see ExerciceAbstract */
    void slotCacheMasque();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotAide();
};

#endif // EXERCICEPARCOURS2_H
