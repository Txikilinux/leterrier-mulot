#include "exerciceparcours2.h"

ExerciceParcours2::ExerciceParcours2(QWidget *parent, QString theme)
    : ExerciceAbstract(parent, theme, ExerciceAbstract::Parcours)
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__<< " " << parent <<" " << theme;

    NB_MASQUESATTENDUS = NB_MASQUESREELS = 0;
}

ExerciceParcours2::~ExerciceParcours2()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
}

void ExerciceParcours2::slotQuestionEntered()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;

    /* Demarrage du chronometre */
    timer()->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if(!isExerciceRunnning())
    {
        itemImage()->setVisible(true);
//        qDebug() << nbMasquesInteractifs() /*= 0*/;
        setNbMasquesInteractifs(0);
        if(debugAvailable()) qDebug() << getAbeNumQuestion();
        chargerPositionMasque(getAbeNumQuestion());

    }
//    {
//        m_itemImage->setVisible(true);
//        if (m_localDebug) qDebug() << getAbeNumQuestion();
//        chargerPositionMasque(getAbeNumQuestion());
//        // Chargement et Controle de la liste
//        if (positionMasquesParcours.isEmpty() && positionMasquesParcours.count() != opt_nbMasquesChoisis)
//        {
//            if (m_localDebug) qDebug() << "PROBLEME Liste parcours " + QString::number(getAbeNumQuestion());
//            return;
//        }

//        /* Masque arrivee (1 de la liste positionMasque) */
//        m_masqueArrivee = m_listeMasquesFixes.at(positionMasquesParcours.takeFirst());
//        m_masqueArrivee->setColor(QColor(Qt::red));
//        connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);

//        /* Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee) */
//        m_masqueDepart = m_listeMasquesFixes.at(positionMasquesParcours.takeFirst());
//        m_masqueDepart->setColor(QColor(Qt::green));
//        connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
//        m_masqueDepart->setHideOnMouseOver(false);
//        m_masqueDepart->setHideOnClick(true);

//        /* Le premier de la liste est le départ */
//        m_listeMasquesParcours << m_masqueDepart; // en premier

//        /* Masque parcours (le reste de la liste) */
//        /*  Il faut cependant effectuer quelques petits contrôles afin de ne pas se retrouver avec des labyrinthes bizarres. */
//        QList<int> voisinsPossibles;
//        while (!positionMasquesParcours.isEmpty())
//        {
//            voisinsPossibles = AbulEduTools::masquesVoisins(m_listeMasquesParcours.last()->getNumero(), opt_nbMasquesLargeur, opt_nbMasquesHauteur);
//            if(voisinsPossibles.contains(m_listeMasquesFixes.at(positionMasquesParcours.first())->getNumero()))
//            {
//                /* Ce masque est ok, on y va */
//                m_masqueParcours = m_listeMasquesFixes.at(positionMasquesParcours.takeFirst());
//                m_masqueParcours->setColor(QColor(Qt::black));
//                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
//                m_listeMasquesParcours << m_masqueParcours;

//            }
//            else{
//                /* Ce masque n'est pas bon */
//                positionMasquesParcours.removeFirst();
//            }
//        }
//    }
//    m_listeMasquesParcours << m_masqueArrivee; // en dernier
//    m_exerciceEnCours = true;

//    NB_MASQUESREELS = m_listeMasquesParcours.count();
//    if(NB_MASQUESATTENDUS != NB_MASQUESREELS){
//        // Message Box Alerte
//        QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
//                + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
//                + trUtf8("merci de nous le signaler.") +" <br />"
//                + trUtf8("Le programme va quitter l'exercice.") +" <br />"
//                +" </td>" ;
//        AbulEduMessageBoxV1* messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, m_parent);

//        messageBox->show();
//        slotQuitterAccueil();
//    }
}

void ExerciceParcours2::slotCacheMasque()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
}

void ExerciceParcours2::slotAide()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;

    eventFilter(this, new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1));
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Suis le parcours.")+" <br />"
            + trUtf8("Clique sur le rectangle vert pour commencer.") +" <br />"
            + trUtf8("Survole les rectangles noirs.") +" <br />"
            + trUtf8("Clique sur le rectangle rouge pour terminer.") +" <br />"
            +trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.")
            +" </td>" ;

    _messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,false, 0);
    connect(_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    _messageBox->setWink();
    _messageBox->show();
}

void ExerciceParcours2::chargerPositionMasque(const int& numeroQuestion)
{
    _positionMasquesParcours.clear();

    qDebug() << "Chargement nombre masques choisis : " << OPT_nbMasquesChoisis();
    qDebug() << "Chargement nombre masques largeur : " << OPT_nbMasquesLargeur() ;
    qDebug() << "Chargement nombre masques hauteur : " << OPT_nbMasquesHauteur() ;

    QList<int> listePosition;
    qDebug() << parametres()->allKeys();
    parametres()->beginGroup("parcours");
    parametres()->beginGroup("parcours"+QString::number(numeroQuestion));
//    set_OPT_nbMasquesChoisis(parametres()->childKeys().count());



    /* Recuperation des masques de départ et d'arrivée.. */
    int i = 0;
    if(debugAvailable()) {
        qDebug() << "Nombre de masques pour le parcours = " << parametres()->childKeys().count();
        qDebug() << "Recherche du départ et de l'arrivee";
    }
    while(i < parametres()->childKeys().count()){
        if(parametres()->childKeys().at(i).contains("MasqueD") || parametres()->childKeys().at(i).contains("MasqueA")){
            _positionMasquesParcours << parametres()->value(parametres()->childKeys().at(i)).toInt();
            ++i;
        }
        else
        {
            i++;
        }
    }

    /* Si pas de Départ et pas d'Arrivée, on quitte */
    if(_positionMasquesParcours.count() != 2){
        // Message Box Alerte
        QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
                + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
                + trUtf8("merci de nous le signaler.") +" <br />"
                + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                +" </td>" ;
        _messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, parent());
        _messageBox->show();
        slotQuitterAccueil();
    }

    i = 0;
    while(i < parametres()->childKeys().count()){
        if(parametres()->childKeys().at(i).contains("MasqueParcours")){
            /* Remplissage d'un numero de masque pour le tri */
            listePosition << parametres()->childKeys().at(i).split("MasqueParcours").at(1).toInt();
            ++i;
        }
        else {
            i++;
        }
    }

    /* Ici, j'ai tout les numéros des masques sont dans la liste, il faut effectuer un tri croissant */
    qSort(listePosition.begin(), listePosition.end(), qLess<int>());

    /* La liste listePosition est triée
     * Il faut boucler dessus est recupéré les positions des masques corrspondantes
     */
    int pos;
    while(!listePosition.isEmpty()){
        pos = listePosition.takeFirst();
        foreach (const QString var, parametres()->childKeys()) {
            if(var.contains("MasqueParcours")){
                if(pos == var.split("MasqueParcours").at(1).toInt()){
                    _positionMasquesParcours << parametres()->value(var).toInt();
                }
            }
        }
    }
    /* On enregistre le nombre de masques attendus */
    NB_MASQUESATTENDUS = _positionMasquesParcours.count();
    qDebug() << "Nous avons un parcours de : "<< _positionMasquesParcours.count();
}
