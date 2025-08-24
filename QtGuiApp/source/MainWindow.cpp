﻿#include "MainWindow.h"
// 移除不存在的头文件
#include <QDebug>
#include <QIcon>
#include <QFileDialog>
#include <memory>
#include "MainWindow-UI.h"
#include <thread>
#include <iostream>

#include "MySocketServerDlg.h"
#include "MySocketClient.h"
#include <QNetworkProxy> 
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    m_ui(std::make_unique<Ui::MainWindow_UI>()),
    m_thread(new MyThread(this)),
    m_thread_work(new MyThread_Work(this)),
	m_thread_runnable(new MyThread_Runnable(this)),
    m_DCMScpsub(new QThread(this)),
	m_numsub(new QThread(this)), 
    m_opencvImageUtil(new OpenCVImageUtil(this))
{
   
    m_socketServer = new MySocketServer(this);
	m_socketClient = new MySocketClient(this);
	m_tcpSocket = new QTcpSocket(this);
    //setWindowFlags(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/res/icon/favicon.ico")); // 覆盖可能的默认值
    m_ui->setupUi(this);
    InitSlots();
    UpdateSize();
    UpdateGUI();
    InitThread();
}

MainWindow::~MainWindow() 
{
    if (m_DCMScpsub)
    {
        if (m_DCMScpsub->isRunning())
        {
            m_DCMScpsub->quit();
            m_DCMScpsub->wait();
        }
    }
    if (m_numsub)
    {
        if (m_numsub->isRunning())
        {
            m_numsub->quit();
            m_numsub->wait();
        }
    }
}

;




void MainWindow::StyleChanged(const QString& style)
{
    qDebug() << "当前选中文本：" << style;
    QString styledir = ":/res/QSS/" + style + ".qss";
    loadStyleSheet(styledir);
}



void MainWindow::ShutDown()
{
    QApplication::quit();
}



void MainWindow::Change_CurrentTime()
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
	m_ui->label_currenttime->setText(currentTime);
}



void MainWindow::ProgressChanged(int value, int max)
{
	if (m_progressDialog)
	{
		m_progressDialog->setProgress(value);
		m_progressDialog->setMaximumValue(max);
		m_progressDialog->setLabelText(QString("正在处理... %1/%2").arg(value).arg(max));
		qApp->processEvents();
	}
}




void MainWindow::On_Clicked_Button()
{
    auto senderObj = sender();
    if (senderObj == m_ui->pushButton_openimage)
    {
        OpenImage();
	}
    else if (senderObj = m_ui->pushButton_test)
    {
        QThread::sleep(3); // 单位是秒
        OpenCVtest();
    }
}

void MainWindow::OpenImage()
{
    if (!m_opencvImageUtil)
        return;

    // 打开文件选择对话框，支持常见图片格式
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("选择图片文件"),
        "",
        tr("图片文件 (*.png *.jpg *.bmp *.tif *.tiff *.jpeg);;所有文件 (*.*)")
    );
    if (filePath.isEmpty())
        return;
    QByteArray filePathBytes = filePath.toLocal8Bit();
    // 使用OpenCVImageUtil打开图片
    if (!m_opencvImageUtil->openImage(std::string(filePathBytes.constData()))) {
        QMessageBox::warning(this, tr("打开失败"), tr("无法打开图片文件！"));
        return;
    }

    QImage img = m_opencvImageUtil->getCurrentImage();
    if (!img.isNull() && m_ui && m_ui->graphicsView) {
        // 假设 graphicsView 是 MyQGraphicsView 类型
        m_ui->graphicsView->setImage(img);
    }
}

void MainWindow::OpenCVtest()
{
    if (!m_opencvImageUtil)
        return;
    // 判断是否有图像数据
    if( !m_opencvImageUtil->getHasOpenImage()) {
        QMessageBox::warning(this, tr("测试失败"), tr("请先打开一张图片！"));
        return;
	}

    //m_opencvImageUtil->pixel_visit_demo(m_opencvImageUtil->getCurrentMat());
    m_opencvImageUtil->operator_demo(m_opencvImageUtil->getCurrentMat());

    
}













void MainWindow::InitSlots()
{
	m_current_time = new QTimer(this);
	connect(m_current_time, &QTimer::timeout, this, &MainWindow::Change_CurrentTime);
    m_current_time->start(1000);
    
    connect(m_ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::StyleChanged);
    connect(m_ui->pushButton_shutdown, &QPushButton::clicked, this, &MainWindow::ShutDown);

	connect(m_ui->pushButton_openimage, &QPushButton::clicked, this, &MainWindow::On_Clicked_Button);

    connect(m_ui->pushButton_test, &QPushButton::clicked, this, &MainWindow::On_Clicked_Button);
  
    
    


	
   
}

void MainWindow::UpdateGUI()
{
    if (!m_ui )
        return;
	m_ui->label_hospital->setText("上海泊维胜科技有限公司");
    m_ui->comboBox->setCurrentIndex(8);


}

void MainWindow::UpdateSize()
{
	sysconfig::GlobalConfig& config = sysconfig::GlobalConfig::get();
    QSize windowsize = config.getAdjustedSize();
    this->resize(windowsize);
}

void MainWindow::loadStyleSheet(const QString& path)
{
    QFile qssFile(path);
    if (!qssFile.exists()) {
        qWarning() << "QSS 文件不存在:" << path;
        return;
    }
    if (qssFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(styleSheet); // 全局应用样式
        qssFile.close();
    }
    else {
        qWarning() << "无法打开 QSS 文件:" << path;
    }
}

void MainWindow::InitThread()
{
    //方式一QThread
    //int idealThreads = MyThread::idealThreadCount(); // 获取硬件支持的最大线程数
    //int activeThreads = MyThread::getActiveThreadCount();
    //qDebug() << "硬件支持的最大线程数: " << idealThreads;
    //qDebug() << "现在活跃的线程数: " << activeThreads;

    //if (!m_thread->isRunning())
    //{
    //    m_thread->setPriority(QThread::LowestPriority); // 设置线程优先级
    //    m_thread->start(); // 启动线程
    //}
    //else
    //{
    //    qWarning() << "线程正在运行，无法启动新任务！";
    //}
  /*  connect(m_thread, &MyThread::numberGenerated, this, [this](int num) {
        m_ui->label_showthreadnum->setText(QString::number(num));
        });*/

    

    //方式二QObject 创建线程对象
    
    if (m_thread_work)
    {
        m_thread_work->moveToThread(m_numsub);
    }
    connect(m_thread_work, &MyThread_Work::numberGenerated, this, [this](int num) {
        m_ui->label_showthreadnum->setText(QString::number(num));
        });
    connect(m_numsub, &QThread::started, m_thread_work, &MyThread_Work::working);
    m_numsub->start();
	
        
   

    //方式三 线程池
    /*QThreadPool::globalInstance()->setMaxThreadCount(4);
	if (m_thread_runnable)
	{
		QThreadPool::globalInstance()->start(m_thread_runnable);
	}
    connect(m_thread_runnable, &MyThread_Runnable::numberGenerated, this, [this](int num) {

        m_ui->label_showthreadnum->setText(QString::number(num));
        });
    */

    //if (!m_scpWorkThread)
    //    return;
    //m_scpWorkThread->moveToThread(m_DCMScpsub);
    //connect(m_DCMScpsub, &QThread::started, m_scpWorkThread, &MyThread_DCMTK_SCP_Work::Working);

    ////m_scpWorkThread->GetSCP().setPort(11112); // 设置端口号
    //m_DCMScpsub->start();

}





