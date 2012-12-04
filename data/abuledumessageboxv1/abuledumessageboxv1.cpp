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

#include "abuledumessageboxv1.h"
#include "ui_abuledumessageboxv1.h"

AbulEduMessageBoxV1::AbulEduMessageBoxV1(QString titre, QString texte, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbulEduMessageBoxV1)
{
    m_localDebug = 0;
    m_parent = parent;
    premierAffichage = false;
    ui->setupUi(this);

    ui->lblTitre->setFont(QFont(abeApp->font().family(),ui->lblTitre->font().pointSize()));
    ui->lblTitre->setText(titre);
    ui->lblTexte->setFont(QFont(abeApp->font().family(),ui->lblTexte->font().pointSize()));
    ui->lblTexte->setText(texte);
//    QFontMetrics fm(ui->lblTexte->font());
//    ui->lblTexte->setFixedWidth(this->width() -50);
//    int nombreLignes = (fm.boundingRect(texte).width()/ui->lblTexte->width()) +1;
//    while (texte.contains("<br />"))
//    {
//        texte.remove(texte.indexOf("<br />"),6);
//        nombreLignes++;
//    }
//    while (texte.contains("<br/>"))
//    {
//        texte.remove(texte.indexOf("<br/>"),5);
//        nombreLignes++;
//    }
//    while (texte.contains("\n"))
//    {
//        texte.remove(texte.indexOf("\n"),2);
//        nombreLignes++;
//    }
//    if (nombreLignes > 1)
//    {
//        ui->lblTexte->setFixedHeight(ui->lblTexte->height()+((nombreLignes-1)*fm.boundingRect("texte").height())+10);
//        setFixedHeight(height() + (nombreLignes*fm.boundingRect("texte").height()));
//    }
//    if (m_localDebug) qDebug()<<"Nombre lignes "<<nombreLignes;
    setAttribute( Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    ui->btnFermer->setFont(abeApp->font());
    ui->btnFermer->setCouleurTexteNormale(QColor(233,45,0));
    ui->btnFermer->setCouleurTexteSurvol(QColor(249,80,0));
//    ui->btnFermer->setFont(QFont(ui->btnFermer->font().family(),1.4*ui->btnFermer->font().pointSize(),QFont::DemiBold));
    ui->btnFermer->setIconeNormale(QString(":/abuledumessageboxv1/button/close"));
//    ui->btnFermer->setFixedHeight(QPixmap(":/abuledumessageboxv1/button/close").height());
//    ui->btnFermer->setTextePaddingLeft(3*ui->btnFermer->font().pointSize());
    // On en fait une fenetre modale par défaut
    setWindowModality(Qt::ApplicationModal);

    //On centre le splashscreen
    QRect screenRect;
    if (QApplication::activeWindow())
    {
        QWidget *desktop = QApplication::activeWindow();
        screenRect = desktop->geometry();
    }
    else
    {
        QDesktopWidget* desktop = QApplication::desktop();
        screenRect = desktop->availableGeometry();
    }
    int desktop_width = screenRect.width();
    int desktop_height = screenRect.height();
    this->move((desktop_width-this->width())/2, (desktop_height-this->height())/2);
}

AbulEduMessageBoxV1::~AbulEduMessageBoxV1()
{
    delete ui;
}

void AbulEduMessageBoxV1::setTitleColor(QColor color)
{
    qDebug()<<ui->lblTitre->styleSheet();
    ui->lblTitre->setStyleSheet(ui->lblTitre->styleSheet().replace(ui->lblTitre->styleSheet().indexOf("#"),7,color.name()));
    qDebug()<<ui->lblTitre->styleSheet();

}

void AbulEduMessageBoxV1::setWink()
{
    ui->lblIcone->setPixmap(QPixmap(":/abuledumessageboxv1/icon/wink"));
}

void AbulEduMessageBoxV1::hideCloseButton()
{
    ui->btnFermer->hide();
    resize(width(),height()-ui->btnFermer->height());
}

//Pour les tests
void AbulEduMessageBoxV1::on_btnFermer_clicked()
{
    //    exit(EXIT_SUCCESS);
    close();
}
