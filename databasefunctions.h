#ifndef DATABASEFUNCTIONS_H
#define DATABASEFUNCTIONS_H
#include <QStringList>
#include <QString>
//#include <QDebug>
#include <QSqlDatabase>
#include <QStandardPaths>
#include "imagefunctions.h"
#include <QSqlQuery>
#include <QList>
#include <QUrl>
#include <QDirIterator>
#include <QDir>
#include <QApplication>
#include <exif.h>
void ExifData(QString path, QString ImageID);
bool initialiseDatabase();
QList<QStringList> GetCaseLists();
QList<QDir> CheckCaseFolders(QString caseid);
void ImportImages(QDir url, QString caseID);
QList<ImageInfo> GetCaseImages(int caseID);
QVector<int> GetTags(int ImageID);
bool CreateCase(QString ExaminerName, QString CaseName, QString Description);
double GetNumImages(QString caseID);
void PurgeDatabase();
class DatabaseFunctions
{
public:
    DatabaseFunctions();
};

#endif // DATABASEFUNCTIONS_H
