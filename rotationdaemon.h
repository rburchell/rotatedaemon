#ifndef ROTATIONDAEMON_H
#define ROTATIONDAEMON_H

#include <QApplication>
#include <QFile>
#include <QPoint>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

class RotationDaemon : public QApplication
{
    Q_OBJECT
public:
    explicit RotationDaemon(int argc, char **argv);
private slots:
    void timerEvent();
private:
    QPoint getOrientationInformation();
    void rotate(int angle);

    Window activeWindowId();
    bool checkApplicationBlacklistToForceLandscape();

    QFile m_accelerometer;
    Atom m_NET_ACTIVE_WINDOW;
};

#endif // ROTATIONDAEMON_H
