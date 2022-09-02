#ifndef BLESKUMSGHANDLE_H
#define BLESKUMSGHANDLE_H

#include <QByteArray>
#include <QWidget>
#include <QDockWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>

class bleskumsghandle : public QWidget
{
    Q_OBJECT
public:
    explicit bleskumsghandle(QWidget *parent = nullptr);
    ~bleskumsghandle();
    void govee_sku_blemsg_handle(const QByteArray &d);
    void govee_ble_test_stop_check(QByteArray array);

    // ble test
    void ble_test_handle(uint8_t);
    void ble_test_stop();
    unsigned char index;
    QToolButton *toolBtn1 = nullptr;
    QDockWidget *DockWidgetBleTest = nullptr;
    QWidget *dockWidgetContents;
    QTextEdit   *QTE_bleTest;
    uint64_t ble_test_count_sum;
    uint64_t ble_test_count_miss;
    QTimer *ble_test_timer = nullptr;
    int  ble_test_timer_sec = 100;
public slots:
    void ble_test();

};

#endif // BLESKUMSGHANDLE_H
