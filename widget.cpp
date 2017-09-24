#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QBitmap>
#include <QStringList>
#include <QRect>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->spinBoxInMax->setMinimum(0);
    ui->spinBoxInMin->setMinimum(0);
    ui->spinBoxOutMax->setMinimum(0);
    ui->spinBoxOutMin->setMinimum(0);
    ui->spinBoxInMax->setMaximum(255);
    ui->spinBoxInMin->setMaximum(255);
    ui->spinBoxOutMax->setMaximum(255);
    ui->spinBoxOutMin->setMaximum(255);
    ui->spinBoxInMax->setValue(130);
    ui->spinBoxInMin->setValue(130);
    ui->spinBoxOutMax->setValue(255);
    ui->spinBoxOutMin->setValue(0);
    connect(this,SIGNAL(signal_imageChanged(QImage)),this,SLOT(slot_imageChanged(QImage)));

    fourCharScrollAreaList.append(ui->scrollAreaOneChar1);
    fourCharScrollAreaList.append(ui->scrollAreaOneChar2);
    fourCharScrollAreaList.append(ui->scrollAreaOneChar3);
    fourCharScrollAreaList.append(ui->scrollAreaOneChar4);

    fourCharLineEditList.append(ui->lineEditOneChar1);
    fourCharLineEditList.append(ui->lineEditOneChar2);
    fourCharLineEditList.append(ui->lineEditOneChar3);
    fourCharLineEditList.append(ui->lineEditOneChar4);

    ui->labelImage->setScaledContents(true);//图片缩放
    ui->labelBitMap->setScaledContents(true);

    net_manager = new QNetworkAccessManager(this);//跟网页不是同一个session
    connect(net_manager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(replyFinished(QNetworkReply*)));

    connect(ui->comboBoxData1,SIGNAL(currentIndexChanged(QString)),
            this,SLOT(slot_updateScrollAreaData1(QString)));
    connect(ui->comboBoxData2,SIGNAL(currentIndexChanged(QString)),
            this,SLOT(slot_updateScrollAreaData2(QString)));


    QFile file;
    file.setFileName(QCoreApplication::applicationDirPath()+"/oneChar.dat");
    file.open(QIODevice::ReadOnly);
    if(file.isOpen()){
        QDataStream in(&file);
        in>>oneCharDataMap;
        file.close();
    }
    slot_updateMapData();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButtonOPen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开图片",QCoreApplication::applicationDirPath(),"*");
    if(m_image.load(fileName)){
        slot_imageChanged(m_image);
    }
}

void Widget::slot_imageChanged(const QImage &img)
{
    qDebug()<<"slot_imageChanged m_image.format:"<<img.format();//QImage::Format_RGB32     4    The image is stored using a 32-bit RGB format (0xffRRGGBB).
    if(img.format()==0)
        return;
    m_imageBit = getLevelImage2(img,ui->spinBoxRGB->value(),ui->spinBoxR->value(),ui->spinBoxG->value(),ui->spinBoxB->value());
    ui->labelImage->setPixmap(QPixmap::fromImage(img));
    ui->labelBitMap->setPixmap(QBitmap::fromImage(m_imageBit));

    QStringList t_list = imgToPixelStringList(m_imageBit);//将打开图片转换成字符串列表
    ui->scrollArea->setWidget(getPixelWidget(t_list));//getPixelWidget将字符串列表转换成带有lebal阵列的widget

    fourCharListList.clear();
    while(1){
        QRect rect = PixelList2(t_list);//一个个的获取t_list中单独字符区域
        //qDebug()<<rect<<" "<<rect.topLeft()<<" "<<rect.topRight()<<" "<<rect.bottomLeft()<<" "<<rect.bottomRight();
        if(rect.isValid()){
            QStringList oneCharList = getStrLFromStrL(t_list,rect);//将指定区域转换成字符串列表，即一个字母的图形字符串列表
            fourCharListList.append(oneCharList);
            for(int i=rect.x();i<rect.x()+rect.width();i++){//将已获取区域清零，以显示下一个
                if(i<t_list.size())
                    t_list[i].fill('0');
            }
        }else
            break;
    }
    qDebug()<<"fourCharListList"<<fourCharListList.size();
    QString rt = autoOCR(fourCharListList);
    qDebug()<<"OCR:"<<rt;
    if(rt.size() == fourCharListList.size())
        slot_showFourChar(fourCharListList,rt);//依次显示
}

void Widget::on_spinBoxInMin_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::on_spinBoxInMax_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::on_spinBoxOutMin_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::on_spinBoxOutMax_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::slot_spinBoxChanged()
{
    emit signal_imageChanged(m_image);
}

void Widget::on_pushButtonToBit_clicked()
{

}

void Widget::slot_showFourChar(const QList<QStringList> &ll,const QString& str)
{
    int i=0;
    foreach (QStringList list, ll) {
        if(i<4){
            fourCharScrollAreaList.at(i)->setWidget(getPixelWidget(list));
            fourCharLineEditList.at(i)->setText(str.at(i));
            i++;
        }
    }
}

QImage Widget::getLevelImage(QImage sImg,int inMin, int inMax, int outMin, int outMax) const
{
    int imgH = sImg.height();
    int imgW = sImg.width();

    QImage img_t(imgW,imgH,QImage::Format_RGB888);

    for(int w=0;w<imgW;w++){
        for(int h=0;h<imgH;h++){
            QColor c = sImg.pixelColor(w,h);

            if( c.red() <= inMin ){
                c.setRed(outMin);
            }else if( c.red() >= inMax ){
                c.setRed(outMax);
            }else{
                c.setRed( outMin + (double)(c.red()-inMin)/(double)(inMax-inMin)*(double)(outMax-outMin) );
            }

            if( c.green() <= inMin ){
                c.setGreen(outMin);
            }else if( c.green() >= inMax ){
                c.setGreen(outMax);
            }else{
                c.setGreen( outMin + (double)(c.green()-inMin)/(double)(inMax-inMin)*(double)(outMax-outMin) );
            }

            if( c.blue() <= inMin ){
                c.setBlue(outMin);
            }else if( c.blue() >= inMax ){
                c.setBlue(outMax);
            }else{
                c.setBlue( outMin + (double)(c.blue()-inMin)/(double)(inMax-inMin)*(double)(outMax-outMin) );
            }

            img_t.setPixel(w,h,c.rgb());
        }
    }
    return img_t;
}

QImage Widget::getLevelImage2(QImage sImg, int RGB, int R, int G, int B) const
{
    int imgH = sImg.height();
    int imgW = sImg.width();

    QImage img_t(imgW,imgH,QImage::Format_RGB888);

    for(int w=0;w<imgW;w++){
        for(int h=0;h<imgH;h++){
            QColor c = sImg.pixelColor(w,h);
            if((c.red()+c.green()+c.blue())<RGB && (c.red()<R || c.green()<G || c.blue()<B) ){
                img_t.setPixel(w,h,QColor(0,0,0).rgb());
            }else
                img_t.setPixel(w,h,QColor(255,255,255).rgb());
        }
    }
    return img_t;
}

QWidget *Widget::getPixelWidget(QStringList list) const
{
    QWidget *widget = new QWidget();
    if(list.size()==0)
        return widget;
    int widthSize = list.size();
    QString hLine = list.at(0);
    int heightSize = hLine.size();
    QGridLayout *gLayout = new QGridLayout;
    gLayout->setSpacing(1);
    widget->setLayout(gLayout);

    for(int w=0;w<widthSize;w++){
        for(int h=0;h<heightSize;h++){
            QLabel *label = new QLabel(widget);
            label->setMinimumSize(10,10);
            label->setText(QString("%1-%2").arg(w).arg(h));
            //label->sizePolicy().setWidthForHeight(true);
            label->sizePolicy().setHeightForWidth(true);//height depends on its width
            label->setSizeIncrement(1,1);
            hLine = list.at(w);
            if(hLine.at(h)=="0"){
                //qDebug()<<"h"<<h<<"w"<<w<<hLine.at(h);
                label->setStyleSheet("background-color: rgb(200, 200, 200);font: 7pt;");
            }else{
                label->setStyleSheet("background-color: rgb(255, 0, 0);font: 7pt;");
            }
            gLayout->addWidget(label,h,w);
        }
    }

    return widget;
}

QStringList Widget::imgToPixelStringList(QImage img) const
{
    QImage t_img = img.convertToFormat(QImage::Format_Mono);
    QStringList list;
    for(int w=0;w<t_img.width();w++){
        QString s;
        for(int h=0;h<t_img.height();h++){
            s.append(t_img.pixelColor(w,h).red()==255?"0":"1");//1为有，0为无
        }
        list.append(s);
    }
    //qDebug()<<"setPixelWidget"<<list;
    return list;
}

QWidget *Widget::getPixelWidget(QImage img) const
{
    return getPixelWidget(imgToPixelStringList(img));
}

QRect Widget::PixelList(QStringList list) const
{
    int max = list.size()-1;
    int widStart(0),widEnd(0),heightStart(max),heightEnd(0);
    bool findS(false);
    for(int i=0;i<list.size();i++){
        if(!findS && SHaveT(list.at(i),'1')){
            widStart = i;
            findS = true;
        }
        if(findS && !SHaveT(list.at(i),'1')){
            widEnd = i-1;
            break;
        }
    }
    if(widStart<widEnd){
        for(int j=widStart;j<=widEnd;j++){
            QString line = list.at(j);
            QRegExp exp("0*");
            int t;
            if(exp.indexIn(list.at(j))!=-1){
                t = exp.matchedLength();
                if(heightStart > t){
                    heightStart = t;
                }
            }else{
                heightStart = 0;
            }

            int tt = line.indexOf(QRegExp("0*$"));
            if(tt==-1){
                t = line.size()-1;
            }else{
                t = tt;
            }
            if((t-1)>heightEnd)
                heightEnd = t-1;
        }
    }

    qDebug()<<QString("%1-%2 %3-%4").arg(widStart).arg(heightStart).arg(widEnd).arg(heightEnd);
    if(widEnd==0){
        return QRect();
    }else{
        return QRect(widStart,heightStart,widEnd-widStart+1,heightEnd-heightStart+1);
    }
}

QRect Widget::PixelList2(QStringList list) const
{
    int max = list.size()-1;
    int widStart(0),widEnd(0),heightStart(max),heightEnd(0);
    bool findS(false);
    for(int i=0;i<list.size();i++){
        QString hLine = list.at(i);
        if(!findS){
            findS = hLine.contains('1');
            widStart = i;
        }
        if(findS){
            if(!hLine.contains('1')){
                widEnd = i-1;
                break;
            }else
                widEnd = i;
            heightStart = qMin(heightStart,hLine.indexOf('1'));
            heightEnd = qMax(heightEnd,hLine.lastIndexOf('1'));
        }
    }

    return QRect(widStart,heightStart,widEnd-widStart+1,heightEnd-heightStart+1);
}

QStringList Widget::getStrLFromStrL(QStringList list, QRect rect)
{
    int startW = rect.x();
    int endW = startW+rect.width();
    int startH = rect.y();

    QStringList rList;
    for(int i=startW;i<endW;i++){
        QString lineH = list.at(i);
        rList.append(lineH.mid(startH,rect.height()));
    }
    return rList;
}

bool Widget::SHaveT(QString s, QChar t) const
{
    for(int i=0;i<s.size();i++){
        if(s.at(i)==t)
            return true;
    }
    return false;
}

QString Widget::autoOCR(const QList<QStringList> &ll)
{
    QString rt;
    foreach (QStringList lr, ll) {
        QMapIterator<QString,QStringList> i(oneCharDataMap);
        QString key;
        while(i.hasNext()){
            i.next();
            if(lr==i.value()){
                key = i.key();
                break;
            }
        }
        if(!key.isEmpty()){
            rt.append(key.mid(0,key.indexOf('-')));
        }else
            rt.append('*');
    }
    return rt;
}


void Widget::on_pushButtonSaveOneChar_clicked()
{
    for(int i=0;i<fourCharListList.size();i++)
    {
        QLineEdit* le = fourCharLineEditList.at(i);
        QString leText = le->text();
        if(leText.size()==1 && leText!="*"){
            int index = 0;
            QString key;
            bool doNext(true);
            do{
                key = QString("%1-%2").arg(leText).arg(index);
                index++;
                if(oneCharDataMap.contains(key)){
                    if(oneCharDataMap.value(key)==fourCharListList.at(i)){//在map中已经存在这个value
                        doNext = false;
                        qDebug()<<QString("在%1中已经存在此值").arg(key);
                    }
                }else{//新key
                    oneCharDataMap[key]=fourCharListList.at(i);
                    doNext = false;
                    qDebug()<<QString("新增%1").arg(key);
                }
            }while(doNext);
        }
    }
    QFile file;
    file.setFileName(QCoreApplication::applicationDirPath()+"/oneChar.dat");
    file.open(QIODevice::WriteOnly);
    if(file.isOpen()){
        QDataStream in(&file);
        in<<oneCharDataMap;
        file.close();
    }
    slot_updateMapData();
    qDebug()<<oneCharDataMap.size()<<oneCharDataMap.keys();
}

void Widget::replyFinished(QNetworkReply *rep)
{
    QByteArray byte = rep->readAll();
    if(byte.size()>0){
        m_image = QImage::fromData(byte);
        emit signal_imageChanged(m_image);
    }
}

void Widget::on_pushButtonGetImageFromNet_clicked()
{
    net_manager->get(QNetworkRequest(QUrl("http://xf.faxuan.net/service/gc.html?tt=1506141472000")));
}

void Widget::on_pushButtonSaveImg_clicked()
{
    if(!m_image.isNull())
        m_image.save(QCoreApplication::applicationDirPath()+"/captcha.jpg");
}

void Widget::on_spinBoxR_valueChanged(int arg1)
{
Q_UNUSED(arg1); slot_spinBoxChanged();
}

void Widget::on_spinBoxRGB_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::on_spinBoxG_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::on_spinBoxB_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    slot_spinBoxChanged();
}

void Widget::slot_updateMapData()
{
    QStringList list = oneCharDataMap.keys();
    QStringListModel *model = new QStringListModel(list);
    ui->comboBoxData1->setModel(model);
    ui->comboBoxData2->setModel(model);
}

void Widget::slot_updateScrollAreaData1(const QString& s)
{
    ui->scrollAreaData1->setWidget(getPixelWidget(oneCharDataMap.value(s)));
}

void Widget::slot_updateScrollAreaData2(const QString& s)
{
    ui->scrollAreaData2->setWidget(getPixelWidget(oneCharDataMap.value(s)));
}

void Widget::on_pushButtonDel1_clicked()
{
    QString delKey = ui->comboBoxData1->currentText();
    oneCharDataMap.remove(delKey);
    slot_updateMapData();
}

void Widget::on_pushButtonDel2_clicked()
{
    QString delKey = ui->comboBoxData2->currentText();
    oneCharDataMap.remove(delKey);
    slot_updateMapData();
}
