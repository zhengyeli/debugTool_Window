#ifndef APPINIT_H
#define APPINIT_H

#include <QObject>
#include <QShowEvent>

enum e_mouse_position{
    mouse_up = 12,
    mouse_left_up = 11,
    mouse_right_up = 13,

    mouse_left = 21,
    mouse_middle = 22,
    mouse_right = 23,

    mouse_down = 32,
    mouse_left_down = 31,
    mouse_right_down = 33,
};


class AppInit : public QObject
{
    Q_OBJECT
public:
    static AppInit *Instance();
    explicit AppInit(QObject *parent = 0);

    int countFlag(QPoint p, QSize window);
    void setCursorType(QWidget *w, int t);
    int MARGIN;

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void showEvent(QShowEvent* event);

private:
    static QScopedPointer<AppInit> self;

public slots:
    void start();
};

#endif // APPINIT_H
