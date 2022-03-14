#include "mainwindow.h"
#include "../ui/form/frmmain.h"
#include "../ui/core_common/appdata.h"
#include "../ui/core_common/appinit.h"
#include "../ui/core_common/quihelper.h"

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

    QUIHelper::setFont();
    QUIHelper::setCode();

    frmMain w;
    w.show();
    return a.exec();
}

