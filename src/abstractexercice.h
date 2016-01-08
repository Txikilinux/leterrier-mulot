/** Classe AbstractExercice
  * @file AbstractExercice.h
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

#ifndef ABSTRACTEXERCICE_H
#define ABSTRACTEXERCICE_H

#include <QObject>
#include <QDebug>

#include "abuleducommonstatesv1.h"
#include "masquedeplacesouris.h"
#include "abuledutools.h"
#include "abuledumessageboxv1.h"

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

    QWidget* parent() const {return m_parent;}

    /* ***************************************************************************************************
                            GETTERS & SETTERS
      ****************************************************************************************************/

    AbulEduMessageBoxV1* m_messageBox;

    /** Retourne un pointeur sur le timer */
    QTimer* timer() {return m_timer;}

    bool isExerciceRunning() const{return m_exerciceEnCours ;}
    void setExerciceRunning(const bool& yesNo){ m_exerciceEnCours = yesNo;}

    bool pauseAvailable() const{ return m_onPeutMettreEnPause ;}
    void setPauseAvailable(const bool& yesNo){ m_onPeutMettreEnPause = yesNo ;}

    QGraphicsPixmapItem* itemImage() const{return m_itemImage;}

    int nbMasquesInteractifs() {return m_nbMasquesInteractifs;}
    void setNbMasquesInteractifs(const int& nb){ m_nbMasquesInteractifs = nb;}

    QSettings* parametres() const{return m_parametres;}

    int OPT_nbMasquesChoisis() const{ return m_OPT_nbMasquesChoisis;}
    int OPT_nbMasquesLargeur() const{ return m_OPT_nbMasquesLargeur;}
    int OPT_nbMasquesHauteur() const{ return m_OPT_nbMasquesHauteur;}

    QList<MasqueDeplaceSouris*> listeMasquesFixes() const{return m_listeMasquesFixes ;}

    QTime* chronometre() const{return m_chronometre;}
    void setTempsQuestion1(const float& tps){ m_tempsQuestion1 = tps ;}
    float tempsQuestion1() {return m_tempsQuestion1;}
    void setTempsQuestion2(const float& tps){ m_tempsQuestion2 = tps ;}
    float tempsQuestion2() {return m_tempsQuestion2;}
    void setTempsQuestion3(const float& tps){ m_tempsQuestion3 = tps ;}
    float tempsQuestion3() {return m_tempsQuestion3;}
    void setTempsQuestion4(const float& tps){ m_tempsQuestion4 = tps ;}
    float tempsQuestion4() {return m_tempsQuestion4;}
    void setTempsQuestion5(const float& tps){ m_tempsQuestion5 = tps ;}
    float tempsQuestion5() {return m_tempsQuestion5;}

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
    int verifieReponse();
    inline int donneReponse() {return -1;}

signals:
    void exerciceExited();
    void appuiSuivant();
    void appuiVerifier();

private:

    bool m_exerciceEnCours;

    QString         m_theme;
    QWidget         *m_parent;
    ExerciceType    m_exerciceType;

    QGraphicsView           *m_aireTravail;
    QGraphicsProxyWidget    *m_proxyGraphique;
    QSize                   m_tailleAireTravail;

    /* Paramètres Editeur */
    QSettings   *m_parametres;
    int         m_OPT_timerSuivant;
    int         m_OPT_nbMasquesChoisis;
    float       m_OPT_nbMasquesLargeur;
    float       m_OPT_nbMasquesHauteur;
    QString     m_cheminConf;
    QString     m_cheminImage;

    QPixmap                 m_image;
    QGraphicsPixmapItem     *m_itemImage;
    /* ma liste d'images (choisies aleatoirement) */
    QList<QPixmap>          m_listeImage;
    /* le nombre d'image = le nombre d'exercice */
    int                     m_nbImage;
    /* Le nombre de masques à interaction */
    int                     m_nbMasquesInteractifs;
    /* la liste des fichiers présents dans le dossier pack image */
    QStringList             m_listeFichiers;

    QList<MasqueDeplaceSouris *>     m_listeMasquesFixes;
    MasqueDeplaceSouris             *m_masque;
    MasqueDeplaceSouris             *m_masqueInteractif;
    QSize                           m_taille;

    /* Chronometre */
    QTime *m_chronometre;
    float m_tempsQuestion1;
    float m_tempsQuestion2;
    float m_tempsQuestion3;
    float m_tempsQuestion4;
    float m_tempsQuestion5;
    int   m_tempsTotal;

    QTimer  *m_timer;
    bool    m_onPeutMettreEnPause;
    QLabel  *m_labelImagePause;

    /** True si on est en pause, false sinon */
    bool m_isPaused;

    /** Touche clavier (espace) */
    QKeyEvent *m_keySpace;

    QLabel *m_labelTitreImage;

    /** Redimensionne l'image */
    void redimensionnerImage();

    /** Charge les options contenues dans le fichier de configuration (parametres.ini) */
    void chargerOption();

    QPixmap *m_pixmapPause;

    const QString getTitleFromDataPicture(const QString& pictureName);

};

#endif // ABSTRACTEXERCICE_H
