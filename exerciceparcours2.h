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

    /** Charge la position des masques contenues dans le fichier de configuration (parametres.ini)
      * @param int numeroQuestion, le numero de la question afin de charger le bon parcours
      */
    void chargerPositionMasque(const int &numeroQuestion);

private:
    QList<int> _positionMasquesParcours;
    int NB_MASQUESATTENDUS;
    int NB_MASQUESREELS;

};

#endif // EXERCICEPARCOURS2_H
