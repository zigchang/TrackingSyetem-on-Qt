#ifndef MULTIOBJTRACKER_H
#define MULTIOBJTRACKER_H

#include <opencv2\opencv.hpp>
#include <vector>
#include <map>
#include <queue>
#include "kcftracker.hpp"

using namespace cv;
using std::vector;
using std::map;
using std::queue;
using std::cout;
using std::endl;

const int BUFFERSIZE = 20;

enum class State
{
        S_MOVE = 0, S_STILL = 1, WENT = 2, FALL = 3, TBA = 4, OTHER = 5// BEND = 5, LIE = 6, = 7,
};


struct GeoInfo
{
        int height;
        float rate;
};

struct ObjInfo
{
        int id;
        Rect objBox;
        State objState;
        vector<Point> trajectory;
        //queue<GeoInfo> que;

        ObjInfo(int _id, const Rect &_objBox, State _objState, const vector<Point> &_trajectory) :
                id(_id), objBox(_objBox), objState(_objState), trajectory(_trajectory)
        {
        }
        ~ObjInfo()
        {
        }


};

class MultiObjTracker
{
public:
        MultiObjTracker();
        ~MultiObjTracker();



private:
        void addObj(Mat &grayImg, vector<Rect> &fgObjBoxes);
        void deleteObj(Mat &grayImg);

        void adjustByFg(Mat &grayImg, vector<Rect> &fgObjBoxes);

public:
        bool hasInit;
        int mnum = 0;
        bool hasFall=false;
        void initialize(Mat &grayImg, vector<Rect> &fgObjBoxes);
        void run(Mat &grayImg, vector<Rect> &fgObjBoxes, bool isAddObj = true, bool isAdjustByFg = true);
        State currState(const vector<Point> &locations);

        void analyze();
        void stateout(Mat &img, vector<Rect> &boundingBoxes, vector<ObjInfo> &objInfoVec);

        void getObjInfo(vector<ObjInfo> &objInfoVec);
        void idWithBox(map<int, Rect> &objMap);
        int getCurrObjNum() const;
        int getHisObjNum() const;
        void setFgMask(const Mat &mask);

public:
        struct TrackerInfo
        {
                KCFTracker kcf;
                int id;
                Rect objBox;
                queue<Mat> bufferObjFg;
                queue<Rect> bufferObjBox;
                //list<Rect> buff;
                State objState;
                vector<Point> trajectory;

                TrackerInfo(KCFTracker _kcf, int _id, Rect _objBox) : kcf(_kcf), id(_id), objBox(_objBox), objState(State::TBA)
                {
                }
                ~TrackerInfo()
                {
                }

                void getBufferObj()
                {
                        if (bufferObjFg.size() < BUFFERSIZE)
                        {
                                bufferObjBox.push(objBox);

                                bufferObjFg.push(fgMask(objBox));
                                //imshow("buffer fg", fgMask(objBox));
                                //cout << "size: " << bufferObjFg.size() << endl;
                        }
                        else
                        {
                                for (int i = 1; i < 4; ++i)
                                {
                                        bufferObjBox.pop();
                                        bufferObjBox.push(objBox);
                                        bufferObjFg.pop();
                                        bufferObjFg.push(fgMask(objBox));
                                }

                        }
                }

        };



        int currObjNum;
        int hisObjNum;
        static Mat fgMask;
        vector<TrackerInfo> multiTrackers;
};


#endif
