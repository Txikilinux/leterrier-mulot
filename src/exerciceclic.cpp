/** Classe ExerciceClic
  * @file exerciceclic.cpp
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

#include "exerciceclic.h"

ExerciceClic::ExerciceClic(QWidget *parent, QString theme)
    : AbstractExercice(parent, theme, AbstractExercice::Clic)
{
    ABULEDU_LOG_DEBUG()<< __PRETTY_FUNCTION__ << parent << theme;
}

ExerciceClic::~ExerciceClic()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void ExerciceClic::slotAide()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;
    eventFilter(this, new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1));
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Clique sur les rectangles noirs pour faire apparaitre l'image.")+"<br />"
            + trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.") +" </td>" ;

    m_messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,false, parent());
    connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    m_messageBox->setWink();
    m_messageBox->show();
}

