#ifndef EDITEUR_H
#define EDITEUR_H

#include <QDialog>
#include <QDebug>
#include <QDir>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>


namespace Ui {
class Editeur;
}

class Editeur : public QDialog
{
    Q_OBJECT
    
public:
    explicit Editeur(QWidget *parent = 0);
    ~Editeur();

private :

    void rafraichirListeImages();

private slots:

    void on_btnImporterDossierImage_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);


    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    // actions menu TreeWidet
    void on_action_Supprimer_album_triggered();

private:
    Ui::Editeur *ui;

    bool m_localDebug;

    QStringList m_listeFichiers; // la liste des fichiers images présents dans le dossier choisi
    QStringList m_listeDossiers; // la liste des dossiers ouverts
    QDir *m_dir;

    QMenu *m_menuTreeWidget;


};

#endif // EDITEUR_H
