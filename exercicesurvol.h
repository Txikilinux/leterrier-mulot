/** Classe ExerciceSurvol
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
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

#ifndef EXERCICESURVOL_H
#define EXERCICESURVOL_H

#include <QGraphicsProxyWidget>
#include <QMessageBox>
#include <QtCore/qmath.h>
#include "abuleduetiquettesv1.h"
#include "abuleducommonstatesv1.h"
#include "masquedeplacesouris.h"
#include "abuledutools.h"

class ExerciceSurvol : public AbulEduCommonStatesV1
{
    Q_OBJECT

public:
    /** Constructeur de la classe ExerciceSurvol
      * @param QWidget *parent = 0, le parent de cet objet, instancié par défaut à 0 (aucun objet)
      * @param QString theme = "" , le chemin vers le thème utilisé (abe), instancié à vide ("")
      */
    ExerciceSurvol(QWidget *parent = 0, QString theme ="");

    /** Destructeur de la classe ExerciceDoubleClic */
    ~ExerciceSurvol();

protected:
    /** Fonction qui doit être implémentée pour cause d'héritage. Ici pas de vraie vérification, et pas d'erreur possible
      * @todo créer une évaluation de l'activité, pourquoi pas en fonction du temps mis */
    inline int verifieReponse(){return 0;}

    /** Fonction qui doit être implémentée pour cause d'héritage. Ici pas de bonne réponse à afficher */
    inline int donneReponse() {return -1;}

private:

    AbulEduEtiquettesV1 *gv_AireDeJeu;
    QGraphicsProxyWidget *proxy;
    QWidget *m_parent;
    bool m_localDebug;
    bool m_exerciceEnCours;

    // Paramètres Editeur
    QSettings *m_parametres;
    int opt_timerSuivant;
    int opt_nbMasquesChoisis;
    QString m_theme;
    QString cheminConf;
    QString cheminImage;

    QPixmap m_image;
    QGraphicsPixmapItem *m_itemImage;
    QList<QPixmap> m_listeImage;        // ma liste d'images (choisies aleatoirement)
    int m_nbImage;                      // le nombre d'image = le nombre d'exercice
    int m_nbMasquesInteractifs;         // = à 7
    QStringList m_listeFichiers;        // la liste des fichiers présents dans le dossier pack image

    float nbMasquesLargeur;
    float nbMasquesHauteur;

    QList<MasqueDeplaceSouris *> m_listeMasquesFixes;
        MasqueDeplaceSouris *m_masque;
        MasqueDeplaceSouris *m_masqueInteractif;
        QSize m_taille;

    // Chronometre
    QTime *m_chronometre;
    float m_tempsQuestion1;
    float m_tempsQuestion2;
    float m_tempsQuestion3;
    float m_tempsQuestion4;
    float m_tempsQuestion5;
    int m_tempsTotal;

    QTimer *m_timer;
    bool onPeutMettreEnPause;
    QLabel *m_labelImagePause;
    QLabel *m_labelTextePause;

    // jlf 2012/09/29
    // Dimensions de l'aire de jeu
    QSize m_tailleAireDejeu;

    AbulEduMessageBoxV1* m_messageBox;
    QKeyEvent *keySpace; /** C'est la touche espace, pour la pause */

    /** Redimensionne la consigne */
    void redimensionnerConsigne();

    /** Redimensionne l'image par rapport à sa largeur ou sa hauteur.
      * @obsolete mais conserver au cas où
      */
    void redimensionnerImage();

    /** Redimensionne l'image (2e méthode) */
    void redimensionnerImage2();

    /** Charge les options contenues dans le fichier de configuration (parametres.ini) */
    void chargerOption();

    /** Méthode qui filtre les évènement
      * @param QObject *obj, un pointeur sur l'objet auteur de l'évènement
      * @param QEvent *ev  , un pointeur sur l'évènement filtré
      * @brief Capture l'appui sur la barre espace lorsque le booléen "onPeutMettreEnPause" est à "true".
      *        C'est le cas lorsque tous les masques sont découvert.
      */
    bool eventFilter(QObject *obj, QEvent *ev);

    void pause();

private slots:
    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotSequenceEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1
      * @brief Affichage de l'aide
      */
    void slotAide();

    /** Méthode appelée à la fermeture de l'aire
      * @see AbulEduMessageBoxV1::signalAbeMessageBoxCloseOrHide()
      */
    void slotFermetureAide();

    /** Méthode héritée @see AbulEduCommonStatesV1
      * @brief Mettre tout ce qui est commun à chaque question
      *        Aller chercher le pack image
      *        Choisir 5 images au hasard dans le pack
      *        Condition de garde .abe
      */
    void slotRealisationExerciceEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1
      * @brief Affichage de l'image
      *        Calcul et mise en place des masques
      */
    void slotInitQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1
      * @brief Choix aléatoire du positionnement des masques interactifs
      *        Connexion du slot cacheMasque sur chaque masque interactif
      */
    void slotQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotFinQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotFinVerificationQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1
      * @brief Appeler pour appuyer automatiquement sur le bouton suivant
      */
    void slotAfficheVerificationQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1
      * @todo mettre en place une boucle pour l'affichage des tetes en fonct° des résultats
      */
    void slotBilanExerciceEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotQuitter();

    /** Méthode qui redimmensionne tous les éléments graphiques les uns par rapport aux autres */
    void setDimensionsWidgets();

    /** Méthode appelée à chaque interaction avec un masque interactif
      * @brief A chaque passage sur un masque interactif, on décremente le nombre de masques interactifs.
      *        Dès que les masques interactifs sont tous cliqués, on affiche l'image.
      *        On affiche la tête dans la boiteTete.
      *        On vide m_listeMasquesFixes
      */
    void slotCacheMasque();

    /** Méthode qui emet le signal appuiSuivant
      * @brief Permet donc d'activer (de simuler) l'appui sur le bouton suivant de la telecommande
      */
    void slotAppuiAutoSuivant();

    /** Cette méthode emet le signal appuiVerifier
      * @brief Permet donc d'activer (de simuler) l'appui sur le bouton verifier de la telecommande
      */
    void slotAppuiAutoVerifier();

signals:
    void exerciceExited();
    void appuiSuivant();
    void appuiVerifier();
};

#endif // EXERCICESURVOL_H
