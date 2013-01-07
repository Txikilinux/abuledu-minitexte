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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QTextDocument>
#include <QMenu>
#include <QAction>
#include "abuledutextev1.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

private slots:

    void on_actionQuitter_triggered();
};

#endif // MAINWINDOW_H
