#ifndef EXERCICEABSTRACT_H
#define EXERCICEABSTRACT_H

#include <QObject>
#include <QDebug>

#include "abuleducommonstatesv1.h"
#include "abuleduetiquettesv1.h"
#include "masquedeplacesouris.h"
#include "abuledutools.h"

class ExerciceAbstract : public AbulEduCommonStatesV1
{
    Q_OBJECT

public:
    enum ExerciceType {
        Survol      = 0x0,
        Clic        = 0x2,
        DoubleClic  = 0x4,
        Parcours    = 0x8
    };

    ExerciceAbstract(QWidget *parent, const QString& theme, const ExerciceType& exerciceType);
    ~ExerciceAbstract() = 0;

    /** Permet de définir le localDebug qu'une fois */
    bool debugAvailable() const{return _localDebug;}

    QWidget* parent() const {return _parent;}


    /** Retourne un pointeur sur l'AbulEduMessageBoxV1 commune à tous les exercices */
    AbulEduMessageBoxV1* messageBox(){return _messageBox;}
    AbulEduMessageBoxV1* _messageBox; //! @todo A ameliorer pas satisfaisant point de vue implementation

    QTimer* timer() {return _timer;}

    bool isExerciceRunnning() const{return _exerciceEnCours ;}

    QGraphicsPixmapItem* itemImage() const{return _itemImage;}

    int nbMasquesInteractifs() {return _nbMasquesInteractifs;}
    void setNbMasquesInteractifs(const int& nb){ _nbMasquesInteractifs = nb;}

    QSettings* parametres() const{return _parametres;}

    int OPT_nbMasquesChoisis() const{ return _OPT_nbMasquesChoisis;}
    int OPT_nbMasquesLargeur() const{ return _OPT_nbMasquesLargeur;}
    int OPT_nbMasquesHauteur() const{ return _OPT_nbMasquesHauteur;}



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

    /** Méthode appelée à chaque interaction avec un masque interactif */
    void slotCacheMasque();

    /** Méthode qui redimmensionne tous les éléments graphiques les uns par rapport aux autres */
    void setDimensionsWidgets();

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

    AbulEduEtiquettesV1     *_aireTravail;
    QGraphicsProxyWidget    *_proxyGraphique;
    QSize                   _tailleAireTravail;

    // Paramètres Editeur
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

    // Chronometre
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

    /* C'est la touche espace, pour la pause */
    QKeyEvent *keySpace;

};

Q_DECLARE_INTERFACE(ExerciceAbstract, "ExerciceAbstract/1.0")
#endif // EXERCICEABSTRACT_H
