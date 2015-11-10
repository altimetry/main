#include "stdafx.h"

#include "QbrtMainWindow.h"


///////////////////////////////////////////////////////////////////////////////////////////
//                                  FORMAT TOOLBAR
///////////////////////////////////////////////////////////////////////////////////////////

void QbrtMainWindow::SetFormatActions()
{
    QAction *pa;

    actionTextBold = pa = new QAction(QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png")), tr("&Bold"), this );
    pa->setShortcut(Qt::CTRL + Qt::Key_B);
    pa->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    pa->setFont(bold);
    connect( pa, SIGNAL(triggered()), this, SLOT(textBold()));
    Format_toolBar->addAction( pa );
    menu_Format->addAction( pa );
    pa->setCheckable(true);

    actionTextItalic = pa = new QAction(QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png")), tr("&Italic"), this );
    pa->setPriority(QAction::LowPriority);
    pa->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    pa->setFont(italic);
    connect( pa, SIGNAL(triggered()), this, SLOT(textItalic()));
    Format_toolBar->addAction( pa );
    menu_Format->addAction( pa );
    pa->setCheckable(true);

    actionTextUnderline = pa = new QAction(QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png")), tr("&Underline"), this );
    pa->setShortcut(Qt::CTRL + Qt::Key_U);
    pa->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    pa->setFont(underline);
    connect( pa, SIGNAL(triggered()), this, SLOT(textUnderline()));
    Format_toolBar->addAction( pa );
    menu_Format->addAction( pa );
    pa->setCheckable(true);

    menu_Format->addSeparator();
    Format_toolBar->addSeparator();

    QActionGroup *grp = new QActionGroup( this );
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    // Make sure the alignLeft  is always left of the alignRight
    if (QApplication::isLeftToRight()) {
        actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png")), tr("&Left"), grp);
        actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png")), tr("C&enter"), grp);
        actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png")), tr("&Right"), grp);
    } else {
        actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png")), tr("&Right"), grp);
        actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png")), tr("C&enter"), grp);
        actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png")), tr("&Left"), grp);
    }
    actionAlignJustify = new QAction(QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png")), tr("&Justify"), grp);

    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    Format_toolBar->addActions(grp->actions());
    menu_Format->addActions(grp->actions());

    menu_Format->addSeparator();
    Format_toolBar->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = pa = new QAction(pix, tr("&Color..."), this );
    connect( pa, SIGNAL(triggered()), this, SLOT(textColor()));
    Format_toolBar->addAction( pa );
    menu_Format->addAction( pa );
}



void QbrtMainWindow::textBold()
{
    if ( Editor *editor = activeTextEditor() )
        editor->toBold( actionTextBold->isChecked() );
}


void QbrtMainWindow::textItalic()
{
    if ( Editor *editor = activeTextEditor() )
        editor->toItalic( actionTextItalic->isChecked() );
}


void QbrtMainWindow::textUnderline()
{
    if ( Editor *editor = activeTextEditor() )
        editor->toUnderline( actionTextUnderline->isChecked() );
}


void QbrtMainWindow::textAlign( QAction *a )
{
    if ( Editor *editor = activeTextEditor() ) {
        if ( a == actionAlignLeft)
            editor->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        else if ( a == actionAlignCenter)
            editor->setAlignment(Qt::AlignHCenter);
        else if ( a == actionAlignRight)
            editor->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        else if ( a == actionAlignJustify)
            editor->setAlignment(Qt::AlignJustify);
    }
}


void QbrtMainWindow::textColor()
{
    if ( Editor *editor = activeTextEditor() )
        if ( editor->selectColor() )
            colorChanged( editor->textColor() );
}

void QbrtMainWindow::on_action_Font_triggered()
{
    if ( Editor *pe = activeTextEditor() )
        pe->selectFont();
}



///////////////////////////////////////////////////////////////////////////////////////////
//                                   FONT TOOLBAR
///////////////////////////////////////////////////////////////////////////////////////////


void QbrtMainWindow::textFamily( const QString &f )
{
    if ( Editor *editor = activeTextEditor() )
        editor->toFontFamily( f );
}

void QbrtMainWindow::textSize( const QString &p )
{
    if ( Editor *editor = activeTextEditor() )
        editor->toFontSize( p );
}

void QbrtMainWindow::textStyle( int styleIndex )
{
    Editor *editor = activeTextEditor();
    if ( !editor )
        return;

    QTextListFormat::Style style = QTextListFormat::ListDisc;
    switch (styleIndex) {
    case 0:
        style = QTextListFormat::ListStyleUndefined;
        break;
    default:
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        style = QTextListFormat::ListDecimal;
        break;
    case 5:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 6:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 7:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 8:
        style = QTextListFormat::ListUpperRoman;
        break;
    }
    editor->toListStyle( style );
}



///////////////////////////////////////////////////////////////////////////////////////////
//                          FORMAT UPDATE EVENTS/ACTIONS
///////////////////////////////////////////////////////////////////////////////////////////


//  "events"                  "handlers"
//currentCharFormatChanged => fontChanged
//currentCharFormatChanged => colorChanged
//cursorPositionChanged => alignmentChanged
//
void QbrtMainWindow::colorChanged( const QColor &c )
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void QbrtMainWindow::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft) {
        actionAlignLeft->setChecked(true);
    } else if (a & Qt::AlignHCenter) {
        actionAlignCenter->setChecked(true);
    } else if (a & Qt::AlignRight) {
        actionAlignRight->setChecked(true);
    } else if (a & Qt::AlignJustify) {
        actionAlignJustify->setChecked(true);
    }
}

void QbrtMainWindow::fontChanged( const QFont &f )
{
    Font_toolBar->fontChanged( f );
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void QbrtMainWindow::currentCharFormatChanged( const QTextCharFormat &format )
{
    if ( activeTextEditor() ) {
        fontChanged( format.font() );
        colorChanged( format.foreground().color() );
    }
}

void QbrtMainWindow::cursorPositionChanged()
{
    if ( Editor *editor = activeTextEditor() )
        alignmentChanged( editor->alignment() );
}


