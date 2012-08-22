/** Classe MainWindow
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
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


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editeur.h"
#include "exercicesurvol.h"
#include "exerciceparcours.h"
#include "exerciceclic.h"
#include "exercicedoubleclic.h"
#include "abuleduboxfilemanagerv1.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_localDebug = true;

    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );
    m_exerciceEnCours = false;

    ui->fr_principale->setMinimumSize(QSize(1000, 500));

    //Mettez ce qu'il faut en fonction de votre menu d'accueil
    m_texteBulles.clear();
    m_texteBulles.insert(0, trUtf8("Survol ..."));
    m_texteBulles.insert(1, trUtf8("Clic ..."));
    m_texteBulles.insert(2, trUtf8("L'ami molette ..."));
    m_texteBulles.insert(3, trUtf8("Parcours ..."));
    m_texteBulles.insert(4, trUtf8("Double clic..."));

    m_config = new QSettings("data/abuledupageaccueilv1/settings.conf", QSettings::IniFormat);
    m_abuleduaccueil = new AbulEduPageAccueilV1(m_config, &m_texteBulles, ui->fr_principale);
    connect(m_abuleduaccueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    this->move((desktop_width-this->width())/2, (desktop_height-this->height())/2);

    m_abuleduaccueil->setDimensionsWidgets();
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnQuitterTriggered()), this, SLOT(close()));
    //    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnOuvrirTriggered()), this, SLOT(on_action_Ouvrir_un_exercice_triggered()));
    setWindowTitle(abeApp->getAbeApplicationLongName());

    m_abuleduFileManager = new AbulEduBoxFileManagerV1();
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected()),this, SLOT(slotOpenFile()));

    m_abuleduFile = new AbulEduFileV1(this);
    m_numberExoCalled = -1;
    m_tempDir = new QDir(m_abuleduFile->abeFileGetDirectoryTemp());
    if (m_localDebug) qDebug()<<"Repertoire temporaire : "<< m_tempDir->absolutePath();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_abuleduaccueil->setDimensionsWidgets();
}

MainWindow::~MainWindow()
{
    if (m_localDebug) qDebug() << "##########################  MainWindow::~MainWindow()";
    delete ui;
    delete m_config;
    //    cleanDirectory(m_tempDir->absolutePath(),m_tempDir->absolutePath());
    //    bool exitSucceded = QDir().rmdir(m_tempDir->absolutePath());
    //    if (m_localDebug) qDebug()<<"Destruction du repertoire temporaire reussie : "<<exitSucceded;
}

void MainWindow::slotOpenFile()
{
    if (m_localDebug) qDebug() << trUtf8("Nom du fichier passé :") << m_abuleduFileManager->abeGetFile()->abeFileGetFileName().absoluteFilePath();
    m_abuleduFile = m_abuleduFileManager->abeGetFile();
    m_abuleduFileManager->hide();
    abeAiguillage();
}

void MainWindow::slotDemo()
{
    //    qDebug() << " On passe en mode démo ...";
}

void MainWindow::on_action_Ouvrir_triggered()
{
    btnBoxClicked();
}

void MainWindow::btnBoxClicked()
{
    if (m_localDebug) qDebug()<<"Bouton Box Clicked";
    m_abuleduFileManager->abeSetFile(m_abuleduFile);
    m_abuleduFileManager->show();
}

void MainWindow::abeLanceExo(int numero)
{
    if(m_exerciceEnCours){return;}

    m_numberExoCalled = numero;
    if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty())
    {
        on_action_Ouvrir_triggered();
        return;
    }
    else
        abeAiguillage();
}

void MainWindow::abeAiguillage()
{
    setFixedSize(this->width(), this->height());
    setWindowTitle(abeApp->getAbeApplicationLongName() +" -- "+m_texteBulles[m_numberExoCalled]);
    ui->statusBar->showMessage(trUtf8(" Nom du fichier .abe selectionné : ")+ m_abuleduFile->abeFileGetFileName().fileName());
    show();

    switch (m_numberExoCalled)
    {
    case 0:
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Survol";
    {
        ExerciceSurvol *s = new ExerciceSurvol(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(s, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()));
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); // cache la barre de menu en mode exercice
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); // redimensionnement interdit
        // Appel du destructeur de la MainWindow lors de l'appui sur le bouton Quitter de la télécommande
        connect(s->getAbeExerciceTelecommandeV1()->ui->btnQuitterQuitter, SIGNAL(clicked()), this, SLOT(close()));
        installEventFilter(s);
    }
        m_exerciceEnCours = true;
        break;
    case 1:
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Clic";
    {
        ExerciceClic *c = new ExerciceClic(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(c, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()));
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); // cache la barre de menu en mode exercice
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); // redimensionnement interdit
        // Appel du destructeur de la MainWindow lors de l'appui sur le bouton Quitter de la télécommande
        connect(c->getAbeExerciceTelecommandeV1()->ui->btnQuitterQuitter, SIGNAL(clicked()), this, SLOT(close()));
        installEventFilter(c);
    }
        m_exerciceEnCours = true;
        break;
    case 3:
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Parcours";
    {
        ExerciceParcours *p = new ExerciceParcours(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(p, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()));
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); // cache la barre de menu en mode exercice
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); // redimensionnement interdit
        // Appel du destructeur de la MainWindow lors de l'appui sur le bouton Quitter de la télécommande
        connect(p->getAbeExerciceTelecommandeV1()->ui->btnQuitterQuitter, SIGNAL(clicked()), this, SLOT(close()));
        installEventFilter(p);
    }
        m_exerciceEnCours = true;
        break;
    case 4:
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Double-Clic";
    {
        ExerciceDoubleClic *d = new ExerciceDoubleClic(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(d, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()));
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); // cache la barre de menu en mode exercice
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); // redimensionnement interdit
        // Appel du destructeur de la MainWindow lors de l'appui sur le bouton Quitter de la télécommande
        connect(d->getAbeExerciceTelecommandeV1()->ui->btnQuitterQuitter, SIGNAL(clicked()), this, SLOT(close()));
        installEventFilter(d);
    }
        m_exerciceEnCours = true;
        break;
    }
}

void MainWindow::exerciceExited()
{
    if (m_localDebug) qDebug()<<"Exercice Exited";
    m_abuleduaccueil->abePageAccueilDesactiveZones(false);
    m_abuleduaccueil->abePageAccueilGetMenu()->show();
    m_exerciceEnCours = false;
    setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)); // redimensionnement autorisé
    show();
}

void MainWindow::on_action_Survol_triggered()
{
    abeLanceExo(0);
}

void MainWindow::on_actionClic_triggered()
{
    abeLanceExo(1);
}

void MainWindow::on_action_Parcours_triggered()
{
    abeLanceExo(3);
}

void MainWindow::on_action_Double_Clic_triggered()
{
    abeLanceExo(4);
}

void MainWindow::on_actionEditeur_triggered()
{
    if (!m_exerciceEnCours) // si on est en exercice pas d'éditeur
    {
        Editeur *monEditeur = new Editeur(this);
        monEditeur->setModal(true); // Tant qu'on ne ferme pas l'éditeur, on ne peut rien faire d'autre (évite d'avoir plein de fenetres en arrière plan)
        monEditeur->showMaximized();
    }
    else // On affiche un petit message...
    {
        QMessageBox::critical(this,"Ouverture Editeur", trUtf8("Veuillez quitter l'exercice avant d'ouvrir l'éditeur"),0,0);
    }
}
