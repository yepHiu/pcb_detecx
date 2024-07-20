#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 变量初始化
    script="./yolo/yolov5_detect.py";
    output_path="./output/";
    input_file="./example/testimage/test1.jpg";
    scriptinput_path="./input/";

    // 组件初始化
    ui->txt_output->appendPlainText(output_path);
    ui->txt_input->appendPlainText(input_file);
    ui->txt_scriptPath->appendPlainText(script);
    ui->txt_scriptInputPath->appendPlainText(scriptinput_path);

    // 表格初始化
    QStringList titlelist;   //标题列表
    titlelist<<"Index"<<"X"<<"Y"<<"conf."<<"type.";
    ui->table_detect->setColumnCount(titlelist.size());                        // 设置表格的列数
    ui->table_detect->setHorizontalHeaderLabels(titlelist);                    // 设置水平头
    //ui->table_detect->setEditTriggers(QAbstractItemView::NoEditTriggers);   // 设置表结构默认不可编辑

    //loadJsonIntoTable("./position.json",ui->table_detect);

    // 摄像机相关部件UI初始化
    /////////////////////////////////////////////////////////////////////////////////
    QList<QCameraInfo> infos = QCameraInfo::availableCameras();    // 获取摄像头信息
    qDebug()<<QCameraInfo::availableCameras().count();    // 调试相机信息
    if(QCameraInfo::availableCameras().count()==0){
        qDebug()<<"没有检测到相机";
        ui->cameraBox->addItem("未检测到相机");
    }
    foreach (QCameraInfo info, infos) {
        qDebug()<<"检测到相机如下：";
        qDebug()<<info.deviceName().toUtf8()<<info.description().toUtf8();//把摄像头设备名加入列表
        ui->cameraBox->addItem(info.deviceName().toUtf8());// 添加到comboBox中
    }

    ui->captureButton->setEnabled(false);
    ui->camera_stopButton->setEnabled(false);
    /////////////////////////////////////////////////////////////////////////////////

    // 串口相关部件UI初始化
    /////////////////////////////////////////////////////////////////////////////////
    QList<QSerialPortInfo> s_infos = QSerialPortInfo::availablePorts();
    if(QSerialPortInfo::availablePorts().count()==0)
    {
       qDebug()<<"没有串口连接";
       ui->serialBox->addItem("未找到串口设备");
    }
    foreach (QSerialPortInfo s_info, s_infos) {
        qDebug()<<"串口设备名："<<s_info.portName().toUtf8();
        qDebug()<<"串口设备制造商:"<<s_info.manufacturer().toUtf8();
        qDebug()<<"---------------------------------------------";
        ui->serialBox->addItem(s_info.portName().toUtf8());
    }

    ui->serial_sendButton->setEnabled(false);
    ////////////////////////////////////////////////////////////////////////////////

    setLED(ui->lab_LED1,1,16);// 初始化指示转态
    setLED(ui->lab_LED2,1,16);

    // 对象初始化 文件夹检测对象
    watcher = new filewatch(output_path,ui->lab_OutputImage);
    process = new pyprocess("python3",script,scriptinput_path,output_path);
    binprocess = new pyprocess("python3","./test_cam.py");
    js = new jsonresult("./yolo/yolo5n_save/capture.json",ui->table_detect);
    //js->loadJsonIntoTable();
    jsonwatch = new filewatch("./yolo/yolo5n_save/",[this](void){
        this->js->loadJsonIntoTable();
    });

    MainWindow::setWindowTitle("PCB缺陷检测(YOLOv5) by 逆舟实验室");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_inputSearch_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle="选择一个文件"; //对话框标题
    QString filter="图片文件(*.jpg *.gif *.png);;所有文件(*.*)"; //文件过滤器
    input_file=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (!input_file.isEmpty()){
        ui->txt_input->clear();
        ui->txt_input->appendPlainText(input_file);
    }else{
        ui->lab_inputImage->setText("加载图片失败");
    }
}

void MainWindow::on_btn_inputOpen_clicked()
{
    input_file=ui->txt_input->toPlainText();
    if (!input_file.isEmpty()){
        QPixmap pixmap(input_file);
        if (!pixmap.isNull()) {
            // 设置QLabel的图片
            //ui->lab_inputImage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            ui->lab_inputImage->setPixmap(pixmap.scaled(ui->lab_inputImage->size(), Qt::KeepAspectRatio));
            // 调整QLabel的大小以适应图片
            //ui->lab_inputImage->adjustSize();
        } else {
            // 图片加载失败，显示错误信息
            ui->lab_inputImage->setText("加载图片失败");
        }
    }else{
        ui->lab_inputImage->setText("加载图片失败");
    }
}

void MainWindow::on_btn_outputSearch_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle="选择检测脚本的输出目录"; //对话框标题
    //QString filter=";图片文件(*.jpg *.gif *.png);;所有文件(*.*)"; //文件过滤器
    output_path=QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    if (!output_path.isEmpty()){
        ui->txt_output->clear();
        ui->txt_output->appendPlainText(output_path);
    }

    watcher->setDirectory(output_path);
    process->setOutputPath(output_path);
}

void MainWindow::on_btn_scriptSearch_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle="选择检测脚本文件"; //对话框标题
    QString filter="Python脚本文件(*.py);;所有文件(*.*)"; //文件过滤器
    script=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (!script.isEmpty()){
        ui->txt_scriptPath->clear();
        ui->txt_scriptPath->appendPlainText(script);
     }

    process->setScript(script);
}

void MainWindow::on_btn_scriptRun_clicked()
{
    QString sourceFilePath = input_file;// 设置脚本输入源文件
    QString destinationFolderPath = scriptinput_path;// 复制输入源文件到脚本输入文件夹

    // 清空脚本的输入文件夹，保证输入文件夹只有目标测试文件
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    QDir directory(scriptinput_path);
    // 确保目录存在
    if (!directory.exists()) {
        qWarning() << "Directory does not exist:" << scriptinput_path;
        return;
    }

    // 获取目录中的所有文件
    QStringList fileList = directory.entryList(QDir::Files);

    // 遍历文件列表并删除每个文件
    for (const QString &fileName : fileList) {
        QFile file(directory.filePath(fileName));
        if (file.exists()) {
            if (file.remove()) {
                qDebug() << "Removed file:" << fileName;
            } else {
                qWarning() << "Failed to remove file:" << fileName;
            }
        }
    }

    // 目标文件的完整路径（包括文件名）
    //QString destinationFilePath = destinationFolderPath + QDir::separator() + QFileInfo(sourceFilePath).fileName();
    QString destinationFilePath = destinationFolderPath + QDir::separator() + QFileInfo(sourceFilePath).fileName();
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug()<<"源文件不存在";
    }
    QDir destinationDir(destinationFolderPath);// 检查目标文件夹是否存在，如果不存在则创建
    if (!destinationDir.exists()) {
        if (!destinationDir.mkpath(".")) {
            qDebug()<<"目标文件夹不存在";
        }
    }

    if (sourceFile.copy(destinationFilePath)) {
        qDebug()<<"复制成功";
    } else {
        qDebug()<<"复制错误";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    process->startProcess();
}

void MainWindow::on_btn_scriptInputSearch_clicked()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle="选择检测脚本的输出目录"; //对话框标题
    //QString filter=";图片文件(*.jpg *.gif *.png);;所有文件(*.*)"; //文件过滤器
    scriptinput_path=QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    if (!scriptinput_path.isEmpty()){
        ui->txt_scriptInputPath->clear();
        ui->txt_scriptInputPath->appendPlainText(scriptinput_path);
    }

    process->setInputPath(scriptinput_path);
}

///
/// \brief MainWindow::on_captureButton_clicked
/// 发布拍照信号，然后执行SaveImage函数
void MainWindow::on_captureButton_clicked()
{
    cap->capture();//发布捕获信息
    //QMessageBox::information(this,"good","已经截取"); //取消信息提示
}

///
/// \brief MainWindow::on_camera_startButton_clicked
/// 摄像头启动函数
void MainWindow::on_camera_startButton_clicked()
{
    if(ui->cameraBox->currentText()=="未检测到相机"){
        QMessageBox::information(this,"good","请检查摄像头连接情况");
        return;
    }


    ca = new QCamera(ui->cameraBox->currentText().toUtf8(),this);//构造摄像头对象
    cap = new QCameraImageCapture(ca,this);//构造截屏对象
    QObject::connect(cap,&QCameraImageCapture::imageCaptured,this,&MainWindow::SaveImage);//连接截屏信号和显示截屏图像的槽

#if 0
    w = new QVideoWidget(ui->widget);//使用父类对象构造显示窗口类

#else
    w = new QCameraViewfinder(ui->cameraWidget);
#endif

    w->resize(ui->cameraWidget->size());    //调整大小一致
    ca->setViewfinder(w);    //设置摄像头的显示窗口
    w->show();
    ca->start();    //启动摄像头

    ui->camera_startButton->setEnabled(false);
    ui->camera_stopButton->setEnabled(true);
    ui->captureButton->setEnabled(true);

    QList<QSize> sizes=ca->supportedViewfinderResolutions();    // 查询摄像分辨率
    qDebug() << "viewfinderResolutions sizes.len = " << sizes.length();
    foreach (QSize size, sizes) {
        qDebug() << "Resolutions size = " << size;
    }

}

///
/// \brief MainWindow::on_camera_stopButton_clicked
/// 摄像头关闭函数
void MainWindow::on_camera_stopButton_clicked()
{
    //停止摄像头
    ca->stop();
    //隐藏显示窗口
    w->hide();

    QObject::disconnect(cap,&QCameraImageCapture::imageCaptured,this,&MainWindow::SaveImage);// 断开信号的连接

    ui->camera_startButton->setEnabled(true);
    ui->camera_stopButton->setEnabled(false);
    ui->captureButton->setEnabled(false);

    delete w;
    delete cap;
    delete ca;
}

///
/// \brief MainWindow::SaveImage 保存图像，过程中会二值化图像，又capture函数触发
/// \param id
/// \param preview 图像帧
///
void MainWindow::SaveImage(int id,const QImage &preview){
    qDebug()<<id;
    qDebug()<<"接收到保存信号";

    // 二值化处理
    // 将QImage转换为灰度图像
   QImage grayImage = preview.convertToFormat(QImage::Format_Grayscale8);

   // 裁剪图像的矩形区域 (x, y, width, height)
    QRect cropRect(190, 200, 400, 400); // 替换为你想要裁剪的区域

   // 裁剪图像
   grayImage = grayImage.copy(cropRect);
   // 二值化处理
   QString binary_text=ui->txt_binary->toPlainText();
   int threshold = binary_text.toInt(); // 设置阈值，可以根据需要调整
   for (int y = 0; y < grayImage.height(); ++y) {
       for (int x = 0; x < grayImage.width(); ++x) {
           QRgb pixel = grayImage.pixel(x, y);
           int grayValue = qGray(pixel);
           if (grayValue > threshold) {
               grayImage.setPixel(x, y, qRgb(255, 255, 255)); // 白色
           } else {
               grayImage.setPixel(x, y, qRgb(0, 0, 0)); // 黑色
           }
       }
   }


    QPixmap pix = QPixmap::fromImage(grayImage);//把QImage转换成QPixmap
    ui->lab_inputImage->setPixmap(pix.scaled(ui->lab_inputImage->size(), Qt::KeepAspectRatio));
    QString picpath = QString("./capture.jpg").arg(id);//保存图片
    pix.save(picpath,"JPG",100);
}

void MainWindow::on_serial_sendButton_clicked()
{
    QByteArray writeArray;
    writeArray.append(0x01);
    writeArray.append(0x02);
    writeArray.append(0x03);

    if(sp->isOpen()){
        if(sp->write(writeArray)){
            qDebug()<<"串口数据发送成功";
        }
        else if(!sp->waitForBytesWritten(10000)){
            qDebug()<<"串口发送失败";
        }
    }
}

///
/// \brief MainWindow::on_serial_startButton_clicked
/// 串口启动函数
void MainWindow::on_serial_startButton_clicked()
{
    if(ui->serial_startButton->text()=="开启串口")// 串口未打开
    {
        ui->serial_sendButton->setEnabled(false);
        sp = new QSerialPort(this);
        sp->setPortName(ui->serialBox->currentText().toUtf8());
        connect(sp,&QSerialPort::readyRead,this,&MainWindow::ReadData);
        sp->setBaudRate(ui->baudBox->currentText().toInt());// 需要转换格式
        sp->setParity(QSerialPort::Parity::NoParity);// 没有
        sp->setDataBits(QSerialPort::DataBits::Data8);// 八位数据位
        sp->setStopBits(QSerialPort::StopBits::OneStop);// 一位停止位
        sp->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

        if(sp->isOpen()){ // 假如串口已经开启则关闭这个箱子
            sp->close();
        }

        if(!sp->open(QIODevice::ReadWrite)) // 若串口不能打开串口
        {
            qDebug()<<"串口打开失败,请检查串口连接状态和当前用户权限";
            QMessageBox::warning(this,"串口打开失败","请检查串口连接状态和当前用户权限",QMessageBox::Cancel);
        }
        else{// 串口打开成功
            qDebug()<<"串口打开成功";
            ui->serial_sendButton->setEnabled(true);
            ui->serial_startButton->setText("关闭串口");
            //ui->serial_startButton->setChecked(false);
            return;
        }
    }
    else if(ui->serial_startButton->text()=="关闭串口")// 串口已经打开 true
    {
        if(sp->isOpen()){
            sp->close();
            ui->serial_startButton->setText("开启串口");
            ui->serial_sendButton->setEnabled(false);// 关闭串口后就 失能发送按钮
            qDebug()<<"串口已经关闭";
        }
    }
}


// 流程启动
void MainWindow::on_btn_beltStart_clicked()
{
//    QByteArray data;
//    data.append(0x7E);
//    data.append(0x01);
//    data.append(0x7E);
//    sp->write(data);
    TaskStart(*sp);
    setLED(ui->lab_LED1,2,16);
}

// 向前走
void MainWindow::on_btn_beltStop_clicked()
{
    QByteArray data;
    data.append(0x7E);
    data.append(0x02);
    data.append(0x7E);
    sp->write(data);
    setLED(ui->lab_LED1,1,16);
}

// 检测单片机是否在线
void MainWindow::on_btn_mcuStatus_clicked()
{
    QByteArray data;
    data.append(0x7E);
    data.append(0x97);
    data.append(0x7E);
    sp->write(data);
}

/**
 * @brief MainWindow::ReadData 串口接受处理函数
 */
void MainWindow::ReadData(void){
    if (sp) {
        QByteArray data = sp->readAll();
        qDebug() << "接收到数据：" << data;
        if(data[1]=0x99){
            setLED(ui->lab_LED2,2,16);
        }
    }
}


///
/// \brief MainWindow::on_btn_taskstart_clicked
/// 任务流程函数
void MainWindow::on_btn_taskstart_clicked()
{

    QTimer *timer1=new QTimer();
    timer1->setSingleShot(true);

    QTimer *timer2 = new QTimer();
    timer2->setSingleShot(true); // 设置为单次触发

    // 检查串口和摄像头是否已经启动
    if(!ui->camera_startButton->isEnabled() || !ui->serial_startButton->isEnabled()){
        QMessageBox::warning(this,"警告","检查摄像头与串口是否已经开启","Cancel");
        return;//返回
    }

    //connect(timer1, &QTimer::timeout,this,&MainWindow::on_btn_beltStop_clicked);//第二步 暂停传送带

//    connect(timer2, &QTimer::timeout, []() {
//        // 第三步
//    });

    ///////////////NO.1//////////////
    MainWindow::on_btn_beltStart_clicked();// 发出允许任务启动信息 0x01
    timer1->start(10000); // 等待执行下一步 单位是ms

    ///////////////NO.2//////////////
    MainWindow::on_captureButton_clicked(); // 拍摄照片 执行SaveImage函数 保存./capture.jpg

    MainWindow::on_btn_clearinputDir_clicked();// 清空输入目录

    delay(1000); // 这个延时是有用的
    // 将照片移动到脚本输入文件夹
    QFile binaryfile("./capture.jpg");
    binaryfile.copy("./input/capture.jpg"); // 保存到脚本的输入路径

    ///////////////NO.3//////////////
    // 运行检测脚本
    process->setInputPath("./input/");
    process->setOutputPath("./output/");
    process->startProcess(); // 脚本运行后会
    process->endProcess();

    int rowCount = ui->table_detect->rowCount();
    qDebug() << "Row count:" << rowCount;
    if(rowCount>=1){
        BeltBackward(*sp); // 有缺陷向后走
    }
    else{
        BeltForward(*sp);// 没缺陷向前走
    }
}

void MainWindow::on_check_bindebug_clicked(bool checked)
{

}


/////////////////////////////串口发送函数/////////////////////////

///
/// \brief TaskStart 允许任务启动
/// \param sp Qt串口指针
///
void TaskStart(QSerialPort &sp){
    QByteArray data;
    data.append(0x7E);
    data.append(0x01);
    data.append(0x7E);
    sp.write(data);
}

///
/// \brief BeltForward 传送带向前移动
/// \param sp
///
void BeltForward(QSerialPort &sp){
    QByteArray data;
    data.append(0x7E);
    data.append(0x02);
    data.append(0x7E);
    sp.write(data);
}

///
/// \brief BeltBackward 传送带向后移动
/// \param sp
///
void BeltBackward(QSerialPort &sp){
    QByteArray data;
    data.append(0x7E);
    data.append(0x03);
    data.append(0x7E);
    sp.write(data);
}

void MainWindow::on_btn_clearinputDir_clicked()
{
    QString directoryPath=ui->txt_scriptInputPath->toPlainText();
    QDir dir(directoryPath);
        if (!dir.exists()) {
            qDebug() << "Directory does not exist:" << directoryPath;
            return;
        }
        // 获取目录中的所有文件
        QStringList files = dir.entryList(QDir::Files);
        // 遍历并删除每个文件
        foreach (QString file, files) {
            if (!dir.remove(file)) {
                qDebug() << "Failed to remove file:" << file;
            }
        }
        qDebug() << "All files removed from directory:" << directoryPath;
}

///
/// \brief delay 延时函数
/// \param milliseconds
///
void delay(int milliseconds) {
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}
