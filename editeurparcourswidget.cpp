#include "editeurparcourswidget.h"
#include "ui_editeurparcourswidget.h"

EditeurParcoursWidget::EditeurParcoursWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditeurParcoursWidget)
{
    ui->setupUi(this);
    m_localDebug = false;

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
