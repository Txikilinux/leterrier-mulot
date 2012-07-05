#include "editeur.h"
#include "ui_editeur.h"

#include "QFileDialog"
#include <QMessageBox>




Editeur::Editeur(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editeur)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );

    m_localDebug = true;



}

Editeur::~Editeur()
{
    if (m_localDebug) qDebug() << "##########################  Editeur::~Editeur()";
    delete ui;
}


void Editeur::on_btnImporterImage_clicked()
{
    if(m_localDebug) qDebug() << "##########################  Appuie sur le bouton d'Importation des images";

    QString dossier = QFileDialog::getExistingDirectory(this); //Ouverture d'un dossier
//    QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Images (*.png *.gif *.jpg *.jpeg)");
    if(m_localDebug) qDebug() << dossier;

    // Si dossier choisi, on importe toutes les photos du dossier
    // Sinon Drag&Drop =)




}

