#include "platform.h"
#include <QDebug>

#if MAC_OS_X_VERSION_10_5
    #include <sys/resource.h>
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#elif WINNT
    #include <stdio.h>
#endif

void platformInitialize()
{
#if MAC_OS_X_VERSION_10_5
    struct rlimit rlp;
    getrlimit(RLIMIT_NOFILE, &rlp);
    qDebug() << "Platform initialization: open files limit" << rlp.rlim_cur << "rising to max" << min(OPEN_MAX, rlp.rlim_max);
    rlp.rlim_cur = min(OPEN_MAX, rlp.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rlp);
#elif WINNT
    qDebug() << "Platform initialization: open files limit" << _getmaxstdio() << "rising to max" << 2048;
    _setmaxstdio(2048);
#endif
}
