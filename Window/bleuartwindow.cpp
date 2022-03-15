#include "bleuartwindow.h"

static int line = 1;
static int row  = 0;
int maxrow = 3;
myQPushButton *firstbutton = nullptr;
QWidget *WidgetContents = nullptr;
QGridLayout *gridlayout = nullptr;

QString last_dir = nullptr;
bleUartWindow::bleUartWindow(QWidget *parent)
    : QWidget{parent}
{
    init();
}


void bleUartWindow::init()
{
    dockBleUart = new QDockWidget(this);
    dockBleUart->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockBleUart->setWindowTitle("自定");
    dockBleUart->setObjectName("自定窗口");
    //dockBleUart->setFloating(true);

    WidgetContents = new QWidget(dockBleUart);
    gridlayout     = new QGridLayout(WidgetContents);

    button_add_bleUart = new QPushButton(WidgetContents);
    button_add_bleUart->setMaximumHeight(30); //limit size
    button_add_bleUart->setMaximumWidth(80);
    button_add_bleUart->setText("add");

    button_load_bleUart = new QPushButton(WidgetContents);
    button_load_bleUart->setMaximumHeight(30); //limit size
    button_load_bleUart->setMaximumWidth(80);
    button_load_bleUart->setText("load");

    button_save_bleUart = new QPushButton(WidgetContents);
    button_save_bleUart->setMaximumHeight(30); //limit size
    button_save_bleUart->setMaximumWidth(80);
    button_save_bleUart->setText("save");

    gridlayout->addWidget(button_add_bleUart,0,0);
    gridlayout->addWidget(button_load_bleUart,0,1);
    gridlayout->addWidget(button_save_bleUart,0,2);

    connect(button_add_bleUart , SIGNAL(clicked(bool)), this ,SLOT(addButton()));
    connect(button_load_bleUart, SIGNAL(clicked(bool)), this ,SLOT(loadFile(bool)));
    connect(button_save_bleUart, SIGNAL(clicked(bool)), this ,SLOT(saveFile()));

    MainWindow::mutualUi->creatNewDockWindow(dockBleUart, Qt::TopDockWidgetArea,  false);
    dockBleUart->setWidget(WidgetContents);

    toolBtn = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn->setText(dockBleUart->windowTitle());
    //toolBtn1->setFixedSize(30,20);                              //调图标大小（不是setIconSize)
    MainWindow::mutualUi->toolbar->connect(toolBtn, &QToolButton::clicked, this, &bleUartWindow::closeWindow);
    MainWindow::mutualUi->toolbar->addWidget(toolBtn);                               //向工具栏添加QToolButton按钮
    dockBleUart->setVisible(false);

    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("mainWindow");
    last_dir = settings.value("config_dir").toString();
    settings.endGroup();

    if (!last_dir.isEmpty()){
        loadFile(true);
    }
}


void bleUartWindow::closeWindow()
{
    MainWindow::mutualUi->closeAllWindow();
    if (dockBleUart->isVisible()){
        dockBleUart->setVisible(false);
    }else{
        dockBleUart->setVisible(true);
    }
    toolBtn->setChecked(true);
}

//  dynamic add button
void bleUartWindow::addButton()
{
    myQPushButton *temp = nullptr, *ptemp = nullptr;
    bool ok;
    QString cmd = "";
    QString buttonName = "ButtonN";

    QString text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("input button name"), QLineEdit::Normal,0, &ok);
    if (ok && !text.isEmpty())
    {
        buttonName = text; //获取输入
    }
    else
    {
        return;
    }

    text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("input cmd + data (BB0101)"), QLineEdit::Normal,0, &ok);
    if (ok && !text.isEmpty())
    {
        cmd = text; //获取输入
    }
    else
    {
        return;
    }

    if (firstbutton == nullptr){
        qDebug() << "firstbutton is null";
        firstbutton = new myQPushButton(WidgetContents);
        firstbutton->setMaximumHeight(30); //limit size
        firstbutton->setMaximumWidth(100);
        firstbutton->setText(buttonName);
        firstbutton->nextButton = nullptr;
        firstbutton->prevButton = nullptr;
        firstbutton->cmd = cmd;
        temp = firstbutton;
    }
    else
    {
        qDebug() << "firstbutton is not null";
        temp = firstbutton;
        ptemp = nullptr; // last button
        while (temp->nextButton != nullptr) {
            //qDebug() << temp->text();
            //qDebug() << temp->cmd;
            temp = temp->nextButton;
        }

        temp->nextButton = new myQPushButton(WidgetContents);
        if (temp->nextButton == nullptr){
            qDebug() << "new is FAIL";
            return;
        }
        temp = temp->nextButton;
        temp->setMaximumHeight(20); //limit size
        temp->setMaximumWidth(100);
        temp->setText(buttonName);
        temp->nextButton = nullptr;
        temp->prevButton = ptemp;
        temp->cmd = cmd;
    }

    if (temp == nullptr){
        qDebug() << "temp is null";
        return;
    }

    connect(temp,SIGNAL(myclick(myQPushButton*)),this,SLOT(bleSendData(myQPushButton*)));

    if (row <= maxrow)
    {
        gridlayout->addWidget(temp,line,row);
        row++;
    }
    else
    {
        row = 0;
        line++;
        gridlayout->addWidget(temp,line,row);
    }

    temp = firstbutton;
    while (temp != nullptr) {
        qDebug() << temp->text();
        qDebug() << temp->cmd;
        temp = temp->nextButton;
    }
}

void bleUartWindow::bleSendData(myQPushButton* temp)
{
    qDebug() << temp->cmd.toUtf8(); //"aa11"
    QByteArray array = temp->cmd.toUtf8(); //"aa11"
    MainWindow::mutualUi->ble_send(array);
}


void bleUartWindow::saveFile()
{
    if (firstbutton == nullptr)
    {
        return;
    }
    myQPushButton *temp = firstbutton;
    QByteArray buf;
    while (temp != nullptr) {
        buf += temp->text().toUtf8();
        buf += "+";
        buf += temp->cmd.toUtf8();
        buf += "+";
        temp = temp->nextButton;
    }

    QWidget *qwidget = new QWidget();
    QString dir = QFileDialog::getSaveFileName(qwidget,"save file","",nullptr);
    QFile file(dir);


    file.open(QFile::WriteOnly);
    file.write(buf);
    file.close();
}

void bleUartWindow::loadFile(bool b)
{
    line = 1;
    row  = 0;
    QString dir = nullptr;
    if (b){
        if (last_dir == nullptr){
            return;
        }
        dir = last_dir;
    }else{
        QWidget *qwidget = new QWidget();
        dir = QFileDialog::getOpenFileName(qwidget,"load file","",nullptr);
    }

    qDebug() << dir;
    if (dir == nullptr){
        return;
    }

    QFile file(dir);

    QByteArray buf;
    if (file.open(QFile::ReadOnly)){
        QSettings settings("Software Inc.","Icon Editor");
        settings.beginGroup("mainWindow");
        settings.setValue("config_dir",dir);
        settings.endGroup();
    }
    buf = file.readAll();
    file.close();

    qDebug() << buf;
    myQPushButton *temp = firstbutton;
    if (firstbutton != nullptr)
    {
        while (temp->nextButton != nullptr){
            temp = temp->nextButton;
        }

        temp = temp->prevButton;

        while (temp->nextButton != nullptr && temp->prevButton != nullptr){
            delete  temp->nextButton;
            temp = temp->prevButton;
        }
        delete temp;
    }

    //dockBleUart->close();

    QList<QByteArray> list = buf.split('+');
    for (int i = 0; i < list.count() - 1; i++)
    {
        qDebug() << list.at(i) << i << list.count();
        if (i == 0){
            myQPushButton *newbutton = nullptr;
            newbutton = new myQPushButton(WidgetContents);
            newbutton->setMaximumHeight(30); //limit size
            newbutton->setMaximumWidth(100);
            newbutton->setText(list.at(i));
            newbutton->cmd = list.at(i + 1);
            firstbutton = newbutton;
            firstbutton->nextButton = nullptr;
            firstbutton->prevButton = nullptr;
            temp = firstbutton;
        }
        else if (i % 2 == 0)
        {
            myQPushButton *newbutton = nullptr;
            newbutton = new myQPushButton(WidgetContents);
            newbutton->setMaximumHeight(30); //limit size
            newbutton->setMaximumWidth(100);
            newbutton->setText(list.at(i));
            newbutton->cmd = list.at(i + 1);
            newbutton->prevButton = temp;
            newbutton->nextButton = nullptr;

            temp->nextButton = newbutton;
            temp = newbutton;
        }
    }

    temp = firstbutton;
    while (temp != nullptr)
    {
        qDebug() << temp->text();
        connect(temp,SIGNAL(myclick(myQPushButton*)),this,SLOT(bleSendData(myQPushButton*)));
        if (row < maxrow)
        {
            gridlayout->addWidget(temp,line,row);
            row++;
        }
        else
        {
            row = 0;
            line++;
            gridlayout->addWidget(temp,line,row);
            row = 1;
        }

        temp = temp->nextButton;
    }

}
