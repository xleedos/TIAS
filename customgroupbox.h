#ifndef CUSTOMGROUPBOX_H
#define CUSTOMGROUPBOX_H

#include <QWidget>
#include <QGroupBox>
#include <QMouseEvent>
#include <imagefunctions.h>

class CustomGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    int Image;


signals:

public slots:

protected:


};



#endif // CUSTOMGROUPBOX_H
