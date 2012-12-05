/** MiniTexte pour Tablette
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-minitexte
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
    ui->setupUi(this);
    setWindowTitle(trUtf8("Sans nom")+"[*]");
    // Place une étoile si le fichier texte est modifié, l'enlève sinon
    connect(ui->widgetTextEditor, SIGNAL(somethingHasChangedInText(bool)), this, SLOT(setWindowModified(bool)));
    // Adapte le titre de la fenêtre au nom du fichier sauvegardé
    connect(ui->widgetTextEditor, SIGNAL(fileNameHasChanged(QString)), this, SLOT(setWindowTitle(QString)));

    // Paramètres par défaut du widget de texte
    ui->widgetTextEditor->abeTexteSetFontFamily(QString::fromUtf8("andika"));
    ui->widgetTextEditor->abeTexteSetFontSize(14);
    // Pour éviter l'étoile avant de commencer
    ui->widgetTextEditor->abeTexteGetDocument()->setModified(false);

    // Place la ToolBar dans la QMainwindow
    this->addToolBar(ui->widgetTextEditor->abeTexteGetToolBar());

#ifdef __ABULEDUTABLETTEV1__MODE__
    showFullScreen();
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

