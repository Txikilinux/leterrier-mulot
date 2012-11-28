/** Classe EditeurParcoursWidget
  * @file editeurparcourswidget.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
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
#include "editeurparcourswidget.h"
#include "ui_editeurparcourswidget.h"

EditeurParcoursWidget::EditeurParcoursWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditeurParcoursWidget)
{
    ui->setupUi(this);
    m_localDebug = true;

    m_scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);
}

EditeurParcoursWidget::~EditeurParcoursWidget()
{
    delete ui;
}

QGraphicsScene* EditeurParcoursWidget::getScene() const
{
    return m_scene;
}

QGraphicsView* EditeurParcoursWidget::getGraphicsView() const
{
    return ui->graphicsView;
}

int EditeurParcoursWidget::getBoutonHeight() const
{
    return ui->btnReset->height();
}

int EditeurParcoursWidget::getBoutonWidth() const
{
    return ui->btnReset->width();
}

QPushButton* EditeurParcoursWidget::getBtnReset() const
{
    return ui->btnReset;
}

QPushButton* EditeurParcoursWidget::getBtnSave() const
{
    return ui->btnSave;
}

void EditeurParcoursWidget::connectBtnSave(bool enable)
{
    if(enable)
        ui->btnSave->setEnabled(true);
    else
        ui->btnSave->setEnabled(false);
}

void EditeurParcoursWidget::closeEvent(QCloseEvent *event)
{
    emit signalCloseEvent(event);
}

void EditeurParcoursWidget::on_btnReset_clicked()
{
    connectBtnSave(false);
}
