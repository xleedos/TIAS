#ifndef CASECREATE_H
#define CASECREATE_H

#include <QDialog>

namespace Ui {
class CaseCreate;
}

class CaseCreate : public QDialog
{
    Q_OBJECT

public:
    explicit CaseCreate(QWidget *parent = 0);
    ~CaseCreate();

private slots:
    void on_buttonBox_rejected();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CaseCreate *ui;
};

#endif // CASECREATE_H
