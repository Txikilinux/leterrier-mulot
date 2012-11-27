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

QGraphicsScene* EditeurParcoursWidget::getScene()
{
    return m_scene;
}

QGraphicsView* EditeurParcoursWidget::getGraphicsView()
{
    return ui->graphicsView;
}

int EditeurParcoursWidget::getBoutonHeight()
{
    return ui->btnReset->height();
}

int EditeurParcoursWidget::getBoutonWidth()
{
    return ui->btnReset->width();
}

QPushButton* EditeurParcoursWidget::getBtnReset()
{
    return ui->btnReset;
}

QPushButton* EditeurParcoursWidget::getBtnSave()
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
