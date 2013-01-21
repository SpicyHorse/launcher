#ifndef ADWIDGET_H
#define ADWIDGET_H

#include <QWidget>
#include <QList>

class QImage;

class AdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdWidget(QWidget *parent = 0);

    void setAdvertisment(QStringList);
    void setFrame(QString);
    void setAdvertismentOffset(int, int);

protected:
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent *);

private:
    QImage *back_image;
    QImage *frame_image;
    QList<QImage *> ads_images;
    int ads_index;
    int ads_fx;
    int ads_fy;

signals:
    
public slots:
    
};

#endif // ADWIDGET_H
