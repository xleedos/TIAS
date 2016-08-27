#ifndef FACEANALYSIS_H
#define FACEANALYSIS_H


#include <QTime>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/cuda.hpp>


using namespace cv;
using namespace cv::cuda;
using namespace std;

vector<Rect> DetectFaces(Mat Image,int FaceMin,int FaceNe,double FaceScale);
vector<Rect> DetectEyes(Mat Image, vector<Rect> faces);
Mat DrawFace(Mat greyscaleImage, vector<Rect> faces, vector<Rect> eyes);


class FaceAnalysis
{
public:
    FaceAnalysis();
};

#endif // FACEANALYSIS_H
