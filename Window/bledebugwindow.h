#ifndef BLEDEBUGWINDOW_H
#define BLEDEBUGWINDOW_H

#include <QWidget>
#include "mainwindow.h"

class bledebugwindow : public QWidget
{
    Q_OBJECT
public:
    explicit bledebugwindow(QWidget *parent = nullptr);
    static void debugtext_append(QString str);

public:
    QDockWidget *dockbledebug;
    static QTextEdit *bledebugText;
    QLineEdit *text_blelog_send;
    QPushButton *button_blelog_send, *button_clear;
    QToolButton *toolBtn = nullptr;
signals:

private slots:
    void blelog_send();
    void clear();
    void closeWindow();
};

#endif // BLEDEBUGWINDOW_H
