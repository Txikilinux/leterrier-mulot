/** Classe ExerciceDoubleClic
  * @see https://redmine.ryxeo.com/projects/
  * @author 2013 Icham Sirat <icham.sirat@ryxeo.com>
  * @author 2013 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  *
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

#include "exercicedoubleclic.h"

ExerciceDoubleClic::ExerciceDoubleClic(QWidget *parent, QString theme)
    : ExerciceAbstract(parent, theme, ExerciceAbstract::DoubleClic)
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__ << " " <<  parent << " " << theme;
}

ExerciceDoubleClic::~ExerciceDoubleClic()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
}

void ExerciceDoubleClic::slotAide()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
    eventFilter(this, new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1));
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Double-clique sur les rectangles noirs pour faire apparaitre l'image.")+"<br />"
            + trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.") +" </td>" ;

    _messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,false, 0);
    connect(_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    _messageBox->setWink();
    _messageBox->show();
}
