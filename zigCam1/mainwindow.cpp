#include "mainwindow.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QString>
#include <QMessageBox>
#include <qDebug>
#include <QPainter>
#include <QStackedWidget>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QDateTime>
#include <QTimer>
#include <QFileDialog>
#include <QComboBox>
#include <QImage>
#include <vector>
#include <string>

#include "tools.h"

mainWindow::mainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Detection & Tracking System"));
    setWindowIcon(QIcon(tr("D:\\scut.jpg")));
    setStyleSheet("background-color : rgb(255, 248, 220)");
    setMainLayout();

    openVideo = false;
    start = false;
    pausePlay = false;

    timer1 = new QTimer;
    connect(timer1, SIGNAL(timeout()), this, SLOT(play()));

    timer2 = new QTimer;
    connect(timer2, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer2->start(50);
}

mainWindow::~mainWindow()
{

}

void mainWindow::setMainLayout()
{

    bgLabel = new QLabel;
    QPixmap pixmap;
    pixmap.load(tr("D:\\scut.jpg"));

    pixmap = pixmap.scaled(50, 50);
    bgLabel->setScaledContents(true);
    bgLabel->setPixmap(pixmap);

    textLabel = new QLabel;
    QFont font;
    font.setPointSize(20);
    font.setBold(true);
    font.setItalic(true);
    textLabel->setFont(font);

    QPalette palet;
    palet.setColor(QPalette::WindowText, Qt::blue);
    textLabel->setPalette(palet);
    textLabel->setText(tr("Detection & Tracking System By CT"));

    timeLabel = new QLabel;
    palet.setColor(QPalette::WindowText, Qt::black);
    timeLabel->setPalette(palet);

    timeLabel->setFrameStyle(QFrame::Sunken | QFrame::Box);

    topGridLayout = new QGridLayout;
    topGridLayout->addWidget(textLabel, 0, 0, 2, 1, Qt::AlignLeft);
    topGridLayout->addWidget(timeLabel, 0, 1, 1, 1, Qt::AlignRight);

    topHbLayout = new QHBoxLayout;
    topHbLayout->addWidget(bgLabel);
    topHbLayout->addLayout(topGridLayout, 1);


    openBtn = new QPushButton(tr("Open"));
    connect(openBtn, SIGNAL(pressed()), this, SLOT(openResource()));
    pauseBtn = new QPushButton(tr("Pause"));
    connect(pauseBtn, SIGNAL(pressed()), this, SLOT(pause()));
    startBtn = new QPushButton(tr("Start Track"));
    connect(startBtn, SIGNAL(pressed()), this, SLOT(startTrack()));
    quitBtn = new QPushButton(tr("Quit"));
    connect(quitBtn, SIGNAL(pressed()), this, SLOT(quitPlay()));
    descriptBtn = new QPushButton(tr("Descript"));

    groupBox = new QGroupBox;
    groupBox->setStyleSheet("color: black");
    gridLayout = new QGridLayout;
    gridLayout->addWidget(openBtn, 0, 0, 1, 1);
    gridLayout->addWidget(pauseBtn, 0, 1, 1, 1);
    gridLayout->addWidget(startBtn, 1, 0, 1, 1);
    gridLayout->addWidget(quitBtn, 1, 1, 1, 1);
    gridLayout->addWidget(descriptBtn, 2, 0, 1, 1);
    groupBox->setLayout(gridLayout);

    QPalette palette;
    palette.setColor(QPalette::Text,Qt::black);

    palet.setColor(QPalette::WindowText, Qt::black);
    formLayout = new QFormLayout;
    QLabel *label1 = new QLabel(tr("Total Num:"));
    label1->setPalette(palet);
    totalNumEdit = new QLineEdit;
    totalNumEdit->setAlignment(Qt::AlignCenter);
    totalNumEdit->setPalette(palette);
    formLayout->addRow(label1, totalNumEdit);
    QLabel *label2 = new QLabel(tr("Moving Num:"));
    label2->setPalette(palet);
    moveNumEdit = new QLineEdit;
    moveNumEdit->setAlignment(Qt::AlignCenter);
    moveNumEdit->setPalette(palette);
    formLayout->addRow(label2, moveNumEdit);
    QLabel *label3 = new QLabel(tr("Fall Down:"));
    label3->setPalette(palet);
    fallEdit = new QLineEdit;
    fallEdit->setAlignment(Qt::AlignCenter);
    fallEdit->setPalette(palette);
    formLayout->addRow(label3, fallEdit);

    rightLayout = new QVBoxLayout;
    rightLayout->addWidget(groupBox);
    rightLayout->addStretch();
    rightLayout->addLayout(formLayout);

    showLabel = new QLabel;
    showLabel->setAlignment(Qt::AlignCenter);
    //showLabel->setScaledContents(true);
    showLabel->setMinimumSize(600, 500);
    showLabel->setStyleSheet(tr("background-color : black"));

    btmHbLayout = new QHBoxLayout;
    btmHbLayout->addWidget(showLabel, 1);
    btmHbLayout->addLayout(rightLayout);

    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topHbLayout);
    mainLayout->addLayout(btmHbLayout);

    setLayout(mainLayout);

}

void mainWindow::release()
{

    timer1->stop();

    pauseBtn->setText(tr("Pause"));
    startBtn->setText(tr("Start Track"));
    showLabel->clear();

    totalNumEdit->clear();
    moveNumEdit->clear();
    fallEdit->setStyleSheet("background-color:rgba(255, 248, 220)");

    openVideo = false;
    start = false;
    capture.release();
    bg_model = BackgroundSubtractorMOG2();
    mot.hasInit = false;
    mot.multiTrackers.clear();
    mot.currObjNum = 0;
    mot.hisObjNum = 0;

    boundingBoxes.clear();
    objInfoVec.clear();

}

void mainWindow::timeUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("MM-dd hh:mm:ss ddd");
    timeLabel->setText(str);
}

void mainWindow::openResource()
{
    QString qvideoName = QFileDialog::getOpenFileName(this, tr("Open Video"),
                                                      tr("D:\\test\\"), tr("File Type(*.avi *.wmv *.mp4)"));

    if(qvideoName.isEmpty()) {
        qDebug() << "Resource empty...";
        return;
    }
    string videoName = qvideoName.toStdString();

    capture.open(videoName);

    if(!capture.isOpened())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Fail to open video"));
        msgBox.setWindowFlags(Qt::CustomizeWindowHint);
        msgBox.exec();
        return;
    }

    if(!openVideo) {
        timer1->start(20);
        pauseBtn->setText(tr("Pause"));
    }
    openVideo = true;
}

void mainWindow::play()
{
    if(openVideo) {
        capture >> frame;

        if(!frame.empty()) {
            if(start) {
                Mat fgmask;
                Mat grayFrame;
                bg_model(frame, fgmask, -1);
                //bg_model.
                mot.setFgMask(fgmask);
                getBoundingBoxes(fgmask, boundingBoxes, 3000);
                cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
                if (!mot.hasInit && !boundingBoxes.empty())
                {
                    //KCFTracker kcf;

                    //.init(boundingBoxes[0], img);
                    mot.initialize(frame, boundingBoxes);
                    mot.hasInit = true;
                }

                mot.run(grayFrame, boundingBoxes, true, true);
                mot.analyze();

                mot.getObjInfo(objInfoVec);
                mot.stateout(frame, boundingBoxes, objInfoVec);
                QString totalNum = QString::number(mot.currObjNum);
                totalNumEdit->setText(totalNum);

                QString moveNum = QString::number(mot.mnum);
                qDebug() << moveNum;
                moveNumEdit->setText(moveNum);

                if(mot.hasFall) {
                    fallEdit->setStyleSheet("background-color:rgba(255,0,0)");
                }
                else {
                    fallEdit->setStyleSheet("background-color:rgba(0,255,0)");
                }

                rectangle(frame, Rect(Point(5, 5), Point(frame.cols - 5, frame.rows - 5)), Scalar(0, 255, 0), 2, 8, 0);
                //putText(img, "#" + to_string(frameNum), Point(10, 35), FONT_HERSHEY_SCRIPT_COMPLEX, 1, Scalar(0, 0, 255), 2);

            }

            qimage = matToQimage(frame);
            showLabel->setPixmap(QPixmap::fromImage(qimage).scaled(showLabel->size(),
                                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        else {
            release();
        }
    }
}

void mainWindow::startTrack()
{
    if(!openVideo && !start) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Please open video!!!"));
        msgBox.setWindowFlags(Qt::CustomizeWindowHint);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if(!start) {
        start = true;
        startBtn->setText(tr("Stop Track"));
    }
    else {
        startBtn->setText(tr("Start Track"));
        start = false;
        bg_model = BackgroundSubtractorMOG2();
        mot.hasInit = false;
        mot.multiTrackers.clear();
        mot.currObjNum = 0;
        mot.hisObjNum = 0;

        boundingBoxes.clear();
        objInfoVec.clear();

        totalNumEdit->clear();
        moveNumEdit->clear();
        fallEdit->setStyleSheet("background-color:rgba(0,60,60)");



    }
}

void mainWindow::pause()
{
    if(!openVideo) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Please open video!!!"));
        msgBox.setWindowFlags(Qt::CustomizeWindowHint);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if(!pausePlay) {
        pausePlay = true;
        pauseBtn->setText(tr("Play"));
        timer1->stop();
    }
    else {
        pausePlay = false;
        pauseBtn->setText(tr("Pause"));
        timer1->start(20);
    }
}

void mainWindow::quitPlay()
{
    if(!openVideo) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Please open video!!!"));
        msgBox.setWindowFlags(Qt::CustomizeWindowHint);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    release();
}
