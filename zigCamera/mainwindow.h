#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QList>
#include <QHash>
#include <opencv2/opencv.hpp>
#include "MultiObjTracker.h"

using namespace std;
using namespace cv;

class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QLineEdit;
class QTimer;
class QPaintEvent;

class QComboBox;

class mainWindow : public QWidget
{
    Q_OBJECT

public:
    mainWindow(QWidget *parent = 0);
    ~mainWindow();

private:
    void setMainLayout();

    void release();

private slots:
    void timeUpdate();

    void openResource();
    void play();
    void startTrack();
    void pause();
    void quitPlay();
    void descrip();


private:
    QLabel *bgLabel;
    QLabel *textLabel;
    QLabel *showLabel;
    QLabel *timeLabel;

    QPushButton *openBtn;
    QPushButton *pauseBtn;
    QPushButton *startBtn;
    QPushButton *quitBtn;
    QPushButton *descriptBtn;

    QLineEdit *totalNumEdit;
    QLineEdit *moveNumEdit;
    QLineEdit *fallEdit;

    QVBoxLayout *mainLayout;
    QHBoxLayout *btmHbLayout;
    QHBoxLayout *topHbLayout;
    QGridLayout *topGridLayout;
    QVBoxLayout *rightLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QFormLayout *formLayout;


private:
    QTimer *timer1;
    QTimer *timer2;

    bool openVideo;
    bool start;
    bool pausePlay;
    bool descripPliay;

    VideoCapture capture;
    Mat frame;
    QImage qimage;

    MultiObjTracker mot;
    BackgroundSubtractorMOG2 bg_model;
    //BackgroundSubtractorMOG bg;
    vector<Rect> boundingBoxes;
    vector<ObjInfo> objInfoVec;

};

#endif // MAINWINDOW_H
