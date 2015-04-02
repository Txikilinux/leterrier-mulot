/** Classe ExerciceParcours
  * @file exerciceparcours.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2013-2014 Icham Sirat <icham.sirat@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
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

#include "exerciceparcours.h"

ExerciceParcours::ExerciceParcours(QWidget *parent, QString theme)
    : AbstractExercice(parent, theme, AbstractExercice::Parcours)
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__ << parent << theme;

    NB_MASQUESATTENDUS = NB_MASQUESREELS = 0;
    m_listeMasquesParcours.clear();
}

ExerciceParcours::~ExerciceParcours()
{
    ABULEDU_LOG_DEBUG()<< __PRETTY_FUNCTION__;
}

void ExerciceParcours::slotQuestionEntered()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    /* Demarrage du chronometre */
    chronometre()->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if(!isExerciceRunning())
    {
        itemImage()->setVisible(true);
        setNbMasquesInteractifs(0);
        ABULEDU_LOG_DEBUG() <<  abeStateMachineGetNumExercice();
        /* @notes : m_exercice 0 pour exercice 1 */
        chargerPositionMasque(abeStateMachineGetNumExercice()+1);

        // Chargement et Controle de la liste
        if(m_listePositionMasquesParcours.isEmpty() && (m_listePositionMasquesParcours.count() != OPT_nbMasquesChoisis()))
        {
            ABULEDU_LOG_DEBUG() << "PROBLEME Liste parcours " + QString::number(getAbeNumQuestion());
            return;
        }

        /* Masque arrivee (1 de la liste positionMasque) */
        m_masqueArrivee = listeMasquesFixes().at(m_listePositionMasquesParcours.takeFirst());
        m_masqueArrivee->setColor(QColor(Qt::red));
        connect(m_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);

        /* Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee) */
        m_masqueDepart = listeMasquesFixes().at(m_listePositionMasquesParcours.takeFirst());
        m_masqueDepart->setColor(QColor(Qt::green));
        connect(m_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
        m_masqueDepart->setHideOnMouseOver(false);
        m_masqueDepart->setHideOnClick(true);

        /* Le premier de la liste est le départ */
        m_listeMasquesParcours << m_masqueDepart; // en premier

        /* Masque parcours (le reste de la liste) */
        /*  Il faut cependant effectuer quelques petits contrôles afin de ne pas se retrouver avec des labyrinthes bizarres. */
        QList<int> voisinsPossibles;
        while (!m_listePositionMasquesParcours.isEmpty())
        {
            voisinsPossibles = AbulEduTools::masquesVoisins(m_listeMasquesParcours.last()->getNumero(), OPT_nbMasquesLargeur(), OPT_nbMasquesHauteur());
            if(voisinsPossibles.contains(listeMasquesFixes().at(m_listePositionMasquesParcours.first())->getNumero()))
            {
                /* Ce masque est ok, on y va */
                m_masqueParcours = listeMasquesFixes().at(m_listePositionMasquesParcours.takeFirst());
                m_masqueParcours->setColor(QColor(Qt::black));
                connect(m_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
                m_listeMasquesParcours << m_masqueParcours;
            }
            else{
                /* Ce masque n'est pas bon */
                m_listePositionMasquesParcours.removeFirst();
            }
        }

        m_listeMasquesParcours << m_masqueArrivee; // en dernier
        setExerciceRunning(true);

        NB_MASQUESREELS = m_listeMasquesParcours.count();
        if(NB_MASQUESATTENDUS != NB_MASQUESREELS){
            // Message Box Alerte
            QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
                    + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
                    + trUtf8("merci de nous le signaler.") +" <br />"
                    + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                    +" </td>" ;
            m_messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, parent());
            connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()),this, SLOT(slotQuitterAccueil()));
            m_messageBox->show();
        }
    }
}

void ExerciceParcours::slotCacheMasque()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    //j'enleve le premier masque de la ma liste de parcours
    if(!m_listeMasquesParcours.isEmpty())
    {
        ABULEDU_LOG_DEBUG() << "La liste avant : " << m_listeMasquesParcours.count();

        m_listeMasquesParcours.removeFirst();

        /* On colorie le suivant en bleu Attention, je fais un removeFirst, donc ma liste doit être
         * strictement supérieure à 1 (sinon segfault)
         */
        if(m_listeMasquesParcours.count() > 1){
            m_listeMasquesParcours.first()->setColor(Qt::blue);
            m_listeMasquesParcours.first()->update();
        }
        if (m_listeMasquesParcours.count() != 0)
        {
            if (m_listeMasquesParcours.count() == 1)
                m_listeMasquesParcours.first()->setHideOnClick(true);
            else
                m_listeMasquesParcours.first()->setHideOnMouseOver(true);
        }
        ABULEDU_LOG_DEBUG() << "La liste apres : " << m_listeMasquesParcours.count();
    }

    if (m_listeMasquesParcours.isEmpty())
    {
        foreach (MasqueDeplaceSouris* var, listeMasquesFixes())
        {
            var->setVisible(false);
        }

        /* Faire tout ce qu'il faut qd j'ai plus de masque */
        /* On peut mettre en pause */
        setPauseAvailable(true);

        /* Appui sur le bouton vérifier */
        QTimer::singleShot(0,this,SLOT(slotAppuiAutoVerifier()));

        boiteTetes->setEtatTete(getAbeNumQuestion()-1, abe::evalA );
        listeMasquesFixes().clear();

        /* Affichage du temps passé */
        ABULEDU_LOG_DEBUG() << "Temps écoulé: %d ms",chronometre()->elapsed();
        /* Enregistrement du temps passé pour chaque question */
        switch (abeStateMachineGetNumExercice()+1){
        case 1:
            setTempsQuestion1(chronometre()->elapsed());
            break;
        case 2:
            setTempsQuestion2(chronometre()->elapsed());
            break;
        case 3:
            setTempsQuestion3(chronometre()->elapsed());
            break;
        case 4:
            setTempsQuestion4(chronometre()->elapsed());
            break;
        case 5:
            setTempsQuestion5(chronometre()->elapsed());
            break;
        default :
            ABULEDU_LOG_DEBUG() << "Case Default - Temps écoulé: %d ms", chronometre()->elapsed();
            break;
        }
    }
}

void ExerciceParcours::slotAide()
{
    ABULEDU_LOG_DEBUG() << __PRETTY_FUNCTION__;

    eventFilter(this, new QKeyEvent(QEvent::KeyRelease,Qt::Key_Space,Qt::NoModifier,"space",0,1));
    getAbeExerciceTelecommandeV1()->ui->btnAide->setEnabled(false);

    QString consigne = "<td> " + trUtf8("Suis le parcours.")+" <br />"
            + trUtf8("Clique sur le rectangle vert pour commencer.") +" <br />"
            + trUtf8("Survole les rectangles noirs.") +" <br />"
            + trUtf8("Clique sur le rectangle rouge pour terminer.") +" <br />"
            +trUtf8("Quand une image est trouvée, la suivante arrive toute seule au bout de quelques instants.")
            +" </td>" ;

    m_messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,true, parent());
    connect(m_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    m_messageBox->setWink();
    m_messageBox->show();
}

void ExerciceParcours::chargerPositionMasque(const int& numeroExercice)
{
    m_listePositionMasquesParcours.clear();
    QList<int> listePosition;
    parametres()->beginGroup("parcours");
    parametres()->beginGroup("parcours"+QString::number(numeroExercice));

    /* Recuperation des masques de départ et d'arrivée.. */
    int i = 0;
    ABULEDU_LOG_DEBUG() << "Nombre de masques pour le parcours = " << parametres()->childKeys().count();
    ABULEDU_LOG_DEBUG() << "Recherche du départ et de l'arrivee";

    while(i < parametres()->childKeys().count()){
        if(parametres()->childKeys().at(i).contains("MasqueD") || parametres()->childKeys().at(i).contains("MasqueA")){
            m_listePositionMasquesParcours << parametres()->value(parametres()->childKeys().at(i)).toInt();
            ++i;
        }
        else
        {
            i++;
        }
    }

    /* Si pas de Départ et pas d'Arrivée, on quitte */
    if(m_listePositionMasquesParcours.count() != 2){
        // Message Box Alerte
        QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
                + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
                + trUtf8("merci de nous le signaler.") +" <br />"
                + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                +" </td>" ;
        m_messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, parent());
        m_messageBox->show();
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
     * Il faut boucler dessus est recupéré les positions des masques correspondantes
     */
    int pos;
    while(!listePosition.isEmpty()){
        pos = listePosition.takeFirst();
        foreach (const QString var, parametres()->childKeys()) {
            if(var.contains("MasqueParcours")){
                if(pos == var.split("MasqueParcours").at(1).toInt()){
                    m_listePositionMasquesParcours << parametres()->value(var).toInt();
                }
            }
        }
    }
    parametres()->endGroup();
    parametres()->endGroup();

    /* On enregistre le nombre de masques attendus */
    NB_MASQUESATTENDUS = m_listePositionMasquesParcours.count();
}
