#ifndef QFONTTOOLBAR_H
#define QFONTTOOLBAR_H


#include <QToolBar>
#include <QComboBox>
#include <QFontComboBox>
#include <QApplication>



class QFontToolBar : public QToolBar
{
    Q_OBJECT

public:
    explicit QFontToolBar( QWidget *parent = 0) ;

protected:
    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

public:
    template< class WINDOW >
    void SetActions( WINDOW *window )
    {
        connect( comboStyle, SIGNAL(activated(int)), window, SLOT(textStyle(int)) );
        connect( comboFont, SIGNAL(activated(QString)), window, SLOT(textFamily(QString)) );
        connect( comboSize, SIGNAL(activated(QString)), window, SLOT(textSize(QString)) );

        comboSize->setCurrentIndex( comboSize->findText(QString::number(QApplication::font().pointSize())) );
    }

public slots:
    void fontChanged(const QFont &f);
};


#endif // QFONTTOOLBAR_H
