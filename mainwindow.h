/** Classe MainWindow
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDesktopWidget>
#include <QMainWindow>

#include "abuledupageaccueilv1.h"
#include "abuledufilev1.h"
#include "abuleduapplicationv1.h"
#include "abuleduboxfilemanagerv1.h"
#include "activityfilter.h"
#include "version.h"
#include "lib/abuleduaproposv1/abuleduaproposv1.h"
#include "ui_mainwindow.h"
#include "exercicesurvol.h"
#include "exerciceparcours.h"
#include "exerciceclic.h"
#include "exercicedoubleclic.h"
#include "abuledusplashscreenv1.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /** Constructeur de la classe MainWindow
      * @param QWidget *parent = 0, le parent de cet objet, initialisé par défaut à 0 (aucun)
      */
    explicit MainWindow(QWidget *parent = 0);

    /** Destructeur de la classe MainWindow */
    ~MainWindow();

    /** Méthode appelée à chaque redimensionnement de l'objet MainWindow
      * @param QResizeEvent *, un pointeur sur l'évènement de redimensionnement
      */
    void resizeEvent(QResizeEvent *);

    /** Méthode qui définit l'abe courant
      * @param AbulEduFileV1* abeFile, l'objet abulEduFile
      */
    void abeSetMyAbulEduFile(QSharedPointer<AbulEduFileV1> abeFile);

    /** Méthode qui retourne l'abe courant
      * @return AbulEduFileV1 *, un pointeur sur l'objet abuledufile
      */
    QSharedPointer<AbulEduFileV1> abeGetMyAbulEduFile();

    /** Permet d'accéder à la page d'accueil */
    AbulEduPageAccueilV1* abeGetMyAbulEduAccueil();

public slots:
    /** Permet d'afficher les zones et les bulles lorsque l'utilisateur est inactif */
    void slotDemo();

    /** Permet de desactiver l'affichage des zones et des bulles
      * @brief Permet de cacher les zones et les bulles après l'affichage par le slotDemo()
      */
    void slotFinDemo();

    /** Méthode qui gère l'authentification de la session
      * @param bool enable, true si authentifié, false sino,
      */
    void slotSessionAuthenticated(bool enable);

private slots:
    /** Méthode qui gère l'action de survol avec la souris */
    void on_action_Survol_triggered();

    /** Méthode qui gère le titre de la fenetre
      * @param int authStatus, permet si l'authentification est ok, d'afficher le login dans le titre de la fenetre
      */
    void setTitle(int authStatus);
    void debutTestParametres();
    void afficheEditeur();
    void afficheFrPrincipale();
    void afficheBoxFileManager(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerOpenOrSave);

private:
    Ui::MainWindow *ui;
    AbulEduPageAccueilV1* m_abuleduaccueil;
    QMap<int, QString> m_texteBulles;

    bool m_exerciceEnCours;
    bool m_localDebug;
    bool m_isDemoAvailable;

    QSharedPointer<AbulEduFileV1> m_abuleduFile;
    AbulEduBoxFileManagerV1 *m_abuleduFileManager;
    int m_numberExoCalled;
    void abeAiguillage();

    QTranslator qtTranslator;
    QTranslator myappTranslator;

    /** Méthode qui créée le menu des langues
      * @brief Francais et Anglais pour l'instant
      */
    void creeMenuLangue();

protected:
    /** Méthode qui gère les changement d'évènement
      * @param QEvent *e, un pointeur sur l'évènement
      * @brief ne sert pour l'instant qu'à la translation languistique du programme
      */
    virtual void changeEvent(QEvent *e);

private slots:
    /** Méthode qui gère le changement des langues */
    void slotChangeLangue();

    /** Méthode qui lance les exercices en fonction du numéro
      * @param int numero, le numero de l'exercice
      */
    void abeLanceExo(int numero);

    /** Méthode qui gère le déroulement de fin d'un exercice */
    void exerciceExited();

    /** Méthode appelée lors de l'appui sur le menu Editeur */
    void on_actionEditeur_triggered();

    /** Méthode appelée lors de l'appui sur le menu Exercice, Parcours */
    void on_action_Parcours_triggered();

    /** Méthode appelée lors de l'appui sur le menu Exercice, Clic */
    void on_actionClic_triggered();

    /** Méthode appelée lors de l'appui sur le menu Exercice, Double-Clic */
    void on_action_Double_Clic_triggered();

    /** Méthode appelée lors de l'appui sur le menu Ouvrir un Exercice */
    void on_actionOuvrir_un_exercice_triggered();

    /** Méthode appelée lors de l'appui sur le menu Box */
    void btnBoxClicked();

    /** Méthode appelée lors du choix d'un abe @see AbulEduBoxFileManagerV1 */
    void slotOpenFile(QSharedPointer<AbulEduFileV1> qsp_AbulEduFileV1);

    /** Méthode qui gère le mode démo
      * @param bool checked, true mode demo, false mode normal
      */
    void on_actionMode_D_mo_triggered(bool checked);

    /** Méthode appelée lors de l'appui sur le menu Journal */
    void on_action_Journal_de_mes_activit_s_triggered();

    /** Méthode appelée lors de l'appui sur le menu Changer Utilisateur */
    void on_action_Changer_d_utilisateur_triggered();
};

#endif // MAINWINDOW_H
