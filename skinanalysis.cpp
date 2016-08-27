#include "skinanalysis.h"
#include "autoadjustcontrast.h"
#include "QApplication"
#include <QDebug>
SkinAnalysis::SkinAnalysis()
{

}
QVector<vector<Point>> filterbysize(vector<vector<Point>> contours, int filtersize){
    QVector<vector<Point>> retvect;
    for (int i = 0; i < contours.size(); i++){
        if(contours[i].size() > filtersize){
            //add it to the vectoro

            retvect.append(contours[i]);
        }
    }


    return retvect;
}

//DUPLICATE FUNCTION! CLEAN ME!
double maxi(double x, double y, double z) {
    double max = x;
    if (y > max) {
        max = y;
    }
    if (z > max) {
        max = z;
    }
    return max;
}

double mini(double x, double y, double z) {
    double min = x;
    if (y < min) {
        min = y;
    }
    if (z < min) {
        min = z;
    }
    return min;
}
double DetectSkin(Mat image,vector<Rect> hsvFaces,int UseTomsMethod,int CannyTh,int RatioTh, int Uh, int Us, int Uv, int Lh, int Ls, int Lv){

    //Auto Brightness / Contrast
    BrightnessAndContrastAuto(image,image,0);

    //Zero the return value
    double returnpercent = 0;
    double percent = 0;

    //Create the Mats
    Mat bw = Mat::zeros( image.size(), CV_8UC1 );
    Mat hsv; //Converted to HSV for thesholding
    Mat Original = image.clone();

    //Convert to HSV
    cvtColor(image, hsv, CV_BGR2HSV);

    //BW is mask from HSV

    if(UseTomsMethod == 1){
        inRange(hsv, Scalar(Lh, Ls, Lv), Scalar(Uh, Us, Uv), bw);
    }else{

         for (int i = 0; i < Original.rows; ++i)
         {
             Vec3b* pixel = Original.ptr<cv::Vec3b>(i); // point to first pixel in row
             Vec3b* pixelHSV = hsv.ptr<cv::Vec3b>(i); // point to first pixel in row
             for (int j = 0; j < Original.cols; ++j)
             {
                uchar R = pixel[j][2];
                uchar G = pixel[j][1];
                uchar B = pixel[j][0];
                uchar V = pixelHSV[j][2];
                uchar S = pixelHSV[j][1];
                uchar H = pixelHSV[j][0] * 2;
                if ((R > 220 && G > 210 && B > 170 && (R - G) > 15 && R > B && G > B) || (R > 95 && G > 40 && B > 20 && maxi(R, G ,B) - mini(R, G , B) > 15 && (R - G) > 15 && R > G && R > B)){
                    if ((0 <= H <= 50)||(340 <= H <= 360) && (0.2 < S) && 0.35 < V) {
                     bw.at<uchar>(Point(j,i)) = 255;
                    }
                }

               }
             //qDebug() << "Row Done";
        }



    }



    //inRange(hsv, Scalar(0, 10, 38), Scalar(40, 190, 255), bw);


    //Create some vectors for the contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( bw, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    int width = image.size().width;

    //Remove any blobs that are less than 5% the total picture size
    QVector<vector<Point>> s = filterbysize(contours,(width / 100)*5);


    //Draw the left over contours onto a new MAT
    Mat drawing = Mat::zeros( hsv.size(), CV_8UC1 );
    for(int i = 0; i < s.size(); i++)
    {
        drawContours( drawing, s.toStdVector(), i, Scalar( 255, 255, 255 ), -1, 8, hierarchy,0,Point() );
    }

float CannyPercent;
    if (hsvFaces.size() > 0){

        // We have faces :-)
        for( size_t i = 0; i < hsvFaces.size(); i++ ){
            //For each detected face

            //Get center point
            Point center( hsvFaces[i].x + hsvFaces[i].width*0.5, hsvFaces[i].y + hsvFaces[i].height*0.5 );
            qApp->processEvents();


            int bodywidth = hsvFaces[i].width * 1.7;
            int bodyheight = hsvFaces[i].width * 5;
            Point neckstart(center.x - (hsvFaces[i].width *0.314),center.y + hsvFaces[i].height*0.4 );
            Point neckend(neckstart.x + (hsvFaces[i].width /1.6),neckstart.y + hsvFaces[i].height*0.25 );
            Point bodystart(center.x - (bodywidth /2 ),neckend.y );
            Point bodyend(bodystart.x + bodywidth,bodystart.y + bodyheight);
            Rect DrawSize(Point(0,0),drawing.size());

            //Check if the end of the rectangle is within the MAT (Otherwise nasty crash crash)
            if (bodystart.x < 0){
                bodystart.x = 0;
            }
            if (bodystart.y < 0){
                 bodystart.y = 0;
            }
            if (!DrawSize.contains(Point(bodystart.x,0))){
                bodystart.x = DrawSize.br().x -1 ;
            }
            if (!DrawSize.contains(Point(0,bodystart.y))){
                bodystart.y = DrawSize.br().y -1 ;
            }
            if (!DrawSize.contains(Point(bodyend.x,0))){
                bodyend.x = DrawSize.br().x ;
            }
            if (!DrawSize.contains(Point(0,bodyend.y))){
                 bodyend.y = DrawSize.br().y ;
            }

            Rect R(bodystart,bodyend);

            //Mat body now contains all the white in the rect
            Mat body = drawing(R) ;

            //Canny Line Detection, Count stuff
            Mat canny;
            Canny(Original(R), canny, 50, 200, 3);
            int total = canny.cols * canny.rows;
            float countnonzero = countNonZero(canny);
            CannyPercent = countnonzero /(total / 100);

            //Get total size of body
            total = body.cols * body.rows;
            countnonzero = countNonZero(body);

            if (countnonzero > 0){
                percent = countnonzero /(total / 100);
            }else{
                percent = 0;
            }


            int ratio = ((body.cols / body.rows) * 100);

            if (ratio == RatioTh){

                    if (percent > returnpercent){
                        //UGLY HACK!!!! FIXME
                        if (CannyPercent > CannyTh){


                            returnpercent = 0;
                        }else{
                            returnpercent = percent;

                        }

                    }

                  //  imshow(QString::number(i).toStdString(),body);
            }


         //   rectangle(drawing,neckstart,neckend,Scalar(255,255,0),4,8,0);
         //   rectangle(drawing,bodystart,bodyend,Scalar(255,0,255),4,8,0);

        }
    }

qDebug()<< QString::number(CannyPercent);
    return returnpercent;

}
