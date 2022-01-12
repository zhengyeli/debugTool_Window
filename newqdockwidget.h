#ifndef NEWQDOCKWIDGET_H
#define NEWQDOCKWIDGET_H

#include <QDockWidget>                //浮动窗口

class NewQDockWidget : public QDockWidget
{
    Q_OBJECT
    //Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit NewQDockWidget(QString title, QWidget* parent);

protected:
     void closeEvent(QCloseEvent* event);
signals:
    void errorChanged();
    void infoChanged();
};

#endif // NEWQDOCKWIDGET_H
