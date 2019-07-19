#include "tools.h"

//fgmask: forground binary image

//const int AREATHRESHOLD = 4000;

void getBoundingBoxes(const Mat &originalFgmask, vector<Rect> &boundingBoxes, int areaThreshold)
{
    CV_Assert(originalFgmask.channels() == 1);
    Mat fgmask = originalFgmask.clone();

    threshold(fgmask, fgmask, 200, 255, THRESH_BINARY);


    int morph_size = 2;
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
    morphologyEx(fgmask, fgmask, MORPH_CLOSE, element);

    //morph_size = 6;
    element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

    morphologyEx(fgmask, fgmask, MORPH_OPEN, element);
    //imshow("foreground mask 2", fgmask);

    if (boundingBoxes.size() != 0)
    {
        boundingBoxes.clear();
    }

    //Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Find contours
    findContours(fgmask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point>> contours_poly(contours.size());
    //vector<Rect> boundRect(contours.size());
    //boundingBoxes.resize(contours.size());
    //vector<Point2f> center(contours.size());
    //vector<float> radius(contours.size());
    for (int i = 0; i < contours.size(); ++i)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        Rect tempRect = boundingRect(Mat(contours_poly[i]));

        if (tempRect.area() > areaThreshold)
        {
            boundingBoxes.push_back(tempRect);
        }


        //minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
    }
}

bool isDiffObj(const Rect &objBox1, const Rect &objBox2)
{
    int x1 = (objBox1.tl().x + objBox1.br().x) / 2;
    int y1 = (objBox1.tl().y + objBox1.br().y) / 2;
    int x2 = (objBox2.tl().x + objBox2.br().x) / 2;
    int y2 = (objBox2.tl().y + objBox2.br().y) / 2;
    int w = (objBox1.width + objBox2.width) / 2;
    int h = (objBox1.height + objBox2.height) / 2;

    if ((abs(x1 - x2) < w) && (abs(y1 - y2) < h))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool isSameObj(const Rect &objBox1, const Rect &objBox2)
{
    int minx1 = objBox1.tl().x;
    int miny1 = objBox1.tl().y;
    int maxx1 = objBox1.br().x;
    int maxy1 = objBox1.br().y;
    int minx2 = objBox2.tl().x;
    int miny2 = objBox2.tl().y;
    int maxx2 = objBox2.br().x;
    int maxy2 = objBox2.br().y;

    int minx = max(minx1, minx2);
    int miny = max(miny1, miny2);
    int maxx = min(maxx1, maxx2);
    int maxy = min(maxy1, maxy2);

    Rect interRect(Point(minx, miny), Point(maxx, maxy));
    //cout << interRect.area() << "   " << min(objBox1.area(), objBox2.area()) / 2 << endl;

    if (minx < maxx && miny < maxy && interRect.area() >(min(objBox1.area(), objBox2.area()) / 2))
    {

        return true;
    }
    else
    {
        return false;
    }
}
QImage matToQimage(const Mat &frame)
{
    Mat rgb;
    cvtColor(frame, rgb, COLOR_BGR2RGB);

    QImage qimage((uchar*)(rgb.data), rgb.cols, rgb.rows,
                  rgb.step, QImage::Format_RGB888);
    return qimage.copy();
}
