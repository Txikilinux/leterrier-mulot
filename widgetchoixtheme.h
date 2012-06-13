/** Classe widgetChoixTheme
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
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


#ifndef WIDGETCHOIXTHEME_H
#define WIDGETCHOIXTHEME_H

#include "abuleduapplicationv1.h"
#include <QWidget>
#include <QDir>
#include <QLabel>
#include <QGridLayout>
#include <QDebug>

namespace Ui {
    class widgetChoixTheme;
}

class widgetChoixTheme : public QWidget
{
    Q_OBJECT

public:
    explicit widgetChoixTheme(QWidget *parent = 0);
    ~widgetChoixTheme();

private:
    Ui::widgetChoixTheme *ui;
};

#endif // WIDGETCHOIXTHEME_H
