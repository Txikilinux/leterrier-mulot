#ifndef EDITEUR_H
#define EDITEUR_H

#include <QDialog>
#include <QDebug>
#include <QDir>
#include <QTreeWidgetItem>
#include <QListWidgetItem>


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

    void rafraichirListeImages();
    void on_btnImporterDossierImage_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);


private:
    Ui::Editeur *ui;

    bool m_localDebug;

    QStringList m_listeFichiers; // la liste des fichiers images présents dans le dossier choisi
    QStringList m_listeDossiers; // la liste des dossiers ouverts
    QDir *m_dir;


};

#endif // EDITEUR_H
