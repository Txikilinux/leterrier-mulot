#ifndef EXERCICESURVOL_H
#define EXERCICESURVOL_H

#include <QGraphicsProxyWidget>
#include "abuleduetiquettesv1.h"
#include "abuleducommonstatesv1.h"
//#include "widgetdeplacesouris.h"

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
//    QGraphicsItem *item;
    QPixmap image;

    QGraphicsPixmapItem *itemImage;

    QList<QPixmap> listeImage; // ma liste d'images (choisies aleatoirement)

    int m_nbImage; // le nombre d'image = le nombre d'exercice
    int m_nbTotalMasques; // = à 7
    QStringList m_ListeFichiers; // la liste des fichiers présents dans le dossier pack image

    QWidget *m_parent;
    bool m_localDebug;

    void redimensionnerConsigne();
    void redimensionnerImage();

private slots:
    void slotSequenceEntered();
    void slotPresenteSequenceEntered();
//    void slotPresentationExerciceEntered();
    void slotRealisationExerciceEntered();
    void slotInitQuestionEntered();
    void slotQuestionEntered();
//    void slotVerificationQuestionEntered();
//    void slotBilanExerciceEntered();
//    void slotBilanSequenceEntered();
//    void slotBilanSequenceExited();
    void slotQuitter();

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void setDimensionsWidgets();

signals:
    // Ce slot sert à abeLanceExo -> il redefinit exerciceEnCourt à false..etc
    void exerciceExited();

};

#endif // EXERCICESURVOL_H
