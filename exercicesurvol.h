#ifndef EXERCICESURVOL_H
#define EXERCICESURVOL_H

#include "abuleduetiquettesv1.h"
#include "abuleducommonstatesv1.h"

class ExerciceSurvol : public AbulEduCommonStatesV1
{
    Q_OBJECT

public:
    ExerciceSurvol(QWidget *parent = 0);
    ~ExerciceSurvol();

private:
    //mon AbulEduEtiquettes afin d'avoir une graphicView et tt ce qui va avec
    AbulEduEtiquettesV1 *gv_AireDeJeu;

    QWidget *m_parent;

signals:
    // Ce slot sert à abeLanceExo -> il redefinit exerciceEnCourt à false..etc
    void exerciceExited();

};

#endif // EXERCICESURVOL_H
