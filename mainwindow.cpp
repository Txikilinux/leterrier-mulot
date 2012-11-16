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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_localDebug = true;
    m_isDemoAvailable = true;

    /// Langue et Translator
    QString locale = QLocale::system().name().section('_', 0, 0);

    //Un 1er qtranslator pour prendre les traductions QT Systeme
    //c'est d'ailleur grace a ca qu'on est en RTL
    qtTranslator.load("qt_" + locale,
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    abeApp->installTranslator(&qtTranslator);

    //Et un second qtranslator pour les traductions specifiques du
    //logiciel
    myappTranslator.load("mulot_" + locale, "lang");
    abeApp->installTranslator(&myappTranslator);

    ui->setupUi(this);
    creeMenuLangue();
    move(QApplication::desktop()->screen()->rect().center()-this->rect().center());

    setAttribute( Qt::WA_DeleteOnClose );
    m_exerciceEnCours = false;

    ui->fr_principale->setMinimumSize(QSize(1000, 500));

    /// Gestion bulles page d'accueil
    m_texteBulles.clear();
    m_texteBulles.insert(0, trUtf8("Survol ..."));
    m_texteBulles.insert(1, trUtf8("Clic ..."));
    m_texteBulles.insert(2, trUtf8("Molette ..."));
    m_texteBulles.insert(3, trUtf8("Parcours ..."));
    m_texteBulles.insert(4, trUtf8("Double clic..."));

    m_config = new QSettings("data/abuledupageaccueilv1/settings.conf", QSettings::IniFormat);
    m_abuleduaccueil = new AbulEduPageAccueilV1(m_config, &m_texteBulles, ui->fr_principale);
    connect(m_abuleduaccueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)));

    /// Centrage de la fenetre sur le bureau de l'utilisateur
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    this->move((desktop_width-this->width())/2, (desktop_height-this->height())/2);

    m_abuleduaccueil->setDimensionsWidgets();
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnQuitterTriggered()), this, SLOT(close()));
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnBoxTriggered()), this, SLOT(on_actionOuvrir_un_exercice_triggered()));
    setWindowTitle(abeApp->getAbeApplicationLongName());

    m_abuleduFile = new AbulEduFileV1(this);
    m_abuleduFileManager = new AbulEduBoxFileManagerV1(0, m_abuleduFile);
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected()),this, SLOT(slotOpenFile()));

    m_numberExoCalled = -1;
    m_tempDir = new QDir(m_abuleduFile->abeFileGetDirectoryTemp());
    if (m_localDebug) qDebug()<<"Repertoire temporaire : "<< m_tempDir->absolutePath();

    /// Utilisation de la boite aPropos
    AbulEduAproposV1 *monAide = new AbulEduAproposV1(this);
    monAide->hide();
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnAideTriggered()), monAide, SLOT(montreAide()));

    /// Ajout de l'anglais & français dans le menu langues

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

/**
  * Permet d'afficher les zones et les bulles lorsque l'utilisateur est inactif
  */
void MainWindow::slotDemo()
{
    if (!m_isDemoAvailable){
        return;
    }
    if (!m_exerciceEnCours && m_isDemoAvailable)
    {
        qDebug() << "Affichage des bulles ";
        m_abuleduaccueil->abePageAccueilMontreBulles(true);
        foreach(AbulEduZoneV1* zone, m_abuleduaccueil->abePageAccueilGetZones())
        {
            zone->abeZoneDrawRect(true,QColor("#000028"),4);
        }
        QTimer::singleShot(8000,this,SLOT(slotFinDemo()));
    }
}

void MainWindow::on_actionMode_D_mo_triggered(bool checked)
{
    m_isDemoAvailable = checked;
    if (!m_isDemoAvailable)
        slotFinDemo();
    else
        slotDemo();
}

/**
  * Permet de cacher les zones et les bulles après l'affichage par le slotDemo()
  */
void MainWindow::slotFinDemo()
{
    m_abuleduaccueil->abePageAccueilMontreBulles(false);
    foreach(AbulEduZoneV1* zone, m_abuleduaccueil->abePageAccueilGetZones())
    {
        zone->abeZoneDrawRect(false);
    }

}

void MainWindow::btnBoxClicked()
{
    if (m_localDebug) qDebug()<<"Bouton Box Clicked";
    m_abuleduFileManager->abeSetFile(m_abuleduFile);
#ifdef __ABULEDUTABLETTEV1__MODE__
    m_abuleduFileManager->showFullScreen();
#else
    m_abuleduFileManager->show();
#endif
}

void MainWindow::abeLanceExo(int numero)
{
    if(m_exerciceEnCours){return;}

    m_numberExoCalled = numero;
    if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty())
    {
        on_actionOuvrir_un_exercice_triggered();
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
    if (m_numberExoCalled >= 0) ui->menuBar->setEnabled(false);

#ifdef __ABULEDUTABLETTEV1__MODE__
    showFullScreen();
#else
    show();
#endif

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
    ui->menuBar->setEnabled(true);
    m_exerciceEnCours = false;
    setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)); // redimensionnement autorisé
#ifdef __ABULEDUTABLETTEV1__MODE__
    showFullScreen();
#else
    show();
#endif
}

void MainWindow::on_action_Survol_triggered()
{
    abeLanceExo(0);
}

void MainWindow::slotChangeLangue()
{
    QString lang = sender()->objectName();
    abeApp->removeTranslator(&qtTranslator);
    abeApp->removeTranslator(&myappTranslator);

    //Un 1er qtranslator pour prendre les traductions QT Systeme
    //c'est d'ailleur grace a ca qu'on est en RTL
    qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    abeApp->installTranslator(&qtTranslator);

    //foreach (QWidget *widget, QApplication::allWidgets()) widget->setLayoutDirection(Qt::RightToLeft);
    //Et un second qtranslator pour les traductions specifiques du
    //logiciel
    myappTranslator.load("mulot_" + lang, "lang");
    abeApp->installTranslator(&myappTranslator);
    ui->retranslateUi(this);
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
#ifdef __ABULEDUTABLETTEV1__MODE__
        monEditeur->showFullScreen();
#else
        monEditeur->show();
#endif
    }
    else // On affiche un petit message...
    {
        QMessageBox::critical(this,"Ouverture Editeur", trUtf8("Veuillez quitter l'exercice avant d'ouvrir l'éditeur"),0,0);
    }
}


void MainWindow::creeMenuLangue()
{
    QAction* actionLangueEn = new QAction(trUtf8("English"),this);
    actionLangueEn->setObjectName("en");
    connect(actionLangueEn, SIGNAL(triggered()), this, SLOT(slotChangeLangue()));
    ui->menuLangues->addAction(actionLangueEn);

    QAction* actionLangueFr = new QAction(trUtf8("Français"),this);
    actionLangueFr->setObjectName("fr");
    connect(actionLangueFr, SIGNAL(triggered()), this, SLOT(slotChangeLangue()));
    ui->menuLangues->addAction(actionLangueFr);
}

void MainWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::slotSessionAuthenticated(bool enable)
{
    if (m_localDebug) qDebug() << "MainWindow::slotSessionAuthenticated" << enable;
    if(enable)
        abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
    show();
    //showMaximized();

    activityFilter *ef;
    ef = new activityFilter(abeApp);
    ef->setInterval(7000);
    abeApp->installEventFilter(ef);
    QObject::connect(ef, SIGNAL( userInactive() ),
                     this,  SLOT( slotDemo() ));
}

void MainWindow::on_actionOuvrir_un_exercice_triggered()
{
    btnBoxClicked();
}
