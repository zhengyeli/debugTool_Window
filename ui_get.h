#ifndef UI_GET_H
#define UI_GET_H

#include <QObject>
#include "mainwindow.h"
#include "ui_mainwindow.h"

class MainWindow;

class ui_get : public QObject
{
    Q_OBJECT
public:
    explicit ui_get(Ui::MainWindow *p);
    Ui::MainWindow * exui;

signals:

};

#endif // UI_GET_H
