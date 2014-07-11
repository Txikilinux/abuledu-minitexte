/** MiniTexte pour Tablette
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/abuledu-minitexte
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2013 Eric Seigne <eric.seigne@ryxeo.com>
  * @author 2013-2014 Philippe Cadaugade <philippe.cadaugade@ryxeo.com>
  * @author 2013-2014 Icham Sirat <icham.sirat@ryxeo.com>
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
#include <QFont>

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
#include <QSignalMapper>

#include "abuleduflatboutonv1.h"
#include "abuledumediathequegetv1.h"
#include "abuleduboxfilemanagerv1.h"
#include "abuledumultimediav1.h"

namespace Ui {
class MainWindow;
}

//namespace ABULEDU_MINITEXTE_FONTS {
// const char * const ANDIKA              = "Andika";
// const char * const ECOLIER_LIGNES      = "Ecolier_lignes";
// const char * const CURSIVE_STANDARD    = "CursiveStandard";
//}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /** Retourne un pointeur vers le document de la zone de texte */
    QTextDocument *abeTexteGetDocument();

    /** Retourne un pointeur vers la barre de boutons */
//    QToolBar *abeTexteGetToolBar();

    /** Retourne un pointeur vers la barre de boutons */
//    QMenuBar *abeTexteGetMenuBar();

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
//    void abeTexteToolBarSetVisible(bool ouiNon);

    /** Retourne la visibilité de la ToolBar
      * @return true->visible, false->invisible
      */
//    bool abeTexteToolBarIsVisible();

    /** Crée la QMenuBar si elle ne l'était pas */
//    void abeTexteSetMenuBar(bool ouiNon);

    /** Retourne l'existance de la menuBar
      * @return true->Elle existe, false-> elle n'existe pas.
      */
//    bool abeTexteHasMenuBar();

    /** insére une image à l'endroit du curseur
      * @param cheminImage Le chemin vers l'image à insérer
      */
    bool abeTexteInsertImage(QString cheminImage, qreal width = 0, qreal height = 0, QTextFrameFormat::Position position = QTextFrameFormat::InFlow, QString name = "");

private:
    Ui::MainWindow *ui;

    void closeEvent(QCloseEvent *e);
    bool m_isCloseRequested;

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

    /** Le chemin du fichier */
    QString m_fileName;

    bool m_localDebug;
    int m_hauteurToolBar;

    bool m_isNewFile; //pour savoir si le fichier actuel est un nouveau fichier ou pas ...
    bool m_wantNewFile;

    /** Position de la souris pour gerer le deplacement de la fenetre */
    QPoint m_dragPosition;
    bool   m_isWindowMoving;

    QSharedPointer<AbulEduFileV1>  m_abuledufile;

    AbulEduMultiMediaV1* m_multimedia;

#ifndef QT_NO_PRINTER
    /** Gestion Impression */
    QPrintDialog *m_printDialog;
    QPrinter *m_printer;
#endif
    QTranslator qtTranslator;
    QTranslator myappTranslator;
    QString m_locale;

    QSignalMapper *m_signalMapperFontChange;
    QSignalMapper *m_signalMapperFontFormChange;
    QSignalMapper *m_signalMapperTextAlignChange;

    int m_fontSize;

    QTextCharFormat m_textCharFormat;

    QStringList m_listColors;

    void centrerFenetre();
    void initMultimedia();
    void initSignalMapperFontChange();
    void initSignalMapperFormFontChange();
    void initSignalMapperTextAlignChange();
    void initComboBoxColor(QComboBox *cb);

public slots:
    /** Formate le texte en fonction des toolButtons activés
      * Gras, Souligné, Italic
      */
//    void setTextFormat();

    /** Aligne le texte en fonction des boutons activés */
    void setTextAlign(QAction *action);

    /** Applique la font sélectionnée */
//    void setTextFamily(QAction *action);

    /** Applique la taille de font sélectionnée */
    void setTextSize(int p);

    /** Enregistre le texte */
    bool fileSave();

    /** Enregistre sous ... le texte */
    bool fileSaveAs();

#ifndef QT_NO_PRINTER
    void filePrint(QPrinter*);
#endif

    void slotCursorMoved();
    /** Mets à jour l'état des boutons de la tool barre en fonction du formatage */
//    void updateActions(QTextCharFormat fmt);

    void slotMediathequeDownload(QSharedPointer<AbulEduFileV1> abeFile, int code);

    /** affiche la box file open */
    void fileOpen();

    /** ouvre le fichier */
    void slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile);

    /** gestion du retour de sauvegarde fichier via la box file manager */
    void slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation location, QString fileName, bool success);

    void slotSessionAuthenticated(bool enable);

private slots:
    void on_abeMenuFeuilleBtnNew_clicked();
    void on_abeMenuFeuilleBtnOpen_clicked();
    void on_abeMenuFeuilleBtnSave_clicked();
    void on_abeMenuFeuilleBtnPrint_clicked();
    void on_abeMenuFeuilleBtnHelp_clicked();
    void on_abeMenuFeuilleBtnQuit_clicked();

    void on_stackedWidget_currentChanged(int arg1);
    void slotClearCurrent();

    /** Fixe la couleur de l'icone dans la barre de boutons
      * @param couleur une couleur sous forme de QColor
      */
    void colorChanged(const QColor &col);

    void slotChangeColor(int);

    void showAbeMediathequeGet();
    void showTextPage();

    void slotChangeLangue(QString lang);

    void on_teZoneTexte_textChanged();

    void slotReadContent();

    void slotChangeFont(const QString &);
    void slotChangeFormFont(const QString &);
    void slotChangeFontSize(int );
    void slotCurrentCharFormatChanged(QTextCharFormat);
    void slotChangeTextAlign(const QString &);

    void on_btn_increase_clicked();
    void on_btn_decrease_clicked();

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
