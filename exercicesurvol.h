#ifndef EXERCICESURVOL_H
#define EXERCICESURVOL_H

#include <QGraphicsProxyWidget>
#include "abuleduetiquettesv1.h"
#include "abuleducommonstatesv1.h"
//#include "widgetdeplacesouris.h"
#include "masquedeplacesouris.h"



class ExerciceSurvol : public AbulEduCommonStatesV1
{
    Q_OBJECT

public:
    ExerciceSurvol(QWidget *parent = 0);
    ~ExerciceSurvol();

private:
    //mon AbulEduEtiquettes afin d'avoir une graphicView et tt ce qui va avec
    AbulEduEtiquettesV1 *gv_AireDeJeu;
    // Pour stocker AbulEduLabelWidget
    QGraphicsProxyWidget *proxy;

    // Pour l'exerciceSurvol
    QPixmap m_image;
    QGraphicsPixmapItem *m_itemImage;
    QList<QPixmap> m_listeImage; // ma liste d'images (choisies aleatoirement)
    int m_nbImage; // le nombre d'image = le nombre d'exercice
    int m_nbMasquesInteractifs; // = à 7
    QStringList m_listeFichiers; // la liste des fichiers présents dans le dossier pack image

    QList<masqueDeplaceSouris *> m_listeMasquesFixes;
    masqueDeplaceSouris *m_masque;
    masqueDeplaceSouris *m_masqueInteractif;
    QSize m_taille;
    QWidget *m_parent;
    bool m_localDebug;

    void redimensionnerConsigne();
    void redimensionnerImage();
    void redimensionnerImage2();

private slots:
    void slotSequenceEntered();
    void slotPresenteSequenceEntered();
//    void slotPresentationExerciceEntered();
    void slotRealisationExerciceEntered();
    void slotInitQuestionEntered();
    void slotQuestionEntered();
    void slotFinQuestionEntered();
    void slotFinVerificationQuestionEntered();
    void slotAfficheVerificationQuestionEntered();

//    void slotVerificationQuestionEntered();
//    void slotQuestionExited();
//    void slotBilanExerciceEntered();
//    void slotBilanSequenceEntered();
//    void slotBilanSequenceExited();
    void slotQuitter();

    void setDimensionsWidgets();

    // ExerciceSurvol
    void slotCacheMasque();
    void slotPassageAutoImageSuivante();

signals:
    // Ce slot sert à abeLanceExo -> il redefinit exerciceEnCourt à false..etc
    void exerciceExited();

};

#endif // EXERCICESURVOL_H
