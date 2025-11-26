#include "gpioKeys.h"
#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

GpioKeys::GpioKeys(QObject *parent)
    : QObject(parent),
      lastUpValue('0'),
      lastDownValue('0')
{
    // ---------- UP : GPIO26 ----------
    fdUp = open("/sys/class/gpio/gpio26/value", O_RDONLY);
    if (fdUp < 0) {
        qDebug() << "Failed to open gpio26:" << strerror(errno);
    } else {
        notifierUp = new QSocketNotifier(fdUp, QSocketNotifier::Read, this);
        connect(notifierUp, &QSocketNotifier::activated,
                this, &GpioKeys::handleUp);
    }

    // ---------- DOWN : GPIO46 ----------
    fdDown = open("/sys/class/gpio/gpio46/value", O_RDONLY);
    if (fdDown < 0) {
        qDebug() << "Failed to open gpio46:" << strerror(errno);
    } else {
        notifierDown = new QSocketNotifier(fdDown, QSocketNotifier::Read, this);
        connect(notifierDown, &QSocketNotifier::activated,
                this, &GpioKeys::handleDown);
    }
}

void GpioKeys::handleUp() {
    char buf = '0';
    lseek(fdUp, 0, SEEK_SET);
    if (read(fdUp, &buf, 1) <= 0) return;

    // 0 → 1 = 按下
    if (lastUpValue == '0' && buf == '1') {
        emit keyUpPressed();
    }

    // 1 → 0 = 松开
    if (lastUpValue == '1' && buf == '0') {
        emit keyUpReleased();
    }

    lastUpValue = buf;
}

void GpioKeys::handleDown() {
    char buf = '0';
    lseek(fdDown, 0, SEEK_SET);
    if (read(fdDown, &buf, 1) <= 0) return;

    // 0 → 1 = 按下
    if (lastDownValue == '0' && buf == '1') {
        emit keyDownPressed();
    }

    // 1 → 0 = 松开
    if (lastDownValue == '1' && buf == '0') {
        emit keyDownReleased();
    }

    lastDownValue = buf;
}
