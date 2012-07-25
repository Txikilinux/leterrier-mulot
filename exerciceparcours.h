/** Classe ExerciceParcours
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

#ifndef EXERCICEPARCOURS_H
#define EXERCICEPARCOURS_H

#include <QGraphicsProxyWidget>
#include "abuleduetiquettesv1.h"
#include "abuleducommonstatesv1.h"
#include "masquedeplacesouris.h"

class ExerciceParcours : public AbulEduCommonStatesV1
{
    Q_OBJECT

public:
    ExerciceParcours(QWidget *parent = 0, QString theme ="");
    ~ExerciceParcours();

private:

    AbulEduEtiquettesV1 *gv_AireDeJeu;  // mon AbulEduEtiquettes afin d'avoir une graphicView et tt ce qui va avec
    QGraphicsProxyWidget *proxy;        // Pour stocker AbulEduLabelWidget
    QWidget *m_parent;
    bool m_localDebug;
    bool m_exerciceEnCours;

    // Paramètres Editeur
    QSettings *m_parametres;
    int opt_timerSuivant;
    int opt_timerVerifier;
    int opt_nbMasquesChoisis;
    float opt_nbMasquesLargeur;
    float opt_nbMasquesHauteur;

    QString m_theme;
    QString cheminConf;
    QString cheminImage;

    QPixmap m_image;
    QGraphicsPixmapItem *m_itemImage;
    QList<QPixmap> m_listeImage;        // ma liste d'images (choisies aleatoirement)
    int m_nbImage;                      // le nombre d'image = le nombre d'exercice
    int m_nbMasquesInteractifs;         // = à 7
    QStringList m_listeFichiers;        // la liste des fichiers présents dans le dossier pack image

//    QPair<int, int> plusPetiteDivision(int monChiffre); // Pour le choix du découpage de l'image par rapport au nb de masques interactifs choisis


    QList<masqueDeplaceSouris *> m_listeMasquesFixes;
        masqueDeplaceSouris *m_masqueDepart;
        masqueDeplaceSouris *m_masqueArrivee;
        masqueDeplaceSouris *m_masqueParcours;
        masqueDeplaceSouris *m_masque;
        QSize m_taille;

    // Chronometre
//    QTime *m_chronometre;
//        float m_tempsQuestion1;
//        float m_tempsQuestion2;
//        float m_tempsQuestion3;
//        float m_tempsQuestion4;
//        float m_tempsQuestion5;
//        int m_tempsTotal;

    QTimer *m_timer;
    bool onPeutMettreEnPause;
    QLabel *m_labelImagePause;
    QLabel *m_labelTextePause;

    void redimensionnerConsigne();
    void redimensionnerImage(); // obsolete mais conserver, on ne sait jamais
    void redimensionnerImage2();
    void chargerOption();
//    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    void slotSequenceEntered();
    void slotPresenteSequenceEntered();
    //    void slotPresentationExerciceEntered()
    void slotRealisationExerciceEntered();
    void slotInitQuestionEntered();
    void slotQuestionEntered();
//    void slotFinQuestionEntered();
//    void slotFinVerificationQuestionEntered();
//    void slotAfficheVerificationQuestionEntered();
    //    void slotVerificationQuestionEntered();
    //    void slotQuestionExited();
//    void slotBilanExerciceEntered();
    //    void slotBilanSequenceEntered();
    //    void slotBilanSequenceExited();
    void slotQuitter();

    void setDimensionsWidgets();

//    // ExerciceSurvol
//    void slotCacheMasque();
    void slotAppuiAutoSuivant();
    void slotAppuiAutoVerifier();

signals:
    // Ce slot sert à abeLanceExo -> il redefinit exerciceEnCourt à false..etc
    void exerciceExited();
    void appuiSuivant();
    void appuiVerifier();

};

#endif // EXERCICEPARCOURS_H
