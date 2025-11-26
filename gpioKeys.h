#ifndef GPIOKEYS_H
#define GPIOKEYS_H

#include <QObject>
#include <QSocketNotifier>

class GpioKeys : public QObject {
    Q_OBJECT
public:
    explicit GpioKeys(QObject *parent = nullptr);

signals:
    void keyUpPressed();
    void keyDownPressed();
    void keyDownReleased();
    void keyUpReleased();

private slots:
    void handleUp();
    void handleDown();

private:
    int fdUp;
    int fdDown;
    QSocketNotifier *notifierUp;
    QSocketNotifier *notifierDown;

    char lastUpValue;
    char lastDownValue;
};

#endif
