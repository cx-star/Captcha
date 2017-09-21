#ifndef WIDGET_H
#define WIDGET_H

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
    void slot_haveOneChar(const QStringList& list);

    void on_pushButtonSaveOneChar_clicked();

private:
    Ui::Widget *ui;
    QImage m_image;
    QImage getLevelImage(QImage sImg, int iMin, int iMax, int oMin=0, int outMax=255)const;
    QWidget* getPixelWidget(QStringList list)const;
    QWidget* getPixelWidget(QImage img)const;
    QStringList imgToPixelStringList(QImage img)const;
    QRect PixelList(QStringList list)const;
    QRect PixelList2(QStringList list)const;
    QStringList getStrLFromStrL(QStringList list,QRect rect);
    bool SHaveT(QString s, QChar t)const;

    QList<QScrollArea*> oneCharScrollAreaList;
    int haveOneCharIndex;
    QList<QLineEdit*> oneCharLineEditList;
    QList<QStringList> oneCharListList;

    QMap<QString,QStringList> oneCharDataMap;
};

#endif // WIDGET_H
