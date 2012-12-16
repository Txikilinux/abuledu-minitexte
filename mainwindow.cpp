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
    // Création de la Barre de boutons
    QToolBar *tb = new QToolBar(this);
    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tb->setFixedHeight(48);
    tb->setWindowTitle(trUtf8("&Édition"));

    ui->verticalLayout->insertWidget(0,tb); // On place la barre en haut


    /** @todo utiliser le thème abuledu pour les icones des toolboutons
      * Pour l'instant, on utilise l'icone du thème, sinon celle du fichier de ressources
      */

    // Sauvegarde du texte
    QAction *m_actionSave = new QAction(QIcon::fromTheme("document-save", QIcon("/filesave.png")),
                               trUtf8("&Enregistrer"), this);
    m_actionSave->setObjectName("save");
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    m_actionSave->setEnabled(false);
    tb->addAction(m_actionSave);

    QAction *m_actionPrint = new QAction(QIcon::fromTheme("document-print", QIcon("/fileprint.png")),
                                tr("&Print..."), this);
    m_actionPrint->setObjectName("print");
    m_actionPrint->setPriority(QAction::LowPriority);
    m_actionPrint->setShortcut(QKeySequence::Print);
    connect(m_actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));
    tb->addAction(m_actionPrint);

    tb->addSeparator();

    // Formatage des caractères
    QAction *m_actionTextBold = new QAction(QIcon::fromTheme("format-text-bold", QIcon("/textbold.png")),
                                   trUtf8("&Gras"), this);
    m_actionTextBold->setObjectName("bold");
    m_actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    m_actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    m_actionTextBold->setFont(bold);
    connect(m_actionTextBold, SIGNAL(triggered()), this, SLOT(setTextFormat()));
    tb->addAction(m_actionTextBold);
    m_actionTextBold->setCheckable(true);

    QAction *m_actionTextItalic = new QAction(QIcon::fromTheme("format-text-italic", QIcon("/textitalic.png")),
                                     trUtf8("&Italique"), this);
    m_actionTextItalic->setObjectName("italique");
    m_actionTextItalic->setPriority(QAction::LowPriority);
    m_actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    m_actionTextItalic->setFont(italic);
    connect(m_actionTextItalic, SIGNAL(triggered()), this, SLOT(setTextFormat()));
    tb->addAction(m_actionTextItalic);
    m_actionTextItalic->setCheckable(true);

    QAction *m_actionTextUnderline = new QAction(QIcon::fromTheme("format-text-underline", QIcon("/textunder.png")),
                                        trUtf8("&Souligné"), this);
    m_actionTextUnderline->setObjectName("underline");
    m_actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    m_actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    m_actionTextUnderline->setFont(underline);
    connect(m_actionTextUnderline, SIGNAL(triggered()), this, SLOT(setTextFormat()));
    tb->addAction(m_actionTextUnderline);
    m_actionTextUnderline->setCheckable(true);

    tb->addSeparator();

    // Alignement des paragraphes
    QActionGroup *m_alignActions = new QActionGroup(this);
    m_alignActions->setObjectName("groupalign");
    connect(m_alignActions, SIGNAL(triggered(QAction*)), this, SLOT(setTextAlign(QAction*)));

    QAction *m_actionAlignLeft;
    QAction *m_actionAlignCenter;
    QAction *m_actionAlignRight;

    // On modifie la position des icones en fonction du sens du texte LTR ou RTL
    if (QApplication::isLeftToRight()) {
        m_actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon("/textleft.png")), trUtf8("À gauc&he"), m_alignActions);
        m_actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon("/textcenter.png")), trUtf8("Au c&entre"), m_alignActions);
        m_actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon("/textright.png")), trUtf8("À d&roite"), m_alignActions);
    } else {
        m_actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon("/textright.png")), trUtf8("&À d&roite"), m_alignActions);
        m_actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon("/textcenter.png")), trUtf8("Au c&entre"), m_alignActions);
        m_actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon("/textleft.png")), trUtf8("À gauc&he"), m_alignActions);
    }
    QAction *m_actionAlignJustify = new QAction(QIcon::fromTheme("format-justify-fill", QIcon("/textjustify.png")), trUtf8("&Justifié"), m_alignActions);

    m_actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    m_actionAlignLeft->setObjectName("alignleft");
    m_actionAlignLeft->setCheckable(true);
    m_actionAlignLeft->setPriority(QAction::LowPriority);
    m_actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    m_actionAlignCenter->setObjectName("aligncenter");
    m_actionAlignCenter->setCheckable(true);
    m_actionAlignCenter->setPriority(QAction::LowPriority);
    m_actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    m_actionAlignRight->setObjectName("alignright");
    m_actionAlignRight->setCheckable(true);
    m_actionAlignRight->setPriority(QAction::LowPriority);
    m_actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    m_actionAlignJustify->setObjectName("alignjustify");
    m_actionAlignJustify->setCheckable(true);
    m_actionAlignJustify->setPriority(QAction::LowPriority);

    tb->addActions(m_alignActions->actions());

    tb->addSeparator();

    // Les actions concernant le choix de la police création de la combobox
    QFontComboBox *m_comboFont = new QFontComboBox(tb);
    m_comboFont->setObjectName("combofont");
    tb->addWidget(m_comboFont);
    m_comboFont->setEditable(false);
    connect(m_comboFont, SIGNAL(activated(QString)),
            this, SLOT(setTextFamily(QString)));
    // La taille de la police, création de la combobox
    QComboBox *m_comboSize = new QComboBox(tb);
    m_comboSize->setObjectName("comboSize");
    tb->addWidget(m_comboSize);
    m_comboSize->setEditable(true);

    QFontDatabase db;
    foreach(int size, db.standardSizes())
        m_comboSize->addItem(QString::number(size)); // On ajoute dans la combobox les tailles valides

    connect(m_comboSize, SIGNAL(activated(QString)),
            this, SLOT(setTextSize(QString)));
    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(QApplication::font()
                                                                       .pointSize())));
    // Création de l'icone de la couleur sélectionnée
    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    QAction *m_actionTextColor = new QAction(pix, trUtf8("&Couleur..."), this);
    m_actionTextColor->setObjectName("color");
    connect(m_actionTextColor, SIGNAL(triggered()), this, SLOT(setTextColor()));
    tb->addAction(m_actionTextColor);

#ifdef __ABULEDUTABLETTEV1__MODE__
    showFullScreen();
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

