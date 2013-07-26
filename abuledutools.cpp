/** Classe AbulEduTools
  * @file abuledutools.cpp
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012/2013 Icham Sirat <icham.sirat@ryxeo.com>
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

#include "abuledutools.h"

QList<int> AbulEduTools::masquesVoisins(const int &numeroMasque, const int &largeur, const int &hauteur)
{
    QList<int> voisinsMasques;
    const int nbTotal = largeur * hauteur;

    /* calcul des différentes directions possibles */
    const int gauche  = numeroMasque - 1;
    const int droite  = numeroMasque + 1;
    const int haut    = numeroMasque - largeur;
    const int bas     = numeroMasque + largeur;

    /* Controle gauche et droite */
    if ( ((gauche/largeur)%hauteur) == ((numeroMasque/largeur)%hauteur) && ( gauche >= 0) && (gauche <= nbTotal) ) // c'est la meme ligne
    {
        voisinsMasques << gauche;
    }
    if ( ((droite/largeur)%hauteur) == ((numeroMasque/largeur)%hauteur) && ( droite >= 0) && (droite <= nbTotal)) // c'est la meme ligne
    {
        voisinsMasques << droite;
    }

    /* Controle haut & bas */
    if (haut >= 0 && haut <= nbTotal)
        voisinsMasques << haut;
    if (bas >= 0 && bas <= nbTotal)
        voisinsMasques << bas;

    return voisinsMasques;
}

bool AbulEduTools::supprimerDir(const QString &dirPath)
{
    QDir folder(dirPath);
    /* On va lister dans ce répertoire tous les éléments différents de "." et ".." */
    folder.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    foreach(const QFileInfo fileInfo, folder.entryInfoList())
    {
        /* Si l'élément est un répertoire, on applique la méthode courante à ce répertoire, c'est un appel récursif */
        if(fileInfo.isDir())
        {
            if(!supprimerDir(fileInfo.filePath())) //Si une erreur survient, on retourne false
                return false;
        }
        /* Si l'élément est un fichier, on le supprime */
        else if(fileInfo.isFile())
        {
            if(!QFile::remove(fileInfo.filePath()))
            {
                /* Si une erreur survient, on retourne false */
                return false;
            }
        }
    }
    /* Le dossier est maintenant vide, on le supprime */
    if(!folder.rmdir(dirPath))
    {
        /* Si une erreur survient, on retourne false */
        return false;
    }
    /* Sinon, on retourne true */
    return true;
}

QStringList AbulEduTools::parcoursRecursif(const QString& dossier)
{
    QStringList resultat;
    QDir dir(dossier);
    /* Attention a ne pas prendre le repertoire "." et ".." */
    foreach(const QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        if(fileInfo.isDir()) {
            /* C'est ici que le parcours est récursif */
            resultat << parcoursRecursif(fileInfo.absoluteFilePath());
        }
        else {
            resultat << fileInfo.absoluteFilePath();
        }
    }
    return resultat;
}

QPair<int, int> AbulEduTools::plusPetiteDivision(const int &monChiffre)
{
    QList<QPair<int,int> > listeResultat;
    QPair<int,int> paire;

    for (int a = 2; a<10; a++){
        for (int b=2; b<10; b++)
        {
            if ((a*b) > monChiffre)
            {
                if (listeResultat.isEmpty())
                {
                    listeResultat << QPair<int,int>(a,b);
                }
                else
                {
                    if (a*b < listeResultat.first().first*listeResultat.first().second)
                    {
                        listeResultat.clear();
                        listeResultat << QPair<int,int>(a,b);
                    }
                    else
                    {
                        if(a*b == listeResultat.first().first*listeResultat.first().second)
                        {
                            listeResultat << QPair<int,int>(a,b);
                        }
                    }
                }
            }
        }
    }
    int mini = monChiffre;
    for(int k = 0 ; k<listeResultat.size();k++)
    {
        if(qAbs((listeResultat[k].first - listeResultat[k].second)) < mini)
        {
            mini = qAbs((listeResultat[k].first - listeResultat[k].second));
            paire = listeResultat[k];
        }
    }
    return paire;
}
