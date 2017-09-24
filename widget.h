#ifndef WIDGET_H
#define WIDGET_H

#include <QtNetwork>
#include <QGraphicsView>
#include <QLineEdit>
#include <QScrollArea>
#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void signal_imageChanged(const QImage& img);
private slots:
    void on_pushButtonOPen_clicked();
    void slot_imageChanged(const QImage& img);

    void on_spinBoxInMin_valueChanged(int arg1);
    void on_spinBoxInMax_valueChanged(int arg1);
    void on_spinBoxOutMin_valueChanged(int arg1);
    void on_spinBoxOutMax_valueChanged(int arg1);
    void slot_spinBoxChanged();
    void on_pushButtonToBit_clicked();
    void slot_showFourChar(const QList<QStringList> &ll, const QString &str);

    void on_pushButtonSaveOneChar_clicked();

    void replyFinished(QNetworkReply* rep);
    void on_pushButtonGetImageFromNet_clicked();

    void on_pushButtonSaveImg_clicked();

    void on_spinBoxR_valueChanged(int arg1);

    void on_spinBoxRGB_valueChanged(int arg1);

    void on_spinBoxG_valueChanged(int arg1);

    void on_spinBoxB_valueChanged(int arg1);
    void slot_updateMapData();
    void slot_updateScrollAreaData1(const QString& s);
    void slot_updateScrollAreaData2(const QString& s);

    void on_pushButtonDel1_clicked();

    void on_pushButtonDel2_clicked();

private:
    Ui::Widget *ui;
    QImage m_image,m_imageBit;
    QImage getLevelImage(QImage sImg, int iMin, int iMax, int oMin=0, int outMax=255)const;
    QImage getLevelImage2(QImage sImg, int RGB, int R, int G=0, int B=255)const;
    QWidget* getPixelWidget(QStringList list)const;
    QWidget* getPixelWidget(QImage img)const;
    QStringList imgToPixelStringList(QImage img)const;
    QRect PixelList(QStringList list)const;
    QRect PixelList2(QStringList list)const;
    QStringList getStrLFromStrL(QStringList list,QRect rect);
    bool SHaveT(QString s, QChar t)const;

    QList<QScrollArea*> fourCharScrollAreaList;
    QList<QLineEdit*> fourCharLineEditList;
    QList<QStringList> fourCharListList;

    QMap<QString,QStringList> oneCharDataMap;

    QNetworkAccessManager *net_manager;

    QString autoOCR(const QList<QStringList>& ll);
};

#endif // WIDGET_H
