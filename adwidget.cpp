#include "adwidget.h"

#include <QPainter>
#include <QImage>
#include <QDebug>

#include "platform.h"

AdWidget::AdWidget(QWidget *parent) :
    QWidget(parent), back_image(new QImage(":/images/spicy.png")), frame_image(new QImage()),
    ads_images(), ads_index(0), ads_fx(0), ads_fy(0)
{
    startTimer(5000);
}

void AdWidget::setAdvertisment(QStringList img_list)
{
    qDebug() << "AdWidget::setAdvertisment" << img_list;

    for (int i = 0; i < img_list.size(); ++i) {
        QImage *img = new QImage();
        if (!img->load(getAsset(img_list.at(i)))) {
            qCritical() << "AdWidget::setAdvertisment error loading image" << i;
        }

        ads_images.append(img);
    }

    timerEvent(0);
    repaint();
}
void AdWidget::setFrame(QString img)
{
    frame_image = new QImage(getAsset(img));
    repaint();
}

void AdWidget::timerEvent(QTimerEvent *) {
    if (ads_images.length() == 0)
        return;

    back_image = ads_images[ads_index];

    ads_index++;
    if (ads_index >= ads_images.length())
        ads_index = 0;

    repaint();
}

void AdWidget::setAdvertismentOffset(int x, int y)
{
   ads_fx = x;
   ads_fy = y;
}

void AdWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(ads_fx, ads_fy, *back_image);
    painter.drawImage(0, 0, *frame_image);
}
