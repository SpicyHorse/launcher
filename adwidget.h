#ifndef ADWIDGET_H
#define ADWIDGET_H

#include <QWidget>
class QImage;

class AdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdWidget(QWidget *parent = 0);

    void setImage(QImage *);

protected:
    void paintEvent(QPaintEvent *);

private:
    QImage *background_image;

signals:
    
public slots:
    
};

#endif // ADWIDGET_H
