#include "thread.h"


Thread::Thread(QString cheminRecherche, QObject *parent, int mode , QStringList *listeFichiersPourItem) :
    QThread(parent)
{
    moveToThread(this); // Permet au thread ayant un pointeur sur celui-ci de communiquer =)
    //    moveToThread(QApplication::instance()->thread());
    m_cheminRecherche = cheminRecherche;
    //    m_listeFichiers.clear();
    m_rechercheTerminee = false;
    m_mode = mode;
//    m_listeWidget = listeWidget ;
    m_listeFichiersPourItem = listeFichiersPourItem;
}

void Thread::run()
{
    qDebug() << "Demarrage de la recherche d'images....";

    m_rechercheTerminee = false;

    if (m_mode == 0)
    {
        QString sPath = m_cheminRecherche;
        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif";
        QDirIterator it(sPath,nameFilters,QDir::NoDotAndDotDot | QDir::AllEntries, QDirIterator::Subdirectories);
        int i = 0;
        while(it.hasNext())
        {
            i++;
            it.next();
            qDebug() << "Parcours de  : " << i << " "<< it.filePath() ;

            if(!it.fileInfo().isDir())
            {
                m_listeFichiers << it.filePath();
            }
        }
    }
    else if(m_mode == 1)
    {

        qDebug() << "Lancement thread de remplissade mode 1";
//        qDebug() << m_listeFichiersPourItem->count();
//        for (int i = 0; i < m_listeFichiersPourItem->count(); i++)
//        {
//            QListWidgetItem *item = new QListWidgetItem();
//            QIcon icone(m_listeFichiersPourItem->at(i));//pour la mettre  à coté de l'item
//            item->setIcon(icone); // ajout de la petite icone sur l'item
//            item->setText(m_listeFichiersPourItem->at(i));
//    //        item->setData(4, destImage->absolutePath() + QDir::separator() + monFichier.fileName());
////            m_listeWidgetItem << item;

        }
        qDebug() << "Remplissage widget terminé";


//    }
}

bool Thread::isRechercheTerminee()
{
    return m_rechercheTerminee;
}

void Thread::slotFinished()
{
    qDebug() << "Thread fini";
    m_rechercheTerminee = true;
}

QStringList Thread::getListeFichiers()
{
    return m_listeFichiers;
}
