#include "appinit.h"
#include "qmutex.h"
#include "qapplication.h"
#include "qevent.h"
#include "qwidget.h"

QScopedPointer<AppInit> AppInit::self;
AppInit *AppInit::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new AppInit);
        }
    }

    return self.data();
}

AppInit::AppInit(QObject *parent) : QObject(parent)
{

    MARGIN = 1;
}

void AppInit::showEvent(QShowEvent* event)
{
    //this->setAttribute(Qt::WA_Mapped);//解决不能及时刷新的bug
    //QMainWindow::showEvent(event);
}

void AppInit::setCursorType(QWidget *w,int p)
{
    Qt::CursorShape cursor;
    switch(p)
    {
    case 11:
    case 33:
        cursor=Qt::SizeFDiagCursor;break;
    case 13:
    case 31:
        cursor=Qt::SizeBDiagCursor;break;
    case 21:
    case 23:
        cursor=Qt::SizeHorCursor;break;
    case 12:
    case 32:
        cursor=Qt::SizeVerCursor;break;
    case 22:
        cursor=Qt::ArrowCursor;break;
    default:
          QApplication::restoreOverrideCursor();//恢复鼠标指针性状
        break;
    }
    w->setCursor(cursor);
}

int AppInit::countFlag(QPoint p, QSize window)//计算鼠标在哪一列和哪一行
{
    //qDebug() << "mouse" << m;
    if (MARGIN > p.x() > 0){
        if (MARGIN > p.y() > 0){
            return 11;
        }else if (p.y() > window.height() - 3){ //使用frammainwidget最大尺寸会小2个像素
            return 31;
        }else{
            return 21;
        }
    }else if (p.x() > window.width() - 3){
        if (MARGIN > p.y() > 0){
            return 13;
        }else if (p.y() > window.height() - 3){
            return 33;
        }else{
            return 23;
        }
    }else{
        if (MARGIN > p.y() > 0){
            return 12;
        }else if (p.y() > window.height() - 3){
            return 32;
        }else{
            return 22;
        }
    }
}

bool AppInit::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *w = (QWidget *)watched;
//    if (!w->property("canMove").toBool()) {
//        return QObject::eventFilter(watched, event);
//    }

    static QWidget *frmMain_adr;
    static QPoint mousePoint;
    static bool mousePressed = false;
    static bool drag = false;
    static bool scaling = false;
    static int  pos = 0;
    static QSize windowSize = QSize(808,600);
    static QPoint windowPos = QPoint(0,0);

    if (w->objectName().compare("frmMain") == 0) {
        // only frmMain can set cursor style
        windowSize = w->size();
        windowPos  = w->pos();
        frmMain_adr = w;
    }

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//    qDebug() << "mouseEvent->type()" << mouseEvent->type();
//    qDebug() << "watched->objectName()" << watched->objectName() ;
    if (mouseEvent->type() == QEvent::MouseButtonPress) {
        qDebug() <<  mouseEvent->pos();
        if (mouseEvent->button() == Qt::LeftButton) {
            if (w->objectName().compare("frmMain") == 0) {
                mousePoint = mouseEvent->globalPos() - windowPos;
                pos = countFlag(mouseEvent->pos(), windowSize);
                if (pos == 22){
                    drag = true;
                    scaling = false;
                }else{
                    scaling = true;
                    drag = false;
                }
            }
        }
    } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        drag = false;
        scaling = false;
    }

    if (mouseEvent->type() == QEvent::MouseMove) {
        //qDebug() << w->objectName() << countFlag(mouseEvent->pos(), windowSize) << mouseEvent->pos() << windowSize;
        if (w->objectName().compare("frmMain") == 0) {
            int position = countFlag(mouseEvent->pos(), windowSize);
            if (drag || scaling){
                if (drag) {
                   w->move(mouseEvent->globalPos() - mousePoint);
                   return true;
                }
                if (scaling){
                    QRect wid = w->geometry();
                    switch (pos)
                    {
                    case mouse_up:
                        wid.setTop(mouseEvent->globalPos().y());
                        break;
                    case mouse_down:
                        wid.setBottom(mouseEvent->globalPos().y());
                        break;
                    case mouse_left:
                        wid.setLeft(mouseEvent->globalPos().x());
                        break;
                    case mouse_right:
                        wid.setRight(mouseEvent->globalPos().x());
                        break;
                    case mouse_left_up:
                        wid.setTopLeft(mouseEvent->globalPos());
                        break;
                    case mouse_right_up:
                        wid.setTopRight(mouseEvent->globalPos());
                        break;
                    case mouse_left_down:
                        wid.setBottomLeft(mouseEvent->globalPos());
                        break;
                    case mouse_right_down:
                        wid.setBottomRight(mouseEvent->globalPos());
                        break;
                    }
                    w->setGeometry(wid);
                    windowSize = w->size();
                    windowPos  = w->pos();
                    return true;
                }
            }else{
                setCursorType(w,position);
                return true;
            }
        }else if (w->objectName().compare("frmMainWindow") == 0){
            //qDebug() << mouseEvent->pos() << windowSize;
            int position = countFlag(mouseEvent->pos(), windowSize);
            setCursorType(frmMain_adr,position);
        }
    }

    return QObject::eventFilter(watched, event);
}

void AppInit::start()
{
    qApp->installEventFilter(this);
}
