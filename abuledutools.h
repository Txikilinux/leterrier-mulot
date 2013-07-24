#ifndef ABULEDUTOOLS_H
#define ABULEDUTOOLS_H

#include <QList>
#include <QDir>
#include <QMessageBox>
#include <QDesktopWidget>

class AbulEduTools
{
    Q_OBJECT

public:

    /** Calcule et retourne la liste des voisins possibles d'un masque
      * @param numeroMasque,    int, le numero du masque courant
      * @param largeur,         int, le nombre de masques dans la largeur du parcours
      * @param hauteur,         int, le nombre de masques dans la hauteur du parcours
      * @return voisinsMasques, QList<Int>, la liste des voisins possibles (gauche, droite, haut, bas)
      */
    static QList<int> masquesVoisins(const int& numeroMasque, const int& largeur, const int& hauteur);

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

    /** Méthode qui retourne la plus petite division d'entiers dont le résultat est supérieur à monChiffre
      * @param int monChiffre, le chiffre à calculer
      * @return QPair<int, int>, la plus petite division d'entiers dont le resultat est supérieur à monChiffre
      * @brief Elle sert au calcul de la taille des masques.
      * @example Pour 11 masques, les divisions possibles sont 2*6 ou 3*4.
      *          le QPair retourné sera 3*4, ce qui nous donne 12.
      *          Donc à la première question, l'image sera divisé par 12 petits masques de taille identique (4 dans la largeur et 3 dans la hauteur)
      */
    static QPair<int, int> plusPetiteDivision(const int& monChiffre);
};

#endif // ABULEDUTOOLS_H
