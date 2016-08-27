#ifndef CASESELECT_H
#define CASESELECT_H

#include <QDialog>
#include <QStringList>
#include "databasefunctions.h"


namespace Ui {
class CaseSelect;
}

class CaseSelect : public QDialog
{
    Q_OBJECT

public:
    explicit CaseSelect(QWidget *parent = 0);
    ~CaseSelect();
    bool UpdateListbox(QStringList);

private slots:


    void on_CaseSelector_currentRowChanged(int currentRow);

    void on_Load_clicked();

    void on_pushButton_3_clicked();

private:
     Ui::CaseSelect *ui;
};

#endif // CASESELECT_H
