#ifndef AUTOADJUSTCONTRAST_H
#define AUTOADJUSTCONTRAST_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
using namespace cv;
void BrightnessAndContrastAuto(Mat src, Mat dst, float clipHistPercent);
class AutoAdjustContrast
{
public:
    AutoAdjustContrast();
};

#endif // AUTOADJUSTCONTRAST_H
