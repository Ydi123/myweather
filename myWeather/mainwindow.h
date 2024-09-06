#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QContextMenuEvent>
#include<QtNetwork/QNetworkAccessManager>
#include<QtNetwork/QNetworkReply>//相应数据的头文件
#include"weatherdata.h"
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QLabel>
#include<QJsonParseError>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

protected:
    void contextMenuEvent(QContextMenuEvent *event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    //获取天气数据
    void getWeatherInfo(QString cityname);

    //解析JSON数据
    void parseJson(QByteArray& byteArray);

    //更新UI 数据
    void updataUI();

    //重写父类的eventfilter方法
    bool eventFilter(QObject* watched, QEvent* event);
    //绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();

    //去除多余内容,只保留int数据
    int parseString(QString str);

private slots:
    void on_btnSearch_clicked();

private:
    void onReplied(QNetworkReply* reply);//当获取网络请求的数据，信号自动带reply的参数
private:
    QMenu* mExitMenu;//右键退出菜单
    QAction* mExitAct;//退出的行为，菜单项

    QPoint mOffset;//窗口移动式，鼠标与窗口左上角的偏移

    QNetworkAccessManager* mNetAccessManger;

    //今天和6天的数据
    Today mToday;
    Day  mDay[6];

    //星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDataList;

    //天气和天气图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    //风力和风向
    QList<QLabel*> mFlList;
    QList<QLabel*> mFxList;

    //天气污染指数
    QList<QLabel*> mAqiList;

    //定义一个map，更新图标
    QMap<QString,QString> mTypeMap;

};
#endif // MAINWINDOW_H
