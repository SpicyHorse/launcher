#include "adwidget.h"

#include <QPainter>
#include <QImage>

AdWidget::AdWidget(QWidget *parent) :
    QWidget(parent), background_image(0)
{
    background_image = new QImage();
}

void AdWidget::setImage(QImage *i)
{
    background_image = i;
    repaint();
}

void AdWidget::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    painter.drawImage(0,0, *background_image);
}
