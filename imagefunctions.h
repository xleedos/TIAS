#ifndef IMAGEFUNCTIONS_H
#define IMAGEFUNCTIONS_H
#include <QByteArray>
#include <QFile>
//#include <QDebug>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDateTime>
#include <QVector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <QPixmap>
#include <QBuffer>
#include <QGroupBox>
#include "faceanalysis.h"
#include "skinanalysis.h"


using namespace cv;

Mat ResizeMat(Mat image, int width, int height);
QByteArray MD5SUM(QString filename);
QDateTime ToUnixEpoch(QString time);
bool fileExists(QString path);
class CustomGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    int Image;


signals:

public slots:

protected:


};

class ImageFunctions
{
public:
    ImageFunctions();

};

struct Meta {
  QString Descriptiom;
  QString Make;
  QString Model;
  QString Software;
  QString Filetime;
  QString DateTime_Analogue;
  QString DateTime_Digital;
  QString Copyright;
  QString Longitude;
  QString Latitude;
  QString Altitude;
  QString GPSDOP;

} ;
struct Flags {
  bool KFFBad = false;
  bool Skin = false;
  bool Face = false;
  bool Small = false;
  bool Meta = false;
  bool KFFGood = false;
  bool GPS = false;

} ;
class ImageInfo
{

   public:
   QString id;
   QByteArray thumbnail;
   QString md5;
   int width;
   int height;
   qint64 filesize;
   QString path;
   Meta meta;
   Flags flags;
   QDateTime datefirstseen;
   QDateTime modified;
   QDateTime accessed;
   QDateTime created;
   CustomGroupBox * Display;
   vector<Rect> faces;
   vector<Rect> eyes;
   double skin;
   int sort;

   //Meta


};

ImageInfo AnalyseImage(QString path, int FaceMin, double FaceScale, int FaceNe, int UseTomsMethod, int CannyTh, double RatioTh, int Uh, int Us, int Uv, int Lh, int Ls, int Lv);
#endif // IMAGEFUNCTIONS_H
