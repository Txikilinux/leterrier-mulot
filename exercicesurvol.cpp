#include "exercicesurvol.h"

ExerciceSurvol::ExerciceSurvol(QWidget *parent):
    AbulEduCommonStatesV1(parent)
{
    m_localDebug = true;

    m_parent = parent;
    connect(m_parent, SIGNAL(dimensionsChangees()), this, SLOT(setDimensionsWidgets()));

    //Création de l'aire de travail + propriétés
    gv_AireDeJeu = new AbulEduEtiquettesV1(QPointF(0,0));

    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->setStyleSheet("background-color: yellow");
    getAbeExerciceTelecommandeV1()->setStyleSheet("background-color: red");

    // On la place sur l'AireDeTravail par l'intermédiaire d'un QGraphicsProxyWidget
    proxy = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(gv_AireDeJeu) ;
    proxy->setGeometry(gv_AireDeJeu->rect());
    proxy->setZValue(-1) ;

    // Instanciation de toutes les variables membres
    m_itemImage = new QGraphicsPixmapItem(0,getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene());
    m_listeImage.clear();
    m_listeFichiers.clear();
    m_listeMasquesFixes.clear();
    m_nbImage = 0;
    m_nbMasquesInteractifs = 0; // = à 7

    gv_AireDeJeu->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gv_AireDeJeu->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    gv_AireDeJeu->setStyleSheet("background-color: rgba(0,0,0,0)"); // Fond transparent
    gv_AireDeJeu->setStyleSheet("background-color: blue");
    gv_AireDeJeu->setFrameShape(QFrame::NoFrame);

    getAbeExerciceMessageV1()->setParent(gv_AireDeJeu);

    // Demarrage de la machine à états
    sequenceMachine->start();
}

ExerciceSurvol::~ExerciceSurvol()
{
    // Permet à la MainWindow de savoir que l'exercice est terminé
    emit exerciceExited();
}

void ExerciceSurvol::slotSequenceEntered() // en cours
{
    if (m_localDebug) qDebug()<<"--------------ExerciceSurvol::slotSequenceEntered()";

    AbulEduCommonStatesV1::slotSequenceEntered();

    setAbeNbExercices(5);
    setAbeNbTotalQuestions(1);
    getAbeExerciceMessageV1()->setParent(0);
    getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->scene()->addWidget(getAbeExerciceMessageV1());

    abeEnableAnimationsConsigneBilan(false);

    boiteTetes->resetTetes(m_nbExercices);

}

void ExerciceSurvol::slotPresenteSequenceEntered() //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotPresenteSequenceEntered()";

    // Normalement, on n'efface pas cette ligne, sinon tant pis
    AbulEduCommonStatesV1::slotPresenteSequenceEntered();

    getAbeExerciceMessageV1()->abeWidgetMessageSetTitre(trUtf8("Survol"));
    QString debutTableau = "<tr valign=middle>";
    QString imagetete = "<td> " + QString(" <img src=\":/evaluation/neutre\"></td>");
    QString consigne = "<td align = right>  Ma consigne qui presentera la sequence (video souhaitee)    </td>" ;
    QString finTableau = "</tr>";
    getAbeExerciceMessageV1()->abeWidgetMessageSetConsigne(debutTableau + imagetete + consigne + finTableau);

    getAbeExerciceMessageV1()->abeWidgetMessageSetZoneTexteVisible(true);
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->setVisible(true);
    redimensionnerConsigne();
    onPeutPresenterExercice = false; // permet de "sauter" la présentation de l'exercice
}


void ExerciceSurvol::slotRealisationExerciceEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotRealisationExerciceEntered()";

    // Mettre tout ce qui est commun à chaque question
    m_nbImage = m_nbExercices; // le nb image = le nb exercice
    m_nbMasquesInteractifs = 0;

    // aller chercher le pack image
    QDir dir("data/images/animaux/");
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++) {
        m_listeFichiers << list.at(i).absoluteFilePath();
    }

    // choisir 5 images au hasard dans le pack
    {
        for(int i = 0; i < m_nbImage; i++)
        {
            qsrand(QDateTime::currentDateTime ().toTime_t ());
            int n = (qrand() % (m_listeFichiers.size()));
            QFileInfo fileInfo = m_listeFichiers.takeAt(n);

            m_image.load(fileInfo.absoluteFilePath(), 0, Qt::AutoColor);

            m_listeImage << m_image;
        }

    }

    AbulEduCommonStatesV1::slotRealisationExerciceEntered();
    //  setDimensionsWidgets();
}


void ExerciceSurvol::slotInitQuestionEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotInitQuestionEntered()";
    getAbeExerciceMessageV1()->setVisible(false);
    gv_AireDeJeu->show();

    // Choix de l'image dans la liste
    if (m_localDebug) qDebug()<<"Methode mon exercice !!!!!";

    qDebug() << m_numExercice;

    // Je récupere la première image de ma liste et je la fixe sur l'aire de jeu
    m_itemImage = gv_AireDeJeu->scene()->addPixmap(m_listeImage[m_numExercice]);

    //mise en place du masque
    int largeur=150;
    int hauteur=150;

    m_itemImage->topLevelItem();

    int nbTotalPieces = 0;
    //Calcul du nombre de lignes et de colonnes necessaires
    for(int i = 0; i < gv_AireDeJeu->height(); i+=hauteur) {
        for(int j = 0; j < gv_AireDeJeu->width(); j+=largeur) {
            qDebug() << "ajout d'une piece ... " << nbTotalPieces;
            nbTotalPieces++;

            m_masque = new masqueDeplaceSouris();
            m_masque->setSize(largeur,hauteur);
            m_masque->moveBy(j,i);
            m_masque->setColor(Qt::black);
            m_masque->setParent(gv_AireDeJeu->scene());
            m_masque->setHideOnMouseOver(false);
            gv_AireDeJeu->scene()->addItem(m_masque);
            m_listeMasquesFixes << m_masque;
        }
    }


    AbulEduCommonStatesV1::slotInitQuestionEntered();
    setDimensionsWidgets();
}

void ExerciceSurvol::slotQuestionEntered() //todo
{
    if (m_localDebug) qDebug()<<"*******************ExerciceSurvol::slotQuestionEntered()";
    if (m_localDebug) qDebug()<< m_nbMasquesInteractifs;

    //Et ensuite on fait en sorte que seuls 7 masques soient actifs
    m_nbMasquesInteractifs = 0;
    while (m_nbMasquesInteractifs < 7)
    {
        if (m_localDebug) qDebug()<<"*******************//   Boucle des Survolables ";

        //Seuls quelques masques sont "survolables", les autres ne bougent pas quand
        //on les survole mais disparaissent quand il n'y a plus de masques sensibles
        int alea = (qrand() % (m_listeMasquesFixes.count()));
        qDebug() << "alea = " << alea;
        //m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
        m_masqueInteractif = m_listeMasquesFixes.takeAt(alea);
        connect(m_masqueInteractif, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()));
        m_masqueInteractif->setColor(Qt::yellow);
        m_masqueInteractif->setHideOnMouseOver(true);

        m_nbMasquesInteractifs++;
        alea++;
    }

    if (m_nbMasquesInteractifs ==0)

        AbulEduCommonStatesV1::slotQuestionEntered();
}

//void ExerciceSurvol::slotQuestionExited()
void ExerciceSurvol::slotFinQuestionEntered()
{
    qDebug()<< "!!!!!!!!!!!!!!!!!!!!!!!!!  On passe à l'image suivante";
    // Nettoyage de la liste des masques
    m_listeMasquesFixes.clear();
    m_nbMasquesInteractifs = 0;
    boiteTetes->setEtatTete(0,abe::evalA );

    sequenceMachine->postEvent(new StringEvent("QuestionLoopCorrect"));

    // Vider itemImage
    gv_AireDeJeu->scene()->removeItem(m_itemImage);

    // Passer à l'image suivante
    // Incrementer nb exercice
    //
}


//////Autres slots
///////////////////
////////////////



void ExerciceSurvol::slotQuitter() // ok
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::slotQuitter";
    AbulEduCommonStatesV1::slotQuitter();
}

void ExerciceSurvol::resizeEvent(QResizeEvent *event) //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::resizeEvent()";
    setDimensionsWidgets();
}

void ExerciceSurvol::showEvent(QShowEvent *event) //todo
{
    if (m_localDebug) qDebug()<<"               ExerciceSurvol::showEvent()";
    setDimensionsWidgets();
}

void ExerciceSurvol::setDimensionsWidgets() //todo
{
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---start";

    // Placement de AireDeTravailV1
    float ratio = abeApp->getAbeApplicationDecorRatio();
    getAbeExerciceAireDeTravailV1()->move(0, 0);

    // Placement de WidgetTelecommandeV1
    getAbeExerciceTelecommandeV1()->abeTelecommandeResize();
    getAbeExerciceTelecommandeV1()->move(1550*ratio, 0);

    // Placement de l'AireDeJeu
    int haut  = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 * ratio;
    int large = getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width();
    gv_AireDeJeu->abeEtiquettesSetDimensionsWidget(QSize(large-125 * ratio, haut - 50 * ratio));
    gv_AireDeJeu->move((100 * ratio) / 2, 50 * ratio);

    // Placement des têtes
    boiteTetes->setPos((getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->width() - boiteTetes->geometry().width())/2,
                       getAbeExerciceAireDeTravailV1()->ui->gvPrincipale->height() - boiteTetes->geometry().height() - 60 *ratio);

    // Redimensionne le widget de consignes et l'image
    redimensionnerConsigne();
    redimensionnerImage();

    AbulEduCommonStatesV1::setDimensionsWidgets();
    if (m_localDebug) qDebug()<<"                ExerciceSurvol::setDimensionsWidgets()---end";
}


////////////////////////////// Méthodes propres à la classe ///////////////////////////////////////

void ExerciceSurvol::redimensionnerConsigne()
{
    //    fondConsigne->setVisible(false); // Scorie liée à la compatibilité avec l'ancienne façon de faire
    getAbeExerciceMessageV1()->abeWidgetMessageResize();
    getAbeExerciceMessageV1()->move((getAbeExerciceAireDeTravailV1()->width() - getAbeExerciceMessageV1()->width())/2,
                                    ((getAbeExerciceAireDeTravailV1()->height() - getAbeExerciceMessageV1()->height())/2) - 200*abeApp->getAbeApplicationDecorRatio());
}

void ExerciceSurvol::redimensionnerImage()
{
    qDebug()<< m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
    int largeurImage = m_itemImage->pixmap().width();
    int hauteurImage = m_itemImage->pixmap().height();

    //Redimensionner l'image avec condition de garde car peut lever une exception donc bug
    if(!m_itemImage->pixmap().isNull())
    {
        //Si l'image est plus large que haute
        if (largeurImage > hauteurImage)
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToHeight(gv_AireDeJeu->height(), Qt::SmoothTransformation));
            qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            // Centrage de l'image
            m_itemImage->moveBy(differenceLargeur/2,0);

        }
        // Si l'image est plus haute que large
        else // largeurImage < hauteurImage
        {
            m_itemImage->setPixmap(m_itemImage->pixmap().scaledToWidth(gv_AireDeJeu->width(), Qt::SmoothTransformation));
            qDebug()<<"Redimm" <<m_itemImage->pixmap().width()<<" " << m_itemImage->pixmap().height();
            int differenceLargeur = (gv_AireDeJeu->width()) - (m_itemImage->pixmap().width());
            // Centrage de l'image
            m_itemImage->moveBy(differenceLargeur/2,0);
        }
    }
}

void ExerciceSurvol::slotCacheMasque()
{
    qDebug() << "ExerciceSurvol::slotCacheMasque : " << m_nbMasquesInteractifs;
    m_nbMasquesInteractifs--;

    if(m_nbMasquesInteractifs == 0) {
        //On affiche l'image en entier
        for(int i = 0; i < m_listeMasquesFixes.count(); i++)
        {
            m_listeMasquesFixes.at(i)->setVisible(false);

            qDebug()<< m_listeMasquesFixes.count();
        }
        emit slotQuestionExited();
    }
    //        if(m_listeImage.count() > 0) {
    //            m_nbImage++;
    //         //   QTimer::singleShot(2000, this, SLOT(lanceLeJeu()));
    //        }
    //        else {
    //            qDebug() << "L'exercice est terminé ...";
    //        }

}
