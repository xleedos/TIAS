#include "casecreate.h"
#include "ui_casecreate.h"
#include <QMessageBox>
#include <QDebug>
#include "databasefunctions.h"
#include "caseselect.h"

CaseCreate::CaseCreate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaseCreate)
{
    ui->setupUi(this);
}

CaseCreate::~CaseCreate()
{
    delete ui;
}

void CaseCreate::on_buttonBox_rejected()
{

}

void CaseCreate::on_pushButton_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Discard changes?", "Are you sure you want to discard this case?", QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
            this->destroy();
      }
}

void CaseCreate::on_pushButton_2_clicked()
{
    //Create case
    if (ui->CaseName->text() != ""){
        if(ui->ExaminerName->text() != ""){
            if(CreateCase(ui->ExaminerName->text(),ui->CaseName->text(),ui->Descr->text())){
                RefreshCaseSelect();
                this->destroy();


            }else{
                //Error creating the case
            }
        }else{
            //No Examiner name
        }
    }else{
        //No Case Name

    }


}
