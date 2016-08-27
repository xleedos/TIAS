#include "databasefunctions.h"
#include "mainui.h"
#include <QDir>


QSqlDatabase m_db;

DatabaseFunctions::DatabaseFunctions()
{

}


QString SerialiseFaces(vector<Rect> faces){

    if(faces.size() > 0){

    QString Output;
    for (int i =0; i < faces.size(); i++){
        //qDebug()<< QString::number(faces.at(i).br().x) + ":" + QString::number(faces.at(i).br().y) + ":" + QString::number(faces.at(i).tl().x) + ":" + QString::number(faces.at(i).tl().y) + ";";
       Output.append( QString::number(faces.at(i).br().x) + ":" + QString::number(faces.at(i).br().y) + ":" + QString::number(faces.at(i).tl().x) + ":" + QString::number(faces.at(i).tl().y) + ";");

    }

    return Output;
    }else{
        return "";
    }

}

vector<Rect> DeserialiseFaces( QString faces){

    QVector<Rect> Output;
    QStringList vectors = faces.split(";",QString::SkipEmptyParts);

    for (int i = 0; i < vectors.size(); i++){

        QStringList face = vectors.at(i).split(":",QString::SkipEmptyParts);

        Rect rect(Point(face.at(0).toInt(),face.at(1).toInt()),Point(face.at(2).toInt(),face.at(3).toInt()));
        //qDebug() << rect.x;
//        rect.br() = Point(face.at(0).toInt(),face.at(1).toInt());
//        rect.tl() = Point(face.at(2).toInt(),face.at(3).toInt());

        Output.append(rect);

    }
    return Output.toStdVector();


}

void PurgeDatabase(){

QFile destinationFile;
QFile sourceFile(":/TomsImageAnalyser/CaseDB");
QDir destination(QDir::toNativeSeparators(QDir::currentPath()));
destinationFile.setFileName(destination.filePath("CaseDB"));
sourceFile.copy(destinationFile.fileName());

}



void ExifData(QString path, QString ImageID){

    //Return 0 = No Exif
    //Return 1 = Exif No GPS
    //Return 2 = Exif and GPS
    QFile file(path);


    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray fileData = file.readAll();
        easyexif::EXIFInfo Result;
        Result.parseFrom(fileData.toStdString());
        QSqlQuery InsertExifData(m_db);

        if(QString::fromStdString(Result.Make) == "" && QString::fromStdString(Result.Model) == ""){


        }else{

            //qDebug() << ImageID + ": EXIF EXIF EXIF : " + QString::fromStdString(Result.Make);
            InsertExifData.prepare("INSERT INTO `ExifData` VALUES (NULL,?,?,?,?,?,?,?,?,?,?,?,?,?);");
            InsertExifData.addBindValue(ImageID);
            InsertExifData.addBindValue(QString::fromStdString(Result.ImageDescription));
            InsertExifData.addBindValue(QString::fromStdString(Result.Make));
            InsertExifData.addBindValue(QString::fromStdString(Result.Model));
            InsertExifData.addBindValue(QString::fromStdString(Result.Software));
            InsertExifData.addBindValue(QString::fromStdString(Result.DateTime));
            InsertExifData.addBindValue(QString::fromStdString(Result.DateTimeOriginal));
            InsertExifData.addBindValue(QString::fromStdString(Result.DateTimeDigitized));
            InsertExifData.addBindValue(QString::fromStdString(Result.Copyright));
            InsertExifData.addBindValue(Result.GeoLocation.Longitude);
            InsertExifData.addBindValue(Result.GeoLocation.Latitude);
            InsertExifData.addBindValue(Result.GeoLocation.Altitude);
            InsertExifData.addBindValue(Result.GeoLocation.DOP);
            InsertExifData.exec();

            //qDebug() << ImageID + ": Exif : " + QString::number(Result.GeoLocation.Latitude);
            //We have a face so lets add the coords to the face database
            QSqlQuery InsertExifFlag(m_db);
            InsertExifFlag.prepare("INSERT INTO `Tags` VALUES (NULL,?,?,5,NULL);");
            InsertExifFlag.addBindValue(ImageID);//ImageID
            InsertExifFlag.addBindValue(QDateTime::currentDateTime().toTime_t());//CurrentTime
            InsertExifFlag.exec();

            if((int)round(Result.GeoLocation.Longitude) != 0 && (int)round(Result.GeoLocation.Latitude) != 0){
                //We have a face so lets add the coords to the face database
                QSqlQuery InsertGeoFlag(m_db);
                InsertGeoFlag.prepare("INSERT INTO `Tags` VALUES (NULL,?,?,7,NULL);");
                InsertGeoFlag.addBindValue(ImageID);//ImageID
                InsertGeoFlag.addBindValue(QDateTime::currentDateTime().toTime_t());//CurrentTime
                InsertGeoFlag.exec();


            }




    }

    }
}

//SetupDatabase
//###############################################
bool initialiseDatabase(){

    QString DatabasePath = QStandardPaths::locate(QStandardPaths::AppDataLocation, QString(), QStandardPaths::LocateDirectory);
    if (!fileExists(QDir::toNativeSeparators(QDir::currentPath())+"CaseDB")){
        //PurgeDatabase();
        //Couldnt find the database! Create a new one?
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(DatabasePath + "/CaseDB");
    if (!m_db.open())
    {
       //qDebug() << "Error: connection with database fail";
       return false;
    }
    else
    {
        return true;
    }

}

QList<QDir> GetImages(QString caseid){
    QList<QDir> DirList;
    QSqlQuery query(m_db);
    query.exec("select URL from ImageDetails where Case_ID = '" + caseid + "'");
    //qDebug() << DirList;
    return DirList;

}


void ImportImages(QDir url, QString caseID){

    int skinimages = 0;
    UpdateStatusLabel("Loading already analysed images");
    qApp->processEvents();


    QSqlQuery GetAnalysedImages(m_db);
    GetAnalysedImages.prepare("select URL from `ImageDetails` where `Case_ID` = ?;");
    GetAnalysedImages.addBindValue(caseID);//ImageID
    GetAnalysedImages.exec();
    QVector<QString> AlreadyAnalysed;
        while (GetAnalysedImages.next()) {
            AlreadyAnalysed.append(GetAnalysedImages.value(0).toString());
    }


            //#######################################################

   QDirIterator ImageFinder(url,QDirIterator::Subdirectories);
   QVector<QString> Files;

   QFile file("Debug.txt");
   file.open(QIODevice::ReadWrite);
   QTextStream stream( &file );



   while (ImageFinder.hasNext()){
       QString Path = ImageFinder.next();
       if(Path.endsWith("JPG",Qt::CaseInsensitive)||
          Path.endsWith("PNG",Qt::CaseInsensitive)){
           if(!AlreadyAnalysed.contains(Path)){
               Files.append(Path);
           }
       }
   }

   QVector<double> Settings = GetSettings();


       for (int i =0; i < Files.count(); i++){

        UpdateStatusLabel("Analysing image : " + QString::number(i) + " of " + QString::number(Files.size()) + "\nSkin Images : " + QString::number(skinimages) );
        stream << Files.at(i) << "\n";

        Mat test = imread(Files.at(i).toStdString(),3);
        if (test.data != NULL){




        ImageInfo image = AnalyseImage(Files.at(i),Settings[0],Settings[1],Settings[2],Settings[12],Settings[4],Settings[5],Settings[6],Settings[7],Settings[8],Settings[9],Settings[10],Settings[11]);
        qApp->processEvents();
        QSqlQuery InsertImageDetail(m_db);
        InsertImageDetail.prepare("INSERT INTO `ImageDetails` VALUES (NULL,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
        InsertImageDetail.addBindValue(caseID);
        InsertImageDetail.addBindValue(image.datefirstseen.toTime_t());
        InsertImageDetail.addBindValue(image.filesize);
        InsertImageDetail.addBindValue(image.width);
        InsertImageDetail.addBindValue(image.height);
        InsertImageDetail.addBindValue(image.md5);
        InsertImageDetail.addBindValue("");
        InsertImageDetail.addBindValue(image.thumbnail);
        InsertImageDetail.addBindValue(image.path);
        InsertImageDetail.addBindValue(image.modified.toTime_t());
        InsertImageDetail.addBindValue(image.accessed.toTime_t());
        InsertImageDetail.addBindValue(image.created.toTime_t());
        InsertImageDetail.addBindValue(image.skin);
        InsertImageDetail.addBindValue(SerialiseFaces(image.faces));
        InsertImageDetail.exec();
        int lastrow = InsertImageDetail.lastInsertId().toInt();

        //If MD5 is in the lisst

        QSqlQuery GetKFF(m_db);
        //qDebug() << image.md5;
        GetKFF.prepare("select * from KFFDatabase where KFFDatabase.MD5 = ? LIMIT 1;");
        GetKFF.addBindValue(image.md5);//ImageID
        GetKFF.exec();
        QSqlQuery InsertKFFFlag(m_db);




            while (GetKFF.next()) {

                //qDebug() << "FOUND A KFF";
                if (GetKFF.value(1).toString() == "Bad"){
                    //KFF Bad file
                    InsertKFFFlag.prepare("INSERT INTO `Tags` VALUES (NULL,?,?,1,NULL);");
                    //qDebug() << "FOUND A BAD ONE";

                }else{
                    //KFF Good file
                    InsertKFFFlag.prepare("INSERT INTO `Tags` VALUES (NULL,?,?,6,NULL);");
                    //qDebug() << "FOUND A GOOD ONE";

                }
                InsertKFFFlag.addBindValue(lastrow);//ImageID
                InsertKFFFlag.addBindValue(image.datefirstseen.toTime_t());//CurrentTime
                InsertKFFFlag.exec();

               }







        if(image.faces.size() > 0){

            //We have a face so lets add the coords to the face database
            QSqlQuery InsertFaceFlag(m_db);
            InsertFaceFlag.prepare("INSERT INTO `Tags` VALUES (NULL,?,?,3,NULL);");
            InsertFaceFlag.addBindValue(lastrow);//ImageID
            InsertFaceFlag.addBindValue(image.datefirstseen.toTime_t());//CurrentTime
            InsertFaceFlag.exec();

        }

        if(image.skin > Settings[3]){
            //May have done something silly here! ##FIXME double skin check
            QSqlQuery InsertSkinFlag(m_db);
            InsertSkinFlag.prepare("INSERT INTO `Tags` VALUES (NULL,?,?,2,?);");
            InsertSkinFlag.addBindValue(lastrow);//ImageID
            InsertSkinFlag.addBindValue(image.datefirstseen.toTime_t());//CurrentTime
            InsertSkinFlag.addBindValue(image.skin);
            skinimages++;
            InsertSkinFlag.exec();

        }






            ExifData(image.path,QString::number(lastrow));
       }

           }
   }



QList<QStringList> GetCaseLists(){

   QList<QStringList> GetCaseDetails;
   QSqlQuery query(m_db);
   query.exec("select * from `CaseDetails`");

   while (query.next()) {
       QStringList cases;
       cases.append(query.value(0).toString());//ID
       cases.append(query.value(1).toString());//Investigator
       cases.append(query.value(2).toString());//Date
       cases.append(query.value(3).toString());//CaseName
       cases.append(query.value(4).toString());//Number of Images
       cases.append(query.value(5).toString());//Description
       GetCaseDetails.append(cases);

      }

   return GetCaseDetails ;

}

QVector<int> GetTags(int imageID){
    QVector<int> tags;
    QSqlQuery query(m_db);
    query.prepare("select * from Tags where Image_ID = ?");
    query.addBindValue(imageID);
    query.exec();
    while (query.next()) {
    tags.append( query.value(3).toInt());
    }
    return tags;


}







QList<ImageInfo> GetCaseImages(int caseID){


    QSqlQuery query(m_db);
    query.prepare("select * from ImageDetails where Case_ID = ?");
    query.addBindValue(caseID);
    query.exec();
    QList<ImageInfo> ReturnList;
    while (query.next()) {
        ImageInfo ReturnImage;

        QDateTime accessed;
        accessed.setTime_t(query.value(11).toInt());
        QDateTime created;
        created.setTime_t(query.value(12).toUInt());
        QDateTime modified;
        modified.setTime_t(query.value(10).toUInt());
        QDateTime firstseen;
        firstseen.setTime_t(query.value(2).toUInt());


        //Offset by one for PK an FK
        ReturnImage.id = query.value(0).toString();
        ReturnImage.accessed = accessed;//2
        ReturnImage.created = created;//3
        ReturnImage.datefirstseen = firstseen;//4
        ReturnImage.filesize = query.value(3).toUInt();
        ReturnImage.height = query.value(5).toInt();
        ReturnImage.md5 = query.value(6).toString();
        ReturnImage.modified = modified;//9
        ReturnImage.path = query.value(9).toString();

        ReturnImage.thumbnail = query.value(8).toByteArray();
        ReturnImage.width = query.value(4).toUInt();
        ReturnImage.skin = query.value(13).toDouble();

        if (query.value(14).toString().contains(":")){
            ReturnImage.faces = DeserialiseFaces(query.value(14).toString());
        }


        //Get Das MEta DATAS!

        QSqlQuery GetMeta(m_db);
        GetMeta.prepare("select * from ExifData where ImageID = ? Limit 1");
        GetMeta.addBindValue(ReturnImage.id);
        GetMeta.exec();
        while (GetMeta.next()) {
            ReturnImage.meta.Descriptiom = GetMeta.value(2).toString();
            ReturnImage.meta.Make = GetMeta.value(3).toString();
            ReturnImage.meta.Model = GetMeta.value(4).toString();
            ReturnImage.meta.Software = GetMeta.value(5).toString();
            ReturnImage.meta.Filetime = GetMeta.value(6).toString();
            ReturnImage.meta.DateTime_Analogue = GetMeta.value(7).toString();
            ReturnImage.meta.DateTime_Digital = GetMeta.value(8).toString();
            ReturnImage.meta.Copyright = GetMeta.value(9).toString();
            ReturnImage.meta.Longitude = GetMeta.value(10).toString();
            ReturnImage.meta.Latitude = GetMeta.value(11).toString();
            ReturnImage.meta.Altitude = GetMeta.value(12).toString();
            ReturnImage.meta.GPSDOP = GetMeta.value(13).toString();
        }





        ReturnList.append(ReturnImage);
       }
    return ReturnList;

}
double GetNumImages(QString caseID){
    QSqlQuery query(m_db);
    query.prepare("select count(*) from ImageDetails where Case_ID = ?");
    query.addBindValue(caseID.toInt());
    query.exec();
    int count;
    while (query.next()) {
        count = query.value(0).toInt();
    }


    return count;
}
