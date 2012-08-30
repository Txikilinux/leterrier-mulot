#include "thread.h"
#include "QImageReader"
#include "QBuffer"
#include "QCryptographicHash"
#include "QDateTime"

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
            creerThumbs(it.filePath(), QDir::homePath() + "/.thumbnails/normal");
        }


    }

}


void Thread::creerThumbs(QFileInfo cheminImage, QString cheminSauvegardeThumbs)
{
    qDebug() << "CREATION THUMBS";
//    cheminSauvegardeThumbs = QDir::homePath() + "/.thumbnails/normal";
    qDebug() << cheminSauvegardeThumbs;
    // MD5
    // QFILEINFO DU MD5
    // SI il existe dans .thumb/normal
        // comparaison date
        // generation thumb ou pas
    /** Recuperation du MD5Sum */
    QByteArray nomFichier = QString("file://%1").arg(cheminImage.absoluteFilePath()).toAscii();
    QByteArray hashData = QCryptographicHash::hash(nomFichier,QCryptographicHash::Md5);
    qDebug() <<"nomFichier" << nomFichier;
    qDebug() <<"HASH "<< hashData.toHex() +"." +cheminImage.suffix();

    QFileInfo fComparaison(cheminSauvegardeThumbs + QDir::separator() + hashData.toHex() +"." +cheminImage.suffix());
    qDebug() << fComparaison.absoluteFilePath();

    if (fComparaison.exists())
    {
        qDebug() << trUtf8("Le thumbs existe déjà");
        // Si le fichier existe -> comparaison
        if (cheminImage.lastRead() > fComparaison.lastRead())
        {
            qDebug() << "Mon thumbs est trop vieux";

            // Recreation du thumbs
            /** Creation QImage */
            int length = 128; // choix de la taille
            QImageReader image_reader(cheminImage.absoluteFilePath());

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
            QDir::setCurrent(cheminSauvegardeThumbs);
            QFile file;
            file.setFileName(hashData.toHex() +"." +cheminImage.suffix());
            file.open(QIODevice::WriteOnly);
            if(thumbnail.save(&file))
                qDebug() << "Ecriture du fichier OK";
        }
        else
            qDebug() << "Le Thumbs est a jour";

    }
    else // Le fichier n'existe pas -> creation Thumbs
    {
        //generation du thumbs
        /** Creation QImage */
        int length = 128; // choix de la taille
        QImageReader image_reader(cheminImage.absoluteFilePath());

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
        QDir::setCurrent(cheminSauvegardeThumbs);
        QFile file;
        file.setFileName(hashData.toHex() +"." +cheminImage.suffix());
        file.open(QIODevice::WriteOnly);
        if(thumbnail.save(&file))
            qDebug() << "Ecriture du fichier OK";

    }

    // chercher si m_thumbs existe dans cheminSauvegardeThumbs





//    /** Creation QImage */
//    int length = 128; // choix de la taille
//    QImageReader image_reader(cheminImage.absoluteFilePath());

//    int image_width = image_reader.size().width();
//    int image_height = image_reader.size().height();
//    if (image_width > image_height) {
//        image_height = static_cast<double>(length) / image_width * image_height;
//        image_width = length;
//    } else if (image_width < image_height) {
//        image_width = static_cast<double>(length) / image_height * image_width;
//        image_height = length;
//    } else {
//        image_width = length;
//        image_height = length;
//    }

//    image_reader.setScaledSize(QSize(image_width, image_height));
//    QImage thumbnail = image_reader.read();

    /** MD5Sum */
//    QByteArray nomFichier = QString("file://%1").arg(cheminImage.absoluteFilePath()).toAscii();
//    qDebug() <<"nomFichier" << nomFichier;
//    QByteArray hashData = QCryptographicHash::hash(nomFichier,QCryptographicHash::Md5);
//    qDebug() <<"HASH "<< hashData.toHex() +"." +cheminImage.suffix();


//    /** Sauvegarde */
//    QDir::setCurrent(cheminSauvegardeThumbs);
//    QFile file;
//    file.setFileName(hashData.toHex() +"." +cheminImage.suffix());
//    file.open(QIODevice::WriteOnly);
//    if(thumbnail.save(&file))
//        qDebug() << "Ecriture du fichier OK";




//    QByteArray nomFichier = "file:///tmp/t/2trainFantome.jpg";
//    QByteArray hashData = QCryptographicHash::hash(nomFichier,QCryptographicHash::Md5);
//    qDebug() << "MD5 :: " << hashData.toHex();

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
