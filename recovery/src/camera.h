#ifndef _CAMERA_H
#define _CAMERA_H

#include "../lib/raspicam/src/raspicam_cv.h"
#include "image.h"
#include "part.h"

class Camera : public Part {
   public:
    int times;
    VideoCapture Capture;
    Mat Frame, TempImage, ImageDest, LabImage, HlsImage, ThreshImage,
        ThreshImagez, ThreshImagey, FinalDisp, H, Hinv;
    Mat DispImage;
    Mat WinSlip;
    Mat absm, mag, hls, luv, lab, dst, persp, blur;
    Mat Frame1;

    vector<vector<Point>> Contours;
    vector<Vec4i> Hierarchy;
    vector<Point2f> DestSrc;
    vector<Mat> Channels;

    UserData Data;

    int Array[1280];
    int LeftBaseTemp;
    int RightBaseTemp;
    int LeftBase, Thresh, RightBase;
    int Lock;

    double lxPositon, rxPositon, lNum, rNum, Cur, distance, r;
    Point3d ControlPts[40];
    Point3d ControlPtsR[40];
    Point Cura, Curb, Curc;
    uchar* CurrRowP;

    raspicam::RaspiCam_Cv raspi_cam;

    // int t;
    // ofstream file;

    Camera();
    virtual void WaitNeededNewData() override;
    virtual void UpdateIsNewData() override;
    void Run() override;
    virtual ~Camera();
};

#endif
