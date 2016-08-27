#ifndef SKINANALYSIS_H
#define SKINANALYSIS_H

#include <QMainWindow>
#include <QFileDialog>
#include <qstring.h>
//#include <QDebug>
#include "QMessageBox"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d.hpp>
#include "faceanalysis.h"


using namespace cv;
double DetectSkin(Mat image, vector<Rect> hsvFaces, int UseTomsMethod, int CannyTh, int RatioTh, int Uh, int Us, int Uv, int Lh, int Ls, int Lv);
QVector<vector<Point>> filterbysize(vector<vector<Point>> contours, int filtersize);
void Experiment(QString picturepath);
class SkinAnalysis
{
public:
    SkinAnalysis();
};

#endif // SKINANALYSIS_H
