#include <QApplication>
#include <QX11Info>
#include <QDebug>
#include <QTimer>
#include <QFile>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#include "rotationdaemon.h"

RotationDaemon::RotationDaemon(int argc, char **argv) :
    QApplication(argc, argv),
    m_accelerometer(QLatin1String("/sys/class/i2c-adapter/i2c-3/3-001d/coord"))
{
    if (!m_accelerometer.open(QFile::ReadOnly)) {
        qFatal("Cannot open accelerometer FD");
    }

    m_NET_ACTIVE_WINDOW = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
    if (m_NET_ACTIVE_WINDOW == None) {
        qFatal("_NET_ACTIVE_WINDOW is apparantly None?");
    }

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), SLOT(timerEvent()));
    t->start(1000);
}

void RotationDaemon::rotate(int angle)
{
    // XXX: technically this is leaked
    static XRRScreenConfiguration *config = XRRGetScreenInfo(QX11Info::display(), DefaultRootWindow(QX11Info::display()));

    ushort rotation;
    ushort size;
    size = XRRConfigCurrentConfiguration(config, &rotation);

    short rate;
    rate = XRRConfigCurrentRate(config);

    unsigned long timestamp, current_time;
    timestamp = XRRTimes(QX11Info::display(), XDefaultScreen(QX11Info::display()), &current_time);

    qDebug() << "RotationDaemon::rotate(" << angle << ")";
    XRRSetScreenConfigAndRate(QX11Info::display(), config, DefaultRootWindow(QX11Info::display()), size, angle, rate, timestamp);
}

void RotationDaemon::timerEvent()
{
    QPoint p = getOrientationInformation();

    if (checkApplicationBlacklistToForceLandscape()) {
        // if we're not already landscape, make us so.
        return;
    }

    if (p.y() < -500) {
        rotate(1);
    } else if (p.y() > 500) {
        rotate(4);
    } else if (p.x() < -500) {
        rotate(2);
    } else if (p.x() > 500) {
        rotate(8);
    }
}

QPoint RotationDaemon::getOrientationInformation()
{
    QPoint p;

    // rewind
    m_accelerometer.seek(0);

    // populate
    p.setX(m_accelerometer.readLine().toInt());
    p.setY(m_accelerometer.readLine().toInt());

    return p;
}

Window RotationDaemon::activeWindowId()
{
    Window active_win = None;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_return = NULL;

    if(Success == XGetWindowProperty(QX11Info::display(), DefaultRootWindow(QX11Info::display()), m_NET_ACTIVE_WINDOW, 0L, sizeof(Window),
                                     False, XA_WINDOW, &actual_type,
                                     &actual_format, &nitems, &bytes_after,
                                     &prop_return) && prop_return)
    {
        active_win = *(Window *)prop_return;
        XFree(prop_return);
    }

    return active_win;
}


bool RotationDaemon::checkApplicationBlacklistToForceLandscape()
{
    // Find active window
    Window activeWindow = activeWindowId();

    // Now find what it is called
    XClassHint *xh = XAllocClassHint();
    Status s = XGetClassHint(QX11Info::display(), activeWindow, xh);

    qDebug() << "Active window is " << xh->res_name << xh->res_class;

    // clean up
    XFree(xh->res_name);
    XFree(xh->res_class);
    XFree(xh);

    return false;
}
