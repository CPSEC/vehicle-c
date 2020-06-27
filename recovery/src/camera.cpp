#include "camera.h"

#include <chrono>
#include <fstream>
#include <iostream>

#include "car.h"
#include "image.h"
#include "raspicam/src/raspicam_cv.h"
using namespace std::chrono;
using namespace std;

Camera::Camera() : Part(PartType::camera) {}

void Camera::Run(int test) {
    int times = 1;

    VideoCapture Capture("../project_video.mp4");
    Mat Frame, TempImage, ImageDest, LabImage, HlsImage, ThreshImage,
        ThreshImagez, ThreshImagey, FinalDisp, H, Hinv;
    Mat DispImage = Mat::zeros(720, 1280, CV_8UC1);
    Mat WinSlip = Mat::zeros(720, 1280, CV_8UC1);
    Mat absm, mag, hls, luv, lab, dst, persp, blur;
    Mat Frame1;

    vector<vector<Point>> Contours;
    vector<Vec4i> Hierarchy;
    vector<Point2f> DestSrc;
    vector<Mat> Channels;

    UserData Data;

    int Array[1280] = {0};
    int LeftBaseTemp = 0;
    int RightBaseTemp = 0;
    int LeftBase = 10000, Thresh = 720, RightBase = 10000;
    int Lock = 0;

    double lxPositon = -1, rxPositon = -1, lNum = 0, rNum = 0, Cur = 0.0,
           distance = 0.0, r = 0.0;
    Point3d ControlPts[40];
    Point3d ControlPtsR[40];
    Point Cura, Curb, Curc;
    uchar* CurrRowP;

    raspicam::RaspiCam_Cv Camera;

    cout << "Connecting to camera" << endl;

    cout << "Connected to camera =" << Camera.getId() << endl;

    Camera.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    Camera.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    int t = 100;
    ofstream file;
    file.open("camera times.txt");
    while (t > 0) {
        auto start1 = high_resolution_clock::now();
        Capture.read(Frame);

        auto stop1 = high_resolution_clock::now();
        file << "capture: "
             << double(duration_cast<microseconds>(stop1 - start1).count())
             << endl;

        start1 = high_resolution_clock::now();
        TempImage = Frame.clone();
        Data.Image = TempImage;

        abs_sobel_thresh(Frame, absm, 'x', 55, 200);
        mag_thresh(Frame, mag, 3, 45, 150);
        hls_select(Frame, hls, 's', 120, 255);
        luv_select(Frame, luv, 'l', 180, 255);
        TempImage = (absm & mag & luv) | (hls & luv);
        Perspective(TempImage, ThreshImage, Frame.cols, Frame.rows);

        ImageDest = Mat::zeros(Frame.size(), CV_8UC3);
        WinSlip = Mat::zeros(720, 1280, CV_8UC1);
        Mat Test = Mat::zeros(720, 1280, CV_8UC1);

        if (0 == Lock) {
            Lock = 1;
            for (int i = 0; i < 1280; i++) {
                Array[i] = 0;
            }
            for (int row = 0; row < Frame.rows; row++) {
                for (int col = 0; col < Frame.cols; col++) {
                    if (ThreshImage.at<uchar>(row, col) == 255) {
                        Array[col]++;
                    }
                }
            }

            for (int col = 0; col < Frame.cols; col++) {
                line(DispImage, Point(col, 720 - Array[col]),
                     Point(col + 1, 720 - Array[col + 1]), Scalar(255), 2,
                     LINE_8);
            }

            for (int row = 0; row < Frame.rows; row++) {
                for (int col = 0; col < Frame.cols / 2; col++) {
                    if (DispImage.at<uchar>(row, col) == 255) {
                        if (row < Thresh) {
                            Thresh = row;
                            LeftBase = col;
                        }
                    }
                }
            }
            Thresh = 720;
            for (int row = 0; row < Frame.rows; row++) {
                for (int col = Frame.cols / 2; col < Frame.cols; col++) {
                    if (DispImage.at<uchar>(row, col) == 255) {
                        if (row < Thresh) {
                            Thresh = row;
                            RightBase = col;
                        }
                    }
                }
            }
            ControlPts[0].x = LeftBase;
            ControlPts[0].y = 720;
            ControlPts[0].z = 0;

            ControlPtsR[0].x = RightBase;
            ControlPtsR[0].y = 720;
            ControlPtsR[0].z = 0;
        } else {
            ControlPts[0].x = LeftBaseTemp;
            ControlPts[0].y = 720;
            ControlPts[0].z = 0;

            ControlPtsR[0].x = RightBaseTemp;
            ControlPtsR[0].y = 720;
            ControlPtsR[0].z = 0;

            LeftBase = LeftBaseTemp;
            RightBase = RightBaseTemp;
            line(WinSlip, Point(ControlPts[0].x, ControlPts[0].y),
                 Point(ControlPtsR[0].x, ControlPtsR[0].y), Scalar(255), 2,
                 LINE_AA);
        }
        cv::cvtColor(ThreshImage, ThreshImage, COLOR_GRAY2BGR);
        for (int i = 0; i < 12; i++) {
            for (int WinRow = 720 - 60 * (i + 1); WinRow < 720 - 60 * (i);
                 WinRow++) {
                CurrRowP = ThreshImage.ptr<uchar>(WinRow);
                CurrRowP += ((LeftBase - 75) * 3);

                for (int lWinCol = LeftBase - 75; lWinCol < LeftBase + 75;
                     lWinCol++) {
                    if (((*CurrRowP) != 0) || ((*(CurrRowP + 1)) != 0) ||
                        ((*(CurrRowP + 2)) != 0)) {
                        lxPositon += lWinCol;
                        lNum++;
                    }
                    CurrRowP += 3;
                }

                CurrRowP =
                    ThreshImage.ptr<uchar>(WinRow) + ((RightBase - 75) * 3);
                for (int rWinCol = RightBase - 75; rWinCol < RightBase + 75;
                     rWinCol++) {
                    if (((*CurrRowP) != 0) || ((*(CurrRowP + 1)) != 0) ||
                        ((*(CurrRowP + 2)) != 0)) {
                        rxPositon += rWinCol;
                        rNum++;
                    }
                    CurrRowP += 3;
                }
            }

            if (lNum > 0) {
                LeftBase = (lxPositon / lNum);
                lNum = 0;
                lxPositon = 0;
            }

            if (rNum > 0) {
                RightBase = (rxPositon / rNum);
                rNum = 0;
                rxPositon = 0;
            }

            if (0 == i) {
                LeftBaseTemp = LeftBase;
                RightBaseTemp = RightBase;
            }
            if (i == 4) {
                ControlPts[1].x = LeftBase;
                ControlPts[1].y = (720 - 60 * (i + 2));
                ControlPts[1].z = 0;

                ControlPtsR[1].x = RightBase;
                ControlPtsR[1].y = (720 - 60 * (i + 2));
                ControlPtsR[1].z = 0;
            }
            if (i == 7) {
                ControlPts[2].x = LeftBase;
                ControlPts[2].y = (720 - 60 * (i + 2));
                ControlPts[2].z = 0;

                ControlPtsR[2].x = RightBase;
                ControlPtsR[2].y = (720 - 60 * (i + 2));
                ControlPtsR[2].z = 0;
            }
            if (i == 10) {
                ControlPts[3].x = LeftBase;
                ControlPts[3].y = (720 - 60 * (i + 2));
                ControlPts[3].z = 0;

                ControlPtsR[3].x = RightBase;
                ControlPtsR[3].y = (720 - 60 * (i + 2));
                ControlPtsR[3].z = 0;
                line(WinSlip, Point(ControlPts[3].x, ControlPts[3].y),
                     Point(ControlPtsR[3].x, ControlPtsR[3].y), Scalar(255), 2,
                     LINE_AA);
            }
        }
        DrawBezier(WinSlip, ControlPts);
        DrawBezier(WinSlip, ControlPtsR);

        cv::findContours(WinSlip, Contours, Hierarchy, RETR_EXTERNAL,
                         CHAIN_APPROX_SIMPLE, Point(0, 0));
        cv::cvtColor(WinSlip, WinSlip, COLOR_GRAY2BGR);
        for (int i = 0; i < Contours.size(); i++) {
            drawContours(WinSlip, Contours, i, Scalar(0, 255, 0), -1, LINE_8);
        }
        inverse_Perspective(WinSlip, TempImage, Frame.cols, Frame.rows);

        Cura.x = (ControlPts[0].x + ControlPtsR[0].x) / 2;
        Cura.y = (ControlPts[0].y + ControlPtsR[0].y) / 2;
        Curb.x = (ControlPts[2].x + ControlPtsR[2].x) / 2;
        Curb.y = (ControlPts[2].y + ControlPtsR[2].y) / 2;
        Curc.x = (ControlPts[3].x + ControlPtsR[3].x) / 2;
        Curc.y = (ControlPts[3].y + ControlPtsR[3].y) / 2;
        Cur = Curvature(Cura, Curb, Curc);
        cv::putText(Frame, format("Cur:%f m", Cur), Point(20, 100),
                    FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2, LINE_8);

        distance = (Curb.x - Frame.cols / 2) * 3.75 /
                   (ControlPtsR[3].x - ControlPts[3].x);
        cv::putText(Frame, format("Distance from center:%f m", distance),
                    Point(20, 200), FONT_HERSHEY_SIMPLEX, 1.5,
                    Scalar(255, 255, 255), 2, LINE_8);

        FinalDisp = Frame * 0.8 + TempImage * 0.2;

        stop1 = high_resolution_clock::now();
        file << "process: "
             << double(duration_cast<microseconds>(stop1 - start1).count())
             << endl;

        cout << "Curve: " << Cur << endl;

        t--;
    }
    file.close();

    while (times) {
    }
}

Camera::~Camera() {}