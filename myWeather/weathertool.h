#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H

#include <QMainWindow>
#include<QMap>
#include<QFile>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonParseError>
#include<QJsonValue>
#include<QDebug>


//实现有城市的名称获取城市的编码
class WeatherTool{
private:
    //inline static QMap<QString,QString> mCityMap = {};
    static QMap<QString,QString> mCityMap;
    static void initCityMap(){
        //1 读取文件
        QString filePath = "D:/1_YD/project/c/project/Weather/MyWeather/myWeather/citycode.json";
        QFile file(filePath);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        qDebug() << file.isOpen();
        QByteArray json = file.readAll();
        file.close();

        //2 解析，并写入到map
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(json, &err);
        if(err.error != QJsonParseError::NoError){//表示出错了
            qDebug() <<"写入map错误";
            return;
        }
        if(!doc.isArray()){
            return;
        }

        QJsonArray cities = doc.array();
        for(int i=0;i<cities.size();i++){
            QString city = cities[i].toObject().value("city_name").toString();
            QString code = cities[i].toObject().value("city_code").toString();

            if(code.size()>0){                  //如果是省份的话就不添加code了
                mCityMap.insert(city,code);
            }

        }
    }
public:
    static QString getCityCode(QString cityName){
        if(mCityMap.isEmpty()){
            initCityMap();
        }
        QMap<QString, QString>::iterator it = mCityMap.find(cityName);
        //北京 、北京市
        if(it == mCityMap.end()){
            it = mCityMap.find(cityName + "市");
        }
        if(it != mCityMap.end()){
            return it.value();
        }
        return "";
    }
};

QMap<QString,QString> WeatherTool::mCityMap = {};

#endif // WEATHERTOOL_H
