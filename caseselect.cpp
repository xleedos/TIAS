#include "caseselect.h"
#include "ui_caseselect.h"
#include "mainui.h"
#include "about.h"
#include "casecreate.h"
QList<QStringList> cases;
About *AboutBox;
CaseCreate *CaseCreator;
QListWidget *List;
CaseSelect::CaseSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaseSelect)
{
    ui->setupUi(this);

//Need to populate the list box is open cases
List = ui->CaseSelector;
    RefreshCaseSelect();

}



CaseSelect::~CaseSelect()
{
    delete ui;
}


void RefreshCaseSelect(){
    List->clear();
    cases = GetCaseLists();
    if (cases.count() > 0){


        for ( int i = 0; i != cases.count(); i++ )
        {
           QStringList Record = cases.at(i);
           QString ID = Record.at(0);
           QString InvestigatorName = Record.at(1);
           QDateTime CreatedTime = ToUnixEpoch(Record.at(2));
           QString CaseName = Record.at(3);
           QString Description = Record.at(4);
           List->addItem(CreatedTime.toString("dd/MM/yy HH:mm:ss") + " - " + CaseName);
           //ui->tableWidget->insertRow(tableWidget->rowCount());

        }
    List->item(0)->setSelected(true);
    }






}

void CaseSelect::on_CaseSelector_currentRowChanged(int currentRow)
{
    //Hopefully the current row will always equil the correct case...

    QStringList Record = cases.at(currentRow);
    ui->I_Case->setText(Record.at(0));
    ui->I_Examiner->setText(Record.at(1));
    QDateTime CreatedTime = ToUnixEpoch(Record.at(2));
    ui->I_Created->setText(CreatedTime.toString("dd/MM/yy hh:mm:ss"));
    ui->I_CaseName->setText(Record.at(3));
    ui->I_Image->setNum(GetNumImages(Record.at(0)));
    ui->I_Description->setText(Record.at(5));



}

void CaseSelect::on_Load_clicked()
{
//Load Selected Case
if(ui->CaseSelector->selectedItems().count() > 0){
 // initialiseCase(cases.at(ui->CaseSelector->currentRow());
    initialiseCase(cases.at(ui->CaseSelector->currentRow()));


}
}

void CaseSelect::on_pushButton_3_clicked()
{
    QApplication::quit();
}

void CaseSelect::on_pushButton_4_clicked()
{
    AboutBox = new About();
    AboutBox->show();
}

void CaseSelect::on_pushButton_2_clicked()
{
    CaseCreator = new CaseCreate();
    CaseCreator->show();
}
