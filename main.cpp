#include "frmmain.h"
#include "appinit.h"
#include "quihelper.h"

#define ble_only 0

int main(int argc, char *argv[])
{
    //设置不应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
    QApplication a(argc, argv);
    AppInit::Instance()->start();

#if ble_only
    MainWindow w;
    w.show();
#else
    QUIHelper::setFont();
    QUIHelper::setCode();

    frmMain w;
    QUIHelper::setFormInCenter(&w);
    w.show();
#endif

    return a.exec();
}
