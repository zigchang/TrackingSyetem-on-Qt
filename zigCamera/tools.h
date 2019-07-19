#ifndef TOOL_H
#define TOOL_H

#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <QImage>

using namespace cv;
using namespace std;

void getBoundingBoxes(const Mat &fgmask, vector<Rect> &boundingBoxes, int areaThreshold);

bool isDiffObj(const Rect &objBox1, const Rect &objBox2);
bool isSameObj(const Rect &objBox1, const Rect &objBox2);

QImage matToQimage(const Mat &frame);

#endif
