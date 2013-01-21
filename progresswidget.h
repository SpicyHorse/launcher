#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>

class QImage;

class ProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressWidget(QWidget *parent = 0);
    void setBackground(QString);
    void setForeground(QString);
    void setFrame(QString);
    void setBarSize(int, int, int, int);

protected:
    void paintEvent(QPaintEvent *);

private:
    int progress;
    int dx;
    int dy;
    int dw;
    int dh;
    QImage *background_image;
    QImage *foreground_image;
    QImage *frame_image;

signals:
    
public slots:
    void setValue(int);

};

#endif // PROGRESSWIDGET_H
