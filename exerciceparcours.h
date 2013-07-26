/** Classe ExerciceParcours
  * @file exerciceparcours.h
  * @see https://redmine.ryxeo.com/projects/
  * @author 2013 Icham Sirat <icham.sirat@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef ExerciceParcours_H
#define ExerciceParcours_H

#include "abstractexercice.h"

class ExerciceParcours : public AbstractExercice
{

public:
    ExerciceParcours(QWidget *parent, QString theme);
    ~ExerciceParcours();

private slots:
    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotQuestionEntered();

    /** Méthode héritée @see AbstractExercice */
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
