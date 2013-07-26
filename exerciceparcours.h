#ifndef ExerciceParcours_H
#define ExerciceParcours_H

#include "exerciceabstract.h"

class ExerciceParcours : public ExerciceAbstract
{
    Q_INTERFACES(ExerciceAbstract)

public:
    ExerciceParcours(QWidget *parent, QString theme);
    ~ExerciceParcours();

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
    QList<int> _listePositionMasquesParcours;
    QList<MasqueDeplaceSouris *> _listeMasquesParcours;

    int NB_MASQUESATTENDUS;
    int NB_MASQUESREELS;

    MasqueDeplaceSouris *_masqueDepart;
    MasqueDeplaceSouris *_masqueArrivee;
    MasqueDeplaceSouris *_masqueParcours;
    MasqueDeplaceSouris *_masque;
    QSize _taille;

};

#endif // ExerciceParcours_H
