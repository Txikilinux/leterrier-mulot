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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_localDebug = true;

    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose );
    m_exerciceEnCours = false;

    ui->fr_principale->setMinimumSize(QSize(1000, 500));

    //    m_widgetChoixTheme = new widgetChoixTheme(ui->centralWidget);
    //    m_widgetChoixTheme->show();

    //Mettez ce qu'il faut en fonction de votre menu d'accueil
    m_texteBulles.clear();
    m_texteBulles.insert(0, trUtf8("Survol ..."));
    m_texteBulles.insert(1, trUtf8("Clic ..."));
    m_texteBulles.insert(2, trUtf8("L'ami molette ..."));
    m_texteBulles.insert(3, trUtf8("Parcours ..."));
    m_texteBulles.insert(4, trUtf8("Double clic..."));

    QSettings *m_config = new QSettings("data/abuledupageaccueilv1/settings.conf", QSettings::IniFormat);
    m_abuleduaccueil = new AbulEduPageAccueilV1(m_config, &m_texteBulles, ui->fr_principale);
    connect(m_abuleduaccueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)));

    //On centre la fenetre sur l'ecran de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    this->move((desktop_width-this->width())/2, (desktop_height-this->height())/2);

    m_abuleduaccueil->setDimensionsWidgets();
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnQuitterTriggered()), this, SLOT(close()));
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnOuvrirTriggered()), this, SLOT(on_action_Ouvrir_un_exercice_triggered()));
    setWindowTitle(abeApp->getAbeApplicationLongName());
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_abuleduaccueil->setDimensionsWidgets();
}

MainWindow::~MainWindow()
{
    if (m_localDebug) qDebug() << "##########################  MainWindow::~MainWindow()";
    delete ui;
}

void MainWindow::slotDemo()
{
    qDebug() << " On passe en mode démo ...";
}

void MainWindow::abeLanceExo(int numero)
{
    //si un exercice est en cours -> on ignore
    if(m_exerciceEnCours)
    {
        return;
    }

    // todo histoire des modules !
    setWindowTitle(abeApp->getAbeApplicationLongName()+"--"+m_texteBulles[numero]);
    show();

    switch (numero)
    {
    case 0:
        if (m_localDebug) qDebug()<<"Exercice No :"<< numero<<" Survol";
    {
        //Exer *b = new ExercicePhraseSansEspaces(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        ExerciceSurvol *s = new ExerciceSurvol(m_abuleduaccueil);
            connect(s, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()));
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); // cache la barre de menu en mode exercice
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); // redimensionnement interdit
        // Appel du destructeur de la MainWindow lors de l'appui sur le bouton Quitter de la télécommande
        connect(s->getAbeExerciceTelecommandeV1()->ui->btnQuitterQuitter, SIGNAL(clicked()), this, SLOT(close()));
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
    abeLanceExo(1);
}

void MainWindow::on_actionEditeur_triggered()
{
    if (!m_exerciceEnCours) // si on est en exercice pas d'éditeur
    {
        Editeur *monEditeur = new Editeur(this);
        monEditeur->setModal(true); // Tant qu'on ne ferme pas l'éditeur, on ne peut rien faire d'autre (évite d'avoir plein de fenetres en arrière plan)
        monEditeur->show();
    }
    else // On affiche un petit message...
    {
        QMessageBox::critical(this,"Ouverture Editeur", trUtf8("Veuillez quitter l'exercice avant d'ouvrir l'éditeur"),0,0);
    }
}
void MainWindow::on_actionDefinirTheme_triggered()
{
    if (m_localDebug) qDebug() << "Choix du thème";
    // Aller chercher un .abe
    QString destinationIdUnique;
    QString fichierAbe = QFileDialog::getOpenFileName(this, "Ouvrir un .abe", QString(), "Abe(*.abe)");

    if (fichierAbe.isNull()) // dossier est nul, donc pas la peine d'aller plus loin
    {
        if (m_localDebug) qDebug() << "Appui sur le bouton annuler";
        return;
    }
    else
    {
        if (m_localDebug) qDebug() << fichierAbe;
    }
    // Ok j'ai le chemin de mon .abe

    // Le dezipper dans un fichier temp
    AbulEduFileV1 *m_abuledufilev1 = new AbulEduFileV1();
    destinationIdUnique = m_abuledufilev1->abeFileGetDirectoryTemp().absolutePath();

    QDir *temp = new QDir(destinationIdUnique);
    if (m_localDebug) qDebug() << destinationIdUnique;
    m_abuledufilev1->abeFileOpen(fichierAbe,temp);
    // Et les exercices doivent pouvoir aller le chercher =)
}
