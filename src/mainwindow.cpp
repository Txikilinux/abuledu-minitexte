/** MiniTexte pour Tablette
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/abuledu-minitexte
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @author 2012-2015 Eric Seigne <eric.seigne@ryxeo.com>
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
    ui(new Ui::MainWindow),
    m_fileName(),
    m_hauteurToolBar(48),
    m_isNewFile(true),
    m_isCloseRequested(false),
    m_wantNewFile(false),
    m_wantOpenFile(false),
    m_abuledufile(0),
    m_multimedia(0),
#ifndef QT_NO_PRINTER
      /** Gestion Impression */
      m_printDialog(0),
      m_printer(0),
#endif
      m_fileDialog(0),
  qtTranslator(0),
  myappTranslator(0),
  m_locale(QString()),
  m_signalMapperFontChange(0),
  m_signalMapperFontFormChange(0),
  m_signalMapperTextAlignChange(0),
  m_fontSize(0),
  m_textCharFormat(QTextCharFormat()),
  m_listColors(0),
  m_textToSpeech(QString()),
  m_lastOpenDir(QDir::homePath())

{
    setAttribute(Qt::WA_QuitOnClose);
    ui->setupUi(this);
    ui->teZoneTexte->installEventFilter(this);
    setWindowFlags(Qt::FramelessWindowHint);

#ifdef Q_OS_WIN
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_2000: ABULEDU_LOG_DEBUG()<< "Windows 2000";break;
    case QSysInfo::WV_XP: ABULEDU_LOG_DEBUG()<< "Windows XP";break;
    case QSysInfo::WV_VISTA: ABULEDU_LOG_DEBUG()<< "Windows Vista";break;
    case QSysInfo::WV_WINDOWS7: ABULEDU_LOG_DEBUG()<< "Windows Seven";break;
    case QSysInfo::WV_WINDOWS8: ABULEDU_LOG_DEBUG()<< "Windows 8";break;
    default: ABULEDU_LOG_DEBUG()<< "Windows";break;
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

    /* L'ajout d'image depuis le disque dur: on evite la popup en integrant le qfiledialog dans notre widget */
    QFileDialog *f = new QFileDialog(this, trUtf8("Sélectionner une image"),
                                     m_lastOpenDir,trUtf8("Fichiers Images (*.png *.jpg *.JPG *.jpeg *.svg)"));
    f->setOption(QFileDialog::DontUseNativeDialog, true);
    ui->tabWidgetImagesPage1Layout->addWidget(f);
    //Si on ne fait pas ca le dialog "disparait" quand on clique sur un fichier
    connect(f, SIGNAL(accepted()), f, SLOT(show()),Qt::UniqueConnection);
    connect(f, SIGNAL(rejected()), f, SLOT(show()),Qt::UniqueConnection);
    connect(f, SIGNAL(rejected()), this, SLOT(showTextPage()),Qt::UniqueConnection);
    //et on importe une image quand c'est le cas
    connect(f, SIGNAL(fileSelected(QString)), this, SLOT(slotLocalFileDialogSelected(QString)),Qt::UniqueConnection);
    ui->tabWidgetImages->setCurrentWidget(ui->tabWidgetImagesPage2);

    m_abuledufile = QSharedPointer<AbulEduFileV1>(new AbulEduFileV1, &QObject::deleteLater);
    setCurrentFileName(m_abuledufile->abeFileGetDirectoryTemp().absolutePath() + "/document.html");

    /***************************** AbeBoxFileManager ***************************************/
    ui->abeBoxFileManager->abeMediathequeGetHideCloseBouton(true);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)),
            SLOT(slotOpenFile(QSharedPointer<AbulEduFileV1>)), Qt::UniqueConnection);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)),
            SLOT(slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation,QString,bool)), Qt::UniqueConnection);
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileCloseOrHide()),
            SLOT(showTextPage()), Qt::UniqueConnection);

    /***************************** MenuFeuille ***************************************/
    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),
            SLOT(slotChangeLangue(QString)),Qt::UniqueConnection);

    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)),
            SLOT(setWindowModified(bool)), Qt::UniqueConnection);

    /* Le curseur a été déplacé*/
    connect(ui->teZoneTexte, SIGNAL(cursorPositionChanged()), SLOT(slotCursorMoved()), Qt::UniqueConnection);

    /* #3933 Affichage texte au chargement d'un abe*/
    connect(ui->abeBoxFileManager, SIGNAL(signalAbeFileSelected(QSharedPointer<AbulEduFileV1>)), SLOT(showTextPage()), Qt::UniqueConnection);

    initMultimedia();
    initSignalMapperFontChange();
    initSignalMapperFormFontChange();
    initSignalMapperTextAlignChange();
    ui->frmMenuFeuille->abeMenuFeuilleSetTitle(abeApp->getAbeApplicationLongName()+ " -- " + trUtf8("Sans nom"));
    ui->frTopMicroTexte->setMinimumWidth(500);

    /***************************** Chargement des Fonts ***************************************/
    QFontDatabase fonts;
    if(!fonts.addApplicationFont(":/abuledutextev1/Ecolier")) {ABULEDU_LOG_DEBUG()<<"Erreur :/fonts/ECOLIER.TTF";}
    if(!fonts.addApplicationFont(":/abuledutextev1/Cursive")) {ABULEDU_LOG_DEBUG()<<"Erreur :/fonts/CURSIVE.TTF";}

#ifndef QT_NO_PRINTER
    /* Gestion Impression */
    m_printer = new QPrinter(QPrinter::HighResolution);
    m_printDialog = new QPrintDialog(m_printer /*, this*/);
    m_printDialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    m_printDialog->setStyleSheet("background-color:#FFFFFF");
    /* #4077 -> impossible d'integrer QPrintDialog dans un widget sous windows */
    #ifndef Q_OS_WIN
        ui->glPrint->addWidget(m_printDialog);
        connect(m_printDialog, SIGNAL(rejected()), this, SLOT(showTextPage()), Qt::UniqueConnection);
        connect(m_printDialog, SIGNAL(accepted(QPrinter*)), this, SLOT(filePrint(QPrinter*)), Qt::UniqueConnection);
    #endif
#endif
    m_fileDialog = new QFileDialog(this, trUtf8("Exporter sous..."),
                                                      QDir::homePath(), trUtf8("ODF files (*.odt)"));
    m_fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    ui->glFileDialog->addWidget(m_fileDialog);
    m_fileDialog->setVisible(true);
    connect(m_fileDialog, SIGNAL(accepted()), m_fileDialog, SLOT(show()), Qt::UniqueConnection);
    connect(m_fileDialog, SIGNAL(rejected()), m_fileDialog, SLOT(show()), Qt::UniqueConnection);
    connect(m_fileDialog, SIGNAL(accepted()), this, SLOT(showTextPage()),Qt::UniqueConnection);
    connect(m_fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(slotExportAsOdt(QString)),Qt::UniqueConnection);
    connect(m_fileDialog, SIGNAL(rejected()), this, SLOT(showTextPage()),Qt::UniqueConnection);

#ifndef __ABULEDUTABLETTEV1__MODE__
    /* On Centre la fenetre */
    abeApp->abeCenterWindow(this);
    ui->teZoneTexte->setFocus();
#endif

    /***************************** Gestion Couleur ***************************************/
    initComboBoxColor(ui->cb_colorChooser);
    connect( ui->cb_colorChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeColor(int)), Qt::UniqueConnection);

    /***************************** Gestion du retour de la page à propos ***************************************/
    connect(ui->pageAbout, SIGNAL(signalAbeAproposBtnCloseClicked()), this, SLOT(showTextPage()),Qt::UniqueConnection);

    /***************************** Gestion Bouton Data ***************************************/
    connect(ui->btn_data, SIGNAL(clicked()), SLOT(showAbeMediathequeGet()), Qt::UniqueConnection);

    /***************************** Page par défaut ***************************************/
    ui->stackedWidget->setCurrentWidget(ui->pageTexte);

    /***************************** Font par défaut ***************************************/
    connect(ui->teZoneTexte, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(slotCurrentCharFormatChanged(QTextCharFormat)));
    m_fontSize = 30;            /* taille par defaut */

    ui->btn_andika->click();    /* Andika par defaut */
    ui->btn_leftText->click();  /* Alignement à gauche par défaut */
    m_textCharFormat = ui->teZoneTexte->textCursor().charFormat();

    ui->frTopMicroTexte->hide();
    ui->lblTitreMicroTexte->hide();
    setWindowModified(false);
}

void MainWindow::initMultimedia()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_multimedia = new AbulEduMultiMediaV1(AbulEduMultiMediaV1::Sound, ui->frmControlAudio);
    m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetSpeedControlVisible(true);
    m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioGetFrameSpeed()->setStyleSheet("color:#0a73f4;");
    m_multimedia->abeMultiMediaGetAudioControlWidget()->abeControlAudioSetDirection(QBoxLayout::TopToBottom);
    m_multimedia->abeMultiMediaSetButtonVisible(AbulEduMultiMediaV1::BtnMagnifyingGlass | AbulEduMultiMediaV1::BtnPrevious | AbulEduMultiMediaV1::BtnNext | AbulEduMultiMediaV1::BtnHide | AbulEduMultiMediaV1::BtnRecord,false);
    m_multimedia->abeMultiMediaForceStop();
    m_multimedia->abeMultiMediaSetTextVisible(false);
    connect(m_multimedia->abeMultiMediaGetAudioControlWidget(), SIGNAL(signalAbeControlAudioPlayClicked()),this, SLOT(slotReadContent()),Qt::UniqueConnection);

    /** @todo autres langues ? */
    if(m_multimedia->abeMultiMediaGetTTSlang() != AbulEduPicottsV1::fr){
        m_multimedia->abeMultimediaSetTTS(AbulEduPicottsV1::fr);
    }
    connect(ui->frmMenuFeuille, SIGNAL(signalAbeMenuFeuilleChangeLanguage(QString)),
            m_multimedia, SLOT(slotAbeMultimediaSetTTSfromIso6391(QString)), Qt::UniqueConnection);
}

void MainWindow::initSignalMapperFontChange()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_signalMapperFontChange = new QSignalMapper(this);
    connect(m_signalMapperFontChange, SIGNAL(mapped(QString)), SLOT(slotChangeFont(QString)), Qt::UniqueConnection);
    m_signalMapperFontChange->setMapping(ui->btn_andika,  "Andika");
    m_signalMapperFontChange->setMapping(ui->btn_seyes,   "Ecolier_lignes");
    m_signalMapperFontChange->setMapping(ui->btn_plume,   "Cursive standard");

    connect(ui->btn_andika, SIGNAL(clicked()), m_signalMapperFontChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_seyes, SIGNAL(clicked()), m_signalMapperFontChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_plume, SIGNAL(clicked()), m_signalMapperFontChange, SLOT(map()), Qt::UniqueConnection);
}

void MainWindow::initSignalMapperFormFontChange()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_signalMapperFontFormChange = new QSignalMapper(this);
    connect(m_signalMapperFontFormChange, SIGNAL(mapped(QString)), SLOT(slotChangeFormFont(QString)), Qt::UniqueConnection);
    m_signalMapperFontFormChange->setMapping(ui->btn_bold, "bold");
    m_signalMapperFontFormChange->setMapping(ui->btn_italic, "italic");
    m_signalMapperFontFormChange->setMapping(ui->btn_underlined, "underlined");

    connect(ui->btn_bold, SIGNAL(clicked()), m_signalMapperFontFormChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_italic, SIGNAL(clicked()), m_signalMapperFontFormChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_underlined, SIGNAL(clicked()), m_signalMapperFontFormChange, SLOT(map()), Qt::UniqueConnection);
}

void MainWindow::initSignalMapperTextAlignChange()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_signalMapperTextAlignChange = new QSignalMapper(this);
    connect(m_signalMapperTextAlignChange, SIGNAL(mapped(QString)), SLOT(slotChangeTextAlign(QString)), Qt::UniqueConnection);
    m_signalMapperTextAlignChange->setMapping(ui->btn_leftText, "left");
    m_signalMapperTextAlignChange->setMapping(ui->btn_centerText, "center");
    m_signalMapperTextAlignChange->setMapping(ui->btn_rightText, "right");
    m_signalMapperTextAlignChange->setMapping(ui->btn_justifyText, "justify");

    connect(ui->btn_leftText, SIGNAL(clicked()), m_signalMapperTextAlignChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_centerText, SIGNAL(clicked()), m_signalMapperTextAlignChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_rightText, SIGNAL(clicked()), m_signalMapperTextAlignChange, SLOT(map()), Qt::UniqueConnection);
    connect(ui->btn_justifyText, SIGNAL(clicked()), m_signalMapperTextAlignChange, SLOT(map()), Qt::UniqueConnection);
}

void MainWindow::initComboBoxColor(QComboBox *cb)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(!cb) return;

//    const QStringList colorNames = QColor::colorNames();
//    int index = 0;
//    foreach (const QString &colorName, colorNames) {
//        const QColor color(colorName);
//        cb->addItem(colorName, color);
//        const QModelIndex idx = cb->model()->index(index++, 0);
//        cb->model()->setData(idx, color, Qt::BackgroundColorRole);
//    }

    m_listColors << "black"<<  "white" << "darkGray" << "gray" <<  "lightGray" << "red"
                 << "green" << "blue" << "cyan" << "magenta" << "yellow" << "darkRed"
                 << "darkGreen" << "darkBlue" << "darkCyan" << "darkMagenta" ;

    int index = 0;
    foreach (const QString &colorName, m_listColors) {
        const QColor color(colorName);
        cb->addItem("",color);
        const QModelIndex idx = cb->model()->index(index++, 0);
        cb->model()->setData(idx, color, Qt::BackgroundColorRole);
        ABULEDU_LOG_DEBUG() << color;
    }

    /* Par défaut, couleur noire */
    slotChangeColor(0);
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

bool MainWindow::fileSave()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    setCurrentFileName(m_abuledufile->abeFileGetDirectoryTemp().absolutePath() + "/document.html");
    ABULEDU_LOG_DEBUG() << "Ecriture dans le fichier " << m_fileName;

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

bool MainWindow::abeTexteInsertImage(const QString &cheminImage, qreal width, qreal height, const QTextFrameFormat::Position &position, QString name)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    const QFile fichier(cheminImage);
    if(!fichier.exists()){
        ABULEDU_LOG_DEBUG() << "Le fichier n'existe pas" << cheminImage;
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
         * ce qui devrait donner plus de souplesse peut-être :-D */
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
        msg->abeMessageBoxSetMultimedia();
        msg->show();
        connect(msg,SIGNAL(signalAbeMessageBoxYES()),SLOT(on_abeMenuFeuilleBtnSave_clicked()),Qt::UniqueConnection);
        connect(msg,SIGNAL(signalAbeMessageBoxNO()),SLOT(deleteLater()),Qt::UniqueConnection);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_V:
            if(keyEvent->modifiers() == Qt::ControlModifier && !keyEvent->isAutoRepeat())
            {
                if (QApplication::clipboard()->mimeData()->hasImage())
                {
                    if(QApplication::clipboard()->image().size().isNull()){
                        AbulEduMessageBoxV1::showInformation(trUtf8("Impossible de coller l'image, sans taille est probablement trop importante."));
                        return false;
                    }
                    else{
                        QFileInfo fi(m_fileName);
                        QDir rep(fi.absolutePath() + "/data/");
                        if(!rep.exists()) {
                            rep.mkpath(fi.absolutePath() + "/data/");
                        }
                        QString timeStamp = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
                        QApplication::clipboard()->image().save(fi.absolutePath()+QString("/data/imageCollee%1.png").arg(timeStamp));
                        addPicture(QString("imageCollee%1.png").arg(timeStamp),fi.absolutePath()+QString("/data/imageCollee%1.png").arg(timeStamp));
                        return true;
                    }
                }
                else {
                    return QObject::eventFilter(obj,event);
                }
            }
            else {
                return QObject::eventFilter(obj,event);
            }
            break;

        default:
            return QObject::eventFilter(obj,event);
            break;
        }

    }
    else return QObject::eventFilter(obj,event);
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
    msgImpression->abeMessageBoxSetMultimedia();
    msgImpression->setWink();
    msgImpression->show();
    connect(msgImpression, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(showTextPage()), Qt::UniqueConnection);
}
#endif

void MainWindow::slotCursorMoved()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* Répercussions graphiques de l'alignement */
    if(ui->teZoneTexte->alignment().testFlag(Qt::AlignLeft)){
        ABULEDU_LOG_DEBUG() << "TEST GAUCHE OK";
        ui->btn_leftText->click();
    }
    else if(ui->teZoneTexte->alignment().testFlag(Qt::AlignHCenter)){
        ABULEDU_LOG_DEBUG() << "TEST CENTER OK";
        ui->btn_centerText->click();
    }
    else if(ui->teZoneTexte->alignment().testFlag(Qt::AlignRight)){
        ABULEDU_LOG_DEBUG() << "TEST RIGHT OK";
        ui->btn_rightText->click();
    }
    else if(ui->teZoneTexte->alignment().testFlag(Qt::AlignJustify)){
        ABULEDU_LOG_DEBUG() << "TEST JUSTIFY OK";
        ui->btn_justifyText->click();
    }
}

void MainWindow::slotMediathequeDownload(QSharedPointer<AbulEduFileV1> abeFile, int code)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__<< code;
    QString file = abeFile->abeFileGetContent(0).absoluteFilePath();
    QString filename = abeFile->abeFileGetContent(0).baseName() + ".png";

    ABULEDU_LOG_DEBUG() << "slotMediathequeDownload : " << file << " et " << filename;

    addPicture(filename,file);
    /*Les sources et l'auteur (?) */
    QTextCursor cursor = ui->teZoneTexte->textCursor();
    QTextListFormat listFormat;
    cursor.insertList(listFormat);
    QTextCharFormat fmt;
    fmt.setFontPointSize(8);
    fmt.setFontItalic(true);
    cursor.insertText("Source: " + abeFile->abeFileGetIdentifier() +";"+ "\n",fmt);
    cursor.insertText("Auteur: " + abeFile->abeFileGetCreator() +";",fmt);
    fmt = m_textCharFormat;
    cursor.insertText("",fmt);

    /* Retour normal */
    QTextBlockFormat blockFormat;
    fmt.setFontItalic(false);
    cursor.insertBlock(blockFormat,fmt);
}


void MainWindow::addPicture(QString filename, QString file)
{
    QImage image;
    if(file.isNull()){
        image = QImageReader ( filename ).read().scaledToWidth(150,Qt::SmoothTransformation);
    }
    else{
        image = QImageReader ( file ).read().scaledToWidth(150,Qt::SmoothTransformation);
    }

    QFileInfo fi(m_fileName);
    /* 20151221 Philippe et Jean-Louis : on fait le choix d'enregistrer toutes les images en png à cause de problème survenus à l'enregistrement d'images jpg venues du disque dur */
    QString imageDest = QString("%1/data/%2").arg(fi.absolutePath()).arg(filename);
    QFileInfo fi2(imageDest);
    QString imageDestBis = QString("%1/data/%2.png").arg(fi.absolutePath()).arg(fi2.baseName());
    const QUrl Uri ( QString ( "mydata://data/%1.png" ).arg (fi2.baseName()) );

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
    if(!image.save(imageDestBis)) {
        //        if (m_localDebug) qDebug() << "******* ERREUR de sauvegarde de " << imageDest;
    }
    ABULEDU_LOG_DEBUG() << "Sauvegarde de l'image dans " << imageDestBis;

    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);


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
        ui->frmMenuFeuille->abeMenuFeuilleSetTitle(abeApp->getAbeApplicationLongName()+ " -- " + m_abuledufile->abeFileGetFileName().fileName());
    }

    ABULEDU_LOG_DEBUG() << "Ouverture du fichier " << m_abuledufile->abeFileGetFileName().filePath()
                        <<" dont le repertoire temporaire est "<<m_abuledufile->abeFileGetDirectoryTemp().absolutePath()
                        << m_fileName;

    m_isNewFile = false;
    setCurrentFileName(m_abuledufile->abeFileGetContent(0).absoluteFilePath());

    /* lecture du fichier html */
    QFile  htmlFile(m_fileName);
    if (!htmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    QString htmlContent;
    QTextStream in(&htmlFile);
#ifdef Q_OS_WIN
    in.setCodec(QTextCodec::codecForName("UTF-8"));
#endif
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
            ABULEDU_LOG_DEBUG() << " ++ " << fi.absoluteFilePath() << " en tant que " << Uri;
        }
    }
    ui->teZoneTexte->update();
    setWindowModified(false);
}

void MainWindow::slotAbeFileSaved(AbulEduBoxFileManagerV1::enumAbulEduBoxFileManagerSavingLocation location, QString fileName, bool success)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << location << fileName << " et " << success ;
    QString emplacement;

    qDebug() << ui->abeBoxFileManager->abeBoxFileManagerGetSavingLocation();
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
    msgEnregistrement->abeMessageBoxSetMultimedia();
    /* #3935 Retour pageTexte apres appui bouton fermer */
    if(!m_wantOpenFile){
        connect(msgEnregistrement, SIGNAL(signalAbeMessageBoxCloseOrHide()), this, SLOT(showTextPage()), Qt::UniqueConnection);
    }

    if(success){
        msgEnregistrement->setWink();
    }
    if(m_isCloseRequested){
        connect(msgEnregistrement,SIGNAL(signalAbeMessageBoxCloseOrHide()),this,SLOT(deleteLater()),Qt::UniqueConnection);
    }

    msgEnregistrement->show();
    if(m_wantNewFile){
        slotClearCurrent();
        m_wantNewFile = false;
    }
    if(m_wantOpenFile){
        fileOpen();
        m_wantOpenFile = false;
    }
    ui->frmMenuFeuille->abeMenuFeuilleSetTitle(abeApp->getAbeApplicationLongName()+ " -- " + m_abuledufile->abeFileGetFileName().fileName());
}

void MainWindow::on_abeMenuFeuilleBtnPrint_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
#ifndef QT_NO_PRINTER
    if(!m_printDialog->isVisible())
        m_printDialog->showNormal();
#endif
#ifndef Q_OS_WIN32
    /* #4077 -> impossible d'integrer QPrintDialog dans un widget sous windows */
    ui->stackedWidget->setCurrentWidget(ui->pagePrint);
#endif
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
    ABULEDU_LOG_DEBUG() << "page courante : "<<ui->stackedWidget->widget(arg1)->objectName();
}

void MainWindow::slotClearCurrent()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /* Je veux faire un nouveau texte, mais je ne veux pas changer d'abe */
    m_abuledufile->abeCleanDirectoryRecursively(m_abuledufile->abeFileGetDirectoryTemp().absolutePath());
    ui->frmMenuFeuille->abeMenuFeuilleSetTitle(abeApp->getAbeApplicationLongName()+ " -- " + trUtf8("Sans nom"));
    ui->teZoneTexte->clear();
    setWindowModified(false);
}

void MainWindow::on_abeMenuFeuilleBtnOpen_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(isWindowModified()){
        m_wantOpenFile = true;
        AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(trUtf8("Ouvrir un projet"),trUtf8("Le projet actuel comporte des modifications non enregistrées. Voulez-vous sauvegarder ?"),true,ui->stackedWidget->currentWidget());
        msg->abeSetModeEnum(AbulEduMessageBoxV1::abeYesNoCancelButton);
        msg->abeMessageBoxSetMultimedia();
        msg->show();
        connect(msg,SIGNAL(signalAbeMessageBoxYES()),SLOT(on_abeMenuFeuilleBtnSave_clicked()),Qt::UniqueConnection);
        connect(msg,SIGNAL(signalAbeMessageBoxNO()),SLOT(fileOpen()),Qt::UniqueConnection);
    }
    else{
        fileOpen();
    }
    return;
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

void MainWindow::on_abeMenuFeuilleBtnExport_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageFileDialog);
}


void MainWindow::slotExportAsOdt(QString fileName)
{
    if (fileName.isEmpty()){
        return;
    }
    if (!fileName.endsWith(".odt", Qt::CaseInsensitive)){
        fileName += ".odt";
    }
    QTextDocumentWriter writer(fileName);
    writer.write(ui->teZoneTexte->document());
    return ;
}

void MainWindow::slotLocalFileDialogSelected(QString fileName)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    QFileInfo fi(fileName);
    if(fi.exists())
    {
        addPicture(fi.fileName(),fi.absoluteFilePath());
        m_lastOpenDir = fi.absolutePath();
    }
}

void MainWindow::on_abeMenuFeuilleBtnNew_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    if(isWindowModified()){
        m_wantNewFile = true;
        AbulEduMessageBoxV1* msg = new AbulEduMessageBoxV1(trUtf8("Nouveau projet"),trUtf8("Le projet comporte des modifications non enregistrées. Voulez-vous sauvegarder ?"),true,ui->stackedWidget->currentWidget());
        msg->abeMessageBoxSetMultimedia();
        msg->abeSetModeEnum(AbulEduMessageBoxV1::abeYesNoCancelButton);
        msg->show();
        connect(msg,SIGNAL(signalAbeMessageBoxYES()),SLOT(on_abeMenuFeuilleBtnSave_clicked()),Qt::UniqueConnection);
        connect(msg,SIGNAL(signalAbeMessageBoxNO()),SLOT(slotClearCurrent()),Qt::UniqueConnection);
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

void MainWindow::slotChangeLangue(const QString &lang)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << " en "<<lang;
     /* #3766 : le retranslateUi() efface tout le texte */
    const QString textToReplace = ui->teZoneTexte->document()->toHtml();
    qApp->removeTranslator(&qtTranslator);
    qApp->removeTranslator(&myappTranslator);
    qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&qtTranslator);
    myappTranslator.load("abuledu-minitexte_" + lang, "lang");
    qApp->installTranslator(&myappTranslator);
    ui->retranslateUi(this);
    ui->teZoneTexte->setHtml(textToReplace);
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
    (ui->teZoneTexte->textCursor().hasSelection()) ? (m_textToSpeech = ui->teZoneTexte->textCursor().selectedText()) : (m_textToSpeech = ui->teZoneTexte->toPlainText());

    if(m_textToSpeech.isEmpty()) return;

    while(m_textToSpeech.indexOf("Source:",Qt::CaseSensitive) > -1){
        int beginning = m_textToSpeech.indexOf("Source:",Qt::CaseSensitive);
        int end = m_textToSpeech.indexOf(";",m_textToSpeech.indexOf("Source:",Qt::CaseSensitive),Qt::CaseSensitive);
        m_textToSpeech.remove(m_textToSpeech.mid(beginning,1+end-beginning));
        beginning = m_textToSpeech.indexOf("Auteur:",Qt::CaseSensitive);
        end = m_textToSpeech.indexOf(";",m_textToSpeech.indexOf("Auteur:",Qt::CaseSensitive),Qt::CaseSensitive);
        m_textToSpeech.remove(m_textToSpeech.mid(beginning,1+end-beginning));
    }
    m_multimedia->abeMultiMediaSetNewMedia(AbulEduMediaMedias(QString(),QString(),m_textToSpeech));
    m_multimedia->abeMultiMediaPlay();
    m_textToSpeech.clear();
}

/*************************************************************************************************************************************
 *
 *  GESTION CHANGEMENT POLICE
 *
 * ***********************************************************************************************************************************/
void MainWindow::slotChangeFont(const QString &font)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << font << ui->teZoneTexte->textCursor().charFormat();
    m_textCharFormat.setFontFamily(font);
    m_textCharFormat.setFont(font);
    m_textCharFormat.setFontPointSize(m_fontSize);

    if(ui->btnMajusculeMicroTexte->isChecked())
        m_textCharFormat.setFontCapitalization(QFont::AllUppercase);
    else if(ui->btnMinusculeMicroTexte->isChecked())
        m_textCharFormat.setFontCapitalization(QFont::AllLowercase);

    mergeFormatOnWordOrSelection(m_textCharFormat);
}

void MainWindow::slotChangeFormFont(const QString &form)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__  << form;
    /* On crée le format à appliquer */
    m_textCharFormat.setFontWeight(ui->btn_bold->isChecked() ? QFont::Bold : QFont::Normal);
    m_textCharFormat.setFontItalic(ui->btn_italic->isChecked());
    m_textCharFormat.setFontUnderline(ui->btn_underlined->isChecked());
    /* On l'applique */
    mergeFormatOnWordOrSelection(m_textCharFormat);
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    /** 20150713 [ich]: pourquoi remettrait-on le curseur au début ? (d'ailleurs je n'ai pas vu où cela sert ! */
    /* mettre le cursor au début */
//    QTextCursor cursor = ui->teZoneTexte->textCursor();
//    cursor.setPosition(QTextCursor::Start);
//    while(!cursor.position() == QTextCursor::End){
//        cursor.select(QTextCursor::WordUnderCursor);
//        cursor.mergeCharFormat(format);
//        cursor.movePosition(QTextCursor::Right);
//    }

    ui->teZoneTexte->mergeCurrentCharFormat(format);
}

void MainWindow::slotCurrentCharFormatChanged(QTextCharFormat tcf)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ /*<< tcf.fontFamily() << m_textCharFormat.fontFamily() << ui->teZoneTexte->currentFont()*/ ;

    if(ui->teZoneTexte->textCursor().hasSelection())
        return;

    /* Bouton bold */
    ui->btn_bold->setChecked((tcf.fontWeight() > 50));
    /* Bouton underlined */
    ui->btn_underlined->setChecked(tcf.fontUnderline());
    /* Bouton Italic */
    ui->btn_italic->setChecked(tcf.fontItalic());

    /*  Ici gérer les changements de police et repercuter sur interface */
    if (tcf.fontFamily() == "Cursive standard" ){
        ui->btn_plume->setChecked(true);
        ui->btnCursiveMicroTexte->setChecked(true);
    }
    else if (tcf.fontFamily()  ==  "Andika") {
        ui->btn_andika->setChecked(true);
    }
    else if (tcf.fontFamily() ==  "Ecolier_lignes" ){
        ui->btn_seyes->setChecked(true);
    }

    ABULEDU_LOG_DEBUG() << "Alignement:"  <<  ui->teZoneTexte->alignment()
                        << "Color:"       <<  ui->teZoneTexte->textCursor().charFormat().foreground().color();

    /* Definition de la bonne couleur dans la comboBox suivant celle présente sous le curseur */
    int index = ui->cb_colorChooser->findData(ui->teZoneTexte->textCursor().charFormat().foreground().color());
    if ( index != -1 ) {
        ui->cb_colorChooser->setCurrentIndex(index);
    }

    /* Repercussions Majuscule/Minuscule/Cursive Microtexte */
    if(ui->teZoneTexte->textCursor().charFormat().fontCapitalization()== QFont::AllUppercase){
        ABULEDU_LOG_DEBUG() << "Test CAPS OK";
        ui->btnMajusculeMicroTexte->setChecked(true);
    }
    else if(ui->teZoneTexte->textCursor().charFormat().fontCapitalization() == QFont::AllLowercase){
        ABULEDU_LOG_DEBUG() << "Test LOWER OK";
        ui->btnMinusculeMicroTexte->setChecked(true);
    }

    if(/*m_textCharFormat.fontFamily() != tcf.fontFamily() && */ui->teZoneTexte->toPlainText().isEmpty()){
        ABULEDU_LOG_DEBUG() << "+++++++++++++++++++++++++++++++   +++++++++++++++++++++++++  " << "C'est mon cas ";
        mergeFormatOnWordOrSelection(m_textCharFormat);
//        m_textCharFormat = tcf;
    }
}

void MainWindow::slotChangeTextAlign(const QString& align)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << align;
    if(align == "left"){
        ui->teZoneTexte->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    }
    else if(align == "center"){
        ui->teZoneTexte->setAlignment(Qt::AlignHCenter);
    }
    else if(align == "right"){
        ui->teZoneTexte->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    }
    else if(align == "justify"){
        ui->teZoneTexte->setAlignment(Qt::AlignJustify);
    }
}

void MainWindow::slotFontCaps()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    slotChangeFont("Andika");
}

void MainWindow::slotFontLower()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    slotChangeFont("Andika");
}

void MainWindow::on_teZoneTexte_textChanged()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ ;
    if(!isWindowModified() && !ui->teZoneTexte->document()->isEmpty()) {
        setWindowModified(true);
    }
}

void MainWindow::slotChangeFontSize(int newSize)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << newSize ;
    const qreal pointSize = newSize;
    if(newSize > 0){
        QTextCharFormat fmt = m_textCharFormat;
        fmt.setFontPointSize(pointSize);
        m_textCharFormat.setForeground(ui->teZoneTexte->textColor());
        mergeFormatOnWordOrSelection(fmt);
        m_textCharFormat = fmt;
    }
}

void MainWindow::on_btn_increase_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_fontSize += 2;
    slotChangeFontSize(m_fontSize);
}

void MainWindow::on_btn_decrease_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    m_fontSize -= 2;
    slotChangeFontSize(m_fontSize);
}

void MainWindow::slotChangeColor(int index)
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__ << index;
    // const QStringList colorNames = QColor::colorNames();
    /* On change le fond */
    QColor color(m_listColors.at(index));
    QPalette palette = ui->cb_colorChooser->palette();
    palette.setColor(QPalette::Base, color);
    ui->cb_colorChooser->setPalette(palette);

   /* Méthode petity carré conservée (petites icones) */
//   int size = ui->cb_colorChooser->style()->pixelMetric(QStyle::PM_SmallIconSize);
//   QPixmap pixmap(size, size);
//   pixmap.fill(color);
//   ui->cb_colorChooser->setItemData(index, pixmap, Qt::DecorationRole);

   QTextCharFormat fmt;
   fmt.setForeground(color);
   mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::on_btnCursiveMicroTexte_clicked()
{
    ABULEDU_LOG_TRACE() << __PRETTY_FUNCTION__;
    slotChangeFont("Cursive standard");
}

