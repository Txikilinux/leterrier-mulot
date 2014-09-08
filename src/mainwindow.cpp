/** Classe MainWindow
  * @file mainwindow.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2012-2014 Icham Sirat <icham.sirat@ryxeo.com>
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
    m_localDebug = false;
    m_isDemoAvailable = true;

    ui->setupUi(this);
    ui->menuExercice->setEnabled(false);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setMinimumSize(1024, 600);

    creeMenuLangue();

    /* Centrage de la fenetre sur le bureau de l'utilisateur */
    move(QApplication::desktop()->screen()->rect().center()-this->rect().center());

    setAttribute(Qt::WA_DeleteOnClose);
    m_exerciceEnCours = false;

    ui->fr_principale->setMinimumSize(QSize(1000, 500));

    /* Gestion bulles page d'accueil */
    m_texteBulles.insert(0, trUtf8("Survol"));
    m_texteBulles.insert(1, trUtf8("Clic"));
    m_texteBulles.insert(2, trUtf8("Double-Clic"));
    m_texteBulles.insert(3, trUtf8("Parcours"));
//    m_texteBulles.insert(4, trUtf8("Double-clic"));

    m_abuleduaccueil = new AbulEduPageAccueilV1(&m_texteBulles, ui->fr_principale);
    m_abuleduaccueil->setDimensionsWidgets();
    connect(m_abuleduaccueil, SIGNAL(boutonPressed(int)), this, SLOT(abeLanceExo(int)), Qt::UniqueConnection);
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnQuitterTriggered()), this, SLOT(close()), Qt::UniqueConnection);
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnBoxTriggered()), this, SLOT(on_actionOuvrir_un_exercice_triggered()), Qt::UniqueConnection);
    m_messageAide = trUtf8("Choisis un module en cliquant sur le bouton <b>Ouverture</b> du parchemin, les zones de lancement des exercices disponibles deviendront alors actives.");
    m_abuleduMessageBox = new AbulEduMessageBoxV1(trUtf8("On y va ?"),m_messageAide,false,m_abuleduaccueil);
    m_abuleduMessageBox->setWink();
    m_abuleduMessageBox->abeSetModeEnum(AbulEduMessageBoxV1::abeNoButton);
    m_abuleduMessageBox->setVisible(false);
    m_demoTimeLine = new QTimeLine(6000,this);
    connect(m_demoTimeLine, SIGNAL(finished()),this, SLOT(slotFinDemo()),Qt::UniqueConnection);

    m_abuleduFile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1(this), &QObject::deleteLater);

    ui->AbulEduBoxFileManager->abeSetFile(m_abuleduFile);
    ui->AbulEduBoxFileManager->abeSetDisplaySimpleOrCompleteEnum(AbulEduBoxFileManagerV1::abeDisplaySimple);
    ui->AbulEduBoxFileManager->abeMediathequeGetHideCloseBouton(true);

    connect(ui->AbulEduBoxFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
    //    connect(m_abuleduFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), ui->editeur, SLOT(on_btnModificationCourant_clicked()/*(QSharedPointer<AbulEduFileV1>)*/), Qt::UniqueConnection);
    connect(ui->AbulEduBoxFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),this,SLOT(slotAfficheEtatEnregistrement(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), Qt::UniqueConnection);
    connect(ui->AbulEduBoxFileManager, SIGNAL(signalAbeFileCloseOrHide()), this, SLOT(btnQuitBoxFileManagerClicked()), Qt::UniqueConnection);

    m_numberExoCalled = -1;

    /* Utilisation de la boite aPropos */
    AbulEduAproposV1 *monAide = new AbulEduAproposV1(this);
    monAide->setVisible(false);
    connect(m_abuleduaccueil->abePageAccueilGetMenu(), SIGNAL(btnAideTriggered()), monAide, SLOT(montreAide()), Qt::UniqueConnection);

    connect(ui->editeur, SIGNAL(editorExited()), SLOT(exerciceExited()), Qt::UniqueConnection);
    connect(ui->editeur, SIGNAL(editorNewAbe(int)), SLOT(setTitle(int)), Qt::UniqueConnection);
    /** @todo reconnecter les différents siganux petit à petit (après test de chaque fonctionnalité ) */
    connect(ui->editeur, SIGNAL(editorTest()), SLOT(debutTestParametres()), Qt::UniqueConnection);
    connect(ui->editeur, SIGNAL(editorChooseOrSave(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave)),
            SLOT(afficheBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave)), Qt::UniqueConnection);

    /* On passe à l'éditeur l'abe courant */
    ui->editeur->setAbeFile(m_abuleduFile);

    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

#ifdef __ABULEDUTABLETTEV1__MODE__
    ui->menuBar->setVisible(false);
#endif
    m_abuleduaccueil->abePageAccueilDesactiveZones(true);

}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_abuleduaccueil->setDimensionsWidgets();
}

//void MainWindow::abeSetMyAbulEduFile(QSharedPointer<AbulEduFileV1> abeFile)
//{
//    m_abuleduFile = abeFile;
//    ui->editeur->setAbeFile(abeFile);
//}

MainWindow::~MainWindow()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    delete ui;
}

void MainWindow::slotOpenFile(const QSharedPointer<AbulEduFileV1> qsp_AbulEduFileV1)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
    m_abuleduFile = qsp_AbulEduFileV1;

    ABULEDU_LOG_DEBUG() << "Nom Fichier abe [" << ui->AbulEduBoxFileManager->abeGetFile()->abeFileGetFileName().absoluteFilePath() << "]";

    AbulEduBoxFileManagerV1* box = (AbulEduBoxFileManagerV1*) sender();
    if (box->abeGetSender() > 0){
        if (box->abeGetSender()->objectName() == "editeur"){
            ui->stCentral->setCurrentWidget(ui->pageEditeur);
            ui->editeur->modificationAbe();
        }
        else{
            ui->stCentral->setCurrentWidget(ui->fr_principale);
        }
    }

    abeAiguillage();
    if(m_abuleduFile->abeFileGetFileList().count() > 0){
        QList<QAction *> listMenuEntries = ui->menuExercice->actions();
        QList<AbulEduZoneV1 *> listZonesPageAccueil = m_abuleduaccueil->abePageAccueilGetZones();
        /** @todo a voir pourquoi c'est là */
        qDebug()<<"menu";
        qDebug()<<listMenuEntries;
        qDebug()<<"zones";

        /* Récupération d'un fichier de settings dans le répertoire temporaire */
        QSettings *settings = new QSettings(m_abuleduFile->abeFileGetDirectoryTemp().path()+"/conf/parametres.conf", QSettings::IniFormat );

        /* Reste maintenant deux choses : remettre au bon endroit les activités et désactiver les zones (pour les entrées du menu ça marche) */

        listMenuEntries[0]      ->setEnabled(settings       ->value("survol/exerciceActive",  true).toBool());
        listZonesPageAccueil[0] ->abeZoneSetActif(settings  ->value("survol/exerciceActive",  true).toBool());
        listMenuEntries[1]      ->setEnabled(settings       ->value("clic/exerciceActive",   true).toBool());
        listZonesPageAccueil[1] ->abeZoneSetActif(settings  ->value("clic/exerciceActive",   true).toBool());
        listMenuEntries[2]      ->setEnabled(settings       ->value("doubleClic/exerciceActive",     true).toBool());
        listZonesPageAccueil[2] ->abeZoneSetActif(settings  ->value("doubleClic/exerciceActive",     true).toBool());
        listMenuEntries[3]      ->setEnabled(settings       ->value("parcours/exerciceActive",     true).toBool());
        listZonesPageAccueil[3] ->abeZoneSetActif(settings  ->value("parcours/exerciceActive",     true).toBool());
        m_messageAide = trUtf8("Clique sur la flèche d'une des zones actives pour lancer un exercice.<br/>Tu peux voir les zones actives en appuyant sur la barre d'espace.");
        m_abuleduMessageBox->abeMessageBoxUpdate(m_abuleduMessageBox->abeMessageBoxGetTitle(),m_messageAide,m_abuleduMessageBox->abeMessageBoxGetWink());
        m_activityFilter->setInterval(12000);
        QListIterator<QAction*> it(listMenuEntries);
        bool someEnabled = false;
        while(it.hasNext() && !someEnabled){
            if(it.next()->isEnabled()){
                someEnabled = true;
            }
        }
        ui->menuExercice->setEnabled(someEnabled);
        delete settings;
    }
    else{
        ui->menuExercice->setEnabled(false);
    }
}

void MainWindow::slotDemo()
{
    if (!m_isDemoAvailable || ui->stCentral->currentWidget() != ui->fr_principale){
        return;
    }
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    float ratio = abeApp->getAbeApplicationDecorRatio();
    if (m_localDebug) qDebug()<<" ++++++++ "<< __FILE__ <<  __LINE__ << __FUNCTION__;
    QKeyEvent* pressSpace = new QKeyEvent(QEvent::KeyPress,Qt::Key_Space,Qt::NoModifier);
    QApplication::sendEvent(m_abuleduaccueil,pressSpace);
    delete pressSpace;
    m_abuleduMessageBox->setGeometry(50*ratio,540*ratio,500*ratio,250*ratio);
    m_abuleduMessageBox->show();
    m_abuleduaccueil->abePageAccueilMontreBulles(true,m_abuleduFile->abeFileGetFileName().fileName().isEmpty());
    foreach(AbulEduZoneV1* zone, m_abuleduaccueil->abePageAccueilGetZones())
    {
        if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty()){
            zone->abeZoneDrawRect(true,QColor("#000028"),4);
        }
        else {
            if(zone->abeZoneIsActif()){
                zone->abeZoneDrawRect(true,QColor("#000028"),4);
            }
        }
    }
    m_demoTimeLine->start();
}

void MainWindow::slotFinDemo()
{
    if(sender()->objectName() != "demoActivityFilter"){
            ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
        }
    m_demoTimeLine->stop();
    if(m_abuleduMessageBox){
        m_abuleduMessageBox->setVisible(false);
    }
    m_abuleduaccueil->abePageAccueilMontreBulles(false,true);
    foreach(AbulEduZoneV1* zone, m_abuleduaccueil->abePageAccueilGetZones())
    {
        zone->abeZoneDrawRect(false);
    }
    QKeyEvent* releaseSpace = new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier);
    QApplication::sendEvent(m_abuleduaccueil,releaseSpace);
    delete releaseSpace;
}

void MainWindow::on_actionMode_D_mo_triggered(bool checked)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << "Checked [" << checked << "]";

    m_isDemoAvailable = checked;
    if (!m_isDemoAvailable)
        slotFinDemo();
    else
        slotDemo();
}

void MainWindow::btnBoxClicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* Permet de remettre AbulEduBoxFileManager en mode Open */
    afficheBoxFileManager(AbulEduBoxFileManagerV1::abeOpen);
    //    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}

void MainWindow::abeLanceExo(int numero)
{
    if(m_exerciceEnCours){return;}

    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << "Numero Exercice [" << numero << "]";

    m_numberExoCalled = numero;

    if(m_abuleduFile->abeFileGetFileName().fileName().isEmpty() && m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->isHidden())
    {
        on_actionOuvrir_un_exercice_triggered();
    }
    else
        abeAiguillage();
}

void MainWindow::abeAiguillage()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());
    if (m_numberExoCalled >= 0)
    {
        ui->menuBar->setEnabled(false);
        m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->setEnabled(false);
    }

    switch(m_numberExoCalled) {
    case 0:
    {
        ABULEDU_LOG_DEBUG() << "Exercice [" << m_numberExoCalled << "] Exercice Survol";
        ExerciceSurvol *s = new ExerciceSurvol(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(s, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->setVisible(false); /* cache la barre de menu en mode exercice */
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height()); /* redimensionnement interdit */
        installEventFilter(s);
        break;
    }
    case 1:
    {
        ABULEDU_LOG_DEBUG() << "Exercice [" << m_numberExoCalled << "] Exercice Clic";
        ExerciceClic *c = new ExerciceClic(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(c, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->setVisible(false);
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height());
        installEventFilter(c);
        break;
    }
    case 2:
    {
        ABULEDU_LOG_DEBUG() << "Exercice ["<< m_numberExoCalled << "] Exercice Double-Clic";
        ExerciceDoubleClic *d = new ExerciceDoubleClic(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(d, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->setVisible(false);
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height());
        installEventFilter(d);
        break;
    }
    case 3:
    {
        ABULEDU_LOG_DEBUG() << "Exercice ["<< m_numberExoCalled << "] Exercice Parcours";
        ExerciceParcours *p = new ExerciceParcours(m_abuleduaccueil, m_abuleduFile->abeFileGetDirectoryTemp().absolutePath());
        connect(p, SIGNAL(exerciceExited()), this, SLOT(exerciceExited()), Qt::UniqueConnection);
        m_abuleduaccueil->abePageAccueilDesactiveZones(true);
        m_abuleduaccueil->abePageAccueilGetMenu()->setVisible(false);
        m_exerciceEnCours = true;
        setFixedSize(this->width(), this->height());
        installEventFilter(p);
        break;
    }
    default:
        break;
    } //! Fin Switch
}

void MainWindow::exerciceExited()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->stCentral->setCurrentWidget(ui->fr_principale);
    m_abuleduaccueil->abePageAccueilDesactiveZones(false);
    m_abuleduaccueil->abePageAccueilGetMenu()->show();
    m_exerciceEnCours = false;
    m_numberExoCalled = -1;
    m_isDemoAvailable = ui->actionMode_D_mo->isChecked();
    m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->setEnabled(true);

    ui->menuBar->setEnabled(true);

    setTitle(abeApp->getAbeNetworkAccessManager()->abeSSOAuthenticationStatus());

    // on peut redimmensionner la fenetre
    setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
    setMinimumSize(1024,600);
}

void MainWindow::on_actionOuvrir_un_exercice_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    btnBoxClicked();
}

void MainWindow::slotChangeLangue()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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

void MainWindow::on_action_Survol_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* 0 = Survol */
    abeLanceExo(0);
}

void MainWindow::on_actionClic_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* 1 = Clic */
    abeLanceExo(1);
}

void MainWindow::on_action_Parcours_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* 3 = Parcours */
    abeLanceExo(3);
}

void MainWindow::on_action_Double_Clic_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* 2 = Double-Clic */
    abeLanceExo(2);
}

void MainWindow::on_actionEditeur_triggered()
{
    /* Petite protection, si l'editeur est la fenetre en cours, on zappe */
    if(ui->editeur->isVisible()) return;

    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if (!m_exerciceEnCours) /* si on est en exercice pas d'éditeur */
    {
        if(m_localDebug) qDebug() << __PRETTY_FUNCTION__;
        ui->stCentral->setCurrentWidget(ui->pageEditeur);
        /* Si le titre de l'abe n'est pas vide, on active le bouton de modification du module courant */
        if(!m_abuleduFile->abeFileGetTitle().isEmpty()){
            if(m_localDebug) qDebug() << "Le titre ABE n'est pas vide, edition possible";
            ui->editeur->btnModificationCourant()->setEnabled(true);
            ui->editeur->btnModificationCourant()->setText(ui->editeur->btnModificationCourant()->text().remove("en cours") + "\"" + m_abuleduFile->abeFileGetTitle()+"\"");
        }
        else {
            if(m_localDebug) qDebug() << "Le titre ABE est vide, pas d'edition";
            ui->editeur->btnModificationCourant()->setText(trUtf8("Editer le module en cours"));
            ui->editeur->btnModificationCourant()->setEnabled(false);
        }
    }
    else /* On affiche un petit message */
    {
        QMessageBox::critical(this,"Ouverture Editeur", trUtf8("Veuillez quitter l'exercice avant d'ouvrir l'éditeur"),0,0);
    }
}

void MainWindow::creeMenuLangue()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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
        e->accept();
        break;
    default:
        e->ignore();
        break;
    }
}

void MainWindow::slotSessionAuthenticated(bool enable)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << "Enable ? [" << enable << "]";

    if(enable) {
        abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
    }
    ui->stCentral->setCurrentWidget(ui->fr_principale);

    m_activityFilter = new ActivityFilter(abeApp);
    m_activityFilter->setObjectName("demoActivityFilter");
    m_activityFilter->setInterval(3000);
    abeApp->installEventFilter(m_activityFilter);
    QObject::connect(m_activityFilter, SIGNAL( userInactive() ),this,  SLOT( slotDemo()),Qt::UniqueConnection);
    QObject::connect(m_activityFilter, SIGNAL(userActive()), this, SLOT(slotFinDemo()), Qt::UniqueConnection);
    connect(abeApp->getAbeNetworkAccessManager(), SIGNAL(ssoAuthStatus(int)), this,SLOT(setTitle(int)), Qt::UniqueConnection);
}

void MainWindow::setTitle(int authStatus)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << "AuthStatus ? [" << authStatus << "]";

    QString title = abeApp->getAbeApplicationLongName();

    if(m_numberExoCalled >=0)
    {
        title.append(" -- "+m_texteBulles[m_numberExoCalled]);
    }
    if(!m_abuleduFile->abeFileGetFileName().fileName().isEmpty())
    {
        title.append(" -- "+m_abuleduFile->abeFileGetFileName().fileName());
    }
    else { /* On affiche que l'user n'a pas de module */
        title.append(" -- " + trUtf8("Pas de module chargé"));
    }
    if(authStatus == 1)
    {
        title.append(" -- "+abeApp->getAbeNetworkAccessManager()->abeGetSSOLogin());
    }
    setWindowTitle(title);
}

void MainWindow::on_action_Journal_de_mes_activit_s_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    abeApp->getAbeIdentite()->abeGetMyLogsAsPDF();
}

void MainWindow::on_action_Changer_d_utilisateur_triggered()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    abeApp->getAbeNetworkAccessManager()->abeSSOLogout();
    abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
}

void MainWindow::slotAfficheEtatEnregistrement(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation loc, QString nom, bool reussite)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

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
    AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"), message, true, this);
    msgEnregistrement->setWindowModality(Qt::ApplicationModal);
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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    if(ui->AbulEduBoxFileManager->abeGetSender()->objectName() == "editeur")
        ui->stCentral->setCurrentWidget(ui->pageEditeur);
    else
        ui->stCentral->setCurrentWidget(ui->fr_principale);
}

void MainWindow::debutTestParametres()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur()->setVisible(true);
    connect(m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()), SLOT(afficheEditeur()), Qt::UniqueConnection);
    connect(m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur(), SIGNAL(clicked()),m_abuleduaccueil->abePageAccueilGetBtnRevenirEditeur(),SLOT(hide()), Qt::UniqueConnection);
    ui->stCentral->setCurrentWidget(ui->fr_principale);

    setWindowTitle(abeApp->getAbeApplicationLongName() +" -- " + m_abuleduFile->abeFileGetFileName().fileName() + " -- MODE TEST");
}

void MainWindow::afficheEditeur()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    ui->stCentral->setCurrentWidget(ui->pageEditeur);
    /* On enleve le "MODE TEST" de la barre des titres quand on revient dans l'editeur */
    if(windowTitle().contains("MODE TEST")){
        setWindowTitle(windowTitle().split(" -- MODE TEST").first());
    }
}

void MainWindow::afficheFrPrincipale()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    ui->stCentral->setCurrentWidget(ui->fr_principale);
}

void MainWindow::afficheBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave openOrSave)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    ui->AbulEduBoxFileManager->abeSetOpenOrSaveEnum(openOrSave);
    ui->AbulEduBoxFileManager->abeSetFile(m_abuleduFile);
    ui->AbulEduBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->AbulEduBoxFileManager->abeSetSender(sender());
    ui->AbulEduBoxFileManager->abeSetDisplaySimpleOrCompleteEnum(AbulEduBoxFileManagerV1::abeDisplayComplete);
    ui->stCentral->setCurrentWidget(ui->pageBoxFileManager);
}
