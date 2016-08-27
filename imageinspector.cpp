#include "imageinspector.h"
#include "imagefunctions.h"
#include "ui_imageinspector.h"
//#include <QDebug>
#include <skinanalysis.h>
#include <autoadjustcontrast.h>
#include "mainui.h"
using namespace cv;
ImageInfo Imager;
Mat Image;
String fullbody_cascade_name = QDir::toNativeSeparators(QDir::currentPath() + "//haarcascade_fullbody.xml").toStdString();
String upperbody_cascade_name = QDir::toNativeSeparators(QDir::currentPath() + "//haarcascade_upperbody.xml").toStdString();
String lowerbody_cascade_name = QDir::toNativeSeparators(QDir::currentPath() + "//haarcascade_lowerbody.xml").toStdString();
CascadeClassifier fullbody;
CascadeClassifier upperbody;
CascadeClassifier lowerbody;


Ui::ImageInspector* uip; //YUK! Fo Shame! D: !

ImageInspector::ImageInspector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageInspector)
{
    ui->setupUi(this);
    uip = ui;
}

ImageInspector::~ImageInspector()
{
    delete ui;
    delete uip;
}


double maximum(double x, double y, double z) {
    double max = x;
    if (y > max) {
        max = y;
    }
    if (z > max) {
        max = z;
    }
    return max;
}

double minimum(double x, double y, double z) {
    double min = x;
    if (y < min) {
        min = y;
    }
    if (z < min) {
        min = z;
    }
    return min;
}


void UpdateDisplay(){

    double FullSkinPercent = 0;
    double SkeletonSkinPercent = 0;
    double CannyPercent = 0;
    double CannyBodyPercent = 0;
    QVector<double> Settings = GetSettings();

    QString path = Imager.path;
    Image = imread(path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);


    //Rezise to 1000px to improve performance
    Mat inputImage = ResizeMat(Image,1000,1000);

    Mat inputImageGrey;
    Mat inputImageHSV;
    Mat inputImageSkinMask;
    Mat inputImageCanny;
    Mat DisplaySkin;
    Mat ThumbHOG = ResizeMat(Image,200,200); //Resize to 200px for SVM detection
    Mat filteredContour = Mat::zeros( inputImage.size(), CV_8UC1 );
    Mat filteredContourHSV = Mat::zeros( inputImage.size(), CV_8UC1 );
    BrightnessAndContrastAuto(inputImage,inputImage,0);
    Mat inputImageSkel = inputImage.clone();
    //Setup the MATS
    cvtColor(inputImage,inputImageGrey,CV_BGR2GRAY);
    cvtColor(inputImage, inputImageHSV, CV_BGR2HSV);





    inRange(inputImageHSV,Scalar(Settings[9], Settings[10], Settings[11]), Scalar(Settings[6], Settings[7], Settings[8]), inputImageSkinMask);



    //HSV RGB EXP
    //#############################################################################

        Mat BGRHSVMat = Mat::zeros( inputImage.size(), CV_8UC1 );
         for (int i = 0; i < inputImage.rows; ++i)
         {
             Vec3b* pixel = inputImage.ptr<cv::Vec3b>(i); // point to first pixel in row
             Vec3b* pixelHSV = inputImageHSV.ptr<cv::Vec3b>(i); // point to first pixel in row
             for (int j = 0; j < inputImage.cols; ++j)
             {
                uchar R = pixel[j][2];
                uchar G = pixel[j][1];
                uchar B = pixel[j][0];
                uchar V = pixelHSV[j][2];
                uchar S = pixelHSV[j][1];
                uchar H = pixelHSV[j][0] * 2;
                if ((R > 220 && G > 210 && B > 170 && (R - G) > 15 && R > B && G > B) || (R > 95 && G > 40 && B > 20 && maximum(R, G ,B) - minimum(R, G , B) > 15 && (R - G) > 15 && R > G && R > B)){
                    if ((0 <= H <= 50)||(340 <= H <= 360) && (0.2 < S) && 0.35 < V) {
                     BGRHSVMat.at<uchar>(Point(j,i)) = 255;
                    }
                }
               }
        }




    //###############################################################################

    //Calculate contours and hide smaller ones

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours( inputImageSkinMask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    int area = inputImage.size().width;
    QVector<vector<Point>> s = filterbysize(contours,(area / 100)*5); //Remove smaller 5%

    vector<vector<Point>> contoursHSV;
    vector<Vec4i> hierarchyHSV;
    findContours( BGRHSVMat, contoursHSV, hierarchyHSV, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    int areaHSV = inputImage.size().width;
    QVector<vector<Point>> sHSV = filterbysize(contours,(areaHSV / 100)*5); //Remove smaller 5%

    for(int i = 0; i < s.size(); i++)
    {
        drawContours( filteredContour, s.toStdVector(), i, Scalar( 255, 255, 255 ), -1, 8, hierarchy,0,Point() );
    }
    for(int i = 0; i < sHSV.size(); i++)
    {
        drawContours( filteredContourHSV, sHSV.toStdVector(), i, Scalar( 255, 255, 255 ), -1, 8, hierarchyHSV,0,Point() );
    }
//                        Mat gsrc;
//                        cvtColor(inputImage,gsrc,CV_BGR2GRAY);
//                        vector<Point2f> corners;
//                        goodFeaturesToTrack(gsrc,corners, 500, 0.5, 10);
//                        for (size_t idx = 0; idx < corners.size(); idx++) {
//                                circle(inputImage,corners.at(idx),2,Scalar(255,255,0),2);
//                        }
//                        imshow("corners",inputImage);

   // imshow("Contour", filteredContour);


    //Calculate Canny

    Canny(inputImage, inputImageCanny, 50, 200, 3);


    int total = inputImageCanny.cols * inputImageCanny.rows;
    int countnonzero = countNonZero(inputImageCanny);
    CannyPercent = countnonzero /(total / 100);


    total = filteredContour.cols * filteredContour.rows;
    countnonzero = countNonZero(filteredContour);
    FullSkinPercent = countnonzero /(total / 100);

    //Find faces and

                if (Imager.faces.size() > 0){
                    for( size_t i = 0; i < Imager.faces.size(); i++ ){
                        Point center( Imager.faces[i].x + Imager.faces[i].width*0.5, Imager.faces[i].y + Imager.faces[i].height*0.5 );
                        int bodywidth = Imager.faces[i].width * 2;
                        int bodyheight = Imager.faces[i].width * 4;
                        Point neckstart(center.x - (Imager.faces[i].width *0.314),center.y + Imager.faces[i].height*0.4 );
                        Point neckend(neckstart.x + (Imager.faces[i].width /1.6),neckstart.y + Imager.faces[i].height*0.25 );

                        Point bodystart(center.x - (bodywidth /2 ),neckend.y );
                        Point bodyend(bodystart.x + bodywidth,bodystart.y + bodyheight);


                        if (bodystart.x < 0){
                            bodystart.x = 0;
                        }
                        if (bodystart.y < 0){
                            bodystart.y = 0;
                        }
                        if (bodyend.x > inputImage.size().width){
                            bodyend.x = inputImage.size().width ;
                        }
                        if (bodyend.y > inputImage.size().height){
                            bodyend.y = inputImage.size().height ;
                        }
                        cv::Rect R(bodystart,bodyend);
                        Mat SkeletalBody = filteredContour(R);
                        DisplaySkin = SkeletalBody;
                        Mat CannyBody = inputImageCanny(R);



                        ellipse( inputImageSkel, center, Size( Imager.faces[i].width*0.5, Imager.faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
                        rectangle(inputImageSkel,neckstart,neckend,Scalar(255,255,0),4,8,0);
                        rectangle(inputImageSkel,bodystart,bodyend,Scalar(255,0,255),4,8,0);
                       // rectangle(inputImageSkel,neckstart,neckend,Scalar(255,255,0),4,8,0);
                       // rectangle(inputImageSkel,bodystart,bodyend,Scalar(255,0,255),4,8,0);


                      int ratio = ((SkeletalBody.cols / SkeletalBody.rows) * 100);
                      qDebug() << SkeletalBody.cols;
                      qDebug() << SkeletalBody.rows;
                      qDebug() << ratio;
                        total = CannyBody.cols * CannyBody.rows;
                        countnonzero = countNonZero(CannyBody);
                        CannyBodyPercent = countnonzero /(total / 100);

                        total = SkeletalBody.cols * SkeletalBody.rows;
                        countnonzero = countNonZero(SkeletalBody);
                        SkeletonSkinPercent = countnonzero /(total / 100);



                        }}
    //==== Cascades =============================================
                    vector<Rect> fullbody_results;
                    vector<Rect> upperbody_results;
                    vector<Rect> lowerbody_results;
                    vector<Rect> hog_results;

                    if (uip->HaarFull->isChecked()){
                        vector<Rect> fullbody_results;
                        fullbody.detectMultiScale( inputImage, fullbody_results, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(5, 5) );
                        for (int i =0; i < fullbody_results.size(); i++){

                            rectangle(inputImageSkel,fullbody_results.at(i),Scalar( 255, 0, 255 ),2,1,0);

                        }


                    }
                    if (uip->HaarUB->isChecked()){

                        vector<Rect> upperbody_results;
                        upperbody.detectMultiScale( inputImage, upperbody_results, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(5, 5) );
                        for (int i =0; i < upperbody_results.size(); i++){

                            rectangle(inputImageSkel,upperbody_results.at(i),Scalar( 230, 0, 255 ),2,1,0);

                        }


                    }
                    if (uip->HaarLB->isChecked()){
                        vector<Rect> lowerbody_results;
                        lowerbody.detectMultiScale( inputImage, lowerbody_results, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(5, 5) );
                        for (int i =0; i < lowerbody_results.size(); i++){

                            rectangle(inputImageSkel,lowerbody_results.at(i),Scalar( 200, 0, 255 ),2,1,0);

                        }

                    }
                    if (uip->HogFull->isChecked()){

                            vector<Rect> found;
                            HOGDescriptor hog;
                            hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
                            hog.detectMultiScale(inputImage, found, 0, cv::Size(4,4), cv::Size(8,8), 1.5);


                            for (int i =0; i < found.size(); i++){

                                rectangle(inputImageSkel,found.at(i),Scalar( 255, 0, 255 ),2,1,0);

                            }



                    }



    //======= Update UI values ====================

    uip->SkinPerc->setText("Skin %: " + QString::number(FullSkinPercent)) ;
    uip->FullCont->setText("Full Cont %:"+ QString::number(CannyPercent));
    uip->BoundSkin->setText("Bounding %: " + QString::number(SkeletonSkinPercent)) ;
    uip->SkelCont->setText("Skel Cont %:"+ QString::number(CannyBodyPercent));

    //======== Draw Result =======================


    //=======Draw UI================================

    uip->ResHaarFull->setText("HaarFull :" + QString::number(fullbody_results.size()));
    uip->ResHaarUB->setText("HaarUB :" + QString::number(upperbody_results.size()));
    uip->ResHaarLB->setText("HaarLB :" + QString::number(lowerbody_results.size()));
    uip->ResHog->setText("HOG :" + QString::number(hog_results.size()));



        QPixmap Display1 = ASM::cvMatToQPixmap(inputImage);
        QPixmap Scaled1 = Display1.scaled(uip->ImagePreview->width(),uip->ImagePreview->height(),Qt::KeepAspectRatio,Qt::FastTransformation);
        uip->ImagePreview->setPixmap(Scaled1);


        QPixmap Display2 = ASM::cvMatToQPixmap(filteredContour);
        QPixmap Scaled2 = Display2.scaled(uip->ImagePreview_Skin->width(),uip->ImagePreview_Skin->height(),Qt::KeepAspectRatio,Qt::FastTransformation);
        uip->ImagePreview_Skin->setPixmap(Scaled2);

        QPixmap Display3 = ASM::cvMatToQPixmap(inputImageCanny);
        QPixmap Scaled3 = Display3.scaled(uip->ImagePreview_Cont->width(),uip->ImagePreview_Cont->height(),Qt::KeepAspectRatio,Qt::FastTransformation);
        uip->ImagePreview_Cont->setPixmap(Scaled3);

        QPixmap Display4 = ASM::cvMatToQPixmap(inputImageSkel);
        QPixmap Scaled4 = Display4.scaled(uip->ImagePreview_Cont->width(),uip->ImagePreview_Cont->height(),Qt::KeepAspectRatio,Qt::FastTransformation);
        uip->ImagePreview_Skel->setPixmap(Scaled4);

        QPixmap Display5 = ASM::cvMatToQPixmap(filteredContourHSV);
        QPixmap Scaled5 = Display5.scaled(uip->ImagePreview_Skin_RGB->width(),uip->ImagePreview_Skin_RGB->height(),Qt::KeepAspectRatio,Qt::FastTransformation);
        uip->ImagePreview_Skin_RGB->setPixmap(Scaled5);






//##############################################


//    Mat erodey;
//    Mat bw; //Black and white
//    Mat hsv; //Converted to HSV for thesholding

////    erode(inputImage, erodey, Mat(), Point(-1, -1), 2, 1, 1);
//  //  erode(erodey, erodey, Mat(), Point(-1, -1), 2, 1, 1);
//    cvtColor(inputImage, hsv, CV_BGR2HSV);
//    inRange(hsv, Scalar(0, 10, 60), Scalar(20, 190, 255), bw);
//   // inRange(hsv, Scalar(0, 10, 38), Scalar(40, 190, 255), bw);



//    vector<vector<Point>> contours;
//    vector<Vec4i> hierarchy;


//    findContours( bw, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
//    int width = inputImage.size().width;

//    QVector<vector<Point>> s = filterbysize(contours,(width / 100)*5);


//    Mat drawing = Mat::zeros( hsv.size(), CV_8UC1 );

//    RNG rng(12345);
//    for(int i = 0; i < s.size(); i++)
//    {
//        drawContours( drawing, s.toStdVector(), i, Scalar( 255, 255, 255 ), -1, 8, hierarchy,0,Point() );
//    }


//    cvtColor(drawing, drawing, CV_GRAY2BGR);

//    if(uip->Skeleton->isChecked()){


//            if (Imager.faces.size() > 0){
//                for( size_t i = 0; i < Imager.faces.size(); i++ ){
//                    Point center( Imager.faces[i].x + Imager.faces[i].width*0.5, Imager.faces[i].y + Imager.faces[i].height*0.5 );
//                    int bodywidth = Imager.faces[i].width * 2;
//                    int bodyheight = Imager.faces[i].width * 4;
//                    Point neckstart(center.x - (Imager.faces[i].width *0.314),center.y + Imager.faces[i].height*0.4 );
//                    Point neckend(neckstart.x + (Imager.faces[i].width /1.6),neckstart.y + Imager.faces[i].height*0.25 );

//                    Point bodystart(center.x - (bodywidth /2 ),neckend.y );
//                    Point bodyend(bodystart.x + bodywidth,bodystart.y + bodyheight);


//                    if (bodystart.x < 0){

//                        bodystart.x = 0;

//                    }
//                    if (bodystart.y < 0){

//                         bodystart.y = 0;
//                    }
//                    if (bodyend.x > drawing.size().width){

//                        bodyend.x = drawing.size().width ;

//                    }

//                    if (bodyend.y > drawing.size().height){

//                         bodyend.y = drawing.size().height ;
//                    }


//                    cv::Rect R(bodystart,bodyend);

//                    Mat body = drawing(R) ;
//                    Mat src = inputImage(R);



//                    rectangle(drawing,neckstart,neckend,Scalar(255,255,0),4,8,0);
//                    rectangle(drawing,bodystart,bodyend,Scalar(255,0,255),4,8,0);
//                    rectangle(inputImage,neckstart,neckend,Scalar(255,255,0),4,8,0);
//                    rectangle(inputImage,bodystart,bodyend,Scalar(255,0,255),4,8,0);

//                    Mat dst, gsrc;
//                    Canny(src, dst, 50, 200, 3);
//                    imshow("j",dst);

//                    cvtColor(src,gsrc,CV_BGR2GRAY);

//                    vector<Point2f> corners;
//                    goodFeaturesToTrack(gsrc,corners, 500, 0.5, 10);
//                    for (size_t idx = 0; idx < corners.size(); idx++) {
//                            circle(src,corners.at(idx),2,Scalar(255,255,0),2);
//                    }
//                    imshow("srv",src);





//                }


//            }





//    }


//    if (uip->Skin->isChecked()){
//    inputImage = drawing;
//    }

//    if (uip->HaarFull->isChecked()){
//        vector<Rect> fullbody_results;
//        fullbody.detectMultiScale( ThumbHOG, fullbody_results, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(5, 5) );
//        for (int i =0; i < fullbody_results.size(); i++){

//            rectangle(ThumbHOG,fullbody_results.at(i),Scalar( 255, 0, 255 ),2,1,0);

//        }
//        imshow("HaarFull", ThumbHOG);


//    }
//    if (uip->HaarUB->isChecked()){

//        vector<Rect> upperbody_results;
//        upperbody.detectMultiScale( ThumbHOG, upperbody_results, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(5, 5) );
//        for (int i =0; i < upperbody_results.size(); i++){

//            rectangle(ThumbHOG,upperbody_results.at(i),Scalar( 230, 0, 255 ),2,1,0);

//        }
//        imshow("HaarUB", ThumbHOG);

//    }
//    if (uip->HaarLB->isChecked()){
//        vector<Rect> lowerbody_results;
//        lowerbody.detectMultiScale( ThumbHOG, lowerbody_results, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100) );
//        for (int i =0; i < lowerbody_results.size(); i++){

//            rectangle(ThumbHOG,lowerbody_results.at(i),Scalar( 200, 0, 255 ),2,1,0);

//        }
//        imshow("HaarLB", ThumbHOG);

//    }
//    if (uip->HogFull->isChecked()){

//            vector<Rect> found;
//            HOGDescriptor hog;
//            hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
//            hog.detectMultiScale(ThumbHOG, found, 0, Size(4,4), Size(32,32), 1.05,2);


//            for (int i =0; i < found.size(); i++){

//                rectangle(ThumbHOG,found.at(i),Scalar( 255, 0, 255 ),2,1,0);

//            }
//            imshow("HOG", ThumbHOG);



//    }



//    if (uip->Faces->isChecked()){


////        //Draw the face rectangles
//        for( size_t i = 0; i < Imager.faces.size(); i++ ){

////            qDebug() << i;
//            Point center( Imager.faces[i].x + Imager.faces[i].width*0.5, Imager.faces[i].y + Imager.faces[i].height*0.5 );


//            ellipse( inputImage, center, Size( Imager.faces[i].width*0.5, Imager.faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
//        }

//    }

//    QPixmap DisplayI = ASM::cvMatToQPixmap(inputImage);
//    QPixmap Scaled = DisplayI.scaled(uip->ImagePreview->width(),uip->ImagePreview->height(),Qt::KeepAspectRatio,Qt::FastTransformation);
//    uip->ImagePreview->setPixmap(Scaled);


}
void DisplayImage(ImageInfo Image){
    if( !fullbody.load( fullbody_cascade_name ) ){
        //qDebug() << "Cannot load face cascase";

    };
    if( !upperbody.load( upperbody_cascade_name ) ){
        //qDebug() << "Cannot load face cascase";

    };
    if( !lowerbody.load( lowerbody_cascade_name ) ){
        //qDebug() << "Cannot load face cascase";

    };
    Imager = Image;


    uip->SkinPerc->setText("Skin % : " + QString::number(Imager.skin));

    UpdateDisplay();

}
void ImageInspector::on_Refresh_clicked()
{
UpdateDisplay();
}

void ImageInspector::on_Faces_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_Skin_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_Skeleton_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_pushButton_clicked()
{

}

void ImageInspector::on_HaarUB_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_HaarLB_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_HaarFull_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_HogFull_clicked()
{
    UpdateDisplay();
}

void ImageInspector::on_NextRight_clicked()
{
    //Get next image
    Imager = GetNextImage(Imager.Display->Image,1);
    uip->IDLabel->setText("IMG" + Imager.id);
    this->setWindowTitle("Image Inspector - IMG" + Imager.id);
    UpdateDisplay();

}

void ImageInspector::on_NextLeft_clicked()
{
    //Get prev image
    Imager = GetNextImage(Imager.Display->Image,2);
    uip->IDLabel->setText("IMG" + Imager.id);
    this->setWindowTitle("Image Inspector - IMG" + Imager.id);
    UpdateDisplay();
}
