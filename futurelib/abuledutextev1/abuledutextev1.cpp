/**
  * (le terrier d'AbulEdu)
  *
  * @warning aucun traitement d'erreur n'est pour l'instant implémenté
  * @see https://redmine.ryxeo.com/projects/leterrier-aller
  * @author 2011 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
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

#include "abuledutextev1.h"
#include "ui_abuledutextev1.h"

// Choix des icones en fonction de la plateforme
#ifdef Q_WS_MAC
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif

AbulEduTexteV1::AbulEduTexteV1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbulEduTexteV1)
{
    ui->setupUi(this);
    m_localDebug = false;

#ifdef __ABULEDUTABLETTEV1__MODE__
    m_hauteurToolBar = 48;
#else
    m_hauteurToolBar = 48;
#endif

    if (isTopLevel())
    {
        m_menuBar = new QMenuBar(this);
        m_hasMenuBar = true;
    }
    else
    {
        m_menuBar = 0;
        m_hasMenuBar = false;
    }
    // Au cas ou le widget serait un topLevelWidget()
    setWindowTitle(trUtf8("Sans nom")+"[*]");
    // On crée la barre d'icones et les QActions qui vont bien
    //    setupToolBarAndActions();
    if(isTopLevel())
    {
        setupMenuBar();
    }
    // Les connexions concernant les modifications du texte et de son nom
    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)),
            m_actionSave, SLOT(setEnabled(bool)));
    // Des connexions si ce n'est pas un toplevelWidget

    // On émet un signal inquant si le texte a été modifié
    connect(ui->teZoneTexte->document(), SIGNAL(modificationChanged(bool)),
            this, SIGNAL(somethingHasChangedInText(bool)));
    //    // Le curseur a changé lors d'une opération d'édition
    //    connect(ui->teZoneTexte->document(), SIGNAL(cursorPositionChanged(QTextCursor)),
    //            this, SLOT(cursorMoved(QTextCursor)));
    // Le curseur a été déplacé
    connect(ui->teZoneTexte, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorMoved()));

    connect(this, SIGNAL(alignmentRight()),   m_actionAlignRight,   SIGNAL(triggered()));
    connect(this, SIGNAL(alignmentLeft()),    m_actionAlignLeft,    SIGNAL(triggered()));
    connect(this, SIGNAL(alignmentCenter()),  m_actionAlignCenter,  SIGNAL(triggered()));
    connect(this, SIGNAL(alignmentJustify()), m_actionAlignJustify, SIGNAL(triggered()));

}

AbulEduTexteV1::~AbulEduTexteV1()
{
    delete ui;
}

QTextDocument *AbulEduTexteV1::abeTexteGetDocument()
{
    return ui->teZoneTexte->document();
}

QToolBar *AbulEduTexteV1::abeTexteGetToolBar()
{
    return tb;
}

QMenuBar *AbulEduTexteV1::abeTexteGetMenuBar()
{
    return m_menuBar;
}

void AbulEduTexteV1::abeTexteSetFontFamily(QString fontFamily)
{
//    m_comboFont->setCurrentFont(QFont(fontFamily));
    setTextFamily(fontFamily);
}

QString AbulEduTexteV1::abeTexteGetFontFamily()
{
//    return m_comboFont->font().family();
}

void AbulEduTexteV1::abeTexteSetFontSize(int taille)
{
    /** @todo Tester taille et l'adapter en fonction des tailles disponibles dans la combobox m_comboSize */
    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(taille)));
    setTextSize(QString::number(taille));
}

void AbulEduTexteV1::abeTexteSetAlignment(Qt::Alignment align)
{
    if(align.testFlag(Qt::AlignRight))
        emit alignmentRight();
    else if(align.testFlag(Qt::AlignLeft))
        emit alignmentLeft();
    else if(align.testFlag(Qt::AlignHCenter))
        emit alignmentCenter();
    else if(align.testFlag(Qt::AlignJustify))
        emit alignmentCenter();
    updateActions(ui->teZoneTexte->textCursor().charFormat()); // Met le bouton concerné à jour
}

void AbulEduTexteV1::abeTexteSetBold(bool onOff)
{
    m_actionTextBold->setChecked(onOff);
    setTextFormat();
}

void AbulEduTexteV1::abeTexteSetItalic(bool onOff)
{
    m_actionTextItalic->setChecked(onOff);
    setTextFormat();
}

void AbulEduTexteV1::abeTexteSetUnderline(bool onOff)
{
    m_actionTextUnderline->setChecked(onOff);
    setTextFormat();
}

void AbulEduTexteV1::abeTexteToolBarSetVisible(bool ouiNon)
{
    tb->setVisible(ouiNon);
}

bool AbulEduTexteV1::abeTexteToolBarIsVisible()
{
    return tb->isVisible();
}

void AbulEduTexteV1::setTextFormat()
{
    // On crée le format à appliquer
    QTextCharFormat fmt;
    fmt.setFontWeight(m_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    fmt.setFontItalic(m_actionTextItalic->isChecked());
    fmt.setFontUnderline(m_actionTextUnderline->isChecked());
    // On l'applique
    mergeFormatOnWordOrSelection(fmt);
}

void AbulEduTexteV1::setTextAlign(QAction *action)
{
    // On applique le bon alignement pour le paragraphe sous le curseur
    if (action == m_actionAlignLeft)
        ui->teZoneTexte->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute); // Toujours à gauche même en cas de RTL
    else if (action == m_actionAlignCenter)
        ui->teZoneTexte->setAlignment(Qt::AlignHCenter);
    else if (action == m_actionAlignRight)
        ui->teZoneTexte->setAlignment(Qt::AlignRight | Qt::AlignAbsolute); // Toujours à droite même en cas de RTL
    else if (action == m_actionAlignJustify)
        ui->teZoneTexte->setAlignment(Qt::AlignJustify);
}

void AbulEduTexteV1::setTextFamily(const QString &f)
{
    // On applique le format de font sélectionnée
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void AbulEduTexteV1::setTextSize(const QString &p)
{
    // On applique la taille de font sélectionnée
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void AbulEduTexteV1::setTextColor()
{
    // Applique la couleur sélectionnée au texte
    QColor col = QColorDialog::getColor(ui->teZoneTexte->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);

    // On met à jour l'icone dans la barre de boutons
    colorChanged(col);
}

void AbulEduTexteV1::colorChanged(const QColor &couleur)
{
    QPixmap pix(16, 16);
    pix.fill(couleur);
    m_actionTextColor->setIcon(pix);
}

void AbulEduTexteV1::setupToolBarAndActions()
{
    // Création de la Barre de boutons
    tb = new QToolBar(this);
    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tb->setFixedHeight(m_hauteurToolBar);
    tb->setWindowTitle(trUtf8("&Édition"));

    ui->verticalLayout->insertWidget(0,tb); // On place la barre en haut


    /** @todo utiliser le thème abuledu pour les icones des toolboutons
      * Pour l'instant, on utilise l'icone du thème, sinon celle du fichier de ressources
      */

    // Sauvegarde du texte
    m_actionSave = new QAction(QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png")),
                               trUtf8("&Enregistrer"), this);
    m_actionSave->setObjectName("save");
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    m_actionSave->setEnabled(false);
    tb->addAction(m_actionSave);

    m_actionPrint = new QAction(QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png")),
                                tr("&Print..."), this);
    m_actionPrint->setObjectName("print");
    m_actionPrint->setPriority(QAction::LowPriority);
    m_actionPrint->setShortcut(QKeySequence::Print);
    connect(m_actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));
    tb->addAction(m_actionPrint);

    tb->addSeparator();

    // Formatage des caractères
    m_actionTextBold = new QAction(QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png")),
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

    m_actionTextItalic = new QAction(QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png")),
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

    m_actionTextUnderline = new QAction(QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png")),
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
    m_alignActions = new QActionGroup(this);
    m_alignActions->setObjectName("groupalign");
    connect(m_alignActions, SIGNAL(triggered(QAction*)), this, SLOT(setTextAlign(QAction*)));

    // On modifie la position des icones en fonction du sens du texte LTR ou RTL
    if (QApplication::isLeftToRight()) {
        m_actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png")), trUtf8("À gauc&he"), m_alignActions);
        m_actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png")), trUtf8("Au c&entre"), m_alignActions);
        m_actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png")), trUtf8("À d&roite"), m_alignActions);
    } else {
        m_actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png")), trUtf8("&À d&roite"), m_alignActions);
        m_actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png")), trUtf8("Au c&entre"), m_alignActions);
        m_actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png")), trUtf8("À gauc&he"), m_alignActions);
    }
    m_actionAlignJustify = new QAction(QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png")), trUtf8("&Justifié"), m_alignActions);

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
    m_comboFont = new QComboBox(tb);
    m_comboFont->setObjectName("combofont");
    m_comboFont->addItem("Andika");
    m_comboFont->addItem("Liberation");
    m_comboFont->addItem("Cursive standard");
    m_comboFont->addItem("seyesBDE");
    tb->addWidget(m_comboFont);
    m_comboFont->setEditable(false);
    connect(m_comboFont, SIGNAL(activated(QString)),
            this, SLOT(setTextFamily(QString)));
    // La taille de la police, création de la combobox
    m_comboSize = new QComboBox(tb);
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
    m_actionTextColor = new QAction(pix, trUtf8("&Couleur..."), this);
    m_actionTextColor->setObjectName("color");
    connect(m_actionTextColor, SIGNAL(triggered()), this, SLOT(setTextColor()));
    tb->addAction(m_actionTextColor);
}

void AbulEduTexteV1::setupMenuBar()
{
    if(!m_menuBar)
    {
        m_menuBar = new QMenuBar(this);
    }
    ui->verticalLayout->insertWidget(0,m_menuBar); // On place la barre en haut
    QMenu *menu = new QMenu(trUtf8("&Fichier"), this);
    menu->setObjectName("fichier");
    m_menuBar->addMenu(menu);
    menu->addAction(m_actionSave);
    menu->addAction(m_actionPrint);

    QMenu *menu2 = new QMenu(trUtf8("F&ormat"));
    m_menuBar->addMenu(menu2);
    menu2->addAction(m_actionTextBold);
    menu2->addAction(m_actionTextItalic);
    menu2->addAction(m_actionTextUnderline);
    menu2->addSeparator();
    menu2->addActions(m_alignActions->actions());
    menu2->addSeparator();
    menu2->addAction(m_actionTextColor);
}

void AbulEduTexteV1::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->teZoneTexte->textCursor();
    if (!cursor.hasSelection())
        // Si pas de sélection, on utilise le mot sous le curseur
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->teZoneTexte->mergeCurrentCharFormat(format);
}
bool AbulEduTexteV1::fileSave()
{
    if (m_fileName.isEmpty())
        return fileSaveAs(); // Ouverture du sélecteur de fichier

    //
    QTextDocumentWriter writer(m_fileName);
    bool success = writer.write(ui->teZoneTexte->document());
    if (success)
        ui->teZoneTexte->document()->setModified(false);
    return success;
}

bool AbulEduTexteV1::fileSaveAs()
{
    /** @todo Voir si l'on propose d'autres formats d'enregistrement du texte */
    //    QString fn = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder sous..."),
    //                                              QString(), trUtf8("Fichiers ODF (*.odt);;Fichiers HTML (*.htm *.html);;All Files (*)"));
    QString fn = QFileDialog::getSaveFileName(this, trUtf8("Sauvegarder sous..."),
                                              QString(), trUtf8("Fichiers ODF (*.odt)"));
    if (fn.isEmpty())
        return false;
    //    if (! (fn.endsWith(".odt", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) || fn.endsWith(".html", Qt::CaseInsensitive)) )
    if (! (fn.endsWith(".odt", Qt::CaseInsensitive)))
        fn += ".odt"; // default
    setCurrentFileName(fn);
    return fileSave();
}
void AbulEduTexteV1::setCurrentFileName(const QString &fileName)
{
    m_fileName = fileName;
    // Comme le nom vient de changer, c'est que le fichier vient d'être crée ou vient d'être sauvegardé
    ui->teZoneTexte->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = trUtf8("Sans nom") +"[*]";
    else
        shownName = QFileInfo(m_fileName).fileName() + "[*]";


    // Au cas ou le widget serait un topLevelWidget()
    setWindowTitle(shownName);
    // On émet un signal avec le nom du fichier suivi de [*] pour affichage dans titre de fenêtre
    emit fileNameHasChanged(shownName);
}


void AbulEduTexteV1::abeTexteSetMenuBar(bool ouiNon)
{
    if(!m_hasMenuBar)
    {
        setupMenuBar();
    }
    m_hasMenuBar = ouiNon;

}

bool AbulEduTexteV1::abeTexteHasMenuBar()
{
    return m_hasMenuBar;
}

bool AbulEduTexteV1::abeTexteInsertImage(QString cheminImage, qreal width, qreal height, QTextFrameFormat::Position position, QString name)
{
    QFile fichier(cheminImage);
    if(!fichier.exists()){
        qDebug()<<__PRETTY_FUNCTION__<<"ligne"<<__LINE__<<"Le fichier n'existe pas"<<cheminImage;
        return false;
    }
    else
    {
        if(name == "")
        {
            name = cheminImage;
        }
        QImage image(cheminImage);
        if(width == 0 || height == 0)
        {
            width = image.width();
            height = image.height();
        }
        else
        {
            image = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        ui->teZoneTexte->document()->addResource(QTextDocument::ImageResource, QUrl(name), image);

        /** Changer la méthode d'insertion pour insérer une QTetxtFrame dans laquelle on insérerait une image
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

void AbulEduTexteV1::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (ui->teZoneTexte->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(trUtf8("Imprimer le document"));
    if (dlg->exec() == QDialog::Accepted) {
        ui->teZoneTexte->print(&printer);
    }
    delete dlg;
#endif
}

//void AbulEduTexteV1::cursorMoved(QTextCursor curseur)
//{


//}

void AbulEduTexteV1::cursorMoved()
{
    //    if(m_localDebug)qDebug()<<__PRETTY_FUNCTION__<<"ligne"<<__LINE__;
    updateActions(ui->teZoneTexte->textCursor().charFormat());
}

void AbulEduTexteV1::updateActions(QTextCharFormat fmt)
{
    // On actualise les toolButtons ou plutôt les QActions sous-jacentes en fonction du formatage du texte
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

    m_comboFont->blockSignals(true);
//    m_comboFont->setCurrentFont(fmt.font());
    m_comboFont->blockSignals(false);

    m_comboSize->blockSignals(true);
    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(fmt.font().pointSize())));
    m_comboSize->blockSignals(false);
}
