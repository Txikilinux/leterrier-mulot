/**
  * Classe
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


#include "widgetdeplacesouris.h"
#include "ui_widgetdeplacesouris.h"

widgetDeplaceSouris::widgetDeplaceSouris(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetDeplaceSouris)
{
    ui->setupUi(this);

    m_numImage = 0;
    m_nbTotalMasques = 0;

    QDir dir("data/images/animaux/");
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++) {
        m_ListeFichiers << list.at(i).absoluteFilePath();
    }
    m_scene = new QGraphicsScene();

    lanceLeJeu();
}

widgetDeplaceSouris::~widgetDeplaceSouris()
{
    delete ui;
}

void widgetDeplaceSouris::slotCacheMasque()
{
    qDebug() << "widgetDeplaceSouris::slotCacheMasque : " << m_nbTotalMasques;
    m_nbTotalMasques--;
    if(m_nbTotalMasques == 0) {
        qDebug() << "Bravo on passe à la suite ...";

        //On affiche l'image en entier
        for(int i = 0; i < m_listeMasquesFixes.count(); i++) {
            m_listeMasquesFixes.at(i)->setVisible(false);
        }

        if(m_ListeFichiers.count() > 0) {
            m_numImage++;
            QTimer::singleShot(2000, this, SLOT(lanceLeJeu()));
        }
        else {
            qDebug() << "L'exercice est terminé ...";
        }
    }
}

void widgetDeplaceSouris::lanceLeJeu()
{
    QGraphicsItem *item;
    QPixmap image;

    m_scene->clear();
    m_listeMasquesFixes.clear();

    qsrand(QDateTime::currentDateTime ().toTime_t ());
    int n = (qrand() % (m_ListeFichiers.size()));
    QFileInfo fileInfo = m_ListeFichiers.takeAt(n);

    image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);
    item = m_scene->addPixmap(image);
    ui->graphicsView->setScene(m_scene);

    int largeur=150;
    int hauteur=150;
    if(m_numImage > 9) {
        largeur=15;
        hauteur=15;
    }
    else if(m_numImage > 7) {
        largeur=25;
        hauteur=25;
    }
    else if(m_numImage > 5) {
        largeur=50;
        hauteur=50;
    }
    else if(m_numImage > 3) {
        largeur=100;
        hauteur=100;
    }
    int nbTotalPieces = 0;
    //Calcul du nombre de lignes et de colonnes necessaires
    for(int i = 0; i < image.height(); i+=hauteur) {
        for(int j = 0; j < image.width(); j+=largeur) {
            qDebug() << "ajout d'une piece ... " << nbTotalPieces;
            nbTotalPieces++;
            masqueDeplaceSouris *m = new masqueDeplaceSouris();
            m->setSize(largeur,hauteur);
            m->moveBy(j,i);
            m->setColor(Qt::black);
            m->setParent(m_scene);
            m->setHideOnMouseOver(false);
            ui->graphicsView->scene()->addItem(m);
            m_listeMasquesFixes << m;
        }
    }

    //Et ensuite on fait en sorte que seuls 7 masques soient actifs
    while(m_nbTotalMasques < 7) {
        //Seuls quelques masques sont "survolables", les autres ne bougent pas quand
        //on les survole mais disparaissent quand il n'y a plus de masques sensibles
        int alea = (qrand() % (m_listeMasquesFixes.count()));
        qDebug() << "alea = " << alea;
        masqueDeplaceSouris *m = m_listeMasquesFixes.takeAt(alea);
        connect(m, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
        m_nbTotalMasques++;
        m->setColor(Qt::yellow);
        m->setHideOnMouseOver(true);
    }
}
