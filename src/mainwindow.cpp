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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_QuitOnClose);
    ui->setupUi(this);
    m_hauteurToolBar = 48;

    m_localDebug = true;

    installTranslator();

    ui->abeMediathequeGet->abeSetSourceEnum(AbulEduMediathequeGetV1::abeData);
    ui->abeMediathequeGet->abeHideBoutonTelecharger();
    ui->abeMediathequeGet->abeSetCustomBouton1(trUtf8("Insérer l'image"));
    ui->abeMediathequeGet->abeSetCustomBouton1Download(true);
    ui->abeMediathequeGet->abeSetDefaultView(AbulEduMediathequeGetV1::abeMediathequeThumbnails);

    /* Attention au cas où il n'y a pas de réponse, on est bloqué à un endroit du stackedWidget */
    connect(ui->abeMediathequeGet, SIGNAL(signalMediathequeFileDownloaded(QSharedPointer<AbulEduFileV1>,int)), this, SLOT(slotMediathequeDownload(QSharedPointer<AbulEduFileV1>,int)),Qt::UniqueConnection);
    connect(ui->abeMediathequeGet, SIGNAL(signalAbeMediathequeGetCloseOrHide()),this, SLOT(showTextPage()),Qt::UniqueConnection);

    m_abuledufile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
    setCurrentFileName(m_abuledufile->abeFileGetDirectoryTemp().absolutePath() + "/document.html");

    //    m_abuleduFileManagerOpen = new AbulEduBoxFileManagerV1(0);
    //    m_abuleduFileManagerOpen->abeSetFile(m_abuledufile);
    //    connect(m_abuleduFileManagerOpen, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)));
    ui->abeBoxFileManager->abeMediathequeGetHideCloseBouton(true);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);


    //    m_abuleduFileManagerSave = new AbulEduBoxFileManagerV1(0);
    //    m_abuleduFileManagerSave->abeSetFile(m_abuledufile);
    //    m_abuleduFileManagerSave->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeSave);
    //    connect(m_abuleduFileManagerSave, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
    //            this, SLOT(slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)));
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
            this, SLOT(slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), Qt::UniqueConnection);

    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileCloseOrHide()),this, SLOT(showTextPage()), Qt::UniqueConnection);

    /* On crée la barre d'icones et les QActions qui vont bien */
    setupToolBarAndActions();
    /* Les connexions concernant les modifications du texte et de son nom*/
    connect(this, SIGNAL(alignmentRight()),   m_actionAlignRight,   SIGNAL(triggered()), Qt::UniqueConnection);
    connect(this, SIGNAL(alignmentLeft()),    m_actionAlignLeft,    SIGNAL(triggered()), Qt::UniqueConnection);
    connect(this, SIGNAL(alignmentCenter()),  m_actionAlignCenter,  SIGNAL(triggered()), Qt::UniqueConnection);
    connect(this, SIGNAL(alignmentJustify()), m_actionAlignJustify, SIGNAL(triggered()), Qt::UniqueConnection);

    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)), Qt::UniqueConnection);
    /* On émet un signal inquant si le texte a été modifié */
    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(somethingHasChangedInText(bool)), Qt::UniqueConnection);
    /* Le curseur a été déplacé*/
    connect(ui->teZoneTexte, SIGNAL(cursorPositionChanged()), this, SLOT(cursorMoved()), Qt::UniqueConnection);

    m_isNewFile = true;
    m_wantNewFile = false;

    setWindowFlags(Qt::CustomizeWindowHint);
    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),this,SLOT(slotChangeLangue(QString)),Qt::UniqueConnection);

#ifndef Q_OS_ANDROID
    m_isPicoReading = false;
    m_picoLecteur = new AbulEduPicottsV1(4);
    ui->btnLire->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(false);
#endif

    ui->toolBar->setParent(ui->frFormat);
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
    QTextCharFormat tcf;
    tcf.setFontFamily("Andika");
    tcf.setFontItalic(false);
    tcf.setFontPointSize(16);
    ui->teZoneTexte->textCursor().setCharFormat(tcf);

    /* Gestion Impression */
#ifndef QT_NO_PRINTER
    m_printer = new QPrinter(QPrinter::HighResolution);
    m_printDialog = new QPrintDialog(m_printer, this);
    m_printDialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    m_printDialog->setStyleSheet("background-color:#FFFFFF");
    ui->glPrint->addWidget(m_printDialog);

    connect(m_printDialog, SIGNAL(rejected()), this, SLOT(showTextPage()), Qt::UniqueConnection);
    connect(m_printDialog, SIGNAL(finished(int)), this, SLOT(test(int)), Qt::UniqueConnection);
    connect(m_printDialog, SIGNAL(accepted(QPrinter*)), this, SLOT(filePrint(QPrinter*)), Qt::UniqueConnection);
#endif

    /* Gestion Couleur*/
    m_colorDialog = new QColorDialog(this);
    ui->vlColor->addWidget(m_colorDialog);
    connect(m_colorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(colorChanged(QColor)), Qt::UniqueConnection);
    connect(m_colorDialog, SIGNAL(rejected()),this,SLOT(showTextPage()), Qt::UniqueConnection);

    /* On Centre la fenetre */
    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width = widget->width();
    int desktop_height = widget->height();
    this->move((desktop_width-this->width())/2, (desktop_height-this->height())/2);
}

/* Slot de Test ---> Ne Pas Degommer Icham */
void MainWindow::test(int a)
{
    qDebug() << a;
}

void MainWindow::installTranslator()
{
    m_locale = QLocale::system().name().section('_', 0, 0);
    myappTranslator.load("abuledu-minitexte_"+m_locale, "./lang");
    abeApp->installTranslator(&myappTranslator);
    /* pour avoir les boutons des boîtes de dialogue dans la langue locale (fr par défaut) */
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    abeApp->installTranslator(&qtTranslator);

}

void MainWindow::increaseFontSize(int increase)
{
    QTextCursor cursor = ui->teZoneTexte->textCursor();
    if (!cursor.hasSelection())
        /* Si pas de sélection, on utilise le mot sous le curseur */
        cursor.select(QTextCursor::WordUnderCursor);
    QTextCharFormat fmt;
    if(cursor.charFormat().fontPointSize() > 15){
        fmt.setFontPointSize(cursor.charFormat().fontPointSize()+increase);
    }
    else{
        fmt.setFontPointSize(15+increase);
    }
    /* On l'applique */
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::decreaseFontSize(int decrease)
{
    QTextCursor cursor = ui->teZoneTexte->textCursor();
    if (!cursor.hasSelection())
        /* Si pas de sélection, on utilise le mot sous le curseur */
        cursor.select(QTextCursor::WordUnderCursor);
    QTextCharFormat fmt;
    if(cursor.charFormat().fontPointSize() > 15)
    {
        fmt.setFontPointSize(cursor.charFormat().fontPointSize()-decrease);
    }
    /* On l'applique */
    mergeFormatOnWordOrSelection(fmt);
}

MainWindow::~MainWindow()
{
    delete ui;
    m_abuledufile->abeClean();
}

QTextDocument *MainWindow::abeTexteGetDocument()
{
    return ui->teZoneTexte->document();
}

QToolBar *MainWindow::abeTexteGetToolBar()
{
    return tb;
}

void MainWindow::abeTexteSetFontFamily(QString fontFamily)
{
    //    m_comboFont->setCurrentFont(QFont(fontFamily));
    setTextFamily();
}

QString MainWindow::abeTexteGetFontFamily()
{
    //    return m_comboFont->font().family();
}

void MainWindow::abeTexteSetFontSize(int taille)
{
    /** @todo Tester taille et l'adapter en fonction des tailles disponibles dans la combobox m_comboSize */
    //    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(taille)));
    setTextSize(QString::number(taille));
}

void MainWindow::abeTexteSetAlignment(Qt::Alignment align)
{
    if(align.testFlag(Qt::AlignRight))
        emit alignmentRight();
    else if(align.testFlag(Qt::AlignLeft))
        emit alignmentLeft();
    else if(align.testFlag(Qt::AlignHCenter))
        emit alignmentCenter();
    else if(align.testFlag(Qt::AlignJustify))
        emit alignmentCenter();
    updateActions(ui->teZoneTexte->textCursor().charFormat()); /* Met le bouton concerné à jour */
}

void MainWindow::abeTexteSetBold(bool onOff)
{
    m_actionTextBold->setChecked(onOff);
    setTextFormat();
}

void MainWindow::abeTexteSetItalic(bool onOff)
{
    m_actionTextItalic->setChecked(onOff);
    setTextFormat();
}

void MainWindow::abeTexteSetUnderline(bool onOff)
{
    m_actionTextUnderline->setChecked(onOff);
    setTextFormat();
}

void MainWindow::abeTexteToolBarSetVisible(bool ouiNon)
{
    //    tb->setVisible(ouiNon);
}

bool MainWindow::abeTexteToolBarIsVisible()
{
    return true;// tb->isVisible();
}

void MainWindow::setTextFormat()
{
    /* On crée le format à appliquer */
    QTextCharFormat fmt;
    fmt.setFontWeight(m_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    fmt.setFontItalic(m_actionTextItalic->isChecked());
    fmt.setFontUnderline(m_actionTextUnderline->isChecked());
    /* On l'applique */
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::setTextAlign(QAction *action)
{
    /* On applique le bon alignement pour le paragraphe sous le curseur */
    if (action == m_actionAlignLeft)
        ui->teZoneTexte->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute); /* Toujours à gauche même en cas de RTL */
    else if (action == m_actionAlignCenter)
        ui->teZoneTexte->setAlignment(Qt::AlignHCenter);
    else if (action == m_actionAlignRight)
        ui->teZoneTexte->setAlignment(Qt::AlignRight | Qt::AlignAbsolute); /* Toujours à droite même en cas de RTL */
    else if (action == m_actionAlignJustify)
        ui->teZoneTexte->setAlignment(Qt::AlignJustify);
}

void MainWindow::setTextFamily()
{
    QString f = sender()->objectName();
    if (m_localDebug) qDebug() << " Fonte : " << f;
    /* On applique le format de font sélectionnée */
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
    setTextSize("20");
    //    ui->teZoneTexte->setFocus();

    /* Espacement vertical different */
    QTextBlockFormat format;
#if QT_VERSION >= 0x040700
    format.setLineHeight(sender()->property("interligne").toInt(), QTextBlockFormat::ProportionalHeight);
#endif
    QTextCursor curseur = ui->teZoneTexte->textCursor();
    curseur.setBlockFormat(format);
}

void MainWindow::setTextSize(const QString &p)
{
    /* On applique la taille de font sélectionnée */
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MainWindow::setTextColor()
{
    ui->stackedWidget->setCurrentWidget(ui->pageColor);

    if(!m_colorDialog->isVisible())
        m_colorDialog->setVisible(true);
}

void MainWindow::colorChanged(const QColor &col)
{

    if (!col.isValid())
        return;

    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    QPixmap pix(16,16);
    pix.fill(col);
    m_actionTextColor->setIcon(pix);
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
}

void MainWindow::setupToolBarAndActions()
{
    /* Création de la Barre de boutons */
    tb = ui->toolBar;
    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tb->setFixedHeight(m_hauteurToolBar);
    tb->setWindowTitle(trUtf8("&Édition"));

    /** @todo utiliser le thème abuledu pour les icones des toolboutons
      * Pour l'instant, on utilise l'icone du thème, sinon celle du fichier de ressources
      */

    /* Formatage des caractères */
    m_actionTextBold = new QAction(QIcon(":/abuledutextev1/format/bold"), trUtf8("&Gras"), this);
    m_actionTextBold->setObjectName("bold");
    m_actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    m_actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    m_actionTextBold->setFont(bold);
    connect(m_actionTextBold, SIGNAL(triggered()), this, SLOT(setTextFormat()), Qt::UniqueConnection);
    tb->addAction(m_actionTextBold);
    m_actionTextBold->setCheckable(true);

    m_actionTextItalic = new QAction(QIcon(":/abuledutextev1/format/italic"), trUtf8("&Italique"), this);
    m_actionTextItalic->setObjectName("italique");
    m_actionTextItalic->setPriority(QAction::LowPriority);
    m_actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    m_actionTextItalic->setFont(italic);
    connect(m_actionTextItalic, SIGNAL(triggered()), this, SLOT(setTextFormat()), Qt::UniqueConnection);
    tb->addAction(m_actionTextItalic);
    m_actionTextItalic->setCheckable(true);

    m_actionTextUnderline = new QAction(QIcon(":/abuledutextev1/format/underlined"), trUtf8("&Souligné"), this);
    m_actionTextUnderline->setObjectName("underline");
    m_actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    m_actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    m_actionTextUnderline->setFont(underline);
    connect(m_actionTextUnderline, SIGNAL(triggered()), this, SLOT(setTextFormat()), Qt::UniqueConnection);
    tb->addAction(m_actionTextUnderline);
    m_actionTextUnderline->setCheckable(true);

    m_btnFontDecrease = new AbulEduFlatBoutonV1();
    m_btnFontDecrease->setFixedSize(30,30);
    m_btnFontDecrease->setCouleurFondPressed(QColor("#328aec"));
    m_btnFontDecrease->setIconeNormale(":/abuledutextev1/format/decrease");
    m_btnFontDecrease->setObjectName("decrease");
    m_btnFontDecrease->setProperty("interligne",100);
    tb->addWidget(m_btnFontDecrease);
    connect(m_btnFontDecrease, SIGNAL(clicked()), this, SLOT(decreaseFontSize()), Qt::UniqueConnection);

    m_btnFontIncrease = new AbulEduFlatBoutonV1();
    m_btnFontIncrease->setFixedSize(30,30);
    m_btnFontIncrease->setCouleurFondPressed(QColor("#328aec"));
    m_btnFontIncrease->setIconeNormale(":/abuledutextev1/format/increase");
    m_btnFontIncrease->setObjectName("increase");
    m_btnFontIncrease->setProperty("interligne",100);
    tb->addWidget(m_btnFontIncrease);
    connect(m_btnFontIncrease, SIGNAL(clicked()), this, SLOT(increaseFontSize()), Qt::UniqueConnection);

    tb->addSeparator();

    // Alignement des paragraphes
    m_alignActions = new QActionGroup(this);
    m_alignActions->setObjectName("groupalign");
    connect(m_alignActions, SIGNAL(triggered(QAction*)), this, SLOT(setTextAlign(QAction*)), Qt::UniqueConnection);

    // On modifie la position des icones en fonction du sens du texte LTR ou RTL
    if (QApplication::isLeftToRight()) {
        m_actionAlignLeft = new QAction(QIcon(":/abuledutextev1/format/left"), trUtf8("À gauc&he"), m_alignActions);
        m_actionAlignCenter = new QAction(QIcon(":/abuledutextev1/format/center"), trUtf8("Au c&entre"), m_alignActions);
        m_actionAlignRight = new QAction(QIcon(":/abuledutextev1/format/right"), trUtf8("À d&roite"), m_alignActions);
    } else {
        m_actionAlignRight = new QAction(QIcon(":/abuledutextev1/format/right"), trUtf8("À d&roite"), m_alignActions);
        m_actionAlignCenter = new QAction(QIcon(":/abuledutextev1/format/center"), trUtf8("Au c&entre"), m_alignActions);
        m_actionAlignLeft = new QAction(QIcon(":/abuledutextev1/format/left"), trUtf8("À gauc&he"), m_alignActions);
    }
    m_actionAlignJustify = new QAction(QIcon(":/abuledutextev1/format/justify"), trUtf8("&Justifié"), m_alignActions);

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

    QFontDatabase fonts;
    if( ! fonts.addApplicationFont(":/abuledutextev1/Seyes")) {
        if (m_localDebug) qDebug() << "Erreur sur :/fonts/SEYESBDE.TTF";
    }
    if( ! fonts.addApplicationFont(":/abuledutextev1/Crayon")) {
        if (m_localDebug) qDebug() << "Erreur sur :/fonts/CRAYONE.TTF";
    }
    if( ! fonts.addApplicationFont(":/abuledutextev1/Plume")) {
        if (m_localDebug) qDebug() << "Erreur sur :/fonts/PLUMBAE.TTF";
    }

    //Pour tablettes, je préfère des boutons ...
    m_btnFontAndika = new AbulEduFlatBoutonV1();
    m_btnFontAndika->setFixedWidth(80);
    m_btnFontAndika->setCouleursTexte(QColor(Qt::white),QColor(Qt::white),QColor(Qt::white),QColor(Qt::lightGray));
    //    m_btnFontAndika->setCouleurFondPressed(QColor("#328aec"));
    m_btnFontAndika->setText("Andika");
    m_btnFontAndika->setFont(QFont("andika",14));
    m_btnFontAndika->setObjectName("andika");
    m_btnFontAndika->setProperty("interligne",100);
    tb->addWidget(m_btnFontAndika);
    connect(m_btnFontAndika, SIGNAL(clicked()), this, SLOT(setTextFamily()), Qt::UniqueConnection);

    m_btnFontSeyes= new AbulEduFlatBoutonV1();
    m_btnFontSeyes->setFixedWidth(80);
    m_btnFontSeyes->setText("Seyes");
    m_btnFontSeyes->setCouleursTexte(QColor(Qt::white),QColor(Qt::white),QColor(Qt::white),QColor(Qt::lightGray));
    //    m_btnFontSeyes->setCouleursFond(QColor("#67beff"),QColor("#67beff"),QColor("#328aec"),QColor(Qt::lightGray));
    //    m_btnFontSeyes->setCouleurFondPressed(QColor("#328aec"));
    m_btnFontSeyes->setFont(QFont("SeyesBDE",16));
    m_btnFontSeyes->setObjectName("SeyesBDE");
    m_btnFontSeyes->setProperty("interligne",200);
    //m_btnFontSeyes->setTextePadding(30,10,30,10);
    tb->addWidget(m_btnFontSeyes);
    connect(m_btnFontSeyes, SIGNAL(clicked()), this, SLOT(setTextFamily()), Qt::UniqueConnection);

    /* Philippe 20130926
     * On veut que ce soit le plus simple possible, je pense qu'une seule police cursive suffit

    m_btnFontCrayon= new AbulEduFlatBoutonV1();
    m_btnFontCrayon->setFixedWidth(80);
    m_btnFontCrayon->setCouleursTexte(QColor(Qt::white),QColor(Qt::white),QColor(Qt::white),QColor(Qt::lightGray));
    //    m_btnFontCrayon->setCouleurFondPressed(QColor("#328aec"));
    m_btnFontCrayon->setText("Crayon");
    m_btnFontCrayon->setFont(QFont("CrayonE",16));
    m_btnFontCrayon->setObjectName("CrayonE");
    m_btnFontCrayon->setProperty("interligne",120);
    tb->addWidget(m_btnFontCrayon);
    connect(m_btnFontCrayon, SIGNAL(clicked()), this, SLOT(setTextFamily()), Qt::UniqueConnection);
    */

    m_btnFontPlume= new AbulEduFlatBoutonV1();
    m_btnFontPlume->setFixedWidth(80);
    m_btnFontPlume->setCouleursTexte(QColor(Qt::white),QColor(Qt::white),QColor(Qt::white),QColor(Qt::lightGray));
    //    m_btnFontPlume->setCouleurFondPressed(QColor("#328aec"));
    m_btnFontPlume->setText("Plume");
    m_btnFontPlume->setFont(QFont("PlumBAE",16));
    m_btnFontPlume->setObjectName("PlumBAE");
    m_btnFontPlume->setProperty("interligne",120);
    tb->addWidget(m_btnFontPlume);
    connect(m_btnFontPlume, SIGNAL(clicked()), this, SLOT(setTextFamily()), Qt::UniqueConnection);

    tb->addSeparator();

    // Les actions concernant le choix de la police création de la combobox
    //    m_comboFont = new QComboBox(tb);
    //    m_comboFont->setObjectName("combofont");
    //    m_comboFont->addItem("Andika");
    //    m_comboFont->addItem("CrayonE");
    //    m_comboFont->addItem("PlumBAE");
    //    m_comboFont->addItem("SeyesBDE");
    //    tb->addWidget(m_comboFont);
    //    m_comboFont->setEditable(false);
    //    connect(m_comboFont, SIGNAL(activated(QString)),
    //            this, SLOT(setTextFamily(QString)));
    //    // La taille de la police, création de la combobox
    //    m_comboSize = new QComboBox(tb);
    //    m_comboSize->setObjectName("comboSize");
    //    tb->addWidget(m_comboSize);
    //    m_comboSize->setEditable(true);

    //    QFontDatabase db;
    //    foreach(int size, db.standardSizes())
    //        m_comboSize->addItem(QString::number(size)); // On ajoute dans la combobox les tailles valides

    //    connect(m_comboSize, SIGNAL(activated(QString)),
    //            this, SLOT(setTextSize(QString)));
    //    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(QApplication::font()
    //                                                                       .pointSize())));

    // Création de l'icone de la couleur sélectionnée
    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    m_actionTextColor = new QAction(pix, trUtf8("&Couleur..."), this);
    m_actionTextColor->setObjectName("color");
    connect(m_actionTextColor, SIGNAL(triggered()), this, SLOT(setTextColor()), Qt::UniqueConnection);
    tb->addAction(m_actionTextColor);

    m_actionImageFromData = new QAction(QIcon::fromTheme("image-from-data", QIcon(":/abuledutextev1/buttons/data")), trUtf8("Insérer une image"), this);
    m_actionImageFromData->setObjectName("mediatheque-data");

    connect(m_actionImageFromData, SIGNAL(triggered()), this, SLOT(showAbeMediathequeGet()), Qt::UniqueConnection);
    tb->addAction(m_actionImageFromData);


    //    QWidget *spacerWidget = new QWidget(ui->toolBar);
    //    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //    spacerWidget->setVisible(true);
    //    QAction *actionQuit = new QAction(QIcon(":/abuledutextev1/fermer-48"),trUtf8("Quit"), this);
    //    connect(actionQuit,SIGNAL(triggered()),this,SLOT(close()));

    //    ui->toolBar->addWidget(spacerWidget);
    //    ui->toolBar->addAction(actionQuit);

}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->teZoneTexte->textCursor();
    if (!cursor.hasSelection())
        /* Si pas de sélection, on utilise le mot sous le curseur */
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->teZoneTexte->mergeCurrentCharFormat(format);
}

bool MainWindow::fileSave()
{
    setCurrentFileName(m_abuledufile->abeFileGetDirectoryTemp().absolutePath() + "/document.html");

    if (m_localDebug) qDebug() << "Ecriture dans le fichier " << m_fileName;

    QFileInfo fi(m_fileName);

    QTextDocumentWriter writer(fi.absoluteFilePath(),"HTML");
    bool success = writer.write(ui->teZoneTexte->document());
    if (success)
        ui->teZoneTexte->document()->setModified(false);

    /* Le 1er fichier de la liste, c'est le fichier document maitre html */
    QStringList liste(m_fileName);
    /* Parcours du repertoire data pour enquiller tous les autres fichiers */
    QDir dir(fi.absolutePath() + "/data/");
    /* Attention a ne pas prendre le repertoire "." et ".." */
    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        if(fileInfo.isFile()) {
            liste << fileInfo.absoluteFilePath();
        }
    }

    m_abuledufile->abeFileExportPrepare(liste, fi.absolutePath(), "abe");

    if (m_isNewFile) {
        fileSaveAs();
    }

    ui->abeBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeSave);
    ui->stackedWidget->setCurrentWidget(ui->pageBoxFileManager);

    return success;
}

bool MainWindow::fileSaveAs()
{
    m_isNewFile = false;
    return fileSave();
}

void MainWindow::setCurrentFileName(const QString &fileName)
{
    m_fileName = fileName;
    /* Comme le nom vient de changer, c'est que le fichier vient d'être crée ou vient d'être sauvegardé */
    ui->teZoneTexte->document()->setModified(false);

    QDir rep(m_abuledufile->abeFileGetDirectoryTemp().absolutePath());
    if(!rep.exists()) {
        rep.mkpath(m_abuledufile->abeFileGetDirectoryTemp().absolutePath());
    }

    QString shownName;
    if (fileName.isEmpty()) {
        shownName = trUtf8("Sans nom") +"[*]";
    }
    else {
        shownName = QFileInfo(m_fileName).fileName() + "[*]";
    }

    /* Au cas ou le widget serait un topLevelWidget() */
    setWindowTitle(shownName);
    /* On émet un signal avec le nom du fichier suivi de [*] pour affichage dans titre de fenêtre */
    emit fileNameHasChanged(shownName);
}

bool MainWindow::abeTexteInsertImage(QString cheminImage, qreal width, qreal height, QTextFrameFormat::Position position, QString name)
{
    QFile fichier(cheminImage);
    if(!fichier.exists()){
        if (m_localDebug) qDebug()<<__PRETTY_FUNCTION__<<"ligne"<<__LINE__<<"Le fichier n'existe pas"<<cheminImage;
        return false;
    }
    else{
        if(name.isEmpty()){
            name = cheminImage;
        }
        QImage image(cheminImage);
        if(width == 0 || height == 0){
            width = image.width();
            height = image.height();
        }
        else{
            image = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        ui->teZoneTexte->document()->addResource(QTextDocument::ImageResource, QUrl(name), image);

        /* Changer la méthode d'insertion pour insérer une QTetxtFrame dans laquelle on insérerait une image
         * ce qui devrait donner plus de souplesse
         * peut-être :-D
         */
        QTextImageFormat *imageFmt = new QTextImageFormat();
        imageFmt->setName(name);
        //        imageFmt->setWidth(width);
        //        imageFmt->setHeight(height);

        ui->teZoneTexte->textCursor().insertImage(*imageFmt, position);
        return true;
    }
}

#ifndef QT_NO_PRINTER
void MainWindow::filePrint(QPrinter *printer)
{
    /* On imprime */
    ui->teZoneTexte->print(printer);

    /* On affiche un message */
    QString message("Impression en cours");
    AbulEduMessageBoxV1* msgImpression = new AbulEduMessageBoxV1(trUtf8("Impression"), message,this);
    msgImpression->setWink();
    msgImpression->show();
    connect(msgImpression, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(showTextPage()), Qt::UniqueConnection);

}
#endif

void MainWindow::cursorMoved()
{
    updateActions(ui->teZoneTexte->textCursor().charFormat());
}

void MainWindow::updateActions(QTextCharFormat fmt)
{
    /* On actualise les toolButtons ou plutôt les QActions sous-jacentes en fonction du formatage du texte */
    m_actionTextBold->blockSignals(true);
    m_actionTextBold->setChecked(fmt.fontWeight() >= QFont::Bold);
    m_actionTextBold->blockSignals(false);

    m_actionTextItalic->blockSignals(true);
    m_actionTextItalic->setChecked(fmt.fontItalic());
    m_actionTextItalic->blockSignals(false);

    m_actionTextUnderline->blockSignals(true);
    m_actionTextUnderline->setChecked(fmt.fontUnderline());
    m_actionTextUnderline->blockSignals(false);

    //    m_actionTextColor->blockSignals(true);
    colorChanged(fmt.foreground().color());
    //    m_actionTextColor->blockSignals(false);

    m_actionAlignLeft->blockSignals(true);
    m_actionAlignLeft->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignLeft));
    m_actionAlignLeft->blockSignals(false);

    m_actionAlignCenter->blockSignals(true);
    m_actionAlignCenter->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignHCenter));
    m_actionAlignCenter->blockSignals(false);

    m_actionAlignRight->blockSignals(true);
    m_actionAlignRight->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignRight));
    m_actionAlignRight->blockSignals(false);

    m_actionAlignJustify->blockSignals(true);
    m_actionAlignJustify->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignJustify));
    m_actionAlignJustify->blockSignals(false);

    //    m_comboFont->blockSignals(true);
    //    //    m_comboFont->setCurrentFont(fmt.font());
    //    m_comboFont->blockSignals(false);

    //    m_comboSize->blockSignals(true);
    //    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(fmt.font().pointSize())));
    //    m_comboSize->blockSignals(false);
}

void MainWindow::slotMediathequeDownload(QSharedPointer<AbulEduFileV1> abeFile, int code)
{
    QString file = abeFile->abeFileGetContent(0).absoluteFilePath();
    QString filename = abeFile->abeFileGetContent(0).baseName() + ".png";

    if (m_localDebug) qDebug() << "  slotMediathequeDownload : " << file << " et " << filename;

    QUrl Uri ( QString ( "mydata://data/%1" ).arg ( filename ) );
    QImage image = QImageReader ( file ).read().scaledToWidth(150,Qt::SmoothTransformation);

    QFileInfo fi(m_fileName);
    QString imageDest = QString("%1/data/%2").arg(fi.absolutePath()).arg(filename);
    //    Uri.setUrl(imageDest);

    QTextDocument * textDocument = ui->teZoneTexte->document();
    textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant ( image ) );
    QTextCursor cursor = ui->teZoneTexte->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth( image.width() );
    imageFormat.setHeight( image.height() );

    QDir rep(fi.absolutePath() + "/data/");
    if(!rep.exists()) {
        rep.mkpath(fi.absolutePath() + "/data/");
    }
    if(!image.save(imageDest)) {
        //        if (m_localDebug) qDebug() << "******* ERREUR de sauvegarde de " << imageDest;
    }
    if (m_localDebug) qDebug() << "Sauvegarde de l'image dans " << imageDest;

    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);
    cursor.insertText("\n");

    //Les sources et l'auteur (?)
    QTextListFormat listFormat;
    cursor.insertList(listFormat);
    QTextCharFormat fmt;
    fmt.setFontItalic(true);
    cursor.insertText("Source: " + abeFile->abeFileGetIdentifier() + "\n",fmt);
    cursor.insertText("Auteur: " + abeFile->abeFileGetCreator(),fmt);

    //Retour normal
    QTextBlockFormat blockFormat;
    fmt.setFontItalic(false);
    cursor.insertBlock(blockFormat,fmt);

    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
}

void MainWindow::fileOpen()
{
    ui->abeBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeOpen);
    ui->abeBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->stackedWidget->setCurrentWidget(ui->pageBoxFileManager);
    ui->frFormat->setEnabled(true);
}

void MainWindow::slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile)
{
    /* Correction du bug de non apparition de l'image */
    if(abeFile)
    {
        m_abuledufile = abeFile;
    }
    if (m_localDebug) qDebug() << "Ouverture du fichier " << m_abuledufile->abeFileGetFileName().filePath();
    qDebug()<<" dont le repertoire temporaire est "<<m_abuledufile->abeFileGetDirectoryTemp().absolutePath();
    setCurrentFileName(m_abuledufile->abeFileGetContent(0).absoluteFilePath());

    // ==============================================================================
    /* lecture du fichier html */
    if (m_localDebug) qDebug()<<m_fileName;
    QFile  htmlFile(m_fileName);
    if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QString htmlContent;
    QTextStream in(&htmlFile);
    while (!in.atEnd()) {
        htmlContent.append(in.readLine());
    }

    QTextDocument *document = new QTextDocument();
    document->setHtml(htmlContent);
    ui->teZoneTexte->setDocument(document);


    // ==============================================================================
    /* chargement des ressources dans le textDocument... */
    QTextDocument * textDocument = ui->teZoneTexte->document();
    QStringList liste = m_abuledufile->abeFileGetFileList();
    for(int i = 0; i < liste.size(); i++) {
        QFileInfo fi(liste.at(i));
        if(fi.suffix() == "png") {
            QUrl Uri ( QString ( "mydata://data/%1" ).arg ( fi.fileName() ) );
            QImage image = QImageReader(fi.absoluteFilePath()).read();
            textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant ( image ) );
            if (m_localDebug) qDebug() << " ++ " << fi.absoluteFilePath() << " en tant que " << Uri;
        }
    }

    ui->teZoneTexte->update();
}

void MainWindow::slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation location, QString fileName, bool success)
{
    if (m_localDebug) qDebug() << "slotAbeFileSaved : " << fileName << " et " << success;
    QString emplacement;
    switch (location) {
    case AbulEduBoxFileManagerV1::abePC:
        emplacement = trUtf8("votre ordinateur");
        break;
    case AbulEduBoxFileManagerV1::abeBoxPerso:
        emplacement = trUtf8("votre abeBox personnelle");
        break;
    case AbulEduBoxFileManagerV1::abeBoxShare:
        emplacement = trUtf8("une abeBox partagée");
        break;
    case AbulEduBoxFileManagerV1::abeMediatheque:
        emplacement = trUtf8("AbulEdu-Médiathèque");
        break;
    default:
        emplacement = trUtf8("un endroit inconnu");
        break;
    }
    QString message;
    if(success)
    {
        message = trUtf8("Votre fichier a été enregistré dans ")+emplacement;
        if (!fileName.isEmpty()){
            message.append(trUtf8(" sous le nom : ")+fileName.split("/").last());
        }
    }
    else{
        message = trUtf8("Votre fichier n'a pas pu être enregistré...");
    }
    AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"), message,this);
    if(success)
    {
        msgEnregistrement->setWink();
    }
    msgEnregistrement->show();
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
    if(m_wantNewFile){
        slotClearCurrent();
        m_wantNewFile = false;
    }
}

void MainWindow::on_btnLire_clicked()
{
#ifndef Q_OS_ANDROID
    QString txt = QString("<break time=\"1s\"><speed level=\"80\"><volume level=\"100\">%1</speed>").arg(ui->teZoneTexte->toPlainText());
    if(m_isPicoReading) {
        m_picoLecteur->abePicoResume();
    }
    else {
        m_picoLecteur->abePicoPlay(txt);
    }
    ui->btnLire->setEnabled(false);
    ui->btnPause->setEnabled(true);
    ui->btnStop->setEnabled(true);
    m_isPicoReading = !m_isPicoReading;
#endif
}

void MainWindow::on_btnPause_clicked()
{
#ifndef Q_OS_ANDROID
    m_picoLecteur->abePicoPause();
    ui->btnLire->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(true);
#endif
}

void MainWindow::on_btnStop_clicked()
{
#ifndef Q_OS_ANDROID
    m_picoLecteur->abePicoStop();
    m_isPicoReading = false;
    ui->btnLire->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(false);
#endif
}

void MainWindow::on_abeMenuFeuilleBtnPrint_clicked()
{
#ifndef QT_NO_PRINTER
    if(!m_printDialog->isVisible())
        m_printDialog->showNormal();
#endif

    ui->stackedWidget->setCurrentWidget(ui->pagePrint);
}

void MainWindow::on_abeMenuFeuilleBtnHelp_clicked()
{
    slotHelp();
}

void MainWindow::slotHelp()
{
    /* On affiche un message */
    QString message = trUtf8("Écris un texte, tu pourras l'enregistrer, l'imprimer, l'écouter lire, etc...");
    AbulEduMessageBoxV1* msgAide = new AbulEduMessageBoxV1(trUtf8("Aide"), message,this);
    msgAide->setWink();
    msgAide->show();
    connect(msgAide, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(showTextPage()), Qt::UniqueConnection);
}
void MainWindow::on_abeMenuFeuilleBtnQuit_clicked()
{
    close();
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    if (m_localDebug) qDebug()<<"page courante : "<<ui->stackedWidget->currentWidget()->objectName();
}

void MainWindow::slotClearCurrent()
{
    /* Je veux faire un nouveau texte, mais je ne veux pas changer d'abe */
    m_abuledufile->abeCleanDirectory(m_abuledufile->abeFileGetDirectoryTemp().absolutePath(),m_abuledufile->abeFileGetDirectoryTemp().absolutePath());
    ui->teZoneTexte->clear();
    setWindowModified(false);
}

void MainWindow::on_abeMenuFeuilleBtnOpen_clicked()
{
    fileOpen();
}

void MainWindow::on_abeMenuFeuilleBtnSave_clicked()
{
    /* Je n'enregistre pas si la zone de texte est vide */
    if(ui->teZoneTexte->toPlainText().isEmpty()) return;

    if (fileSave()){
        ui->stackedWidget->setCurrentWidget(ui->pageBoxFileManager);
        ui->abeBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeSave);
        ui->abeBoxFileManager->abeSetFile(m_abuledufile);
        ui->abeBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    }
    setWindowModified(false);
}

void MainWindow::on_abeMenuFeuilleBtnNew_clicked()
{
    if(isWindowModified()){
        m_wantNewFile = true;
        on_abeMenuFeuilleBtnSave_clicked();
    }
    else{
        slotClearCurrent();
    }
    return;
}

void MainWindow::showAbeMediathequeGet()
{
    ui->abeMediathequeGet->setVisible(true);
    ui->stackedWidget->setCurrentWidget(ui->pageMediathequeGet);
}

void MainWindow::showTextPage()
{
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
    ui->frFormat->setEnabled(true);
}

void MainWindow::slotChangeLangue(QString lang)
{
    if(m_localDebug) qDebug()<<" ---------         slot change langue en "<<lang;
    qApp->removeTranslator(&qtTranslator);
    qApp->removeTranslator(&myappTranslator);

    qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&qtTranslator);
    myappTranslator.load("abuledu-minitexte_" + lang, "lang");
    qApp->installTranslator(&myappTranslator);
    ui->retranslateUi(this);
}