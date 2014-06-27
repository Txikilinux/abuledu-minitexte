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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractPrintDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_QuitOnClose);

    ui->setupUi(this);
    m_hauteurToolBar = 48;

    m_localDebug        = false;
    m_isCloseRequested  = false;
    m_isNewFile         = true;
    m_wantNewFile       = false;

    setWindowFlags(Qt::CustomizeWindowHint);

#ifdef Q_OS_WIN
    switch(QSysInfo::windowsVersion())
    {
        case QSysInfo::WV_2000: qDebug()<< "Windows 2000";break;
        case QSysInfo::WV_XP: qDebug()<< "Windows XP";break;
        case QSysInfo::WV_VISTA: qDebug()<< "Windows Vista";break;
        case QSysInfo::WV_WINDOWS7: qDebug()<< "Windows Seven";break;
        case QSysInfo::WV_WINDOWS8: qDebug()<< "Windows 8";break;
        default: qDebug()<< "Windows";break;
    }
#endif

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
    ui->abeBoxFileManager->abeMediathequeGetHideCloseBouton(true);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
            this, SLOT(slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), Qt::UniqueConnection);

    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileCloseOrHide()),this, SLOT(showTextPage()), Qt::UniqueConnection);

    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)), Qt::UniqueConnection);
    /* On émet un signal inquant si le texte a été modifié */
    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(somethingHasChangedInText(bool)), Qt::UniqueConnection);
    /* Le curseur a été déplacé*/
//    connect(ui->teZoneTexte, SIGNAL(cursorPositionChanged()), this, SLOT(cursorMoved()), Qt::UniqueConnection);

    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),this,SLOT(slotChangeLangue(QString)),Qt::UniqueConnection);

    initMultimedia();

    QTextCharFormat tcf;
    tcf.setFontFamily("Andika");
    tcf.setFontItalic(false);
    tcf.setFontPointSize(16);
    ui->teZoneTexte->textCursor().setCharFormat(tcf);

#ifndef QT_NO_PRINTER
    /* Gestion Impression */
    m_printer = new QPrinter(QPrinter::HighResolution);
    m_printDialog = new QPrintDialog(m_printer, this);
    m_printDialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    m_printDialog->setStyleSheet("background-color:#FFFFFF");
    ui->glPrint->addWidget(m_printDialog);
    connect(m_printDialog, SIGNAL(rejected()), this, SLOT(showTextPage()), Qt::UniqueConnection);
    connect(m_printDialog, SIGNAL(accepted(QPrinter*)), this, SLOT(filePrint(QPrinter*)), Qt::UniqueConnection);
#endif

    /* Gestion Couleur*/
    m_colorDialog = new QColorDialog(this);
    ui->vlColor->addWidget(m_colorDialog);
    m_colorDialog->setOptions(QColorDialog::DontUseNativeDialog);
    connect(m_colorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(colorChanged(QColor)), Qt::UniqueConnection);
    connect(m_colorDialog, SIGNAL(rejected()),this,SLOT(showTextPage()), Qt::UniqueConnection);

    /* Gestion du retour de la page à propos */
    connect(ui->pageAbout, SIGNAL(signalAbeAproposBtnCloseClicked()), this, SLOT(showTextPage()),Qt::UniqueConnection);

#ifndef __ABULEDUTABLETTEV1__MODE__
    /* On Centre la fenetre */
    centrerFenetre();
    ui->teZoneTexte->setFocus();
#endif

    /* Gestion FrameBoutonsFormat */

    /* Bouton Choix Couleur */
    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    ui->btn_color->setIcon(QIcon(pix));
    connect(ui->btn_color, SIGNAL(clicked()), SLOT(setTextColor()), Qt::UniqueConnection);

    /* Bouton Data */
    connect(ui->btn_data, SIGNAL(clicked()), SLOT(showAbeMediathequeGet()), Qt::UniqueConnection);


    /*Page par défaut */
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);


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
}

void MainWindow::centrerFenetre()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    QDesktopWidget *widget = QApplication::desktop();
    int desktop_width  = widget->screen(widget->screenNumber(this))->width();
    int desktop_height = widget->screen(widget->screenNumber(this))->height();
    this->move((desktop_width-this->width())/2, (desktop_height-this->height())/2);
}

void MainWindow::initMultimedia()
{
    m_multimedia = new AbulEduMultiMediaV1(AbulEduMultiMediaV1::Sound, ui->frmControlAudio);
    m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetDirection(QBoxLayout::TopToBottom);
    m_multimedia->abeMultiMediaSetButtonVisible(AbulEduMultiMediaV1::BtnMagnifyingGlass | AbulEduMultiMediaV1::BtnPrevious | AbulEduMultiMediaV1::BtnNext | AbulEduMultiMediaV1::BtnHide | AbulEduMultiMediaV1::BtnRecord,false);
    m_multimedia->abeMultiMediaForceStop();
    connect(m_multimedia->abeMultiMediaGetAudioControlWidget(), SIGNAL(signalAbeControlAudioPlayClicked()),this, SLOT(slotReadContent()),Qt::UniqueConnection);

    /** @todo autres langues ? */
    if(m_multimedia->abeMultiMediaGetTTSlang() != AbulEduMultiMediaSettingsV1::fre){
        m_multimedia->abeMultimediaSetTTS(AbulEduMultiMediaSettingsV1::fre);
    }
    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),
            m_multimedia, SLOT(slotAbeMultimediaSetTTSfromIso6391(QString)), Qt::UniqueConnection);
}

void MainWindow::installTranslator()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    m_locale = QLocale::system().name().section('_', 0, 0);
    myappTranslator.load("abuledu-minitexte_"+m_locale, "./lang");
    abeApp->installTranslator(&myappTranslator);
    /* pour avoir les boutons des boîtes de dialogue dans la langue locale (fr par défaut) */
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    abeApp->installTranslator(&qtTranslator);

}

void MainWindow::increaseFontSize(int increase)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << increase;

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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << decrease;

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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    delete ui;
    m_abuledufile->abeClean();
}

QTextDocument *MainWindow::abeTexteGetDocument()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    return ui->teZoneTexte->document();
}

void MainWindow::abeTexteSetFontFamily(QString fontFamily)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << fontFamily;

    //    m_comboFont->setCurrentFont(QFont(fontFamily));
//    QAction* act = m_fontActions->findChild<QAction*>(fontFamily);
//    ABULEDU_LOG_TRACE() << act->objectName();
//    setTextFamily(act);
}

QString MainWindow::abeTexteGetFontFamily()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    //    return m_comboFont->font().family();
}

void MainWindow::abeTexteSetFontSize(int taille)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << taille;

    /** @todo Tester taille et l'adapter en fonction des tailles disponibles dans la combobox m_comboSize */
    //    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(taille)));
    setTextSize(taille);
}

void MainWindow::abeTexteSetAlignment(Qt::Alignment align)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << align;

    if(align.testFlag(Qt::AlignRight))
        emit alignmentRight();
    else if(align.testFlag(Qt::AlignLeft))
        emit alignmentLeft();
    else if(align.testFlag(Qt::AlignHCenter))
        emit alignmentCenter();
    else if(align.testFlag(Qt::AlignJustify))
        emit alignmentCenter();

//    updateActions(ui->teZoneTexte->textCursor().charFormat()); /* Met le bouton concerné à jour */
}

void MainWindow::abeTexteSetBold(bool onOff)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onOff;

//    m_actionTextBold->setChecked(onOff);
//    setTextFormat();
}

void MainWindow::abeTexteSetItalic(bool onOff)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onOff;

//    m_actionTextItalic->setChecked(onOff);
//    setTextFormat();
}

void MainWindow::abeTexteSetUnderline(bool onOff)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onOff;

//    m_actionTextUnderline->setChecked(onOff);
//    setTextFormat();
}

void MainWindow::setTextFormat()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* On crée le format à appliquer */
//    QTextCharFormat fmt;
//    fmt.setFontWeight(m_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
//    fmt.setFontItalic(m_actionTextItalic->isChecked());
//    fmt.setFontUnderline(m_actionTextUnderline->isChecked());
//    /* On l'applique */
//    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::setTextAlign(QAction *action)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << action;

    /* On applique le bon alignement pour le paragraphe sous le curseur */
//    if (action == m_actionAlignLeft)
//        ui->teZoneTexte->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute); /* Toujours à gauche même en cas de RTL */
//    else if (action == m_actionAlignCenter)
//        ui->teZoneTexte->setAlignment(Qt::AlignHCenter);
//    else if (action == m_actionAlignRight)
//        ui->teZoneTexte->setAlignment(Qt::AlignRight | Qt::AlignAbsolute); /* Toujours à droite même en cas de RTL */
//    else if (action == m_actionAlignJustify)
//        ui->teZoneTexte->setAlignment(Qt::AlignJustify);
}

void MainWindow::setTextFamily(QAction* action)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << action;

    QString f = action->objectName();
    if (m_localDebug) qDebug() << " Fonte : " << f;
    /* On applique le format de font sélectionnée */
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
    setTextSize(action->property("defaultPointSize").toInt());

    /* Espacement vertical different */
    QTextBlockFormat format;
#if QT_VERSION >= 0x040700
    format.setLineHeight(action->property("interligne").toInt(), QTextBlockFormat::ProportionalHeight);
#endif
    QTextCursor curseur = ui->teZoneTexte->textCursor();
    curseur.setBlockFormat(format);
}

void MainWindow::setTextSize(int p)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << p;

    /* On applique la taille de font sélectionnée */
    qreal pointSize = p;
    if (p > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MainWindow::setTextColor()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->stackedWidget->setCurrentWidget(ui->pageColor);

    if(!m_colorDialog->isVisible())
        m_colorDialog->setVisible(true);
}

void MainWindow::colorChanged(const QColor &col)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if (!col.isValid()){
        return;
    }
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    QPixmap pix(16,16);
    pix.fill(col);
    ui->btn_color->setIcon(pix);
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
}

//void MainWindow::setupToolBarAndActions()
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* Création de la Barre de boutons */
//    tb = ui->toolBar;
//    QToolBar* tb2 = new QToolBar();
//    tb2->setToolButtonStyle(Qt::ToolButtonIconOnly);
//    tb2->setFixedHeight(m_hauteurToolBar);
//    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
//    tb->setFixedHeight(m_hauteurToolBar);
//    tb->setWindowTitle(trUtf8("&Édition"));


    /* Formatage des caractères */
//    m_actionTextBold = new QAction(QIcon(":/abuledutextev1/format/bold"), trUtf8("&Gras"), this);
//    m_actionTextBold->setObjectName("bold");
//    m_actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
//    m_actionTextBold->setPriority(QAction::LowPriority);
//    QFont bold;
//    bold.setBold(true);
//    m_actionTextBold->setFont(bold);
//    connect(m_actionTextBold, SIGNAL(triggered()), this, SLOT(setTextFormat()), Qt::UniqueConnection);
//    m_actionTextBold->setCheckable(true);

//    m_actionTextItalic = new QAction(QIcon(":/abuledutextev1/format/italic"), trUtf8("&Italique"), this);
//    m_actionTextItalic->setObjectName("italique");
//    m_actionTextItalic->setPriority(QAction::LowPriority);
//    m_actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
//    QFont italic;
//    italic.setItalic(true);
//    m_actionTextItalic->setFont(italic);
//    connect(m_actionTextItalic, SIGNAL(triggered()), this, SLOT(setTextFormat()), Qt::UniqueConnection);
//    m_actionTextItalic->setCheckable(true);

//    m_actionTextUnderline = new QAction(QIcon(":/abuledutextev1/format/underlined"), trUtf8("&Souligné"), this);
//    m_actionTextUnderline->setObjectName("underline");
//    m_actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
//    m_actionTextUnderline->setPriority(QAction::LowPriority);
//    QFont underline;
//    underline.setUnderline(true);
//    m_actionTextUnderline->setFont(underline);
//    connect(m_actionTextUnderline, SIGNAL(triggered()), this, SLOT(setTextFormat()), Qt::UniqueConnection);
//    m_actionTextUnderline->setCheckable(true);

//    m_btnFontDecrease = new QPushButton();
//    m_btnFontDecrease->setFixedSize(m_hauteurToolBar,m_hauteurToolBar);
//    m_btnFontDecrease->setIcon(QIcon(":/abuledutextev1/format/decrease"));
//    m_btnFontDecrease->setIconSize(QSize(32,32));
//    m_btnFontDecrease->setFlat(true);
//    m_btnFontDecrease->setObjectName("decrease");
//    m_btnFontDecrease->setProperty("interligne",100);
//    connect(m_btnFontDecrease, SIGNAL(clicked()), this, SLOT(decreaseFontSize()), Qt::UniqueConnection);

//    m_btnFontIncrease = new QPushButton();
//    m_btnFontIncrease->setFixedSize(m_hauteurToolBar,m_hauteurToolBar);
//    m_btnFontIncrease->setIcon(QIcon(":/abuledutextev1/format/increase"));
//    m_btnFontIncrease->setIconSize(QSize(32,32));
//    m_btnFontIncrease->setFlat(true);
//    m_btnFontIncrease->setObjectName("increase");
//    m_btnFontIncrease->setProperty("interligne",100);
//    connect(m_btnFontIncrease, SIGNAL(clicked()), this, SLOT(increaseFontSize()), Qt::UniqueConnection);

//    /* Alignement des paragraphes */
//    m_alignActions = new QActionGroup(this);
//    m_alignActions->setObjectName("groupalign");
//    connect(m_alignActions, SIGNAL(triggered(QAction*)), this, SLOT(setTextAlign(QAction*)), Qt::UniqueConnection);

//    /* On modifie la position des icones en fonction du sens du texte LTR ou RTL */
//    if (QApplication::isLeftToRight()) {
//        m_actionAlignLeft = new QAction(QIcon(":/abuledutextev1/format/left"), trUtf8("À gauc&he"), m_alignActions);
//        m_actionAlignCenter = new QAction(QIcon(":/abuledutextev1/format/center"), trUtf8("Au c&entre"), m_alignActions);
//        m_actionAlignRight = new QAction(QIcon(":/abuledutextev1/format/right"), trUtf8("À d&roite"), m_alignActions);
//    } else {
//        m_actionAlignRight = new QAction(QIcon(":/abuledutextev1/format/right"), trUtf8("À d&roite"), m_alignActions);
//        m_actionAlignCenter = new QAction(QIcon(":/abuledutextev1/format/center"), trUtf8("Au c&entre"), m_alignActions);
//        m_actionAlignLeft = new QAction(QIcon(":/abuledutextev1/format/left"), trUtf8("À gauc&he"), m_alignActions);
//    }
//    m_actionAlignJustify = new QAction(QIcon(":/abuledutextev1/format/justify"), trUtf8("&Justifié"), m_alignActions);

//    m_actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
//    m_actionAlignLeft->setObjectName("alignleft");
//    m_actionAlignLeft->setCheckable(true);
//    m_actionAlignLeft->setPriority(QAction::LowPriority);
//    m_actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
//    m_actionAlignCenter->setObjectName("aligncenter");
//    m_actionAlignCenter->setCheckable(true);
//    m_actionAlignCenter->setPriority(QAction::LowPriority);
//    m_actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
//    m_actionAlignRight->setObjectName("alignright");
//    m_actionAlignRight->setCheckable(true);
//    m_actionAlignRight->setPriority(QAction::LowPriority);
//    m_actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
//    m_actionAlignJustify->setObjectName("alignjustify");
//    m_actionAlignJustify->setCheckable(true);
//    m_actionAlignJustify->setPriority(QAction::LowPriority);
//    m_actionAlignLeft->trigger();



//    m_fontActions = new QActionGroup(this);
//    m_fontActions->setObjectName("groupfont");
//    connect(m_fontActions, SIGNAL(triggered(QAction*)), this, SLOT(setTextFamily(QAction*)), Qt::UniqueConnection);
//    /* Pour tablettes, je préfère des boutons ... mais pas les retours de test (philippe 20131129) */
//    m_actionFontAndika = new QAction(QIcon(":/abuledutextev1/format/fontAndika"), trUtf8("&Andika"), m_fontActions);
//    m_actionFontAndika->setObjectName("andika");
//    m_actionFontAndika->setPriority(QAction::LowPriority);
//    m_actionFontAndika->setProperty("interligne",100);
//    m_actionFontAndika->setProperty("defaultPointSize",20);
//    m_actionFontAndika->setCheckable(true);
//    m_actionFontAndika->trigger();

//    m_actionFontSeyes = new QAction(QIcon(":/abuledutextev1/format/fontSeyes"), trUtf8("&Seyes"), m_fontActions);
//    m_actionFontSeyes->setObjectName("SeyesBDE");
//    m_actionFontSeyes->setPriority(QAction::LowPriority);
//    m_actionFontSeyes->setProperty("interligne",200);
//    m_actionFontSeyes->setProperty("defaultPointSize",28);
//    m_actionFontSeyes->setCheckable(true);

//    m_actionFontPlume = new QAction(QIcon(":/abuledutextev1/format/fontPlume"), trUtf8("&Plume"), m_fontActions);
//    m_actionFontPlume->setObjectName("PlumBAE");
//    m_actionFontPlume->setPriority(QAction::LowPriority);
//    m_actionFontPlume->setProperty("interligne",120);
//    m_actionFontPlume->setProperty("defaultPointSize",22);
//    m_actionFontPlume->setCheckable(true);
//}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    QTextCursor cursor = ui->teZoneTexte->textCursor();
    /* Je me contente de commenter ces lignes, c'est à cause d'elles qu'on avait la remarque de Christine C :
     *" dans un traitement de texte classique, si je choisis un formatage de texte (ex : gras) avant de taper quoi que ce soit et que je tape du texte après, je tape du gras. Pas là"
     * Je ne les supprime pas encore en cas qu'elles fassent quelque chose qui manque à quelqu'un */
//    if (!cursor.hasSelection())
//        /* Si pas de sélection, on utilise le mot sous le curseur */
//        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->teZoneTexte->mergeCurrentCharFormat(format);
}

bool MainWindow::fileSave()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    m_isNewFile = false;
    return fileSave();
}

void MainWindow::setCurrentFileName(const QString &fileName)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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

void MainWindow::closeEvent(QCloseEvent *e)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    if(!isWindowModified() || (m_isNewFile && ui->teZoneTexte->toPlainText().isEmpty())){
        e->accept();
        return;
    }
    else
    {
        e->ignore();
        m_isCloseRequested = true;
        AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(trUtf8("Enregistrer le projet"),trUtf8("Le projet comporte des modifications non enregistrées. Voulez-vous sauvegarder ?"),true,ui->stackedWidget->currentWidget());
        msg->abeSetModeEnum(AbulEduMessageBoxV1::abeYesNoCancelButton);
        msg->show();
        connect(msg,SIGNAL(signalAbeMessageBoxYES()),SLOT(on_abeMenuFeuilleBtnSave_clicked()),Qt::UniqueConnection);
        connect(msg,SIGNAL(signalAbeMessageBoxNO()),SLOT(deleteLater()),Qt::UniqueConnection);
    }
}

void MainWindow::setupToolButtons()
{

}

#ifndef QT_NO_PRINTER
void MainWindow::filePrint(QPrinter *printer)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* On imprime */
    ui->teZoneTexte->print(printer);
    /* On affiche un message */
    QString message("Impression en cours");
    AbulEduMessageBoxV1* msgImpression = new AbulEduMessageBoxV1(trUtf8("Impression"), message,true,ui->pagePrint);
    msgImpression->setWink();
    msgImpression->show();
    connect(msgImpression, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(showTextPage()), Qt::UniqueConnection);
}
#endif

void MainWindow::cursorMoved()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

//    updateActions(ui->teZoneTexte->textCursor().charFormat());
}

//void MainWindow::updateActions(QTextCharFormat fmt)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* On actualise les toolButtons ou plutôt les QActions sous-jacentes en fonction du formatage du texte */
//    m_actionTextBold->blockSignals(true);
//    m_actionTextBold->setChecked(fmt.fontWeight() >= QFont::Bold);
//    m_actionTextBold->blockSignals(false);

//    m_actionTextItalic->blockSignals(true);
//    m_actionTextItalic->setChecked(fmt.fontItalic());
//    m_actionTextItalic->blockSignals(false);

//    m_actionTextUnderline->blockSignals(true);
//    m_actionTextUnderline->setChecked(fmt.fontUnderline());
//    m_actionTextUnderline->blockSignals(false);

//    //    m_actionTextColor->blockSignals(true);
//    colorChanged(fmt.foreground().color());
//    //    m_actionTextColor->blockSignals(false);

//    m_actionAlignLeft->blockSignals(true);
//    m_actionAlignLeft->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignLeft));
//    m_actionAlignLeft->blockSignals(false);

//    m_actionAlignCenter->blockSignals(true);
//    m_actionAlignCenter->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignHCenter));
//    m_actionAlignCenter->blockSignals(false);

//    m_actionAlignRight->blockSignals(true);
//    m_actionAlignRight->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignRight));
//    m_actionAlignRight->blockSignals(false);

//    m_actionAlignJustify->blockSignals(true);
//    m_actionAlignJustify->setChecked(ui->teZoneTexte->alignment().testFlag(Qt::AlignJustify));
//    m_actionAlignJustify->blockSignals(false);

    //    m_comboFont->blockSignals(true);
    //    //    m_comboFont->setCurrentFont(fmt.font());
    //    m_comboFont->blockSignals(false);

    //    m_comboSize->blockSignals(true);
    //    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(fmt.font().pointSize())));
    //    m_comboSize->blockSignals(false);
//}

void MainWindow::slotMediathequeDownload(QSharedPointer<AbulEduFileV1> abeFile, int code)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    Q_UNUSED(code)
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

    /* Retour normal */
    QTextBlockFormat blockFormat;
    fmt.setFontItalic(false);
    cursor.insertBlock(blockFormat,fmt);

    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
}

void MainWindow::fileOpen()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->abeBoxFileManager->abeSetOpenOrSaveEnum(AbulEduBoxFileManagerV1::abeOpen);
    ui->abeBoxFileManager->abeRefresh(AbulEduBoxFileManagerV1::abePC);
    ui->stackedWidget->setCurrentWidget(ui->pageBoxFileManager);
    ui->frmFormat->setEnabled(true);
}

void MainWindow::slotOpenFile(QSharedPointer<AbulEduFileV1> abeFile)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__  << abeFile;

    /* Correction du bug de non apparition de l'image */
    if(abeFile)
    {
        m_abuledufile = abeFile;
    }
    if (m_localDebug) {
        qDebug() << "Ouverture du fichier " << m_abuledufile->abeFileGetFileName().filePath();
        qDebug()<<" dont le repertoire temporaire est "<<m_abuledufile->abeFileGetDirectoryTemp().absolutePath();
        qDebug()<<m_fileName;
    }

    m_isNewFile = false;
    setCurrentFileName(m_abuledufile->abeFileGetContent(0).absoluteFilePath());

    /* lecture du fichier html */
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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << fileName << " et " << success;

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
    AbulEduMessageBoxV1* msgEnregistrement = new AbulEduMessageBoxV1(trUtf8("Enregistrement"), message,true,ui->pageBoxFileManager);
    if(success)
    {
        msgEnregistrement->setWink();
    }
    if(m_isCloseRequested)
    {
        connect(msgEnregistrement,SIGNAL(signalAbeMessageBoxCloseOrHide()),this,SLOT(deleteLater()),Qt::UniqueConnection);
    }
    msgEnregistrement->show();
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
    if(m_wantNewFile){
        slotClearCurrent();
        m_wantNewFile = false;
    }
}

void MainWindow::on_abeMenuFeuilleBtnPrint_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

#ifndef QT_NO_PRINTER
    if(!m_printDialog->isVisible())
        m_printDialog->showNormal();
#endif

    ui->stackedWidget->setCurrentWidget(ui->pagePrint);
}

void MainWindow::on_abeMenuFeuilleBtnHelp_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->stackedWidget->setCurrentWidget(ui->pageAbout);
}

void MainWindow::on_abeMenuFeuilleBtnQuit_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    close();
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << arg1;

    if (m_localDebug) qDebug()<<"page courante : "<<ui->stackedWidget->widget(arg1)->objectName();
}

void MainWindow::slotClearCurrent()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    /* Je veux faire un nouveau texte, mais je ne veux pas changer d'abe */
    m_abuledufile->abeCleanDirectory(m_abuledufile->abeFileGetDirectoryTemp().absolutePath(),m_abuledufile->abeFileGetDirectoryTemp().absolutePath());
    ui->teZoneTexte->clear();
    setWindowModified(false);
}

void MainWindow::on_abeMenuFeuilleBtnOpen_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    fileOpen();
}

void MainWindow::on_abeMenuFeuilleBtnSave_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

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
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->abeMediathequeGet->setVisible(true);
    ui->stackedWidget->setCurrentWidget(ui->pageMediathequeGet);
}

void MainWindow::showTextPage()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    ui->stackedWidget->setCurrentWidget(ui->pageTexte);
    ui->frmFormat->setEnabled(true);
}

void MainWindow::slotChangeLangue(QString lang)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << " en "<<lang;
    qApp->removeTranslator(&qtTranslator);
    qApp->removeTranslator(&myappTranslator);

    qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&qtTranslator);
    myappTranslator.load("abuledu-minitexte_" + lang, "lang");
    qApp->installTranslator(&myappTranslator);
    ui->retranslateUi(this);
}

void MainWindow::slotSessionAuthenticated(bool enable)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << enable;

    if(enable)
        abeApp->getAbeNetworkAccessManager()->abeSSOLogin();
}

void MainWindow::on_teZoneTexte_textChanged()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    if(!isWindowModified() && !ui->teZoneTexte->document()->isEmpty()) {
        setWindowModified(true);
    }
}

void MainWindow::slotReadContent()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),QString(),ui->teZoneTexte->toPlainText()));
    m_multimedia->abeMultiMediaPlay();
}
