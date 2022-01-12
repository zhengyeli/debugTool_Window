#include "newqdockwidget.h"
#include "mainwindow.h"
#include <QDockWidget>                //浮动窗口


//想重写closeEvent，还没找到例子
NewQDockWidget::NewQDockWidget(QString title, QWidget* parent)
    : QDockWidget(title, parent)
{
}


void NewQDockWidget::closeEvent(QCloseEvent* event)
{
    //想重写closeEvent，还没找到例子

}
