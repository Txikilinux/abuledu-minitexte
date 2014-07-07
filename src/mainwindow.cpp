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

    /***************************** AbeMediatheque ***************************************/
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

    /***************************** AbeBoxFileManager ***************************************/
    ui->abeBoxFileManager->abeMediathequeGetHideCloseBouton(true);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), this, SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
            this, SLOT(slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), Qt::UniqueConnection);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileCloseOrHide()),this, SLOT(showTextPage()), Qt::UniqueConnection);

    /***************************** MenuFeuille ***************************************/
    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),this,SLOT(slotChangeLangue(QString)),Qt::UniqueConnection);


    //    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)), Qt::UniqueConnection);
    //    /* On émet un signal inquant si le texte a été modifié */
    //    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(somethingHasChangedInText(bool)), Qt::UniqueConnection);

    /* Le curseur a été déplacé*/
    //    connect(ui->teZoneTexte, SIGNAL(cursorPositionChanged()), this, SLOT(cursorMoved()), Qt::UniqueConnection);

    initMultimedia();
    initSignalMapperFontChange();
    initSignalMapperFormFontChange();

    /***************************** Chargement des Fonts ***************************************/
    QFontDatabase fonts;
    if(!fonts.addApplicationFont(":/abuledutextev1/Ecolier")) {
        qDebug() << "Erreur sur :/fonts/ECOLIER.TTF";
    }
    if(!fonts.addApplicationFont(":/abuledutextev1/Cursive")) {
        qDebug() << "Erreur sur :/fonts/CURSIVE.TTF";
    }

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

    /* Font par défaut */
    connect(ui->teZoneTexte, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(slotCurrentCharFormatChanged(QTextCharFormat)));
    m_fontSize = 30;            /* taille par defaut */

    ui->btn_andika->click();    /* Andika par defaut */
    m_textCharFormat = ui->teZoneTexte->textCursor().charFormat();
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
    m_multimedia->abeMultiMediaSetTextVisible(false);
    connect(m_multimedia->abeMultiMediaGetAudioControlWidget(), SIGNAL(signalAbeControlAudioPlayClicked()),this, SLOT(slotReadContent()),Qt::UniqueConnection);

    /** @todo autres langues ? */
    if(m_multimedia->abeMultiMediaGetTTSlang() != AbulEduMultiMediaSettingsV1::fre){
        m_multimedia->abeMultimediaSetTTS(AbulEduMultiMediaSettingsV1::fre);
    }
    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),
            m_multimedia, SLOT(slotAbeMultimediaSetTTSfromIso6391(QString)), Qt::UniqueConnection);
}

void MainWindow::initSignalMapperFontChange()
{
    m_signalMapperFontChange = new QSignalMapper(this);
    connect(m_signalMapperFontChange, SIGNAL(mapped(QString)), SLOT(slotChangeFont(QString)), Qt::UniqueConnection);
    m_signalMapperFontChange->setMapping(ui->btn_andika,  "Andika");
    m_signalMapperFontChange->setMapping(ui->btn_seyes,   "Ecolier_lignes");
    m_signalMapperFontChange->setMapping(ui->btn_plume,   "CursiveStandard");

    connect(ui->btn_andika, SIGNAL(clicked()), m_signalMapperFontChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_seyes, SIGNAL(clicked()), m_signalMapperFontChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_plume, SIGNAL(clicked()), m_signalMapperFontChange, SLOT(map()), Qt::UniqueConnection);
}

void MainWindow::initSignalMapperFormFontChange()
{
    m_signalMapperFontFormChange = new QSignalMapper(this);
    connect(m_signalMapperFontFormChange, SIGNAL(mapped(QString)), SLOT(slotChangeFormFont(QString)), Qt::UniqueConnection);
    m_signalMapperFontFormChange->setMapping(ui->btn_bold, "bold");
    m_signalMapperFontFormChange->setMapping(ui->btn_italic, "italic");
    m_signalMapperFontFormChange->setMapping(ui->btn_underlined, "underlined");

    connect(ui->btn_bold, SIGNAL(clicked()), m_signalMapperFontFormChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_italic, SIGNAL(clicked()), m_signalMapperFontFormChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_underlined, SIGNAL(clicked()), m_signalMapperFontFormChange, SLOT(map()), Qt::UniqueConnection);
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

//void MainWindow::abeTexteSetFontFamily(QString fontFamily)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << fontFamily;

//    //    m_comboFont->setCurrentFont(QFont(fontFamily));
//    //    QAction* act = m_fontActions->findChild<QAction*>(fontFamily);
//    //    ABULEDU_LOG_TRACE() << act->objectName();
//    //    setTextFamily(act);
//}

//QString MainWindow::abeTexteGetFontFamily()
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

//    //    return m_comboFont->font().family();
//}

//void MainWindow::abeTexteSetFontSize(int taille)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << taille;

//    /** @todo Tester taille et l'adapter en fonction des tailles disponibles dans la combobox m_comboSize */
//    //    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(taille)));
//    setTextSize(taille);
//}

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

//void MainWindow::abeTexteSetBold(bool onOff)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onOff;

//    //    m_actionTextBold->setChecked(onOff);
//    //    setTextFormat();
//}

//void MainWindow::abeTexteSetItalic(bool onOff)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onOff;

//    //    m_actionTextItalic->setChecked(onOff);
//    //    setTextFormat();
//}

//void MainWindow::abeTexteSetUnderline(bool onOff)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << onOff;

//    //    m_actionTextUnderline->setChecked(onOff);
//    //    setTextFormat();
//}

//void MainWindow::setTextFormat()
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

//    /* On crée le format à appliquer */
//    //    QTextCharFormat fmt;
//    //    fmt.setFontWeight(m_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
//    //    fmt.setFontItalic(m_actionTextItalic->isChecked());
//    //    fmt.setFontUnderline(m_actionTextUnderline->isChecked());
//    //    /* On l'applique */
//    //    mergeFormatOnWordOrSelection(fmt);
//}

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

//void MainWindow::setTextFamily(QAction* action)
//{
//    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << action;

//    QString f = action->objectName();
//    if (m_localDebug) qDebug() << " Fonte : " << f;
//    /* On applique le format de font sélectionnée */
//    QTextCharFormat fmt;
//    fmt.setFontFamily(f);
//    mergeFormatOnWordOrSelection(fmt);
//    setTextSize(action->property("defaultPointSize").toInt());

//    /* Espacement vertical different */
//    QTextBlockFormat format;
//#if QT_VERSION >= 0x040700
//    format.setLineHeight(action->property("interligne").toInt(), QTextBlockFormat::ProportionalHeight);
//#endif
//    QTextCursor curseur = ui->teZoneTexte->textCursor();
//    curseur.setBlockFormat(format);
//}

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

void MainWindow::slotReadContent()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;

    m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),QString(),ui->teZoneTexte->toPlainText()));
    m_multimedia->abeMultiMediaPlay();
}

/*************************************************************************************************************************************
 *
 *  GESTION CHANGEMENT POLICE
 *
 * ***********************************************************************************************************************************/
void MainWindow::slotChangeFont(const QString &font)
{
    qDebug() << "On change de FONT ::" << font;
    //    m_fontFamily = font;

    qDebug() << ui->teZoneTexte->textCursor().charFormat();

    //    QTextCharFormat tcf;
    //    tcf.setFontFamily(font);
    //    tcf.setFont(font);
    //    tcf.setFontPointSize(m_fontSize);
    //    m_textCharFormat = tcf;
    //    mergeFormatOnWordOrSelection(tcf);

    m_textCharFormat.setFontFamily(font);
    m_textCharFormat.setFont(font);
    m_textCharFormat.setFontPointSize(m_fontSize);

    mergeFormatOnWordOrSelection(m_textCharFormat );
}

void MainWindow::slotChangeFormFont(const QString &form)
{
    qDebug() << "On change de form ::" << form;

    /* On crée le format à appliquer */
    //    QTextCharFormat fmt;
    //    fmt.setFontWeight(ui->btn_bold->isChecked() ? QFont::Bold : QFont::Normal);
    //    fmt.setFontItalic(ui->btn_italic->isChecked());
    //    fmt.setFontUnderline(ui->btn_underlined->isChecked());
    //    /* On l'applique */
    //    mergeFormatOnWordOrSelection(fmt);

    m_textCharFormat.setFontWeight(ui->btn_bold->isChecked() ? QFont::Bold : QFont::Normal);
    m_textCharFormat.setFontItalic(ui->btn_italic->isChecked());
    m_textCharFormat.setFontUnderline(ui->btn_underlined->isChecked());
    /* On l'applique */
    mergeFormatOnWordOrSelection(m_textCharFormat);
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;

    QTextCursor cursor = ui->teZoneTexte->textCursor();
    //    if (cursor.hasSelection())
    //        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->teZoneTexte->mergeCurrentCharFormat(format);
}

void MainWindow::slotCurrentCharFormatChanged(QTextCharFormat tcf)
{
    qDebug() << "############################################"<<tcf.fontFamily() << m_textCharFormat.fontFamily() << ui->teZoneTexte->currentFont() ;
    /* Bouton bold */
    ui->btn_bold->setChecked((tcf.fontWeight() > 50));
    /* Bouton underlined */
    ui->btn_underlined->setChecked(tcf.fontUnderline());
    /* Bouton Italic */
    ui->btn_italic->setChecked(tcf.fontItalic());

    /*  Ici gérer les changements de police et repercuter sur interface */
    if (tcf.fontFamily() == "CursiveStandard" ){
        ui->btn_plume->setChecked(true);
    }
    else if (tcf.fontFamily()  ==  "Andika") {
        ui->btn_andika->setChecked(true);
    }
    else if (tcf.fontFamily() ==  "Ecolier_lignes" ){
        ui->btn_seyes->setChecked(true);
    }

    if(/*m_textCharFormat.fontFamily() != tcf.fontFamily() && */ui->teZoneTexte->toPlainText().isEmpty()){
        qDebug() << "+++++++++++++++++++++++++++++++   +++++++++++++++++++++++++  " << "C'est mon cas ";
        mergeFormatOnWordOrSelection(m_textCharFormat);
        //        m_textCharFormat = tcf;
    }
}

void MainWindow::on_teZoneTexte_textChanged()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;
    //    if(!isWindowModified() && !ui->teZoneTexte->document()->isEmpty()) {
    //        setWindowModified(true);
    //    }

    //    qDebug() << ui->teZoneTexte->textCursor().blockCharFormat().font()
    //             << ui->teZoneTexte->textCursor().blockCharFormat().verticalAlignment()
    //             << ui->teZoneTexte->textCursor().blockCharFormat().fontFixedPitch();
}

void MainWindow::slotChangeFontSize(int newSize)
{
    qDebug() << "+++++++++++++++++++++ " << newSize ;
    qreal pointSize = newSize;
    if(newSize > 0){
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MainWindow::on_btn_increase_clicked()
{
    m_fontSize += 2;
    slotChangeFontSize(m_fontSize);
}

void MainWindow::on_btn_decrease_clicked()
{
    m_fontSize -= 2;
    slotChangeFontSize(m_fontSize);
}
