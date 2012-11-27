#ifndef EDITEURPARCOURSWIDGET_H
#define EDITEURPARCOURSWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QPushButton>
#include <QDebug>

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
    QPushButton *getBtnSave();
    QPushButton *getBtnReset();

    int getBoutonHeight();
    int getBoutonWidth();

    void connectBtnSave(bool enable);

private:
    Ui::EditeurParcoursWidget *ui;
    QGraphicsScene *m_scene;

    bool m_localDebug;

private slots:
    void closeEvent(QCloseEvent *event);

    void on_btnReset_clicked();

signals:
    void signalCloseEvent(QCloseEvent *event);

};

#endif // EDITEURPARCOURSWIDGET_H
