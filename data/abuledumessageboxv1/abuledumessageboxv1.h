/** Classe AbulEduMessageBoxV1
  *
  * @author 2012 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program; if not, write to the Free Software Foundation, Inc.,
  * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */

#ifndef ABULEDUMESSAGEBOX_H
#define ABULEDUMESSAGEBOX_H

#include <QMessageBox>
#include <QPainter>
#include <QBitmap>
#include <QDesktopWidget>
#include <QDebug>
#include "abuleduapplicationv1.h"

namespace Ui {
class AbulEduMessageBoxV1;
}

class AbulEduMessageBoxV1 : public QWidget
{
    Q_OBJECT
    
public:
    explicit AbulEduMessageBoxV1(QString titre = trUtf8("Problème"), QString texte = trUtf8("Impossible de continuer"),QWidget *parent = 0);
    ~AbulEduMessageBoxV1();

    /** Change la valeur de la variable m_titleColor
      * @param Accepte les valeurs hexadécimales, rgb ou noms CSS */
    void setTitleColor(QColor color);

    /** Affecte l'icône "clin d'oeil" à la message box pour les utilisations en vue d'aide */
    void setWink();

    void hideCloseButton();
    
private slots:
    void on_btnFermer_clicked();

private:
    Ui::AbulEduMessageBoxV1 *ui;
    QString m_titleColor;
    int m_localDebug;
    QWidget* m_parent;
    bool premierAffichage;
};

#endif // ABULEDUMESSAGEBOX_H
