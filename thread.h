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
    explicit Thread(QString cheminRecherche = 0, QObject *parent = 0, int mode = 0, QStringList *listeFichiersPourItem = 0);

    QStringList getListeFichiers();
    bool isRechercheTerminee();

private:
    virtual void run();

    QString m_cheminRecherche;
    QStringList m_listeFichiers;

    bool m_rechercheTerminee;
    int m_mode;
    QStringList *m_listeFichiersPourItem;
    QList<QListWidgetItem> m_listeWidgetItem;

signals:
    
public slots:
    void slotFinished();
};

#endif // THREAD_H
