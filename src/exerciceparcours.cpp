/** Classe ExerciceParcours
  * @file exerciceparcours.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2013 Icham Sirat <icham.sirat@ryxeo.com>
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
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__<< " " << parent <<" " << theme;

    NB_MASQUESATTENDUS = NB_MASQUESREELS = 0;
    _listeMasquesParcours.clear();
}

ExerciceParcours::~ExerciceParcours()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;
}

void ExerciceParcours::slotQuestionEntered()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;

    /* Demarrage du chronometre */
    chronometre()->start();

    AbulEduCommonStatesV1::slotQuestionEntered();

    if(!isExerciceRunning())
    {
        itemImage()->setVisible(true);
        setNbMasquesInteractifs(0);
        if(debugAvailable()) qDebug() <<  abeStateMachineGetNumExercice();
        /* @notes : m_exercice 0 pour exercice 1 */
        chargerPositionMasque(abeStateMachineGetNumExercice()+1);

        // Chargement et Controle de la liste
        if(_listePositionMasquesParcours.isEmpty() && (_listePositionMasquesParcours.count() != OPT_nbMasquesChoisis()))
        {
            if(debugAvailable()) qDebug() << "PROBLEME Liste parcours " + QString::number(getAbeNumQuestion());
            return;
        }

        /* Masque arrivee (1 de la liste positionMasque) */
        _masqueArrivee = listeMasquesFixes().at(_listePositionMasquesParcours.takeFirst());
        _masqueArrivee->setColor(QColor(Qt::red));
        connect(_masqueArrivee, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);

        /* Masque depart (2 de la liste mais takeFirst car j'ai deja pris l'arrivee) */
        _masqueDepart = listeMasquesFixes().at(_listePositionMasquesParcours.takeFirst());
        _masqueDepart->setColor(QColor(Qt::green));
        connect(_masqueDepart, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
        _masqueDepart->setHideOnMouseOver(false);
        _masqueDepart->setHideOnClick(true);

        /* Le premier de la liste est le départ */
        _listeMasquesParcours << _masqueDepart; // en premier

        /* Masque parcours (le reste de la liste) */
        /*  Il faut cependant effectuer quelques petits contrôles afin de ne pas se retrouver avec des labyrinthes bizarres. */
        QList<int> voisinsPossibles;
        while (!_listePositionMasquesParcours.isEmpty())
        {
            voisinsPossibles = AbulEduTools::masquesVoisins(_listeMasquesParcours.last()->getNumero(), OPT_nbMasquesLargeur(), OPT_nbMasquesHauteur());
            if(voisinsPossibles.contains(listeMasquesFixes().at(_listePositionMasquesParcours.first())->getNumero()))
            {
                /* Ce masque est ok, on y va */
                _masqueParcours = listeMasquesFixes().at(_listePositionMasquesParcours.takeFirst());
                _masqueParcours->setColor(QColor(Qt::black));
                connect(_masqueParcours, SIGNAL(signalCacheMasque()), this, SLOT(slotCacheMasque()), Qt::UniqueConnection);
                _listeMasquesParcours << _masqueParcours;
            }
            else{
                /* Ce masque n'est pas bon */
                _listePositionMasquesParcours.removeFirst();
            }
        }

        _listeMasquesParcours << _masqueArrivee; // en dernier
        setExerciceRunning(true);

        NB_MASQUESREELS = _listeMasquesParcours.count();
        if(NB_MASQUESATTENDUS != NB_MASQUESREELS){
            // Message Box Alerte
            QString msg = "<td> " + trUtf8("Les paramètres du module ne sont pas valides.")+" <br />"
                    + trUtf8("Si ce module provient de la <b>Médiatheque</b>,") +" <br />"
                    + trUtf8("merci de nous le signaler.") +" <br />"
                    + trUtf8("Le programme va quitter l'exercice.") +" <br />"
                    +" </td>" ;
            _messageBox = new AbulEduMessageBoxV1(trUtf8("Corruption données du module"),msg, true, parent());
            connect(_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()),this, SLOT(slotQuitterAccueil()));
            _messageBox->show();
        }
    }
}

void ExerciceParcours::slotCacheMasque()
{
    if(debugAvailable()) qDebug() << __PRETTY_FUNCTION__;

    //j'enleve le premier masque de la ma liste de parcours
    if(!_listeMasquesParcours.isEmpty())
    {
        if(debugAvailable()) qDebug() << "La liste avant : " << _listeMasquesParcours.count();

        _listeMasquesParcours.removeFirst();

        /* On colorie le suivant en bleu Attention, je fais un removeFirst, donc ma liste doit être
         * strictement supérieure à 1 (sinon segfault)
         */
        if(_listeMasquesParcours.count() > 1){
            _listeMasquesParcours.first()->setColor(Qt::blue);
            _listeMasquesParcours.first()->update();
        }
        if (_listeMasquesParcours.count() != 0)
        {
            if (_listeMasquesParcours.count() == 1)
                _listeMasquesParcours.first()->setHideOnClick(true);
            else
                _listeMasquesParcours.first()->setHideOnMouseOver(true);
        }
        if(debugAvailable()) qDebug() << "La liste apres : " << _listeMasquesParcours.count();
    }

    if (_listeMasquesParcours.isEmpty())
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
        if (debugAvailable())
        {
            qDebug("Temps écoulé: %d ms",chronometre()->elapsed());
            qDebug("Temps écoulé: %d sec",(chronometre()->elapsed())/1000);
        }

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
            if (debugAvailable()) qDebug("Case Default - Temps écoulé: %d ms", chronometre()->elapsed());
            break;
        }
    }

}

void ExerciceParcours::slotAide()
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

    _messageBox = new AbulEduMessageBoxV1(trUtf8("Un petit coup de pouce ?"), consigne,true, parent());
    connect(_messageBox, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(slotFermetureAide()), Qt::UniqueConnection);
    _messageBox->setWink();
    _messageBox->show();
}

void ExerciceParcours::chargerPositionMasque(const int& numeroExercice)
{
    _listePositionMasquesParcours.clear();
    QList<int> listePosition;
    parametres()->beginGroup("parcours");
    parametres()->beginGroup("parcours"+QString::number(numeroExercice));

    /* Recuperation des masques de départ et d'arrivée.. */
    int i = 0;
    if(debugAvailable()) {
        qDebug() << "Nombre de masques pour le parcours = " << parametres()->childKeys().count();
        qDebug() << "Recherche du départ et de l'arrivee";
    }
    while(i < parametres()->childKeys().count()){
        if(parametres()->childKeys().at(i).contains("MasqueD") || parametres()->childKeys().at(i).contains("MasqueA")){
            _listePositionMasquesParcours << parametres()->value(parametres()->childKeys().at(i)).toInt();
            ++i;
        }
        else
        {
            i++;
        }
    }

    /* Si pas de Départ et pas d'Arrivée, on quitte */
    if(_listePositionMasquesParcours.count() != 2){
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
     * Il faut boucler dessus est recupéré les positions des masques correspondantes
     */
    int pos;
    while(!listePosition.isEmpty()){
        pos = listePosition.takeFirst();
        foreach (const QString var, parametres()->childKeys()) {
            if(var.contains("MasqueParcours")){
                if(pos == var.split("MasqueParcours").at(1).toInt()){
                    _listePositionMasquesParcours << parametres()->value(var).toInt();
                }
            }
        }
    }
    parametres()->endGroup();
    parametres()->endGroup();

    /* On enregistre le nombre de masques attendus */
    NB_MASQUESATTENDUS = _listePositionMasquesParcours.count();
}
