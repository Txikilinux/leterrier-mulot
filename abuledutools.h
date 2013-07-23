#ifndef ABULEDUTOOLS_H
#define ABULEDUTOOLS_H

#include <QList>
#include <QDir>

class AbulEduTools
{

public:

    /** Calcule et retourne la liste des voisins possibles d'un masque
      * @param numeroMasque, int, le numero du masque courant
      * @param largeur, int, le nombre de masques dans la largeur du parcours
      * @param hauteur, int, le nombre de masques dans la hauteur du parcours
      * @return voisinsMasques, QList<Int>, la liste des voisins possibles (gauche, droite, haut, bas)
      */
    static QList<int> masquesVoisins(int numeroMasque, int largeur, int hauteur);

    /** Supprime un répertoire et tout son contenu
      * Le répertoire passé en paramètre est aussi supprimé
      * @param const QString& dirPath, le chemin du répertoire à supprimer (ex : "/home/user/monRepertoire")
      * @return bool, true si suppression ok, false sinon
      */
    static bool supprimerDir(const QString& dirPath);

    /** Retourne une liste de chemin correspondant à l'arborescence située en dessous du dossier donné en paramètre
      * @param const QString dossier, le chemin du dossier à analyser
      * @return QStringList, la liste des chemins correspondant à l'arborescence descandante du dossier
      */
    static QStringList parcoursRecursif(const QString& dossier);


};

#endif // ABULEDUTOOLS_H
