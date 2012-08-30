/** Classe Thread
  * @see https://redmine.ryxeo.com/projects/
  * @author 2012 Icham Sirat <icham.sirat@ryxeo.com>
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GNU/GPL) v3
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

#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QDebug>
#include <QDirIterator>
#include <QStringListModel>
#include <QListWidget>

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QString cheminRecherche = 0, QObject *parent = 0, QStringList *listeFichiersPourItem = 0);

    QStringList getListeFichiers();
    bool isRechercheTerminee();

private:
    virtual void run();

    QString m_cheminRecherche;
    QStringList m_listeFichiers;

    bool m_rechercheTerminee;
    QStringList *m_listeFichiersPourItem;
    QList<QListWidgetItem> m_listeWidgetItem;

    void creerThumbs(QFileInfo cheminImage, QString cheminSauvegardeThumbs);

signals:
    void signalFichierTrouve(QString cheminFichier, QString nomFichier);

public slots:
    void slotFinished();
};

#endif // THREAD_H
