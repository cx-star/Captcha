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
    ui->spinBoxInMax->setValue(255);
    ui->spinBoxInMin->setValue(0);
    ui->spinBoxOutMax->setValue(255);
    ui->spinBoxOutMin->setValue(0);
    connect(this,SIGNAL(signal_imageChanged(QImage)),this,SLOT(slot_imageChanged(QImage)));

    haveOneCharIndex = 0;
    oneCharScrollAreaList.append(ui->scrollAreaOneChar1);
    oneCharScrollAreaList.append(ui->scrollAreaOneChar2);
    oneCharScrollAreaList.append(ui->scrollAreaOneChar3);
    oneCharScrollAreaList.append(ui->scrollAreaOneChar4);

    oneCharLineEditList.append(ui->lineEditOneChar1);
    oneCharLineEditList.append(ui->lineEditOneChar2);
    oneCharLineEditList.append(ui->lineEditOneChar3);
    oneCharLineEditList.append(ui->lineEditOneChar4);

    oneCharListList.append(QStringList());
    oneCharListList.append(QStringList());
    oneCharListList.append(QStringList());
    oneCharListList.append(QStringList());
    QFile file;
    file.setFileName(QCoreApplication::applicationDirPath()+"/oneChar.dat");
    file.open(QIODevice::ReadOnly);
    if(file.isOpen()){
        QDataStream in(&file);
        in>>oneCharDataMap;
        file.close();
    }

    ui->labelImage->setScaledContents(true);
    ui->labelBitMap->setScaledContents(true);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButtonOPen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开图片",QCoreApplication::applicationDirPath(),"*");
    if(m_image.load(fileName)){
        qDebug()<<"on_pushButtonOPen_clicked m_image.format:"<<m_image.format();//QImage::Format_RGB32     4    The image is stored using a 32-bit RGB format (0xffRRGGBB).
        emit signal_imageChanged(m_image);
        slot_spinBoxChanged();
        QStringList t_list = imgToPixelStringList(m_image);
        ui->scrollArea->setWidget(getPixelWidget(t_list));

        while(1){
            QRect rect = PixelList2(t_list);
            qDebug()<<rect<<" "<<rect.topLeft()<<" "<<rect.topRight()<<" "<<rect.bottomLeft()<<" "<<rect.bottomRight();
            if(rect.isValid()){
                QStringList oneCharList = getStrLFromStrL(t_list,rect);
                qDebug()<<oneCharList;
                slot_haveOneChar(oneCharList);
                for(int i=rect.x();i<=rect.x()+rect.width();i++){
                    t_list[i].fill('0');
                }
            }else
                break;
        }
    }
}

void Widget::slot_imageChanged(const QImage &img)
{
    ui->labelImage->setPixmap(QPixmap::fromImage(img));
    QBitmap bm = QBitmap::fromImage(img);
    ui->labelBitMap->setPixmap(bm);
    qDebug()<<"bm.depth:"<<bm.depth();
    QImage bitImg = bm.toImage();
    qDebug()<<"depth "<<bitImg.depth()<<" size"<<bitImg.size()<<" h "<<bitImg.height()<<" w "<<bitImg.width();
    qDebug()<<"byteCount:"<<bitImg.byteCount();
    for(int i=0;i<bitImg.height();i++){
        QString s;
        for(int j=0;j<bitImg.width();j++){
           //qDebug()<<bitImg.pixelColor(j,i);
            //s.append(QString("%1").arg(bitImg.pixelColor(j,i).red()==255?1:0));
        }
        //qDebug()<<s;
    }
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
    QImage img_t = getLevelImage(m_image,ui->spinBoxInMin->value(),ui->spinBoxInMax->value(),ui->spinBoxOutMin->value(),ui->spinBoxOutMax->value());

    emit signal_imageChanged(img_t);
}

void Widget::on_pushButtonToBit_clicked()
{

}

void Widget::slot_haveOneChar(const QStringList &list)
{
    if(haveOneCharIndex==4)
        haveOneCharIndex = 0;
    oneCharScrollAreaList.at(haveOneCharIndex)->setWidget(getPixelWidget(list));
    oneCharListList[haveOneCharIndex] = list;
    haveOneCharIndex++;
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
            //label->sizePolicy().setHeightForWidth(true);
            label->sizePolicy().setWidthForHeight(true);
            label->setText(QString("%1-%2").arg(w).arg(h));
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
    QImage t_img = getLevelImage(img,130,130);
    t_img = t_img.convertToFormat(QImage::Format_Mono);
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
            }
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

void Widget::on_pushButtonSaveOneChar_clicked()
{
    for(int i=0;i<oneCharLineEditList.size();i++)
    {
        QLineEdit* le = oneCharLineEditList.at(i);
        if(le->text().size()==1){
            oneCharDataMap[le->text()] = oneCharListList.at(i);
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
    qDebug()<<oneCharDataMap.size()<<oneCharDataMap.keys();
}
