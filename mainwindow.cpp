/** MiniTexte pour Tablette
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/abuledu-minitexte
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012 Eric Seigne <eric.seigne@ryxeo.com>
  * @see The GNU Public License (GPL)
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program; if not, write to the Free Software Foundation, Inc.,
  * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
  */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_QuitOnClose);

    ui->setupUi(this);
    ui->widgetTextEditor->abeTexteSetMenuBar(false);

    ui->toolBar->addWidget(ui->widgetTextEditor->abeTexteGetToolBar());

    QWidget *spacerWidget = new QWidget(ui->toolBar);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setVisible(true);
    QAction *actionQuit = new QAction(QIcon(":/abuleduboxfilemanagerv1/fermer-32"),trUtf8("Quit"), this);
    connect(actionQuit,SIGNAL(triggered()),this,SLOT(close()));

    ui->toolBar->addWidget(spacerWidget);
    ui->toolBar->addAction(actionQuit);
    resize(1024,600);

    setWindowTitle(trUtf8("Mini traitement de texte pour AbulÉdu - Fichier Sans nom")+"[*]");

    //    setFixedSize(1024, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuitter_triggered()
{
    close();
}
