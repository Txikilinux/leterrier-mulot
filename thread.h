#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QDebug>
#include <QDirIterator>
#include <QStringListModel>

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QString cheminRecherche, QObject *parent = 0);
    
private:
    virtual void run();

    QString m_cheminRecherche;

signals:
    
public slots:
    void slotFinished();
};

#endif // THREAD_H
