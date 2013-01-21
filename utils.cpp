#include "utils.h"
#include <QTextStream>

QString formatSize(int s) {
    static const char *arr[] = { "B", "KiB", "MiB", "GiB", "TiB"};

    int t = 0;
    while (s > 1024 && t < 5) {
        s = s >> 10;
        t++;
    }
    QString str;
    QTextStream stream(&str);
    stream << s << arr[t];
    return str;
}
