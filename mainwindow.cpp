/** Classe MainWindow
  * @file mainwindow.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2012/2013 Icham Sirat <icham.sirat@ryxeo.com>
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

//    /// Langue et Translator
//    QString locale = QLocale::system().name().section('_', 0, 0);

//    //Un 1er qtranslator pour prendre les traductions QT Systeme
//    //c'est d'ailleur grace a ca qu'on est en RTL
//    qtTranslator.load("qt_" + locale,
//                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
//    abeApp->installTranslator(&qtTranslator);

//    //Et un second qtranslator pour les traductions specifiques du
//    //logiciel
//    myappTranslator.load("mulot_" + locale, "lang");
//    abeApp->installTranslator(&myappTranslator);

    ui->setupUi(this);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setMinimumSize(1024, 600);

    creeMenuLangue();

    /* Centrage de la fenetre sur le bureau de l'utilisateur */
    move(QApplication::desktop()->screen()->rect().center()-this->rect().center());

    setAttribute(Qt::WA_DeleteOnClose);
    m_exerciceEnCours = false;

    ui->fr_principale->setMinimumSize(QSize(1000, 500));

    /* Gestion bulles page d'accueil */
    m_texteBulles.clear();
    m_texteBulles.insert(0, trUtf8("Survol"));
    m_texteBulles.insert(1, trUtf8("Clic"));
    m_texteBulles.insert(2, trUtf8("Molette"));
    m_texteBulles.insert(3, trUtf8("Parcours"));
    m_texteBulles.insert(4, trUtf8("Double-clic"));

    m_abuleduaccueil = new AbulEduPageAccueilV1(&m_texteBulles, ui->fr_principale);
    connect(m_abuleduaccueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)), Qt::UniqueConnection);

    m_abuleduaccueil->setDimensionsWidgets();
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnQuitterTriggered()), this, SLOT(close()), Qt::UniqueConnection);
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnBoxTriggered()), this, SLOT(on_actionOuvrir_un_exercice_triggered()), Qt::UniqueConnection);

    m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);

    m_abuleduFileManager = ui->AbulEduBoxFileManager;
    m_abuleduFileManager->abeSetFile(m_abuleduFile);
    m_abuleduFileManager->abeSetDisplaySimpleOrCompleteEnum(AbulEduBoxFileManagerV1::abeDisplaySimple);

    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
//    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), ui->editeur, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),this,SLOT(slotAfficheEtatEnregistrement(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), Qt::UniqueConnection);
    connect(m_abuleduFileManager, SIGNAL(signalAbeFileCloseOrHide()), this, SLOT(btnQuitBoxFileManagerClicked()), Qt::UniqueConnection);


    m_numberExoCalled = -1;

    /* Utilisation de la boite aPropos */
    AbulEduAproposV1 *monAide = new AbulEduAproposV1(this);
    monAide->hide();
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnAideTriggered()), monAide, SLOT(montreAide()), Qt::UniqueConnection);

    connect(ui->editeur, SIGNAL(editorExited()),this, SLOT(exerciceExited()), Qt::UniqueConnection);
//    connect(ui->editeur, SIGNAL(editorTest()),this, SLOT(debutTestParametres()), Qt::UniqueConnection);
//    connect(ui->editeur, SIGNAL(editorChooseOrSave(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave)),
//            this, SLOT(afficheBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave)), Qt::UniqueConnection);

    /* On passe à l'éditeur l'abe courant */
    ui->editeur->setAbeFile(m_abuleduFile);

    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

//    ui->editeur->abeEditeurSetMainWindow(this);

#ifdef __ABULEDUTABLETTEV1__MODE__
    ui->menuBar->hide();
#endif
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_abuleduaccueil->setDimensionsWidgets();
}

void MainWindow::abeSetMyAbulEduFile(QSharedPointer<AbulEduFileV1> abeFile)
{
    m_abuleduFile = abeFile;
    ui->editeur->setAbeFile(abeFile);
}

MainWindow::~MainWindow()
{
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__;
    delete ui;
}

void MainWindow::slotOpenFile(QSharedPointer<AbulEduFileV1> qsp_AbulEduFileV1)
{
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
    m_abuleduFile = qsp_AbulEduFileV1;
    AbulEduBoxFileManagerV1* box = (AbulEduBoxFileManagerV1*) sender();

    if (box->abeGetSender() > 0)
    {
        if (box->abeGetSender()->objectName() == "editeur")
        {
            ui->stCentral->setCurrentWidget(ui->pageEditeur);
        }
        else
        {
            ui->stCentral->setCurrentWidget(ui->fr_principale);
        }
    }
    else
    {
        ui->stCentral->setCurrentWidget(ui->fr_principale);
    }
    if (m_localDebug) qDebug() << "Nom du fichier abe :" << m_abuleduFileManager->abeGetFile()->abeFileGetFileName().absoluteFilePath();
    abeAiguillage();
}

void MainWindow::slotDemo()
{
    if (!m_isDemoAvailable){
        return;
    }
    if (!m_exerciceEnCours && m_isDemoAvailable)
    {
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
    ui->AbulEduBoxFileManager->abeSetSender(this);
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}

void MainWindow::abeLanceExo(int numero)
{
    if(m_exerciceEnCours){return;}

    m_numberExoCalled = numero;
    if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty() && m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->isHidden())
    {
        on_actionOuvrir_un_exercice_triggered();
        return;
    }
    else
        abeAiguillage();
}

void MainWindow::abeAiguillage()
{
    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());
    if (m_numberExoCalled >= 0)
    {
        ui->menuBar->setEnabled(false);
        m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->setEnabled(false);
    }

    switch(m_numberExoCalled) {
    case 0:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Survol";
        ExerciceSurvol *s = new ExerciceSurvol(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(s, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide(); /* cache la barre de menu en mode exercice */
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); /* redimensionnement interdit */
        installEventFilter(s);
        break;
    }
    case 1:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Clic";
        ExerciceClic *c = new ExerciceClic(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(c, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide();
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height());
        installEventFilter(c);
        break;
    }
    case 3:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Parcours";
        ExerciceParcours *p = new ExerciceParcours(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(p, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide();
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height());
        installEventFilter(p);
        break;
    }
    case 4:
    {
        if (m_localDebug) qDebug()<<"Exercice No :"<< m_numberExoCalled<<" Exercice Double-Clic";
        ExerciceDoubleClic *d = new ExerciceDoubleClic(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(d, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->hide();
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height());
        installEventFilter(d);
        break;
    }
    default:
        qDebug() << "CASE DEFAUT qui arrive quand on appel la box puis aiguillage sans exercice...";
        break;
    } //! Fin Switch
}

void MainWindow::exerciceExited()
{
    if (m_localDebug) qDebug()<< __PRETTY_FUNCTION__;

    ui->stCentral->setCurrentWidget(ui->fr_principale);
    m_abuleduaccueil->abePageAccueilDesactiveZones(false);
    m_abuleduaccueil->abePageAccueilGetMenu()->show();
    m_exerciceEnCours = false;
    m_numberExoCalled = -1;
    m_isDemoAvailable = ui->actionMode_D_mo->isChecked();
    m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->setEnabled(true);

    ui->menuBar->setEnabled(true);

    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());
}

void MainWindow::on_actionOuvrir_un_exercice_triggered()
{
    btnBoxClicked();
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
    /* Petite protection, si l'editeur est la fenetre en cours, on zappe */
    if(ui->editeur->isVisible())
        return;

    if (!m_exerciceEnCours) /* si on est en exercice pas d'éditeur */
    {
        if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
        ui->stCentral->setCurrentWidget(ui->pageEditeur);
        /* Si le titre de l'abe n'est pas vide, on active le bouton de modification du module courant */
        if(!m_abuleduFile->abeFileGetTitle().isEmpty()){
            ui->editeur->btnModificationCourant()->setEnabled(true);
            ui->editeur->btnModificationCourant()->setText(ui->editeur->btnModificationCourant()->text().remove("en cours") + "\"" + m_abuleduFile->abeFileGetTitle()+"\"");
        }
    }
    else /* On affiche un petit message */
    {
        QMessageBox::critical(this,"Ouverture Editeur", trUtf8("Veuillez quitter l'exercice avant d'ouvrir l'éditeur"),0,0);
    }
}

void MainWindow::creeMenuLangue()
{
    QAction* actionLangueEn = new QAction(trUtf8("English"),this);
    actionLangueEn->setObjectName("en");
    connect(actionLangueEn, SIGNAL(triggered()), this, SLOT(slotChangeLangue()), Qt::UniqueConnection);
    ui->menuLangues->addAction(actionLangueEn);

    QAction* actionLangueFr = new QAction(trUtf8("Français"),this);
    actionLangueFr->setObjectName("fr");
    connect(actionLangueFr, SIGNAL(triggered()), this, SLOT(slotChangeLangue()), Qt::UniqueConnection);
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
    if (m_localDebug) qDebug() << __PRETTY_FUNCTION__ << enable;
    if(enable) {
        abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
    }
    ui->stCentral->setCurrentWidget(ui->fr_principale);

    ActivityFilter *ef;
    ef = new ActivityFilter(abeApp);
    ef->setInterval(7000);
    abeApp->installEventFilter(ef);
    QObject::connect(ef, SIGNAL(userInactive()), this, SLOT(slotDemo()), Qt::UniqueConnection);
    connect(abeApp->getAbeNetworkAccessManager(), SIGNAL(ssoAuthStatus(int)), this,SLOT(setTitle(int)), Qt::UniqueConnection);
}

void MainWindow::setTitle(int authStatus)
{
    if (m_localDebug) qDebug()<<  __PRETTY_FUNCTION__ << " " << authStatus;
    QString title = abeApp->getAbeApplicationLongName();
    if (m_numberExoCalled >=0)
    {
        title.append(" -- "+m_texteBulles[m_numberExoCalled]);
    }
    if(!m_abuleduFile->abeFileGetFileName().fileName().isEmpty())
    {
        title.append(" -- "+m_abuleduFile->abeFileGetFileName().fileName());
    }
    if (authStatus == 1)
    {
        title.append(" -- "+abeApp->getAbeNetworkAccessManager()->abeGetSSOLogin());
    }
    setWindowTitle(title);
}

void MainWindow::on_action_Journal_de_mes_activit_s_triggered()
{
    abeApp->getAbeIdentite()->abeGetMyLogsAsPDF();
}

void MainWindow::on_action_Changer_d_utilisateur_triggered()
{
    abeApp->getAbeNetworkAccessManager()->abeSSOLogout();
    abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
}

void MainWindow::slotAfficheEtatEnregistrement(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation loc, QString nom, bool reussite)
{
    QString emplacement;
    if (loc == AbulEduBoxFileManagerV1::abePC)
    {
        emplacement = trUtf8("votre ordinateur");
    }
    else if (loc == AbulEduBoxFileManagerV1::abeBoxPerso)
    {
        emplacement = trUtf8("votre abeBox personnelle");
    }
    else if (loc == AbulEduBoxFileManagerV1::abeBoxShare)
    {
        emplacement = trUtf8("une abeBox partagée");
    }
    else if (loc == AbulEduBoxFileManagerV1::abeMediatheque)
    {
        emplacement = trUtf8("AbulEdu-Médiathèque");
    }
    else
    {
        emplacement = trUtf8("un endroit inconnu");
    }

    QString message;
    if (reussite == true)
    {
        message = trUtf8("Votre fichier a été enregistré dans ")+emplacement;
        if (!nom.isEmpty())
        {
            message.append(trUtf8(" sous le nom : ")+nom.split("/").last());
        }
    }
    else
    {
        message = trUtf8("Votre fichier n'a pas pu être enregistré...");
    }
    AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"), message);
    if (reussite == true)
    {
        msgEnregistrement->setWink();
    }
    msgEnregistrement->show();
    if (ui->AbulEduBoxFileManager->abeGetSender()->objectName() == "editeur")
    {
        ui->stCentral->setCurrentWidget(ui->pageEditeur);
    }
    else
    {
        ui->stCentral->setCurrentWidget(ui->fr_principale);
    }
}

void MainWindow::btnQuitBoxFileManagerClicked()
{
    if(ui->AbulEduBoxFileManager->abeGetSender()->objectName() == "editeur")
        ui->stCentral->setCurrentWidget(ui->pageEditeur);
    else
        ui->stCentral->setCurrentWidget(ui->fr_principale);
}

QSharedPointer<AbulEduFileV1> MainWindow::abeGetMyAbulEduFile()
{
    return m_abuleduFile;
}

AbulEduPageAccueilV1 *MainWindow::abeGetMyAbulEduAccueil()
{
    return m_abuleduaccueil;
}

void MainWindow::debutTestParametres()
{
    m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->setVisible(true);
    connect(m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),this,SLOT(afficheEditeur()), Qt::UniqueConnection);
    connect(m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur(),SLOT(hide()), Qt::UniqueConnection);
    ui->stCentral->setCurrentWidget(ui->fr_principale);
}

void MainWindow::afficheEditeur()
{
    ui->stCentral->setCurrentWidget(ui->pageEditeur);
}

void MainWindow::afficheFrPrincipale()
{
    ui->stCentral->setCurrentWidget(ui->fr_principale);
}

void MainWindow::afficheBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave openOrSave)
{
    ui->AbulEduBoxFileManager->abeSetOpenOrSaveEnum(openOrSave);
    ui->AbulEduBoxFileManager->abeSetFile(m_abuleduFile);
    ui->AbulEduBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->AbulEduBoxFileManager->abeSetSender(sender());
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}
