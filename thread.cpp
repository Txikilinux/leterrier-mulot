#include "thread.h"

Thread::Thread(QString cheminRecherche, QObject *parent) :
    QThread(parent)
{
    m_cheminRecherche = cheminRecherche;
}

void Thread::run()
{
    qDebug() << "Demarrage de la recherche d'images....";

    QStringList listeFichiers;

    QString sPath = m_cheminRecherche;  //"/"; /*QDir::homePath()*/;
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
            listeFichiers << it.filePath();

//            QListWidgetItem *item = new QListWidgetItem();

//            QIcon icone(it.filePath()/* + QDir::separator() + it.fileName()*/);//pour la mettre  à coté de l'item
//            item->setIcon(icone); // ajout de la petite icone sur l'item
//            item->setText(it.fileName());
//            item->setData(4, it.filePath() + QDir::separator() + it.fileName());
//            //          ui->listWidgetImagesSelection->insertItem(0, item);
//            listWidget->insertItem(0, item);
        }
    }
}

void Thread::slotFinished()
{
    qDebug() << "Thread fini";

}
