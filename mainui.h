#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include "caseselect.h"
#include "QGroupBox"
#include "QFileDialog"
#include "QSqlDatabase"
#include "QCryptographicHash"
#include "QDebug"
#include "QStandardPaths"
#include "QFileInfo"
#include "QMessageBox"
#include "QStringList"
#include "imageinspector.h"
void UpdateStatusLabel(QString text);
QVector<double> GetSettings();
void SetMessageStatus(QString message,int timeout);
void initialiseCase(QStringList SelectedCase);
ImageInfo GetNextImage(int ID, int Direction);

namespace Ui {
class MainUI;
}

class MainUI : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainUI(QWidget *parent = 0);
    void SetLabels(int imageID);
    ~MainUI();

public slots:


private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_Debug_ConnectSQLite_clicked();
    void on_groupBox_clicked();




    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_Up_clicked();

    void on_Down_clicked();

    void on_listWidget_itemChanged();

    void on_pushButton_5_clicked();

private:
    Ui::MainUI *ui;
    CaseSelect *mMyNewWindow;
};



#endif // MAINUI_H
