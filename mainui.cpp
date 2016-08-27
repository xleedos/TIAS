#include "imagefunctions.h"
#include "mainui.h"
#include "mainwindow.h"
#include "flowlayout.h"
#include "ui_mainui.h"
#include "databasefunctions.h"
#include "skinanalysis.h"
#include <algorithm>
#include <QMouseEvent>
#include <QMovie>
#include <QtConcurrent>
#include <QApplication>

FlowLayout *flowLayout = new FlowLayout;
CaseSelect *CaseSelectForm;
ImageInspector *ImageInspectorW;
QString CaseID;
QList<ImageInfo> Images;
QList<CustomGroupBox*> Selected;

Ui::MainUI* uip; //YUK! Fo Shame! D: !

void Loading(bool isloading){
    if (isloading){
        uip->Stack->setCurrentIndex(0);

        //uip->FlowHolder->hide();
    }else{

        uip->Stack->setCurrentIndex(1);

    }

}

QVector<double> GetSettings(){
    QVector<double> Settings;
    Settings.append(uip->FaceMin->text().toInt());
    Settings.append(uip->FaceScale->text().toDouble());
    Settings.append(uip->FaceNe->text().toInt());
    Settings.append(uip->SkinTh->text().toInt());
    Settings.append(uip->CannyTh->text().toInt());
    Settings.append(uip->RatioTh->text().toInt());
    Settings.append(uip->Uh->text().toInt());
    Settings.append(uip->Us->text().toInt());
    Settings.append(uip->Uv->text().toInt());
    Settings.append(uip->Lh->text().toInt());
    Settings.append(uip->Ls->text().toInt());
    Settings.append(uip->Lv->text().toInt());

    if (uip->SkinRadFast->isChecked()){
        Settings.append(1);
    }else{
        Settings.append(0);
    }

    return Settings;


}

ImageInfo GetNextImage(int ID, int Direction){


    if (Direction == 1){
        //Forward Ho!
        if ((ID + 1) > Images.size() ){
            return Images.first();
        }else{
            return Images.at(ID + 1);
        }

    }else{
        //Backwards Quickwards!
        if (ID == 0){
            return Images.last();
        }else{
            return Images.at(ID - 1);
        }

    }



}



int getScoreList(QString ListWidgetText){

    for(int i = 0; i < uip->listWidget->count(); i++){

        //For every listitem

        if(uip->listWidget->item(i)->text() == ListWidgetText){
        //Does the text match the label
            if(uip->listWidget->item(i)->checkState() == Qt::Checked){
        //Is the item checked?
                int score = 2 << (uip->listWidget->count() - i);
                //qDebug() << uip->listWidget->count();
                return score;
            }else{
                return -5000;


            }
        }
    }
    return 0; //Return a very high number so error occured

}
bool variantLessThan(const ImageInfo &v1, const ImageInfo &v2)
{
    return v1.sort > v2.sort;
}

void reshuffle(){

    //Remove old widgets
    QLayoutItem *child;
    while ((child = flowLayout->takeAt(0)) != 0) {

            flowLayout->removeWidget(child->widget());
            //Yuk yuk yuk D: Not nice
           child->widget()->setParent(NULL);
           delete child;
    }

    //Clear the sort score for all images

    for (int i = 0; i < Images.size(); i++){
        Images[i].sort = 0;
        //flowLayout->removeItem(Images[i].Display);
    }
    for (int i = 0; i < Images.size(); i++){
        uip->UpdateLabel->setText("Sorting Image# " + QString::number(i) + " of " + QString::number(Images.count()));
        qApp->processEvents();

        //for each image
        if(Images[i].flags.KFFBad){

           Images[i].sort = Images[i].sort + getScoreList("Known Bad File");
           //qDebug() << Images[i].sort;
        }

        if(Images[i].flags.Skin){

           Images[i].sort = Images[i].sort + getScoreList("Skin Analysis");
           //qDebug() << Images[i].sort;
        }
        if(Images[i].flags.Face){

           Images[i].sort = Images[i].sort + getScoreList("Face Detection");
           //qDebug() << Images[i].sort;
        }
        if(Images[i].flags.Meta){

           Images[i].sort = Images[i].sort + getScoreList("Contains Meta");
           //qDebug() << Images[i].sort;
        }
        if(Images[i].flags.Small){

           Images[i].sort = Images[i].sort + getScoreList("Small Size");
           //qDebug() << Images[i].sort;
        }
        if(Images[i].flags.KFFGood){

           Images[i].sort = Images[i].sort + getScoreList("Known Good File");
           //qDebug() << Images[i].sort;
        }

    }


        std::sort(Images.begin(), Images.end(), variantLessThan);



        for(int i =0; i< Images.count(); i++){

            //Ow my head!

            //BUG BUG BUG FIXME!!!!!!!!
            Images[i].Display->Image = i;

            if(Images[i].sort >= 0){
             flowLayout->addWidget(Images[i].Display);
            }


        }

}

void drawimagewidget(){


    //RUN CONCURRENTLY

    QFuture<QList<ImageInfo>> future = QtConcurrent::run(GetCaseImages,CaseID.toInt());
    future.waitForFinished();


    Images = future.result();
    //Images = GetCaseImages(CaseID.toInt());



    for( int ImageInt=0; ImageInt<Images.count(); ++ImageInt )
    {
    uip->UpdateLabel->setText("Loading image details : " + QString::number(ImageInt) + " of " + QString::number(Images.count()));
    qApp->processEvents();

    CustomGroupBox *groupBox = new CustomGroupBox;
    groupBox->setTitle(("Image ID : " + Images.at(ImageInt).id));
    groupBox->setStyleSheet("QGroupBox {  border: 1px solid grey; padding-top: 10px;}");
    groupBox->setMouseTracking(true);
    groupBox->Image = ImageInt;

    QLabel *thumb = new QLabel();

    QByteArray encthumbnail = Images.at(ImageInt).thumbnail;

    QImage image = QImage::fromData(QByteArray::fromBase64(encthumbnail), "PNG");
    thumb->setPixmap(QPixmap::fromImage(image).scaled(150,150,Qt::KeepAspectRatio));
    thumb->setMaximumSize(140,140);
    thumb->setMinimumSize(140,140);
    thumb->setScaledContents(false);

    FlowLayout *IconLayout = new FlowLayout;
    QWidget *iconholder = new QWidget;
    QVector<int> tags = GetTags(Images.at(ImageInt).id.toInt());


    //KFF Flag
    QLabel *kff_flag = new QLabel();
    QPixmap kff_flag_pixmap(":TomsImageAnalyser/kff_flag");
    kff_flag->setPixmap(kff_flag_pixmap);
    kff_flag->setMaximumSize(15,15);
    kff_flag->setMinimumSize(15,15);
    kff_flag->setScaledContents(true);
    kff_flag->setToolTip("Bad file : The images hash value exists within the blacklist database");

    //Good Flag
    QLabel *good_flag = new QLabel();
    QPixmap good_flag_pixmap(":TomsImageAnalyser/good_flag");
    good_flag->setPixmap(good_flag_pixmap);
    good_flag->setMaximumSize(15,15);
    good_flag->setMinimumSize(15,15);
    good_flag->setScaledContents(true);
    good_flag->setToolTip("Good File : The image is known to be non malicious");



    //Skin
    QLabel *skin_flag = new QLabel();
    QPixmap skin_flag_pixmap(":TomsImageAnalyser/skin_flag");
    skin_flag->setPixmap(skin_flag_pixmap);
    skin_flag->setMaximumSize(15,15);
    skin_flag->setMinimumSize(15,15);
    skin_flag->setScaledContents(true);
    skin_flag->setToolTip("Skin Detection : The image contains a high percentage of flesh coloured pixels. ");

    QLabel *skin_int = new QLabel();
    skin_flag->setMaximumSize(15,15);
    skin_flag->setMinimumSize(15,15);
    skin_flag->setScaledContents(true);
    skin_flag->setToolTip("Skin Detection : The image contains a high percentage of flesh coloured pixels. ");


    //Face
    QLabel *face_flag = new QLabel();
    QPixmap face_flag_pixmap(":TomsImageAnalyser/face_flag");
    face_flag->setPixmap(face_flag_pixmap);
    face_flag->setMaximumSize(15,15);
    face_flag->setMinimumSize(15,15);
    face_flag->setScaledContents(true);
    face_flag->setToolTip("Face Detection : The image potentially contains a face. ");

    //Small Flag
    QLabel *small_flag = new QLabel();
    QPixmap small_flag_pixmap(":TomsImageAnalyser/small_flag");
    small_flag->setPixmap(small_flag_pixmap);
    small_flag->setMaximumSize(15,15);
    small_flag->setMinimumSize(15,15);
    small_flag->setScaledContents(true);
    small_flag->setToolTip("Small Image : The images proportions are very small. ");

    //Meta Flag
    QLabel *meta_flag = new QLabel();
    QPixmap meta_flag_pixmap(":TomsImageAnalyser/meta_flag");
    meta_flag->setPixmap(meta_flag_pixmap);
    meta_flag->setMaximumSize(15,15);
    meta_flag->setMinimumSize(15,15);
    meta_flag->setScaledContents(true);
    meta_flag->setToolTip("Meta Data : The image contains data such as the make of the camera");

    //Meta Geo Flag
    QLabel *geo_flag = new QLabel();
    QPixmap geo_flag_pixmap(":TomsImageAnalyser/gps");
    geo_flag->setPixmap(geo_flag_pixmap);
    geo_flag->setMaximumSize(15,15);
    geo_flag->setMinimumSize(15,15);
    geo_flag->setScaledContents(true);
    geo_flag->setToolTip("Geographic Data : The EXIF data contains GPS co-ordinates");



    for(int i = 0; i < tags.count(); i++ ){
        //qDebug() << tags.at(i);
        switch (tags.at(i))
        {
            case 1:
                       //Bad File RED
                       IconLayout->addWidget(kff_flag);
                       groupBox->setStyleSheet("QGroupBox {  border: 1px solid red; padding-top: 10px;}");
                       Images[ImageInt].flags.KFFBad = true;


            break;
            case 2:
                       IconLayout->addWidget(skin_flag);
                       skin_int->setText(QString::number(Images.at(ImageInt).skin) + "%");
                       IconLayout->addWidget(skin_int);
                       Images[ImageInt].flags.Skin = true;
            break;
            case 3:
                       IconLayout->addWidget(face_flag);
                       Images[ImageInt].flags.Face = true;
            break;
            case 4:
                       IconLayout->addWidget(small_flag);
                       Images[ImageInt].flags.Small = true;
            break;
            case 5:
                       IconLayout->addWidget(meta_flag);
                       Images[ImageInt].flags.Meta = true;
            break;
            case 6:
                       IconLayout->addWidget(good_flag);
                       groupBox->setStyleSheet("QGroupBox {  border: 1px solid green; padding-top: 10px;}");
                       Images[ImageInt].flags.KFFGood = true;
            break;
            case 7:
                       IconLayout->addWidget(geo_flag);
                       Images[ImageInt].flags.GPS = true;
            break;


        }


    }




    //Create the group box with image thumbnail
      QVBoxLayout *vbox = new QVBoxLayout;
      vbox->addWidget(thumb);

       //TODO: Check if flags are set

      iconholder->setLayout(IconLayout);
      vbox->addWidget(iconholder);


      groupBox->setLayout(vbox);



qApp->processEvents();
    //Add the image to the browser
    Images[ImageInt].Display = groupBox ;
   // flowLayout->addWidget(Images[ImageInt].Display);

}


    reshuffle();
    Loading(false);
}

MainUI::~MainUI()
{
    delete ui;
}

void MainUI::on_pushButton_clicked()
{

PurgeDatabase();
}

void initialiseCase(QStringList SelectedCase){
    CaseSelectForm->hide();
    //We have a case ID
    CaseID = SelectedCase.at(0);
    QCoreApplication::processEvents();
    drawimagewidget();



}

void MainUI::on_pushButton_2_clicked()
{
    int start = QDateTime::currentMSecsSinceEpoch();
    //Debug : Select image folder
    Loading(true);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir folder;
    folder.setPath(dir);
    ImportImages(folder,CaseID);
    int end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "FINISHED! Time : " + QString::number(end - start);
    Loading(false);
    drawimagewidget();


}
void UpdateStatusLabel(QString text){
    uip->UpdateLabel->setText(text);
    qApp->processEvents();
}


void MainUI::on_Debug_ConnectSQLite_clicked()
{
//DatabaseConnect();
}
MainUI::MainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainUI)
{
    //Setup Flow layout
    ui->setupUi(this);

    ui->FlowHolder->setLayout(flowLayout);
    uip = ui;
    this->setWindowState(Qt::WindowMaximized);
    SetMessageStatus("Initialising Database",0);
    QMovie *movie = new QMovie(":TomsImageAnalyser/load");
    uip->Loader->setMovie(movie);
    movie->start();

    if (!initialiseDatabase()){
        QMessageBox msgBox;
        msgBox.setText("Error : Cannot initialise database");
        msgBox.setInformativeText("Could not connect to the SQLite database");
        msgBox.exec();
    }else{

        SetMessageStatus("Please select a case",0);
        //While a case isnt loaded, open a window.
        ImageInspectorW = new ImageInspector();

        CaseSelectForm = new CaseSelect();
        CaseSelectForm->setWindowFlags(Qt::CustomizeWindowHint);
        CaseSelectForm->show();

    }


}

void MainUI::SetLabels(int imageID){
ui->L_ID->setText(Images.at(imageID).id);

}

void MainUI::on_groupBox_clicked()
{


}
void CustomGroupBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // handle left mouse button here


        for (int i = 0 ; i < Selected.count(); i++){
            if(Images.at(Selected.at(i)->Image).flags.KFFGood){
                Selected.at(i)->setStyleSheet("QGroupBox {  border: 1px solid green; padding-top: 10px;}");

            }else if(Images.at(Selected.at(i)->Image).flags.KFFBad){
                Selected.at(i)->setStyleSheet("QGroupBox {  border: 1px solid red; padding-top: 10px;}");
            }else{
                Selected.at(i)->setStyleSheet("QGroupBox {  border: 1px solid grey; padding-top: 10px;}");

            }



        }

        this->setStyleSheet("QGroupBox {  border: 1px solid blue; padding-top: 10px;}");
        Selected.append(this);
        uip->L_ID->setText(Images.at(this->Image).id);
        uip->L_Path->setText(Images.at(this->Image).path);
        uip->L_FS->setText(QString::number(Images.at(this->Image).filesize));
        uip->L_M->setText(Images.at(this->Image).modified.toString("dd/MM/yy hh:mm:ss"));
        uip->L_A->setText(Images.at(this->Image).accessed.toString("dd/MM/yy hh:mm:ss"));
        uip->L_C->setText(Images.at(this->Image).created.toString("dd/MM/yy hh:mm:ss"));
        uip->L_Height->setText(QString::number(Images.at(this->Image).height) + "px");
        uip->L_Width->setText(QString::number(Images.at(this->Image).width) + "px");

        uip->L_Camera->setText(Images.at(this->Image).meta.Make + " " + Images.at(this->Image).meta.Model);
        uip->L_Descript->setText(Images.at(this->Image).meta.Descriptiom);
        uip->L_Editor->setText(Images.at(this->Image).meta.Software);
        uip->L_FileTime->setText(Images.at(this->Image).meta.Filetime);
        uip->L_AnTime->setText(Images.at(this->Image).meta.DateTime_Analogue);
        uip->L_DiTime->setText(Images.at(this->Image).meta.DateTime_Digital);
        uip->L_Copyright->setText(Images.at(this->Image).meta.Copyright);

        uip->L_Long->setText("Long : " + Images.at(this->Image).meta.Longitude);
        uip->L_Lat->setText("Lat : " + Images.at(this->Image).meta.Latitude);
        uip->L_Alt->setText("Alt : " + Images.at(this->Image).meta.Altitude);
        uip->L_Map->setText("<a href=\"http://maps.google.com/maps?z=12&t=h&q=loc:" + Images.at(this->Image).meta.Latitude + "+" + Images.at(this->Image).meta.Longitude + "\">Map</a>");
        uip->L_Map->setTextFormat(Qt::RichText);
        uip->L_Map->setTextInteractionFlags(Qt::TextBrowserInteraction);
        uip->L_Map->setOpenExternalLinks(true);
        uip->L_MD5->setText(Images.at(this->Image).md5);



    }
}

void SetMessageStatus(QString message,int timeout){
    uip->statusbar->showMessage(message,timeout);

}

void CustomGroupBox::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // handle left mouse button here
        //qDebug() << "DOUBLE CLICKED!";


        DisplayImage(Images.at(this->Image));

        ImageInspectorW->show();





       //qDebug() << Images.at(this->Image).meta.Altitude;




    }
}






void MainUI::on_pushButton_3_clicked()
{
    reshuffle();
}

void MainUI::on_pushButton_4_clicked()
{
    double skin = 0, face = 0, total = 0;
    double skinPerc = 0;
    double facePerc = 0;

if (Images.size() > 0){
    total = Images.size();
    for (int i =0; i < Images.size(); i++){
        if (Images.at(i).flags.Skin == true){
            skin++;
        }
        if (Images.at(i).flags.Face == true){
            face++;
        }
    }

    skinPerc = skin /(total / 100);
    facePerc = face /(total / 100);

    ui->Stat_Tot->setText(QString::number(total));
    ui->Stat_FacePerc->setText(QString::number(facePerc) + "%");
    ui->Stat_SkinPerc->setText(QString::number(skinPerc) + "%");
    ui->Stat_SkinCount->setText(QString::number(skin));
    ui->Stat_FaceTot->setText(QString::number(face));


}

}

void MainUI::on_Up_clicked()
{
    if (uip->listWidget->selectedItems().size() > 0){

        int currentIndex = uip->listWidget->currentRow();
        QListWidgetItem *currentItem = uip->listWidget->takeItem(currentIndex);
        uip->listWidget->insertItem(currentIndex-1, currentItem);
        uip->listWidget->setCurrentRow(currentIndex-1);;
        reshuffle();
    }
}

void MainUI::on_Down_clicked()
{
    if (uip->listWidget->selectedItems().size() > 0){

        int currentIndex = uip->listWidget->currentRow();
        QListWidgetItem *currentItem = uip->listWidget->takeItem(currentIndex);
        uip->listWidget->insertItem(currentIndex+1, currentItem);
        uip->listWidget->setCurrentRow(currentIndex+1);
        reshuffle();


    }
}

void MainUI::on_listWidget_itemChanged()
{
    reshuffle();
}



void MainUI::on_pushButton_5_clicked()
{

    if (uip->Stack->currentIndex() == 1){
        Loading(true);
    }else{
        Loading(false);
    }

}
