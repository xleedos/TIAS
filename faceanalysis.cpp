#include "faceanalysis.h"
#include <QDir>
#include <QDebug>


QString face_cascade_name = QDir::toNativeSeparators(QDir::currentPath())+ "//haarcascade_frontalface_alt.xml"  ;
QString face_alt_cascade_name = QDir::toNativeSeparators(QDir::currentPath())+ "//haarcascade_frontalface_default.xml" ;
QString side_face_cascade_name = QDir::toNativeSeparators(QDir::currentPath())+ "//haarcascade_profileface.xml" ;



CascadeClassifier face_cascade;
CascadeClassifier face_alt_cascade;

CascadeClassifier side_face_cascade;



vector<Rect> DetectFaces(Mat Image, int FaceMin, int FaceNe, double FaceScale){


    vector<Rect> faces;
    vector<Rect> faces_alt;
    vector<Rect> side_faces;

    if( !face_cascade.load( face_cascade_name.toStdString() ) ){
        qDebug() << "Cannot load face cascase";

    };
    if( !face_alt_cascade.load( face_alt_cascade_name.toStdString() ) ){
        qDebug() << "Cannot load face cascase";

    };
    if( !side_face_cascade.load( side_face_cascade_name.toStdString() ) ){
        qDebug() << "Cannot load face cascase";

    };


    Mat greyscaleImage;


    cvtColor(Image, greyscaleImage,CV_BGR2GRAY);

    equalizeHist( greyscaleImage, greyscaleImage );

    face_cascade.detectMultiScale( greyscaleImage, faces, FaceScale, FaceNe, 0|CV_HAAR_SCALE_IMAGE, Size(FaceMin, FaceMin) );
    face_alt_cascade.detectMultiScale( greyscaleImage, faces_alt, FaceScale, FaceNe, 0|CV_HAAR_SCALE_IMAGE, Size(FaceMin, FaceMin) );
    side_face_cascade.detectMultiScale( greyscaleImage, side_faces, FaceScale, FaceNe, 0|CV_HAAR_SCALE_IMAGE, Size(FaceMin, FaceMin) );




    QVector<Rect> combined;

    for (int i = 0; i < faces.size(); i++){
        combined.append(faces.at(i));
    }
    for (int i = 0; i < side_faces.size(); i++){
        combined.append(side_faces.at(i));
    }
    for (int i = 0; i < faces_alt.size(); i++){
        combined.append(faces_alt.at(i));
    }


    return combined.toStdVector();
}

Mat DrawFace(Mat greyscaleImage, vector<Rect> faces, vector<Rect> eyes){

    for( size_t i = 0; i < faces.size(); i++ ){
        //For each detected face
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        ellipse( greyscaleImage, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        //Mat circleface = greyscaleImage( faces[i] );



        return greyscaleImage;

    }

}


