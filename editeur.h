#ifndef EDITEUR_H
#define EDITEUR_H

#include <QDialog>
#include <QDebug>


namespace Ui {
class Editeur;
}

class Editeur : public QDialog
{
    Q_OBJECT
    
public:
    explicit Editeur(QWidget *parent = 0);
    ~Editeur();
    
private slots:

    void on_btnImporterImage_clicked();

private:
    Ui::Editeur *ui;

    bool m_localDebug;
};

#endif // EDITEUR_H
