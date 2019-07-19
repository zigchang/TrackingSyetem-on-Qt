#include <opencv2\opencv.hpp>
#include "MultiObjTracker.h"
#include "tools.h"
#include<numeric>

const int DWSAMPLERATE = 2;
const int EDGEDIS = 5;
const int del = 5;



MultiObjTracker::MultiObjTracker()
{
    hasInit = false;
    currObjNum = 0;
    hisObjNum = 0;
    mnum = 0;
}

Mat MultiObjTracker::fgMask = Mat();

MultiObjTracker::~MultiObjTracker()
{
}

void MultiObjTracker::addObj(Mat &grayImg, vector<Rect> &fgObjBoxes)
{
    for (size_t i = 0; i < fgObjBoxes.size(); ++i)
    {
        bool isDiff = true;
        //bool isSame = false;

        for (size_t j = 0; j < multiTrackers.size(); ++j)
        {
            if (!isDiffObj(fgObjBoxes[i], multiTrackers[j].objBox))
            {
                isDiff = false;
                break;
            }
        }

        if (isDiff)
        {
            Rect rect = fgObjBoxes[i];
            if (rect.x > EDGEDIS && rect.y > EDGEDIS && rect.br().x < (grayImg.cols - EDGEDIS) && rect.br().y < (grayImg.rows - EDGEDIS))
            {
                KCFTracker kcf;
                kcf.init(rect ,grayImg);
                TrackerInfo newTrackerInfo(kcf, ++hisObjNum, rect);
                //newTrackerInfo.getBufferObj();
                multiTrackers.push_back(newTrackerInfo);
                ++currObjNum;
            }

            /*CompressiveTracker ct;
            ct.init(grayImg, fgObjBoxes[i]);
            TrackerInfo newTrackerInfo(ct, ++hisObjNum, fgObjBoxes[i]);
            multiTrackers.push_back(newTrackerInfo);
            ++currObjNum;*/
        }
    }
}
void MultiObjTracker::deleteObj(Mat &grayImg)
{
    //vector<size_t> index;
    for (int i = 0; i < multiTrackers.size(); ++i)
    {
        Rect rect = multiTrackers[i].objBox;
        if (rect.x < del || rect.y < del || rect.br().x >(grayImg.cols - del) || rect.br().y >(grayImg.rows - del))
        {
            multiTrackers.erase(multiTrackers.begin() + i);
            //cout << multiTrackers.size() << endl;
            //cout << "his: " << hisObjNum << endl;
            --i;
            --currObjNum;
            //--mnum;
        }
    }
    if (currObjNum == 0)
    {
        hasInit = false;
    }
}

void MultiObjTracker::adjustByFg(Mat &grayImg, vector<Rect> &fgObjBoxes)
{
    for (size_t i = 0; i < fgObjBoxes.size(); ++i)
    {
        for (size_t j = 0; j < multiTrackers.size(); ++j)
        {
            //cout << isSameObj(boundingBoxes[i], mot.multiTrackers[j].objBox) << endl;
            if (isSameObj(fgObjBoxes[i], multiTrackers[j].objBox))
            {
                /*CompressiveTracker ct;
                ct.init(grayImg, fgObjBoxes[i]);
                TrackerInfo newTrackerInfo(ct, multiTrackers[j].id, fgObjBoxes[i]);
                multiTrackers[multiTrackers[j].id - 1] = newTrackerInfo;*/
                multiTrackers[j].kcf.init(fgObjBoxes[i], grayImg);   //kcf.init(rect, grayImg);
                multiTrackers[j].objBox = fgObjBoxes[i];
                break;
            }
        }
    }
}

void MultiObjTracker::initialize(Mat &grayImg, vector<Rect> &fgObjBoxes)
{
    for (size_t i = 0; i < fgObjBoxes.size(); ++i)
    {
        Rect rect = fgObjBoxes[i];
        if (rect.x > EDGEDIS && rect.y > EDGEDIS && rect.br().x < (grayImg.cols - EDGEDIS) && rect.br().y < (grayImg.rows - EDGEDIS))
        {
            KCFTracker kcf;
            kcf.init(rect, grayImg);
            TrackerInfo newTrackerInfo(kcf, ++hisObjNum, rect);
            //newTrackerInfo.getBufferObj();
            multiTrackers.push_back(newTrackerInfo);
            ++currObjNum;
        }

    }
}

void MultiObjTracker::run(Mat &grayImg, vector<Rect> &fgObjBoxes, bool isAddObj, bool isAdjustByFg)
{
    //imshow("inside class fg", fgMask);


    for (size_t i = 0; i < multiTrackers.size(); ++i)
    {
        multiTrackers[i].objBox = multiTrackers[i].kcf.update(grayImg);
        multiTrackers[i].trajectory.push_back(Point(multiTrackers[i].objBox.x + multiTrackers[i].objBox.width / 2, multiTrackers[i].objBox.y + multiTrackers[i].objBox.height / 2));
        //multiTrackers[i].getBufferObj();
    }

    deleteObj(grayImg);

    if (isAddObj)
    {
        addObj(grayImg, fgObjBoxes);
    }

    if (isAdjustByFg)
    {
        adjustByFg(grayImg, fgObjBoxes);
    }
    for (size_t i = 0; i < multiTrackers.size(); ++i)
    {
        //multiTrackers[i].ct.processFrame(grayImg, multiTrackers[i].objBox);
        multiTrackers[i].getBufferObj();
    }

}

State MultiObjTracker::currState(const vector<Point> &locations)
{


    vector<Point> pathmeasure;
    for (size_t i = locations.size() / 2; i < locations.size(); ++i)
    {
        if (i % DWSAMPLERATE == 0)
        {
            pathmeasure.push_back(locations[i]);
        }
    }
    vector<int> distances;
    for (size_t i = 0; i < pathmeasure.size() - 1; ++i)
    {
        int dis = (pathmeasure[i].x - pathmeasure[i + 1].x) * (pathmeasure[i].x - pathmeasure[i + 1].x) +
            (pathmeasure[i].y - pathmeasure[i + 1].y) * (pathmeasure[i].y - pathmeasure[i + 1].y);
        distances.push_back(dis);
        //cout << dis << endl;
    }
    Scalar tempMean = mean(distances);

    for (size_t i = 0; i < multiTrackers.size(); ++i)
    {
        queue<Rect> temp = multiTrackers[i].bufferObjBox;
        int x_axis = 0;
        vector<int> h;
        vector<int> w;

        while (!temp.empty())
        {
            h.push_back(temp.front().height);
            w.push_back(temp.front().width);
            temp.pop();
        }
        //Height均值
        double sum1 = std::accumulate(std::begin(h), std::end(h), 0.0);
        double mean1 = sum1 / h.size();

        //Height方差
        double accumh = 0.0;
        std::for_each(std::begin(h), std::end(h), [&](const double d) {
            accumh += (d - mean1)*(d - mean1);
        });
        double dev1 = sqrt(accumh / (h.size() - 1));
        //cout << "H的方差为：" << dev1 << endl;

        //width均值
        double sum2 = std::accumulate(std::begin(w), std::end(w), 0.0);
        double mean2 = sum2 / w.size();

        //width方差
        double accumw = 0.0;
        std::for_each(std::begin(w), std::end(w), [&](const double d) {
            accumw += (d - mean2)*(d - mean2);
        });
        double dev2 = sqrt(accumw / (w.size() - 1));
        //cout << "W的方差为：" << dev2 << endl;


        float ratio = static_cast<float>(multiTrackers[i].bufferObjBox.back().width) / multiTrackers[i].bufferObjBox.back().height;
        //(temp.front().height - temp.back().height)
        //((float(temp.front().width) / temp.front().height * 300) > 400) &&
        //cout << "distMean: " << tempMean.val[0] << endl;

        if (ratio < 0.6)
        {
            if (tempMean.val[0] > 0.1)    { return State::S_MOVE; } //standing and moving
            else                                    { return State::S_STILL; }   //standing and static
        }
        else if (ratio >= 1.3) { return State::FALL; }
        else if (ratio > 0.6 && ratio < 1.3) { return State::WENT; }
        else return State::OTHER;
        //return State::RUN;
        //	}
        // {
        //return State::STILL;
        //	}

    }
    //cout << tempMean.val[0] << endl;

}

void MultiObjTracker::analyze()
{

    vector<vector<Point>> locations(currObjNum, vector<Point>());
    //vector<vector<Rect>> boundlines(currObjNum, vector<Rect>());


    int n = 0;
    for (size_t i = 0; i < multiTrackers.size(); ++i)
    {

        if (multiTrackers[i].bufferObjBox.size() == BUFFERSIZE)
        {


            queue<Rect> temp = multiTrackers[i].bufferObjBox;

            int x_axis = 0;
            while (!temp.empty())
            {
                int x = temp.front().x + temp.front().width / 2;
                int y = temp.front().y + temp.front().height / 2;
                locations[i].push_back(Point(x, y));
                //boundlines[i].push_back()

                //cout << x_axis << "  " << float(temp.front().width) / temp.front().height * 300 << endl;

                temp.pop();
            }

            multiTrackers[i].objState = currState(locations[i]);
            if (multiTrackers[i].objState == State::FALL)
            {
                hasFall = true;
                cout << " WARNING:SOMEBODY HAS FALLEN DOWN  !!!!!!!! " << endl;
            }
            else
            {
                hasFall = false;
            }
            if (multiTrackers[i].objState == State::S_MOVE )
            {
                cout << "MOVING" << endl;
                ++n;
            }
            //if (multiTrackers[i].objState == State::S_STILL)
            //{
            //cout << "still..." << endl;
            //}
//            cout << "current object number is :" << currObjNum << endl;
//            cout << "current mover number is :" << mnum << endl;
        }
        //;
    }

    mnum = n;
}

void MultiObjTracker::getObjInfo(vector<ObjInfo> &objInfoVec)
{
    objInfoVec.clear();
    for (size_t i = 0; i < multiTrackers.size(); ++i)
    {
        objInfoVec.push_back(ObjInfo(multiTrackers[i].id, multiTrackers[i].objBox, multiTrackers[i].objState, multiTrackers[i].trajectory));
    }
}


void MultiObjTracker::idWithBox(map<int, Rect> &objMap)
{
    for (size_t i = 0; i < multiTrackers.size(); ++i)
    {
        objMap.insert(map<int, Rect>::value_type(multiTrackers[i].id, multiTrackers[i].objBox));
    }
}


int MultiObjTracker::getCurrObjNum()const
{
    return currObjNum;
}

int MultiObjTracker::getHisObjNum() const
{
    return hisObjNum;
}
void MultiObjTracker::setFgMask(const Mat &mask)
{
    fgMask = mask.clone();
}


void MultiObjTracker::stateout(Mat &img, vector<Rect> &boundingBoxes, vector<ObjInfo> &objInfoVec)
{
    //getObjInfo(objInfoVec);
    for (size_t i = 0; i < objInfoVec.size(); ++i)
    {
        rectangle(img, objInfoVec[i].objBox, Scalar(0, 0, 255), 2, 8, 0);
        //cout << objInfoVec[i].objBox << endl;


        putText(img, to_string(objInfoVec[i].id), objInfoVec[i].objBox.tl(), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0));


        //float aspectRatio = static_cast<float>(objInfoVec[i].objBox.width) / objInfoVec[i].objBox.height;
        //putText(img, "AR: " + to_string(aspectRatio), Point(10, img.rows - 25), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 0));

        switch (objInfoVec[i].objState)
        {
        case State::S_MOVE:
            putText(img, "walk", Point(objInfoVec[i].objBox.x + 30, objInfoVec[i].objBox.y - 5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255)); break;
        case State::S_STILL:
            putText(img, "slow", Point(objInfoVec[i].objBox.x + 30, objInfoVec[i].objBox.y - 5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255)); break;
        case State::WENT:
            putText(img, "bend", Point(objInfoVec[i].objBox.x + 30, objInfoVec[i].objBox.y - 5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255)); break;
        case State::FALL:
            putText(img, "fall ", Point(objInfoVec[i].objBox.x + 30, objInfoVec[i].objBox.y - 5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255)); break;
        case State::TBA:
            putText(img, "analyzing ", Point(objInfoVec[i].objBox.x + 30, objInfoVec[i].objBox.y - 5), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 255)); break;
        default:
            break;
        }

        if (objInfoVec[i].trajectory.size() > 1)
        {
            for (int j = 0; j < objInfoVec[i].trajectory.size() - 1; ++j)
                line(img, objInfoVec[i].trajectory[j], objInfoVec[i].trajectory[j + 1], Scalar(255, 0, 0));
        }
    }
    for (size_t i = 0; i < boundingBoxes.size(); ++i)
    {
        //rectangle(fgmask2, boundingBoxes[i], Scalar(255, 0, 0), 2, 8, 0);
        rectangle(img, boundingBoxes[i], Scalar(255, 0, 0), 2, 8, 0);
        //rectangle(fgmask2, boundingBoxes[i], Scalar(255, 0, 0), 2, 8, 0);
    }

}
