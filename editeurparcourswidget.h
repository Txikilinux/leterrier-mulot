#ifndef EDITEURPARCOURSWIDGET_H
#define EDITEURPARCOURSWIDGET_H

#include <QWidget>
#include <QGraphicsView>

namespace Ui {
class EditeurParcoursWidget;
}

class EditeurParcoursWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit EditeurParcoursWidget(QWidget *parent = 0);
    ~EditeurParcoursWidget();
    
    QGraphicsScene *getScene();
    QGraphicsView *getGraphicsView();

    int getBoutonHeight();
    int getBoutonWidth();

private:
    Ui::EditeurParcoursWidget *ui;
    QGraphicsScene *m_scene;

    bool m_localDebug;
};

#endif // EDITEURPARCOURSWIDGET_H
