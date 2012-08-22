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
#include "abuleduboxfilemanagerv1.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *);
    ~MainWindow();

public slots:
    void slotDemo();

private slots:
    /** Slot qui gère l'action de survol avec la souris */
    void on_action_Survol_triggered();

private:
    Ui::MainWindow *ui;
    AbulEduPageAccueilV1 *m_abuleduaccueil;
    QMap<int, QString> m_texteBulles;

    bool m_exerciceEnCours;
    bool m_localDebug;

    QSettings *m_config;

    AbulEduFileV1 *m_abuleduFile;
    AbulEduBoxFileManagerV1 *m_abuleduFileManager;
    QDir *m_tempDir;
    int m_numberExoCalled;
    void abeAiguillage();

private slots:
    void abeLanceExo(int numero);
    void exerciceExited();

    void on_actionEditeur_triggered();
    void on_action_Parcours_triggered();
    void on_actionClic_triggered();
    void on_action_Double_Clic_triggered();
    void on_action_Ouvrir_triggered();

    void btnBoxClicked();
    void slotOpenFile();
};

#endif // MAINWINDOW_H
