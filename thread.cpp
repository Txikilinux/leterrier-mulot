#include "thread.h"
#include "QImageReader"
#include "QBuffer"

Thread::Thread(QString cheminRecherche, QObject *parent, QStringList *listeFichiersPourItem) :
    QThread(parent)
{
    moveToThread(this); // Permet au thread ayant un pointeur sur celui-ci de communiquer =)
    //    moveToThread(QApplication::instance()->thread());
    m_cheminRecherche = cheminRecherche;
    m_rechercheTerminee = false;
    m_listeFichiersPourItem = listeFichiersPourItem;
}

void Thread::run()
{
    qDebug() << "Demarrage de la recherche d'images....";

    m_rechercheTerminee = false;

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
            /** Signal qui peut être attrapé depuis l'éditeur (test ne sert pas encore)
              * Permet de passer le chemin du fichier et son nom
              */
            emit signalFichierTrouve(it.filePath(), it.fileName());

            /** Creation QImage */
            int length = 128;
            QImageReader image_reader(it.filePath());
            int image_width = image_reader.size().width();
            int image_height = image_reader.size().height();
            if (image_width > image_height) {
                image_height = static_cast<double>(length) / image_width * image_height;
                image_width = length;
            } else if (image_width < image_height) {
                image_width = static_cast<double>(length) / image_height * image_width;
                image_height = length;
            } else {
                image_width = length;
                image_height = length;
            }
            image_reader.setScaledSize(QSize(image_width, image_height));
            QImage thumbnail = image_reader.read();

            /** Sauvegarde */
            QDir::setCurrent("/home/utilisateurs/icham.sirat/Bureau/thumbs");
            QFile file;
            file.setFileName(it.fileName());
            file.open(QIODevice::WriteOnly);
            if(thumbnail.save(&file))
                qDebug() << "Ecriture du fichier OK";


            //            QDir::setCurrent("/home");
            //            file.open(QIODevice::ReadOnly);      // opens "/home/readme.txt" under Unix


            //            QByteArray ba;
            //            QBuffer buffer(&ba);
            //            buffer.open(QIODevice::WriteOnly);
            //            if( thumbnail.save(&buffer , "PNG")) // writes image into ba in PNG format
            //                qDebug() << "ECRITURE OK";



            //            QFile fichierThumbs();
            //            fichierThumbs().setFileName();

            //                    if(thumbnail.save(it.fileName()))
            //                    qDebug() << "Sauvegarde OK";
            //            else
            //            qDebug() << "PAs Sauvegarde";
        }
    }
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
