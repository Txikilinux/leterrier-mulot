/** Classe ExerciceClic
  * @file exerciceclic.h
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

#ifndef EXERCICECLIC_H
#define EXERCICECLIC_H

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

#endif // EXERCICECLIC
