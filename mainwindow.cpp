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
#include "exercicesurvol.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_localDebug = true;

    ui->setupUi(this);
    exerciceEnCours = false;

    ui->fr_principale->setMinimumSize(QSize(1000, 500));


    //    m_widgetChoixTheme = new widgetChoixTheme(ui->centralWidget);
    //    m_widgetChoixTheme->show();

    //Mettez ce qu'il faut en fonction de votre menu d'accueil
    m_texteBulles.clear();
    m_texteBulles.insert(0, trUtf8("Double clic ..."));
    m_texteBulles.insert(1, trUtf8("Survol ..."));
    m_texteBulles.insert(2, trUtf8("Simple clic ..."));
    m_texteBulles.insert(3, trUtf8("Glisser déposer ..."));
    m_texteBulles.insert(4, trUtf8("L'ami molette..."));
    m_texteBulles.insert(5, trUtf8(".../.../.../..."));


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

    setAttribute( Qt::WA_DeleteOnClose );
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

void MainWindow::on_action_Survol_triggered()
{
    widgetDeplaceSouris *w = new widgetDeplaceSouris(m_abuleduaccueil);
    w->show();
}

void MainWindow::abeLanceExo(int numero)
{
    //si un exercice est en cours -> on ignore
    if(exerciceEnCours)
    {
        return;
    }

    // todo histoire des modules !

    setWindowTitle(abeApp->getAbeApplicationLongName()+"--"+m_texteBulles[numero]);
    show();

    switch (numero)
    {
    case 1:
        if (m_localDebug) qDebug()<<"Exercice No :"<< numero<<" Survol";
    {
        //Exer *b = new ExercicePhraseSansEspaces(gv_Accueil,m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        ExerciceSurvol *s = new ExerciceSurvol(m_abuleduaccueil);
            connect(s, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()));
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); // cache la barre de menu en mode exercice
        exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); // redimensionnement interdit
    }

        exerciceEnCours = true;
        break;
    }
}

void MainWindow::exerciceExited()
{
    if (m_localDebug) qDebug()<<"Exercice Exited";
    m_abuleduaccueil->abePageAccueilDesactiveZones(false);
    m_abuleduaccueil->abePageAccueilGetMenu()->show();
    exerciceEnCours = false;
    setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)); // redimensionnement autorisé
    show();
}
