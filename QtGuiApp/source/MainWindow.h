﻿#ifndef MED_IMG_MAINWINDOW_H
#define MED_IMG_MAINWINDOW_H
#include "pch.h"
#include <QNetworkInterface>
#include "OpenCVImageUtil.h"
class QVTKOpenGLNativeWidget;
class vtkRenderer;
class DcmCStoreSender;
class MySocketServer;
class MySocketClient;
class MyThread_DCMTK_SCP_Work;
namespace Ui {
    class MainWindow_UI;
}
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


 

private slots:
    void StyleChanged(const QString& style);
    void ShutDown();
    void Change_CurrentTime();
	void ProgressChanged(int value, int max); // 进度条更新

	void OpenImage(); 
    void OpenCVtest();

 

signals:

private:
    void InitSlots();
    void UpdateGUI();
    void UpdateSize();
    void loadStyleSheet(const QString& path); // 动态加载 QSS
    void InitThread();
   
	
private:
    std::mutex m_mutex; // 互斥锁
    std::unique_ptr<Ui::MainWindow_UI> m_ui; 
    MyThread* m_thread; 
    MyThread_Work* m_thread_work; 
    MyThread_DCMTK_SCP_Work* m_scpWorkThread; // DICOM SCP 工作线程
    MyThread_Runnable* m_thread_runnable; 
    QTimer* m_current_time; 
    QThread* m_DCMScpsub;
    QThread* m_numsub; 
    std::unique_ptr<MyProgressDialog> m_progressDialog; // 进度对话框指针
    QString m_localIpAddress;//ip地址
    //Dcmtk SCP

	bool m_isListening = false; // 是否正在监听
    //Socket
	MySocketServer* m_socketServer; 
	MySocketClient* m_socketClient;
    QTcpSocket* m_tcpSocket;
	OpenCVImageUtil* m_opencvImageUtil; // OpenCV 图像处理工具类
};

#endif // MED_IMG_MAINWINDOW_H