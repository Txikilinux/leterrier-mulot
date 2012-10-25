/** Implémentation Automatique du menu Aide
 *
 * @see https://redmine.ryxeo.com/projects/leterrier-developpeu/
 * @author 2010 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
 * @author 2010-2012 Eric Seigne <eric.seigne@ryxeo.com>
 * @see The GNU Public License (GPL)
 */

/**
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "abuleduaproposv1.h"
#include "ui_abuleduaproposv1.h"
#include <QMenu>
#include <QMenuBar>
#include <QScrollBar>
#include <QDebug>
#include <QDesktopServices>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QtXml>
#include <QHostInfo>
#include <QNetworkProxy>
#include "abuleduapplicationv1.h"

AbulEduAproposV1::AbulEduAproposV1(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AbulEduAproposV1)
{
    ui->setupUi(this);
    this->setWindowTitle(trUtf8("A propos de")+" "+abeApp->getAbeApplicationLongName()+" "+abeApp->applicationVersion());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    installeMenu(); //Installe le menu Aide dans la menuBar

    //Initialisation de quelques variables ...
    QString adresseForum = "http://forum.abuledu.org/rss/tags/" + abeApp->applicationName() + "/redirect/on/lang/" + QLocale::system().name().section('_', 0, 0);
    ui->lblPosezVotreQuestion->setText("<a href='" + adresseForum + "'>" + trUtf8("Posez votre question sur le forum des utilisateurs ...") + "</a>");

    //Pour eviter de re-telecharger 10 fois les ressources en ligne, j'utilise un flag tout con pour chaque ressource.
    p_pedaDownloaded  = false;
    p_forumDownloaded = false;
    p_newsDownloaded  = false;

    //Un seul networkmanager pour tout lemonde me semble suffisant :)
    m_nam = new QNetworkAccessManager(this);

    //Gestion d'un proxy ?
    //il peut-être déterminé par une variable d'environnement
    QString envproxy = getenv("http_proxy");
    if(envproxy.length() > 0)
    {
        qDebug() << "AbulEduAproposv1.cpp: Proxy detected :: " << envproxy;
        envproxy.remove("/");
        //On découpe le proxy
        QRegExp exp(":");
        QStringList split = envproxy.split(exp);

        qDebug() << "AbulEduAproposv1.cpp: Proxy detected :: " << split;
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(split[1]);
        proxy.setPort(split[2].toInt());
        QNetworkProxy::setApplicationProxy(proxy);
        m_nam->setProxy(proxy);
    }
    else {
        //Détection automatique: WPAD
        QUrl adresseWPAD("");
        QHostInfo wpadSingleResolve = QHostInfo::fromName("wpad");
        if (wpadSingleResolve.error() != QHostInfo::NoError) {
            qDebug() << "Lookup failed:" << wpadSingleResolve.errorString();
            //On essaye avec le domaine complet ...
            QHostInfo wpadDomainResolve = QHostInfo::fromName("wpad." + QHostInfo::localDomainName());
            if (wpadDomainResolve.error() != QHostInfo::NoError) {
                qDebug() << "Lookup failed:" << wpadDomainResolve.errorString();
            }
            else {
                //On télécharge le fichier wpad pour l'analyser ...
                adresseWPAD = "http://wpad." + QHostInfo::localDomainName() + "/wpad.dat";
            }
        }
        else {
            //On télécharge le fichier wpad pour l'analyser ...
            adresseWPAD = "http://wpad/wpad.dat";
        }
        if(! adresseWPAD.isEmpty()){
            connect(m_nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotWPAD(QNetworkReply*)));
            QNetworkReply* reply = m_nam->get(QNetworkRequest(adresseWPAD));
        }
    }
}

AbulEduAproposV1::~AbulEduAproposV1()
{
    delete ui;
}


void AbulEduAproposV1::installeMenu()
{
    QString titreAbout=trUtf8("<center>%1 %2</center").arg(abeApp->getAbeApplicationLongName()) //Récupère les paramètres
                       .arg(abeApp->applicationVersion());
    ui->textAbout->setHtml(titreAbout+" "+ui->textAbout->toHtml());         // Ajoute Nom application et Version en haut de la page

    QMenuBar *b = parentWidget()->findChild<QMenuBar *>(); // Cherche la barre de menu
    w=b->parentWidget();
    QMenu *m = w->findChild<QMenu *>("menuAide"); //On cherche si le menu aide existe

    if(!m)  //sinon, on crée le menu Aide
    {
        QMenu *menuAide = new QMenu(trUtf8("&Aide"),b);
        menuAide->setObjectName("menuAide");
        b->addMenu(menuAide); //Ajoute l'entrée Aide au menu

        //Ajout des entrées de menu

        //Aide
        w= b->parent(); //Recupère la fenetre parente
        QString nomItem=trUtf8("Aide");
        actionAide=new QAction(nomItem,w);
        menuAide->addAction(actionAide);
        if(abeApp->organizationDomain()=="")
        {
            actionAide->setDisabled(true);
        }
        connect(actionAide, SIGNAL(triggered()), this, SLOT(aide()));

        //
        nomItem=trUtf8("Aide intégrée...");
        actionAideIntegree=new QAction(nomItem,w);
        actionAideIntegree->setShortcut(Qt::Key_F1);
        menuAide->addAction(actionAideIntegree);
        connect(actionAideIntegree, SIGNAL(triggered()),this,SLOT(montreAide()));

        // Ressources péda
        nomItem=trUtf8("Ressources pédagogiques...");
        actionPeda=new QAction(nomItem,w);
        menuAide->addAction(actionPeda);
        connect(actionPeda, SIGNAL(triggered()),this,SLOT(montrePeda()));

        // News
        nomItem=trUtf8("Nouveautés...");
        actionNews=new QAction(nomItem,w);
        menuAide->addAction(actionNews);
        connect(actionNews, SIGNAL(triggered()),this,SLOT(montreNews()));

        // Contact Forum
        nomItem=trUtf8("Forum des utilisateurs...");
        actionForum=new QAction(nomItem,w);
        menuAide->addAction(actionForum);
        connect(actionForum, SIGNAL(triggered()),this,SLOT(montreForum()));

        // Traductions
        nomItem=trUtf8("Contribuez...");
        actionTraductions=new QAction(nomItem,w);
        menuAide->addAction(actionTraductions);
        connect(actionTraductions, SIGNAL(triggered()),this,SLOT(montreTraduire()));

        // A propos
        nomItem=trUtf8("À Propos de ce logiciel...");
        actionTraductions=new QAction(nomItem,w);
        menuAide->addAction(actionTraductions);
        connect(actionTraductions, SIGNAL(triggered()),this,SLOT(montreAPropos()));

    }
}

//--------------------------------------------------------------------------------------
//
//------------------------------Gestion fenêtre A Propos--------------------------------
//
//--------------------------------------------------------------------------------------


/**
  *les fonctions montreAide(), montreNews(), montreForum(), montreTraduire(), montreAPropos()
  *créent la fenêtre d'APropos en mode modal et positionnent le bon onglet
  *en émettant le signal setIndex(int)
  */
void AbulEduAproposV1::montreAide()
{
    connect(this, SIGNAL(setindex(int)),this,SLOT(changeTab(int)));
    emit setindex(0);
    this->show();
}

void AbulEduAproposV1::montreNews()
{
    connect(this, SIGNAL(setindex(int)),this,SLOT(changeTab(int)));
    emit setindex(2);
    this->show();

    if(!p_newsDownloaded)
    {
        p_newsDownloaded = true;
        //Essaye de telecharger les news du projet redmine
        //Message d'erreur
        QString messageErreur = trUtf8("Le site n'est pas accessible");
        //Récupère l'adresse qui est passée dans le textBrowser
        QUrl adresseFlux = abeBuildUrl("news","");

        //Message d'attente ...
        QString message = trUtf8("Téléchargement en cours ... veuillez patienter.");
        ui->textNews->setHtml(message);

        if(!adresseFlux.isValid())
        {
            ui->textNews->setHtml(messageErreur);
        }
        else
        {
            //On y va
            connect(m_nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotNews(QNetworkReply*)));
            QNetworkReply* reply = m_nam->get(QNetworkRequest(adresseFlux));
        }
    }
}

void AbulEduAproposV1::montrePeda()
{
    connect(this, SIGNAL(setindex(int)),this,SLOT(changeTab(int)));
    emit setindex(1);
    this->show();

    if(!p_pedaDownloaded)
    {
        p_pedaDownloaded = true;
        //Essaye de telecharger les news du projet redmine
        //Message d'erreur
        QString messageErreur = trUtf8("Le site n'est pas accessible");
        //Récupère l'adresse qui est passée dans le textBrowser
        QUrl adresseFlux = abeBuildUrl("peda","");

        //Message d'attente ...
        QString message = trUtf8("Téléchargement en cours ... veuillez patienter.");
        ui->textAide_2->setHtml(message);

        if(!adresseFlux.isValid())
        {
            ui->textAide_2->setHtml(messageErreur);
        }
        else
        {
            //On y va
            connect(m_nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotPeda(QNetworkReply*)));
            QNetworkReply* reply = m_nam->get(QNetworkRequest(adresseFlux));
        }
    }
}


void AbulEduAproposV1::montreForum()
{
    connect(this, SIGNAL(setindex(int)),this,SLOT(changeTab(int)));
    emit setindex(3);
    this->show();

    if(!p_forumDownloaded)
    {
        p_forumDownloaded = true;
        //Essaye de telecharger le contenu du forum ...
        //Message d'erreur
        QString messageErreur = trUtf8("Le forum n'est pas accessible");
        //Récupère l'adresse qui est passée dans le textBrowser
        QUrl adresseFlux = abeBuildUrl("forum","");

        //Message d'attente ...
        QString message = trUtf8("Téléchargement en cours ... veuillez patienter.");
        ui->textForum->setHtml(message);

        if(!adresseFlux.isValid())
        {
            ui->textForum->setHtml(messageErreur);
        }
        else
        {
            //On y va
            connect(m_nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotForum(QNetworkReply*)));
            QNetworkReply* reply = m_nam->get(QNetworkRequest(adresseFlux));
        }
    }
}

void AbulEduAproposV1::montreTraduire()
{
    connect(this, SIGNAL(setindex(int)),this,SLOT(changeTab(int)));
    emit setindex(4);
    this->exec();
}

void AbulEduAproposV1::montreAPropos()
{
    connect(this, SIGNAL(setindex(int)),this,SLOT(changeTab(int)));
    emit setindex(5);
    this->exec();
}

void AbulEduAproposV1::changeTab(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}

/** Slot lançant un navigateur vers l'Url passée dans abeApp->setOrganizationDomain(); */
void AbulEduAproposV1::aide()
{
    QUrl urlSite="http://"+abeApp->organizationDomain()+"/"+abeApp->organizationName()+"/"+abeApp->applicationName();
    QDesktopServices::openUrl(urlSite);
}

void AbulEduAproposV1::finishedSlotForum(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // lectures des données issues du "reply"
        xml= new QDomDocument();
        xml->setContent(reply->readAll());
        QDomElement xmlElem = xml->documentElement();
        QDomNode node = xmlElem.firstChild();
        while(!node.isNull())
        {
            QDomElement element = node.toElement(); // On essaie de convertir le node en element
            if(!element.isNull())
            {
                if(element.tagName()=="channel")    // Le node est bien un élément.
                {
                    QDomNode node=element.firstChild(); //On parcourt le document à partir du tag "channel"
                    while(!node.isNull())
                    {
                        QDomElement elem=node.toElement(); //On cherche le titre du "Channel"
                        if(elem.tagName()=="title")
                        {
                            QDomNode node=elem.firstChild();
                            QString titre=node.toText().data();
                            ui->textForum->setHtml("<h4><font color=red>"+titre+"</font></h4>");
                        }
                        else if((elem.tagName()=="item"))
                        {
                            QDomNode node=elem.firstChild();
                            while(!node.isNull())
                            {
                                QDomElement elem=node.toElement(); //Titre du fils de discussion
                                if(elem.tagName()=="title")
                                {
                                    QDomNode node=elem.firstChild();
                                    itemTitre=node.toText().data();
                                    node=node.nextSiblingElement("link");
                                }
                                if(elem.tagName()=="link")      // Lien vers post
                                {
                                    QDomNode node=elem.firstChild();
                                    itemLink=node.toText().data();
                                    node=node.nextSiblingElement("description");
                                }
                                if(elem.tagName()=="description") // Texte du post
                                {
                                    QDomNode node=elem.firstChild();
                                    itemDescription =node.toText().data();
                                    node=node.nextSibling();
                                }
                                node=node.nextSibling();
                            }
                            QString itemView= "<a href="+itemLink+">"
                                              "<font color=green>"+itemTitre+"</font></a>"
                                              +itemDescription;
                            ui->textForum->append(itemView);
                        }
                        node=node.nextSibling();
                    }
                }
                node = node.nextSibling();
            }
        }
    }
    // Some http error received
    else
    {
        QString messageErreur = trUtf8("Le forum n'est pas accessible");
        ui->textForum->setHtml(messageErreur);
    }
    QScrollBar *sb = ui->textForum->verticalScrollBar();       //On scrolle vers le bas
    sb->setValue(sb->minimum());
    disconnect(m_nam,SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotForum(QNetworkReply*)));
}

void AbulEduAproposV1::finishedSlotNews(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // lectures des données issues du "reply"
        xml= new QDomDocument();
        xml->setContent(reply->readAll());

        QDomElement root = xml->documentElement();
        QDomElement element = root.firstChildElement("title");
        if(!element.isNull())
        {
            ui->textNews->setHtml("<h4><font color=red>" + element.text().trimmed() + "</font></h4>");
        }

        QDomElement child = root.firstChildElement("entry").toElement();
        QDomElement childL;
        while(!child.isNull())
        {
            //qDebug() << "Lecture XML -> " << child.tagName();
            childL = child.childNodes().at(0).toElement();
            if (childL.tagName() == "title")
            {
                itemTitre = childL.text().trimmed();
                childL = childL.nextSiblingElement("id");
                //qDebug() << "title :: " << itemTitre;
            }
            if(childL.tagName()=="id")      // Lien vers post
            {
                itemLink = childL.text().trimmed();
                childL = childL.nextSiblingElement("updated");
                //qDebug() << "Lien :: " << itemLink;
            }
            if(childL.tagName()=="updated") // Date
            {
                itemDate = childL.text().trimmed();
                childL=childL.nextSiblingElement("content");
            }
            if(childL.tagName()=="content") // Texte du post
            {
                itemDescription = childL.text().trimmed();
            }
            QString itemView= "<a href="+itemLink+">"
                              "<font color=green>"+itemTitre+"</font></a>"
                              +itemDescription;
            ui->textNews->append(itemView);

            //child = root.nextSiblingElement("entry").toElement();
            child = child.nextSibling().toElement();
        }
    }
    // Some http error received
    else
    {
        QString messageErreur = trUtf8("Le site n'est pas accessible");
        ui->textNews->setHtml(messageErreur);
    }
    QScrollBar *sb = ui->textNews->verticalScrollBar(); //On scrolle vers le bas
    sb->setValue(sb->minimum());
    disconnect(m_nam,SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotNews(QNetworkReply*)));
}

void AbulEduAproposV1::finishedSlotPeda(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // lectures des données issues du "reply"
        xml= new QDomDocument();
        xml->setContent(reply->readAll());

        QDomElement xmlElem = xml->documentElement();
        QDomNode node = xmlElem.firstChild();
        while(!node.isNull())
        {
            QDomElement element = node.toElement(); // On essaie de convertir le node en element
            if(!element.isNull())
            {
                if(element.tagName()=="channel")    // Le node est bien un élément.
                {
                    QDomNode node=element.firstChild(); //On parcourt le document à partir du tag "channel"
                    while(!node.isNull())
                    {
                        QDomElement elem=node.toElement(); //On cherche le titre du "Channel"
                        if(elem.tagName()=="title")
                        {
                            QDomNode node=elem.firstChild();
                            QString titre=node.toText().data();
                            ui->textAide_2->setHtml("<h4><font color=red>"+titre+"</font></h4>");
                        }
                        else if((elem.tagName()=="item"))
                        {
                            QDomNode node=elem.firstChild();
                            while(!node.isNull())
                            {
                                QDomElement elem=node.toElement(); //Titre du fils de discussion
                                if(elem.tagName()=="title")
                                {
                                    QDomNode node=elem.firstChild();
                                    itemTitre=node.toText().data();
                                    node=node.nextSiblingElement("link");
                                }
                                if(elem.tagName()=="link")      // Lien vers post
                                {
                                    QDomNode node=elem.firstChild();
                                    itemLink=node.toText().data();
                                    node=node.nextSiblingElement("description");
                                }
                                if(elem.tagName()=="description") // Texte du post
                                {
                                    QDomNode node=elem.firstChild();
                                    itemDescription =node.toText().data();
                                    node=node.nextSibling();
                                }
                                node=node.nextSibling();
                            }
                            QString itemView= "<p><a href='"+itemLink+"'>"
                                              "<font color=green>"+itemTitre+"</font></a></p>"
                                              +itemDescription;
                            ui->textAide_2->append(itemView);
                        }
                        node=node.nextSibling();
                    }
                }
                node = node.nextSibling();
            }
        }
    }
    // Some http error received
    else
    {
        QString messageErreur = trUtf8("Le site n'est pas accessible");
        ui->textAide_2->setHtml(messageErreur);
    }
    QScrollBar *sb = ui->textAide_2->verticalScrollBar();       //On scrolle vers le bas
    sb->setValue(sb->minimum());
    disconnect(m_nam,SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotPeda(QNetworkReply*)));
}


void AbulEduAproposV1::on_lblPosezVotreQuestion_linkActivated(QString link)
{
    QString adresseForum = "http://forum.abuledu.org/";
    QDesktopServices::openUrl(adresseForum);
}

void AbulEduAproposV1::on_tabWidget_currentChanged(int index)
{
    //Petite astuce pour lancer les download des ressources en ligne si
    //on change d'onglet au lieu de cliquer directment dans le menu principal
    switch(index)
    {
    case 1:
        montrePeda();
        break;
    case 2:
        montreNews();
        break;
    case 3:
        montreForum();
        break;
    }
}

QUrl AbulEduAproposV1::abeBuildUrl(QString reflector, QString action)
{
    QString os;
#if defined(Q_OS_MAC)
    os="osx";
#elif defined(Q_OS_WIN32)
    os="windows";
#elif defined(Q_OS_LINUX)
    os="linux";
#endif

    QUrl adresseFlux = "http://updates.ryxeo.com/application/" + abeApp->applicationName() + "/version/" + abeApp->applicationVersion() + "/os/" + os + "/action/" + action + "/lang/" + QLocale::system().name().section('_', 0, 0) + "/reflector/" + reflector;
    return adresseFlux;
}

/** @warning: Solution rapide de recherche d'un proxy, je n'analyse pas le javascript retourne et cherche uniquement une ligne PROXY ...*/
void AbulEduAproposV1::finishedSlotWPAD(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        // lectures des données issues du "reply"
        QByteArray contenu = reply->readAll();
        if(contenu.contains("PROXY")) {
            QByteArray temp = contenu.right(contenu.length() - contenu.indexOf("PROXY ") - 6); //6 = longueur du mot "PROXY "
            QString envproxy = temp.left(temp.indexOf(";"));
            qDebug() << "Proxy trouvé: " << envproxy;
            QRegExp exp(":");
            QStringList split = envproxy.split(exp);

            qDebug() << "AbulEduAproposv1.cpp: Proxy detected :: " << split;
            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(split[0]);
            proxy.setPort(split[1].toInt());
            QNetworkProxy::setApplicationProxy(proxy);
            m_nam->setProxy(proxy);
        }
    }
    disconnect(m_nam,SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlotWPAD(QNetworkReply*)));
}

void AbulEduAproposV1::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange) {
        QMenu *m = w->findChild<QMenu *>("menuAide"); //On cherche si le menu aide existe
        if(m) {
            m->setObjectName("vieuxMenuAide");
            m->deleteLater();
        }
        installeMenu(); //Installe le menu Aide dans la menuBar
        ui->retranslateUi(this);
    }
}
