#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QPixmap>
#include <QMessageBox>
#include <QDebug>

// 摄像头相关的库
#include <QCameraInfo>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

// 串口相关库
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QTime>
#include <QTimer>

#include <QProcess>

#include <filewatch.h>
#include <pyprocess.h>
#include <led.h>

#include <jsonresult.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_inputSearch_clicked();

    void on_btn_inputOpen_clicked();

    void on_btn_outputSearch_clicked();

    void on_btn_scriptSearch_clicked();

    void on_btn_scriptRun_clicked();

    void on_btn_scriptInputSearch_clicked();

    void on_captureButton_clicked();

    void on_camera_startButton_clicked();

    void on_serial_sendButton_clicked();

    void on_camera_stopButton_clicked();

    void on_serial_startButton_clicked();

    void on_btn_beltStart_clicked();

    void on_btn_beltStop_clicked();

    void on_btn_mcuStatus_clicked();

    void SaveImage(int id,const QImage &preview);

    void ReadData(void);

    //void on_btn_taskStart_clicked();

    //void on_pushButton_2_clicked();

    void on_btn_taskstart_clicked();

    void on_check_bindebug_clicked(bool checked);


    void on_btn_clearinputDir_clicked();

private:
    Ui::MainWindow *ui;

    //QString script_file; // 脚本文件
    QString output_path; // 脚本目标输出文件夹
    QString input_file; // 目标测试文件
    QString scriptinput_path; //脚本输入路径
    QString script; // 脚本名
    QString model_path;

    filewatch *watcher;
    pyprocess *process;
    pyprocess *binprocess;
    filewatch *jsonwatch;
    jsonresult *js;

    QPushButton *captureButton, *runScriptButton;

    // 摄像头相关对象
    QCamera *ca; //摄像头对象
    QCameraViewfinder *w;
    QCameraImageCapture *cap;//截屏对象

    // 串口相关对象地址
    QSerialPort *sp;
};

void TaskStart(QSerialPort &sp);

void BeltForward(QSerialPort &sp);

void BeltBackward(QSerialPort &sp);

void delay(int milliseconds);
#endif // MAINWINDOW_H
