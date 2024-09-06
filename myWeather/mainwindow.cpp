#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include"weathertool.h"
#include<QString>

#include<QPainter>
#include<QPen>
#define INCREMENT 3 //温度每上升一度，y周的坐标的增量
#define POINT_RADIUS 3// 曲线锚点的大小
#define TEXT_OFFSET_X 12  //x轴的偏移量
#define TEXT_OFFSET_Y 12

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);//设置窗口无边框
    setFixedSize(width(),height());//设置固定窗口大小

    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction(this);
    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/image/close.png"));
    mExitMenu->addAction(mExitAct);
    connect(mExitAct, QAction::triggered, this, [=](){
        qApp->exit(0);
    });

    //将控件添加到控件数组
    //星期和日期
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDataList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;
    //天气和天气图标
    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;
    //天气指数
    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;
    //风向和风力
    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    mTypeMap.insert("暴雪",":/image/type/BaoXue.png");
    //mTypeMap.insert("暴雪",":/image/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/image/type/BaoYu. png");
    mTypeMap.insert("暴雨到大暴雨",":/image/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/image/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨",":/image/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/image/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪",":/image/type/DaXue.png");
    mTypeMap.insert("大雨",":/image/type/DaYu.png");
    mTypeMap.insert("冻雨",":/image/type/DongYu.png");
    mTypeMap.insert("多云",":/image/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/image/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/image/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/image/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/image/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/image/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/image/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/image/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/image/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined",":/image/type/undefined.png");
    mTypeMap.insert("雾",":/image/type/Wu.png");
    mTypeMap.insert("小到中雪",":/image/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨",":/image/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪",":/image/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/image/type/XiaoYu.png");
    mTypeMap.insert("雪",":/image/type/Xue.png");
    mTypeMap.insert("扬沙",":/image/type/YangSha.png");
    mTypeMap.insert("阴",":/image/type/Yin.png");
    mTypeMap.insert("雨",":/image/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/image/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/image/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/image/type/ZhenYu.png");
    mTypeMap.insert("中到大雪",":/image/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨",":/image/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/image/type/ZhongXue.png");
    mTypeMap.insert("中雨",":/image/type/ZhongYu.png");



    //4 网络请求
    mNetAccessManger = new QNetworkAccessManager(this);
    connect(mNetAccessManger, &QNetworkAccessManager::finished, this, &MainWindow::onReplied);

    //connect(ui->btnSearch, &QPushButton::clicked, this, &MainWindow::onBtnSerchClicked);
    //直接在构造中，请求天气预报
    //getWeatherInfo("101280101");//广州的城市编码
    getWeatherInfo("北京");//广州的城市编码

    //5 给标签添加事件过滤器
    //参数指定为this，也就是当前窗口对象mainwindow
    //如果检测到在标签有事件发生时，就会被当前窗口拦截，于是就会调用下面的eventFilter函数进行画曲线
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//重写父类的虚函数
//父类中默认的实现 是忽略右键菜单事件
//重写之后，就可以处理右键菜单
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());//传送鼠标的位置
    event->accept();//表示事件已经处理，不需向上传递了
}

//重写鼠标按下事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //获取窗口位置
    mOffset = event->globalPos() - this->pos();//鼠标距离电脑左上角的位置 减去 鼠标距离窗口左上角的位置
}

//重写鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

//调用getWeatherInfo来发送http请求
void MainWindow::getWeatherInfo(QString cityname)
{
    QString cityCode = WeatherTool::getCityCode(cityname);
    if(cityCode.isEmpty()){
        QMessageBox::warning(this,"天气","请检查输入是否正确!",QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/"+cityCode);
    mNetAccessManger->get(QNetworkRequest(url));
}

//解析json数据
void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);
    if(err.error != QJsonParseError::NoError){ // 如果出错，直接返回
        return;
    }

    QJsonObject rootObj = doc.object();
    qDebug() << rootObj.value("message").toString();

    //1，解析日期和城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    //2，解析昨天数据
    QJsonObject objData = rootObj.value("data").toObject();
    QJsonObject objYesterday = objData.value("yesterday").toObject();

    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();
    mDay[0].type = objYesterday.value("type").toString();
    //qDebug() << "今天周" <<mDay[0].week;

    //分解高低温的数据，用空格进行分割
//    QString s;
//    s = objYesterday.value("high").toString().split(" ").at(1); // 18º
//    qDebug() << "今天最高温度" <<s;
//    s.left(s.length()-2); // 取出 18
//    qDebug() << "今天最高温度" <<s;
//    mDay[0].high =s.toInt();
//    qDebug() << "今天最高温度" <<mDay[0].high;

//    s = objYesterday.value("low").toString().split(" ").at(1); // 6º
//    qDebug() << "今天最高温度" <<s;
//    s.left(s.length()-1); // 取出 6
//    mDay[0].low =s.toInt();
    //简化版
    mDay[0].low = parseString(objYesterday.value("low").toString());
    mDay[0].high = parseString(objYesterday.value("high").toString());//最高温度

    //风向和风力
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();

    //aqi
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    //3，解析forcast中5天的数据
    QJsonArray forecastArr = objData.value("forecast").toArray();
    for(int i=0; i<5; i++){
        QJsonObject objForecast = forecastArr[i].toObject();
        mDay[i+1].week = objForecast.value("week").toString();
        mDay[i+1].date = objForecast.value("ymd").toString();
        mDay[i+1].type = objForecast.value("type").toString();

        //分解高低温的数据，用空格进行分割
        mDay[i+1].low = parseString(objForecast.value("low").toString());
        mDay[i+1].high = parseString(objForecast.value("high").toString());//最高温度
//        qDebug() << "今天最低温度" <<mDay[i].low;
//        qDebug() << "今天最高温度" <<mDay[i].high;

        //风向和风力
        mDay[i+1].fx = objForecast.value("fx").toString();
        mDay[i+1].fl = objForecast.value("fl").toString();

        //aqi
        mDay[i+1].aqi = objForecast.value("aqi").toDouble();
    }

    //4，解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu  = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();

    //5,forcast中的第一个数组也是今天的数据
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;

    mToday.type = mDay[1].type;

    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //6,更新UI
    updataUI();

    //手动更新温度曲线'
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

//6,更新UI
void MainWindow::updataUI()
{
    //1，更新日期和城市
    //ui->lbDate->setText(mToday.date);
    ui->lbDate->setText(QDateTime::fromString(mToday.date, "yyyyMMdd").toString("yyyy/MM/dd")+" " + mDay[1].week);
    ui->lbCity->setText(mToday.city);

    // 2,更新今天
    ui->lbType->setText(mToday.type);
    ui->lbTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lbTemp->setText(QString::number(mToday.wendu)+"°C");// 转换成int类型
    //ui->lbType->setText(mToday.type);
    ui->lbLowHigh->setText(QString::number(mToday.low) + "~" + QString::number(mToday.high) + "°C");

    ui->lbGaoMao->setText("感冒指数：" + mToday.ganmao);
    ui->lbWindFl->setText(mToday.fl);
    ui->lbWindFx->setText(mToday.fx);
    ui->lbPm25->setText(QString::number(mToday.pm25));
    ui->lbShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    // 3,更新6天
    for(int i=0;i<6;i++){
        //1 更新日期和时间
        mWeekList[i]->setText("周"+ mDay[i].week.right(1));// JSON中显示星期几，所以这里只提出那个“几” = right（1）；
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");
        //更新日期
        QStringList ymdList = mDay[i].date.split("-");//按-进行分割
        mDataList[i]->setText(ymdList[1] + "/" + ymdList[2]);//只提取月和日

        //2 更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);

        //3 更新空气质量
        if(mDay[i].aqi >= 0 && mDay[i].aqi <= 50){
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121,184,0);");
        }else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100){
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,187,23);");
        }else if(mDay[i].aqi > 100 && mDay[i].aqi <= 150){
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,87,97);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200){
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235,17,27);");
        }else if(mDay[i].aqi > 200 && mDay[i].aqi <= 250){
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170,0,0);");
        }else {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110,0,0);");
        }

        // 3 更新风力风向
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);

    }

}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }

    return QWidget::eventFilter(watched, event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);
    //1 获取x坐标
    int pointX[6] = {0};
    for(int i=0;i < 6;i++){
        //上面的控件 的中间位置
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
}

    //2 获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0; i<6 ;i++){
        tempSum += mDay[i].high;
    }
    tempAverage = tempSum/6;//最高温的平均值
    //计算y的坐标
    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height()/2;
    for(int i=0; i<6; i++){
        pointY[i] = yCenter - ((mDay[i].high-tempAverage)*INCREMENT);
    }

    //3  绘制
    //初始化画笔
    QPen pen = painter.pen();
    pen.setWidth(1);//设置画笔宽度
    pen.setColor(QColor(255, 170, 0));//设置颜色
    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0));//设置画刷，内部进行填充
    //画点，写文本
    for(int i=0; i<6; i++){
        painter.drawEllipse(QPoint(pointX[i], pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X, pointY[i]-TEXT_OFFSET_Y,QString::number(mDay[i].high)+"°");
    }
    //画线
    for(int i=0; i<5; i++){
        if(i==0){
            pen.setStyle(Qt::DotLine);//虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);//实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i], pointX[i+1], pointY[i+1]);
    }

}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);
    //1 获取x坐标
    int pointX[6] = {0};
    for(int i=0;i < 6;i++){
        //上面的控件 的中间位置
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
}

    //2 获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0; i<6 ;i++){
        tempSum += mDay[i].low;
    }
    tempAverage = tempSum/6;//最低温的平均值
    //计算y的坐标
    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height()/2;
    for(int i=0; i<6; i++){
        pointY[i] = yCenter - ((mDay[i].low-tempAverage)*INCREMENT);
    }

    //3  绘制
    //初始化画笔
    QPen pen = painter.pen();
    pen.setWidth(1);//设置画笔宽度
    pen.setColor(QColor(0, 255, 255));//设置颜色
    painter.setPen(pen);
    painter.setBrush(QColor(0, 255, 255));//设置画刷，内部进行填充
    //画点，写文本
    for(int i=0; i<6; i++){
        painter.drawEllipse(QPoint(pointX[i], pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X, pointY[i]-TEXT_OFFSET_Y,QString::number(mDay[i].low)+"°");
    }
    //画线
    for(int i=0; i<5; i++){
        if(i==0){
            pen.setStyle(Qt::DotLine);//虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);//实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i], pointX[i+1], pointY[i+1]);
    }
}

//当http请求完毕，服务器返回数据时，QNetworkAccessManager就会发送finished信号，进而调用onReplied函数
void MainWindow::onReplied(QNetworkReply *reply)
{
    qDebug() << "onReplied success";
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//    qDebug() << "operation" << reply->operation();
//    qDebug() << "status code" << status_code;
//    qDebug() << "url" << reply->url();
//    qDebug() << "raw header" << reply->rawHeaderList();  //响应头

    if(reply->error() != QNetworkReply::NoError || status_code != 200){ // 如果请求出错，执行此程序
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this, "天气", "请求数据失败",QMessageBox::Ok);
    }else{
        QByteArray byteArray = reply->readAll();
        //qDebug() << "read all" << byteArray.data();//显示所有读到的数据
        parseJson(byteArray);
    }
    reply->deleteLater();
}

//点击按钮进行更新城市数据
void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}

int MainWindow::parseString(QString str)
{
    str = str.split(" ").at(1);//以空格为分割符,去除前面的文字,取出第二个X℃,
    str = str.left(str.length() - 1);//去除后面的摄氏度符号
    return str.toInt();
}
