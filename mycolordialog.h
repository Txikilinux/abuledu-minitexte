#ifndef MYCOLORDIALOG_H
#define MYCOLORDIALOG_H

#include <QColorDialog>

class MyColorDialog : public QColorDialog
{
    Q_OBJECT
public:
    explicit MyColorDialog(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // MYCOLORDIALOG_H
