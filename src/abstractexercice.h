/** Classe AbstractExercice
  * @file AbstractExercice.h
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

#ifndef ABSTRACTEXERCICE_H
#define ABSTRACTEXERCICE_H

#include <QObject>
#include <QDebug>

#include "abuleducommonstatesv1.h"
#include "masquedeplacesouris.h"
#include "abuledutools.h"

class AbstractExercice : public AbulEduCommonStatesV1
{
    Q_OBJECT

public:
    enum ExerciceType {
        Survol      = 0x0,
        Clic        = 0x2,
        DoubleClic  = 0x4,
        Parcours    = 0x8
    };

    AbstractExercice(QWidget *parent, const QString& theme, const ExerciceType& exerciceType);
    ~AbstractExercice() = 0;

    /** Permet de définir le localDebug qu'une fois */
    bool debugAvailable() const{return _localDebug;}

    QWidget* parent() const {return _parent;}

    /* ***************************************************************************************************
                            GETTERS & SETTERS
      ****************************************************************************************************/

    AbulEduMessageBoxV1* _messageBox;

    /** Retourne un pointeur sur le timer */
    QTimer* timer() {return _timer;}

    bool isExerciceRunning() const{return _exerciceEnCours ;}
    void setExerciceRunning(const bool& yesNo){ _exerciceEnCours = yesNo;}

    bool pauseAvailable() const{ return _onPeutMettreEnPause ;}
    void setPauseAvailable(const bool& yesNo){ _onPeutMettreEnPause = yesNo ;}

    QGraphicsPixmapItem* itemImage() const{return _itemImage;}

    int nbMasquesInteractifs() {return _nbMasquesInteractifs;}
    void setNbMasquesInteractifs(const int& nb){ _nbMasquesInteractifs = nb;}

    QSettings* parametres() const{return _parametres;}

    int OPT_nbMasquesChoisis() const{ return _OPT_nbMasquesChoisis;}
    int OPT_nbMasquesLargeur() const{ return _OPT_nbMasquesLargeur;}
    int OPT_nbMasquesHauteur() const{ return _OPT_nbMasquesHauteur;}

    QList<MasqueDeplaceSouris*> listeMasquesFixes() const{return _listeMasquesFixes ;}

    QTime* chronometre() const{return _chronometre;}
    void setTempsQuestion1(const float& tps){ _tempsQuestion1 = tps ;}
    float tempsQuestion1() {return _tempsQuestion1;}
    void setTempsQuestion2(const float& tps){ _tempsQuestion2 = tps ;}
    float tempsQuestion2() {return _tempsQuestion2;}
    void setTempsQuestion3(const float& tps){ _tempsQuestion3 = tps ;}
    float tempsQuestion3() {return _tempsQuestion3;}
    void setTempsQuestion4(const float& tps){ _tempsQuestion4 = tps ;}
    float tempsQuestion4() {return _tempsQuestion4;}
    void setTempsQuestion5(const float& tps){ _tempsQuestion5 = tps ;}
    float tempsQuestion5() {return _tempsQuestion5;}

    /// @todo le reste du temps et meilleure gestion Parcours

public slots:
    /** Méthode qui filtre les évènement
      * @param QObject *obj, un pointeur sur l'objet auteur de l'évènement
      * @param QEvent *ev  , un pointeur sur l'évènement filtré
      * @brief Capture l'appui sur la barre espace lorsque le booléen "onPeutMettreEnPause" est à "true".
      *        C'est le cas lorsque tous les masques sont découvert.
      */
    bool eventFilter(QObject *obj, QEvent *ev);

    void slotFermetureAide();

private slots:
    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotSequenceEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotRealisationExerciceEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotInitQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotFinQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotFinVerificationQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotAfficheVerificationQuestionEntered();

    /** Méthode héritée @see AbulEduCommonStatesV1 */
    void slotBilanExerciceEntered();

    void slotBilanSequenceEntered();

    /** Méthode qui redimmensionne tous les éléments graphiques les uns par rapport aux autres */
    void setDimensionsWidgets();

    /** Méthode appelée à chaque interaction avec un masque interactif */
    virtual void slotCacheMasque();

    /** Redimensionne la consigne */
    void redimensionnerConsigne();

    /** Méthode qui emet le signal appuiSuivant */
    void slotAppuiAutoSuivant();

    /** Cette méthode emet le signal appuiVerifier */
    void slotAppuiAutoVerifier();

    void pause();

protected:
    inline int verifieReponse(){return 0;}
    inline int donneReponse() {return -1;}

signals:
    void exerciceExited();
    void appuiSuivant();
    void appuiVerifier();

private:

    bool _localDebug;
    bool _exerciceEnCours;

    QString         _theme;
    QWidget         *_parent;
    ExerciceType    _exerciceType;

    QGraphicsView           *_aireTravail;
    QGraphicsProxyWidget    *_proxyGraphique;
    QSize                   _tailleAireTravail;

    /* Paramètres Editeur */
    QSettings   *_parametres;
    int         _OPT_timerSuivant;
    int         _OPT_nbMasquesChoisis;
    float       _OPT_nbMasquesLargeur;
    float       _OPT_nbMasquesHauteur;
    QString     _cheminConf;
    QString     _cheminImage;

    QPixmap                 _image;
    QGraphicsPixmapItem     *_itemImage;
    /* ma liste d'images (choisies aleatoirement) */
    QList<QPixmap>          _listeImage;
    /* le nombre d'image = le nombre d'exercice */
    int                     _nbImage;
    /* Le nombre de masques à interaction */
    int                     _nbMasquesInteractifs;
    /* la liste des fichiers présents dans le dossier pack image */
    QStringList             _listeFichiers;


    QList<MasqueDeplaceSouris *>     _listeMasquesFixes;
    MasqueDeplaceSouris             *_masque;
    MasqueDeplaceSouris             *_masqueInteractif;
    QSize                           _taille;

    /* Chronometre */
    QTime *_chronometre;
    float _tempsQuestion1;
    float _tempsQuestion2;
    float _tempsQuestion3;
    float _tempsQuestion4;
    float _tempsQuestion5;
    int   _tempsTotal;

    QTimer  *_timer;
    bool    _onPeutMettreEnPause;
    QLabel  *_labelImagePause;
    QLabel  *_labelTextePause;

    /** Redimensionne l'image */
    void redimensionnerImage();

    /** Charge les options contenues dans le fichier de configuration (parametres.ini) */
    void chargerOption();

    /** Touche clavier (espace) */
    QKeyEvent *_keySpace;
};

#endif // ABSTRACTEXERCICE_H
