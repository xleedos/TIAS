#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainui.h"
#include "faceanalysis.h"
#include "skinanalysis.h"

using namespace cv;
using namespace std;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{


}


void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{
   // MainUI *testWindow = new MainUI();
    //testWindow->show();

}
