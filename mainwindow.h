/** MiniTexte pour Tablette
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/abuledu-minitexte
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2013 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2013 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
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

#include <QTextCharFormat>
#include <QColorDialog>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QTextCursor>
#include <QTextImageFormat>
#include <QTextFrameFormat>
#include <QUrl>
#include <QImageReader>

#include "abuleduflatboutonv1.h"
#include "abuledumediathequegetv1.h"
#include "abuleduboxfilemanagerv1.h"

#ifndef Q_OS_ANDROID
#include "abuledupicottsv1.h"
#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /** Retourne un pointeur vers le document de la zone de texte */
    QTextDocument *abeTexteGetDocument();

    /** Retourne un pointeur vers la barre de boutons */
    QToolBar *abeTexteGetToolBar();

    /** Retourne un pointeur vers la barre de boutons */
    QMenuBar *abeTexteGetMenuBar();

    /** Fixe la police sous le curseur courant
      * @param fontFamily le nom de la police
      */
    void abeTexteSetFontFamily(QString fontFamily);
    QString abeTexteGetFontFamily();

    /** Fixe la taille de la police courante
      * @param taille la taille de la police
      */
    void abeTexteSetFontSize(int taille);

    /** Fixe l'alignement du paragraphe courant
      * @param align L'alignement du paragraphe : Qt::AlignLeft Qt::AlignRight Qt::AlignHCenter QT::AlignJustify
      */
    void abeTexteSetAlignment(Qt::Alignment align);

    /** Active le Bold ou pas
      * @param onOff active/désactive le Bold
      */
    void abeTexteSetBold(bool onOff);

    /** Active l'Italic ou pas
      * @param onOff active/désactive l'italic
      */
    void abeTexteSetItalic(bool onOff);


    /** Active le souligné ou pas
      * @param onOff active/désactive le souligné
      */
    void abeTexteSetUnderline(bool onOff);

    /** Montre ou cache la ToolBar
      * @param ouiNon visible/masquée
      */
    void abeTexteToolBarSetVisible(bool ouiNon);

    /** Retourne la visibilité de la ToolBar
      * @return true->visible, false->invisible
      */
    bool abeTexteToolBarIsVisible();

    /** Crée la QMenuBar si elle ne l'était pas */
    void abeTexteSetMenuBar(bool ouiNon);

    /** Retourne l'existance de la menuBar
      * @return true->Elle existe, false-> elle n'existe pas.
      */
    bool abeTexteHasMenuBar();

    /** insére une image à lo'endroit du curseur
      * @param cheminImage Le chemin vers l'image à insérer
      */
    bool abeTexteInsertImage(QString cheminImage, qreal width = 0, qreal height = 0, QTextFrameFormat::Position position = QTextFrameFormat::InFlow, QString name = "");

private:
    Ui::MainWindow *ui;
    /** Crée la barre de d'icones et les actions correspondantes */
    void setupToolBarAndActions();

    /** Implémente le mécanisme de translation de l'application dans la langue locale */
    void installTranslator();

    /** Applique le format passé en référence sur le mot en dessous du curseur, ou à la sélection
      * @param format le formatage à appliquer au texte
      */
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    /** Fixe le nom du fichier
      * @param fileName le chemin du fichier
      */
    void setCurrentFileName(const QString &fileName);

    /** Crée une barre de menu avec les QActions disponibles */
    void setupMenuBar();

    /** Liste des QActions (pas toutes implémentées) */
    QAction *m_actionTextBold,
            *m_actionTextItalic,
            *m_actionTextUnderline,
            *m_actionTextColor,
            *m_actionAlignLeft,
            *m_actionAlignCenter,
            *m_actionAlignRight,
            *m_actionAlignJustify,
            *m_actionUndo,
            *m_actionRedo,
            *m_actionCut,
            *m_actionCopy,
            *m_actionPaste,
            *m_actionImageFromData;

    /** Combobox des Polices */
//    QComboBox *m_comboFont;
    AbulEduFlatBoutonV1 *m_btnFontAndika;
    AbulEduFlatBoutonV1 *m_btnFontPlume;
    AbulEduFlatBoutonV1 *m_btnFontCrayon;
    AbulEduFlatBoutonV1 *m_btnFontSeyes;
    AbulEduFlatBoutonV1 *m_btnFontIncrease;
    AbulEduFlatBoutonV1 *m_btnFontDecrease;

    /** Combobox des tailles de polices valides */
//    QComboBox *m_comboSize;

    /** Groupe des actions d'alignement */
    QActionGroup *m_alignActions;

    /** Le chemin du fichier */
    QString m_fileName;

    bool m_localDebug;
    QToolBar *tb;
    QMenuBar *m_menuBar;
    int m_hauteurToolBar;
    QString m_font;

    bool m_hasMenuBar;
    bool m_isNewFile; //pour savoir si le fichier actuel est un nouveau fichier ou pas ...
    bool m_wantNewFile;

    /** Position de la souris pour gerer le deplacement de la fenetre */
    QPoint m_dragPosition;
    bool   m_isWindowMoving;

    QSharedPointer<AbulEduFileV1>  m_abuledufile;
#ifndef Q_OS_ANDROID
    AbulEduPicottsV1              *m_picoLecteur;
    bool m_isPicoReading;
#endif


#ifndef QT_NO_PRINTER
    /** Gestion Impression */
    QPrintDialog *m_printDialog;
    QPrinter *m_printer;
#endif
    /** Gestion Choix Couleur */
    QColorDialog* m_colorDialog;

    QTranslator qtTranslator;
    QTranslator myappTranslator;
    QString m_locale;

public slots:
    /** Formate le texte en fonction des toolButtons activés
      * Gras, Souligné, Italic
      */
    void setTextFormat();

    /** Aligne le texte en fonction des boutons activés */
    void setTextAlign(QAction *action);

    /** Applique la font sélectionnée */
    void setTextFamily();

    /** Applique la taille de font sélectionnée */
    void setTextSize(const QString &p);

    /** Applique la couleur de texte sélectionnée */
    void setTextColor();

    /** Enregistre le texte */
    bool fileSave();

    /** Enregistre sous ... le texte */
    bool fileSaveAs();

#ifndef QT_NO_PRINTER
    void filePrint(QPrinter*);
#endif

//    void cursorMoved(QTextCursor curseur);
    void cursorMoved();
    /** Mets à jour l'état des boutons de la tool barre en fonction du formatage */
    void updateActions(QTextCharFormat fmt);

    void slotMediathequeDownload(QSharedPointer<AbulEduFileV1> abeFile, int code);

    /** affiche la box file open */
    void fileOpen();

    /** ouvre le fichier */
    void slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile);

    /** gestion du retour de sauvegarde fichier via la box file manager */
    void slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation location, QString fileName, bool success);

    void test(int);

private slots:
    void on_btnLire_clicked();
    void on_btnPause_clicked();
    void on_btnStop_clicked();
    void on_btnFeuille_clicked();
    void on_btnPrint_clicked();
    void on_btnQuit_clicked();
    void on_stackedWidget_currentChanged(int arg1);
    void slotClearCurrent();

    /** Fixe la couleur de l'icone dans la barre de boutons
      * @param couleur une couleur sous forme de QColor
      */
    void colorChanged(const QColor &col);
    void on_btnOpen_clicked();
    void on_btnSave_clicked();
    void on_btnNew_clicked();

    void showAbeMediathequeGet();
    void showTextPage();
    void myFocusChangedSlot(QWidget *ex, QWidget *neo);

    /** Augmente la police de caractères utilisée */
    void increaseFontSize(int increase = 2);

    /** Diminue la police de caractères utilisée */
    void decreaseFontSize(int decrease = 2);

    void slotHelp();
    /** ************************************************************************************************************************
                                METHODES NON TABLETTE
      ************************************************************************************************************************** **/
#ifndef __ABULEDUTABLETTEV1__MODE__
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

#endif

signals:
    /** Signal émis lors du changement d'état du texte true -> texte modifié, false texte non modifié */
    void somethingHasChangedInText(bool);

    /** Signal émis lors du changement de nom du fichier
      * @param le nom du fichier avec [*] en fin de nom
      */
    void fileNameHasChanged(const QString newFileName);

    // Signaux émis pour simuler l'appui sur les toolbutons
    void alignmentRight();
    void alignmentLeft();
    void alignmentCenter();
    void alignmentJustify();
};

#endif // MAINWINDOW_H
