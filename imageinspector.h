#ifndef IMAGEINSPECTOR_H
#define IMAGEINSPECTOR_H

#include <QDialog>
#include <QString>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <QDebug>
#include "matconversion.h"
#include "imagefunctions.h"


void DisplayImage(ImageInfo Image);

namespace Ui {
class ImageInspector;
}

class ImageInspector : public QDialog
{
    Q_OBJECT

public:
    explicit ImageInspector(QWidget *parent = 0);
    ~ImageInspector();

private slots:
    void on_Refresh_clicked();

    void on_Faces_clicked();

    void on_Skin_clicked();

    void on_Skeleton_clicked();

    void on_pushButton_clicked();

    void on_HaarUB_clicked();

    void on_HaarLB_clicked();

    void on_HaarFull_clicked();

    void on_HogFull_clicked();

    void on_NextRight_clicked();

    void on_NextLeft_clicked();

private:
    Ui::ImageInspector *ui;
};

#endif // IMAGEINSPECTOR_H
