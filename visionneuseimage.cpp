/** Classe VisionneuseImage
  * @see https://redmine.ryxeo.com/projects/
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

#include <QtGui>

#include "visionneuseimage.h"

VisionneuseImage::VisionneuseImage(QWidget *parent)
{
    m_parent = parent;
    setAttribute(Qt::WA_DeleteOnClose);
    m_localDebug = true;

    imageLabel = new QLabel();
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    creerActions();
    creerMenus();

    setWindowTitle(trUtf8("Visionneuse Image"));
    resize(500, 400);
}

VisionneuseImage::~VisionneuseImage()
{
    if (m_localDebug) qDebug() << "##########################  VisionneuseImage::~VisionneuseImage()";
}

void VisionneuseImage::ouvrir()
{
    QString nomFichier = QFileDialog::getOpenFileName(this,
                                    trUtf8("Ouvrir fichier"), QDir::currentPath());
    if (!nomFichier.isEmpty()) {
        QImage image(nomFichier);
        if (image.isNull()) {
            QMessageBox::information(this, trUtf8("Visionneuse Image"),
                                     trUtf8("Impossible d'ouvrir %1.").arg(nomFichier));
            return;
        }
        imageLabel->setPixmap(QPixmap::fromImage(image));
        scaleFactor = 1.0;

        a_imprimer->setEnabled(true);
        a_ajusterFenetre->setEnabled(true);
        majActions();

        if (!a_ajusterFenetre->isChecked())
            imageLabel->adjustSize();
    }

}

 void VisionneuseImage::ouvrirFicher(QString chemin)
 {
     if (!chemin.isEmpty()) {
         QImage image(chemin);
         if (image.isNull()) {
             QMessageBox::information(this, trUtf8("Visionneuse Image"),
                                      trUtf8("Impossible d'ouvrir %1.").arg(chemin));
             return;
         }
         imageLabel->setPixmap(QPixmap::fromImage(image));
         scaleFactor = 1.0;

         a_imprimer->setEnabled(true);
         a_ajusterFenetre->setEnabled(true);
         majActions();

         if (!a_ajusterFenetre->isChecked())
             imageLabel->adjustSize();
     }
 }

 void VisionneuseImage::imprimer()
 {
     Q_ASSERT(imageLabel->pixmap());
 #ifndef QT_NO_PRINTER
     QPrintDialog dialog(&printer, this);
     if (dialog.exec()) {
         QPainter painter(&printer);
         QRect rect = painter.viewport();
         QSize size = imageLabel->pixmap()->size();
         size.scale(rect.size(), Qt::KeepAspectRatio);
         painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
         painter.setWindow(imageLabel->pixmap()->rect());
         painter.drawPixmap(0, 0, *imageLabel->pixmap());
     }
 #endif

 }

 void VisionneuseImage::zoomIn()
 {
     scaleImage(1.25);
 }

 void VisionneuseImage::zoomOut()
 {
     scaleImage(0.8);
 }

 void VisionneuseImage::tailleNormale()
 {
     imageLabel->adjustSize();
     scaleFactor = 1.0;
 }

 void VisionneuseImage::ajusterFenetre()
 {
     bool isAjustable = a_ajusterFenetre->isChecked();
     scrollArea->setWidgetResizable(isAjustable);
     if (!isAjustable) {
         tailleNormale();
     }
     majActions();
 }

 void VisionneuseImage::aPropos()
 {
     QMessageBox::about(this, trUtf8("A Propos"),
             trUtf8("<p><b>La Visionneuse d'Image</b></p>"
                "<p>Permet d'afficher une image, de zoomer, d'ajuster la taille"
                " de l'image à la fenetre et d'imprimer (pdf). </p>"));
 }

 void VisionneuseImage::creerActions()
 {
     a_ouvrir = new QAction(trUtf8("&Ouvrir..."), this);
     a_ouvrir->setShortcut(trUtf8("Ctrl+O"));
     connect(a_ouvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));


     a_imprimer = new QAction(trUtf8("&Imprimer..."), this);
     a_imprimer->setShortcut(trUtf8("Ctrl+P"));
     a_imprimer->setEnabled(false);
     connect(a_imprimer, SIGNAL(triggered()), this, SLOT(imprimer()));

     a_quitter = new QAction(trUtf8("&Quitter"), this);
     a_quitter->setShortcut(trUtf8("Ctrl+Q"));
     connect(a_quitter, SIGNAL(triggered()), this, SLOT(close()));

     a_zoomIn = new QAction(trUtf8("Zoom &In (25%)"), this);
     a_zoomIn->setShortcut(trUtf8("Ctrl++"));
     a_zoomIn->setEnabled(false);
     connect(a_zoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));

     a_zoomOut = new QAction(trUtf8("Zoom &Out (25%)"), this);
     a_zoomOut->setShortcut(trUtf8("Ctrl+-"));
     a_zoomOut->setEnabled(false);
     connect(a_zoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));

     a_tailleNormal = new QAction(trUtf8("&Taille Normale"), this);
     a_tailleNormal->setShortcut(trUtf8("Ctrl+S"));
     a_tailleNormal->setEnabled(false);
     connect(a_tailleNormal, SIGNAL(triggered()), this, SLOT(tailleNormale()));

     a_ajusterFenetre = new QAction(trUtf8("&Ajuster a la fenetre"), this);
     a_ajusterFenetre->setEnabled(false);
     a_ajusterFenetre->setCheckable(true);
     a_ajusterFenetre->setShortcut(trUtf8("Ctrl+F"));
     connect(a_ajusterFenetre, SIGNAL(triggered()), this, SLOT(ajusterFenetre()));

     a_aPropos = new QAction(trUtf8("&A propos"), this);
     connect(a_aPropos, SIGNAL(triggered()), this, SLOT(aPropos()));

     a_aProposQt = new QAction(trUtf8("A propos &Qt"), this);
     connect(a_aProposQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
 }

 void VisionneuseImage::creerMenus()
 {
     menu_fichier = new QMenu(trUtf8("&Fichier"), this);
     menu_fichier->addAction(a_ouvrir);
     menu_fichier->addAction(a_imprimer);
     menu_fichier->addSeparator();
     menu_fichier->addAction(a_quitter);

     menu_vue = new QMenu(trUtf8("&Vue"), this);
     menu_vue->addAction(a_zoomIn);
     menu_vue->addAction(a_zoomOut);
     menu_vue->addAction(a_tailleNormal);
     menu_vue->addSeparator();
     menu_vue->addAction(a_ajusterFenetre);

     menu_aide = new QMenu(trUtf8("&Aide"), this);
     menu_aide->addAction(a_aPropos);
     menu_aide->addAction(a_aProposQt);

     menuBar()->addMenu(menu_fichier);
     menuBar()->addMenu(menu_vue);
     menuBar()->addMenu(menu_aide);
 }

 void VisionneuseImage::majActions()
 {
     a_zoomIn->setEnabled(!a_ajusterFenetre->isChecked());
     a_zoomOut->setEnabled(!a_ajusterFenetre->isChecked());
     a_tailleNormal->setEnabled(!a_ajusterFenetre->isChecked());
 }

 void VisionneuseImage::scaleImage(double facteur)
 {
     Q_ASSERT(imageLabel->pixmap());
     scaleFactor *= facteur;
     imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

     adjustScrollBar(scrollArea->horizontalScrollBar(), facteur);
     adjustScrollBar(scrollArea->verticalScrollBar(), facteur);

     a_zoomIn->setEnabled(scaleFactor < 3.0);
     a_zoomOut->setEnabled(scaleFactor > 0.333);
 }

 void VisionneuseImage::adjustScrollBar(QScrollBar *scrollBar, double facteur)
 {
     scrollBar->setValue(int(facteur * scrollBar->value()
                             + ((facteur - 1) * scrollBar->pageStep()/2)));
 }
