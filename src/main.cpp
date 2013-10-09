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
#include "version.h"
#include "abuleduapplicationv1.h"
#include "abuledusplashscreenv1.h"

int main(int argc, char *argv[])
{
    AbulEduApplicationV1 a(argc, argv,VER_INTERNALNAME_STR, VER_PRODUCTVERSION_STR, VER_COMPANYDOMAIN_STR, "abuledu", VER_UNITVERSION_STR);
    a.setAbeApplicationLongName(QObject::trUtf8(VER_FILEDESCRIPTION_STR));

    MainWindow *w;

#ifdef __ABULEDUTABLETTEV1__MODE__
    w = new MainWindow();
    w->showFullScreen();
#else
    /* Splashscreen */
    AbulEduSplashScreenV1 *splash = new AbulEduSplashScreenV1(0,true,Qt::WindowFullScreen);
    splash->abeSplashScreenLauchUtilitaire(true);
    splash->show();
    w = new MainWindow();
    splash->setMainWindow(w);
    splash->launch(1000); //pour les developpeurs presses, remplacer le 1000 par 100 mais attention : plus le temps de transmettre l'authentification
#endif

    return a.exec();
}