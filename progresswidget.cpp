#include "progresswidget.h"

#include <QPainter>

#include "platform.h"

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent), progress(0), dx(0), dy(0), dw(0), dh(0),
    background_image(new QImage()), foreground_image(new QImage()), frame_image(new QImage())
{
}

void ProgressWidget::setBarSize(int dx, int dy, int dw, int dh)
{
    this->dx = dx;
    this->dy = dy;
    this->dw = dw;
    this->dh = dh;
    repaint();
}

void ProgressWidget::setBackground(QString img)
{
    background_image = new QImage(getAsset(img));
    repaint();
}

void ProgressWidget::setForeground(QString img)
{
    foreground_image = new QImage(getAsset(img));
    repaint();
}

void ProgressWidget::setFrame(QString img)
{
    frame_image = new QImage(getAsset(img));
    repaint();
}

void ProgressWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(dx, dy, *background_image);
    painter.drawImage(dx, dy, foreground_image->copy(QRect(0, 0, dw*progress/10000, dh)));
    painter.drawImage(0, 0, *frame_image);

    painter.setPen(QColor(220,220,220));
    painter.setFont(QFont("sans serif", 14, QFont::Bold));
    painter.drawText(dx, dy, dw, dh, Qt::AlignVCenter| Qt::AlignHCenter, QString("%1 %").arg((float)progress/100));

}

void ProgressWidget::setValue(int p)
{
    progress = p;
    repaint();
}
