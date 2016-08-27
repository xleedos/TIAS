#include "imagefunctions.h"
#include "matconversion.h"
#include "exif.h"


using namespace cv;


ImageFunctions::ImageFunctions()
{

}
//MD5 Function
//#############################################
QByteArray MD5SUM(QString filename){

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray fileData = file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData,QCryptographicHash::Md5);
        //qDebug << hashData.toHex();  // 0e0c2180dfd784dd84423b00af86e2fc
        return hashData.toHex();


    }
}

//File Exists
//###############################################
bool fileExists(QString path) {
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isFile();

}

//Convert Linux Epoch
//###############################################
QDateTime ToUnixEpoch(QString UnixTime) {
    QDateTime timestamp;
    timestamp.setTime_t(UnixTime.toInt());
    return timestamp;
}

//QVector<Rect> NMS(QVector<Rect> boxes,int theshold){
//    int GreatestArea = 0;
//    int GreatestAreaIndex;
//    QVector<Rect> ReturnValue;
//    if (boxes.size > 1){
//        //More than one box




//        //Get the largest box
//        for (int i = 0; i < boxes.size(); i++){
//            if (boxes.at(i).area() > GreatestArea){
//                GreatestArea = boxes.at(i).area();
//                GreatestAreaIndex = i;
//            }
//        for (int i = 0; i < boxes.size(); i++){
//            //For each box
//            if (i != GreatestAreaIndex){
//                //Skipping the biggest
//                if (boxes.at(i).tl().x < (boxes.at(GreatestAreaIndex).tl().x) + threshold &&\
//                    boxes.at(i).tl().x > (boxes.at(GreatestAreaIndex).tl().x) - threshold &&\
//                    boxes.at(i).tl().y > (boxes.at(GreatestAreaIndex).tl().y) - threshold &&\
//                    boxes.at(i).tl().y < (boxes.at(GreatestAreaIndex).tl().y) + threshold){
//                    if (boxes.at(i).br().x < (boxes.at(GreatestAreaIndex).br().x) + threshold &&\
//                        boxes.at(i).br().x > (boxes.at(GreatestAreaIndex).br().x) - threshold &&\
//                        boxes.at(i).br().y > (boxes.at(GreatestAreaIndex).br().y) - threshold &&\
//                        boxes.at(i).br().y < (boxes.at(GreatestAreaIndex).br().y) + threshold){

//                        //If the


//                    }


//                    //The top left corner of the smaller rect is within range. This should be removed


//                }
//            }



//        }
//    }else{
//            return boxes;
//        }

//    }


//}


//Resize keeping aspect ratio
//###############################################

Mat ResizeMat(Mat image, int width, int height){

    Mat ReturnImage;

    //Get image dimensions
    Size osize = image.size();
    int owidth = osize.width;
    int oheight = osize.height;

    // If the width is prominant
    if (owidth > oheight){
        //
        if (owidth > width){
            double ratio = owidth / width; //This will give us x.?
            double scale = 1 / ratio;
            resize(image,ReturnImage,Size(),scale,scale);
        }else{

            //Nothing needs to be resized
            ReturnImage = image;
        }

    }else{
        if (oheight > height){
            double ratio = oheight / height; //This will give us x.?
            double scale = 1 / ratio;
            resize(image,ReturnImage,Size(),scale,scale);
        }else{

            ReturnImage = image;
        }
    }
    return ReturnImage;



}




//Generate Thumb, Size, ColourDepth,
//###############################################
ImageInfo AnalyseImage(QString path,int FaceMin,double FaceScale,int FaceNe,int UseTomsMethod,int CannyTh,double RatioTh,int Uh,int Us, int Uv,int Lh,int Ls, int Lv){




    Mat originalImage = imread(path.toStdString(),3);


    //Dynamically shrinking the image to 1000px will ensure fast analysis times
    Mat inputImage = originalImage.clone();

    //\\qDebug << QTime::currentTime().toString() + "START - Resize, Gen Thumbnail start " ;

    Size osize = originalImage.size();
    int owidth = osize.width;
    int oheight = osize.height;


    if (owidth > oheight){
        if (owidth > 1000){
            double ratio = owidth / 1000; //This will give us x.?
            double scale = 1 / ratio;
            resize(originalImage,inputImage,Size(),scale,scale);
        }

    }else{
        if (oheight > 1000){
            double ratio = oheight / 1000; //This will give us x.?
            double scale = 1 / ratio;
            resize(originalImage,inputImage,Size(),scale,scale);
        }
    }



    Mat thumbnail;
    Size isize = inputImage.size();
    int width = isize.width;
    int height = isize.height;
    //GetExifData(path);

    //Calculate Aspect Ratio

        if (width > height){
            if (width > 200){

                double ratio = width / 200; //This will give us x.?
                double scale = 1 / ratio;
                resize(inputImage,thumbnail,Size(),scale,scale);

            }

        }else{
            if (height > 200){
                double ratio = height / 200; //This will give us x.?
                double scale = 1 / ratio;
                resize(inputImage,thumbnail,Size(),scale,scale);

            }else{
             thumbnail = inputImage.clone();

            }

        }
     //\\qDebug << QTime::currentTime().toString() + "END - Resize, Gen Thumbnail start " ;
     //\\qDebug << QTime::currentTime().toString() + "START - MD5 Sum " ;
     QString MD5 = MD5SUM(path);
     //\\qDebug << QTime::currentTime().toString() + "STOP - MD5 Sum " ;
     //\\qDebug << QTime::currentTime().toString() + "START - Meta " ;
     QFileInfo info(path);
     QDateTime modified = info.lastModified();
     QDateTime accessed = info.lastRead();
     QDateTime created = info.created();
     QDateTime FirstSeen = QDateTime::currentDateTime();
     ImageInfo ReturnImage;

     QImage img = ASM::cvMatToQImage(thumbnail);


     QByteArray bytes;
     QBuffer buffer (&bytes);
     buffer.open(QBuffer::WriteOnly);
     img.save(&buffer, "PNG");
     //\\qDebug << QTime::currentTime().toString() + "STOP - Meta " ;


     //Detect Faces

     //Get Values from mainui i guess?



     //\\qDebug << QTime::currentTime().toString() + "START - Detect Faces" ;


     //UGLY HACK! TODO FIXME! ## Images may be a format that has more channels
     //Checking the channels
     if (inputImage.channels() == 3 || inputImage.channels() == 4){
         //check for channels
         ReturnImage.faces = DetectFaces(inputImage,FaceMin,FaceNe,FaceScale);
         ReturnImage.skin = DetectSkin(inputImage,ReturnImage.faces,UseTomsMethod,CannyTh,RatioTh,Uh,Us,Uv,Lh,Ls,Lv);

     }else{
         //exit the function if less than 2
     }

         //qDebug << QTime::currentTime().toString() + "STOP - Skin Detector " ;

    //qDebug << QTime::currentTime().toString() + "START - Packing META " ;
     //Create the class
     ReturnImage.accessed = accessed;
     ReturnImage.created = created;
     ReturnImage.datefirstseen = FirstSeen;
     ReturnImage.filesize = info.size();
     ReturnImage.height = height;
     ReturnImage.md5 = MD5;

     ReturnImage.modified = modified;
     ReturnImage.path = path;
     ReturnImage.thumbnail = bytes.toBase64();
     ReturnImage.width = width;



     return ReturnImage;
     //qDebug << QTime::currentTime().toString() + "END" ;
}




