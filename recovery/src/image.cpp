#include "image.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>

#include "raspicam/src/raspicam_cv.h"
using namespace cv;
using namespace std;

int Divs = 50;
bool doTestSpeedOnly = false;

int findParam(string param, int argc, char** argv) {
    int idx = -1;
    for (int i = 0; i < argc && idx == -1; i++)
        if (string(argv[i]) == param) idx = i;
    return idx;
}
// parse command line
// returns the value of a command line param. If not found, defvalue is returned
float getParamVal(string param, int argc, char** argv, float defvalue = -1) {
    int idx = -1;
    for (int i = 0; i < argc && idx == -1; i++)
        if (string(argv[i]) == param) idx = i;
    if (idx == -1)
        return defvalue;
    else
        return atof(argv[idx + 1]);
}

void processCommandLine(int argc, char** argv, raspicam::RaspiCam_Cv& Camera) {
    Camera.set(cv::CAP_PROP_FRAME_WIDTH, getParamVal("-w", argc, argv, 1280));
    Camera.set(cv::CAP_PROP_FRAME_HEIGHT, getParamVal("-h", argc, argv, 960));
    Camera.set(cv::CAP_PROP_BRIGHTNESS, getParamVal("-br", argc, argv, 50));
    Camera.set(cv::CAP_PROP_CONTRAST, getParamVal("-co", argc, argv, 50));
    Camera.set(cv::CAP_PROP_SATURATION, getParamVal("-sa", argc, argv, 50));
    Camera.set(cv::CAP_PROP_GAIN, getParamVal("-g", argc, argv, 50));
    Camera.set(cv::CAP_PROP_FPS, getParamVal("-fps", argc, argv, 0));
    if (findParam("-gr", argc, argv) != -1)
        Camera.set(cv::CAP_PROP_FORMAT, CV_8UC1);
    if (findParam("-test_speed", argc, argv) != -1) doTestSpeedOnly = true;
    if (findParam("-ss", argc, argv) != -1)
        Camera.set(cv::CAP_PROP_EXPOSURE, getParamVal("-ss", argc, argv));

    //     Camera.setSharpness ( getParamVal ( "-sh",argc,argv,0 ) );
    //     if ( findParam ( "-vs",argc,argv ) !=-1 )
    //         Camera.setVideoStabilization ( true );
    //     Camera.setExposureCompensation ( getParamVal ( "-ev",argc,argv ,0 )
    //     );
}

void showUsage() {
    cout << "Usage: " << endl;
    cout << "[-gr set gray color capture]\n";
    cout << "[-test_speed use for test speed and no images will be saved]\n";
    cout << "[-w width] [-h height] \n[-br brightness_val(0,100)]\n";
    cout << "[-co contrast_val (0 to 100)]\n[-sa saturation_val (0 to 100)]";
    cout << "[-g gain_val  (0 to 100)]\n";
    cout << "[-ss shutter_speed (0 to 100) 0 auto]\n";
    cout << "[-fps frame_rate (0 to 120) 0 auto]\n";
    cout << "[-nframes val: number of frames captured (100 default). 0 == "
            "Infinite lopp]\n";

    cout << endl;
}

Point3d PointAdd(Point3d p, Point3d q) {
    p.x += q.x;
    p.y += q.y;
    p.z += q.z;
    return p;
}

Point3d PointTimes(float c, Point3d p) {
    p.x *= c;
    p.y *= c;
    p.z *= c;
    return p;
}

// P1*t^3 + P2*3*t^2*(1-t) + P3*3*t*(1-t)^2 + P4*(1-t)^3 = Pnew
Point3d Bernstein(float u, Point3d* p) {
    Point3d a, b, c, d, r;
    a = PointTimes(pow(u, 3), p[0]);
    b = PointTimes(3 * pow(u, 2) * (1 - u), p[1]);
    c = PointTimes(3 * u * pow((1 - u), 2), p[2]);
    d = PointTimes(pow((1 - u), 3), p[3]);

    r = PointAdd(PointAdd(a, b), PointAdd(c, d));
    return r;
}

void DrawBezier(Mat& Image, Point3d* pControls) {
    Point NowPt, PrePt;
    for (int i = 0; i <= Divs; i++) {
        float u = (float)i / Divs;
        Point3d NewPt = Bernstein(u, pControls);

        NowPt.x = (int)NewPt.x;
        NowPt.y = (int)NewPt.y;
        if (i > 0) {
            line(Image, NowPt, PrePt, Scalar(255), 2, LINE_AA, 0);
        }
        PrePt = NowPt;
    }
}

/*******************************************************************************************************/
double Distance(Point p1, Point p2) {
    double Dis;
    double x2, y2;
    x2 = (p1.x - p2.x) * (p1.x - p2.x);
    y2 = (p1.y - p2.y) * (p1.y - p2.y);
    Dis = sqrt(x2 + y2);
    return Dis;
}

int Collinear(Point p1, Point p2, Point p3) {
    double k1, k2;
    double kx1, ky1, kx2, ky2;
    if (p1.x == p2.x && p2.x == p3.x) {
        return 1;
    } else {
        kx1 = p2.x - p1.x;
        kx2 = p2.x - p3.x;
        ky1 = p2.y - p1.y;
        ky2 = p2.y - p3.y;
        k1 = ky1 / kx1;
        k2 = ky2 / kx2;
        if (k1 == k2) {
            return 1;
        } else {
            return 0;
        }
    }
}

double Curvature(Point p1, Point p2, Point p3) {
    double Cur;
    double Radius = 0.0;
    cv::Point p0;
    if (1 == Collinear(p1, p2, p3)) {
        Cur = 0.0;
    } else {
        double a = p1.x - p2.x;
        double b = p1.y - p2.y;
        double c = p1.x - p3.x;
        double d = p1.y - p3.y;
        double e =
            ((p1.x * p1.x - p2.x * p2.x) + (p1.y * p1.y - p2.y * p2.y)) / 2.0;
        double f =
            ((p1.x * p1.x - p3.x * p3.x) + (p1.y * p1.y - p3.y * p3.y)) / 2.0;
        double det = b * c - a * d;

        p0.x = -(d * e - b * f) / det;
        p0.y = -(a * f - c * e) / det;
        Radius = Distance(p0, p1);
    }
    return Radius;
}

void Perspective(const cv::Mat& src, cv::Mat& dst, const int& width,
                 const int& height) {
    vector<Point2f> points(4), points_after(4);
    points[0] = Point2f(577, 460);
    points[1] = Point2f(700, 460);
    points[2] = Point2f(1112, 720);
    points[3] = Point2f(232, 720);

    points_after[0] = Point2f(300, 0);
    points_after[1] = Point2f(950, 0);
    points_after[2] = Point2f(950, 720);
    points_after[3] = Point2f(300, 720);

    Mat transform = getPerspectiveTransform(points, points_after);

    warpPerspective(src, dst, transform, Size(width, height), INTER_LINEAR);
}

void inverse_Perspective(const cv::Mat& src, cv::Mat& dst, const int& width,
                         const int& height) {
    vector<Point2f> points(4), points_after(4);
    points[0] = Point2f(577, 460);
    points[1] = Point2f(700, 460);
    points[2] = Point2f(1112, 720);
    points[3] = Point2f(232, 720);

    points_after[0] = Point2f(300, 0);
    points_after[1] = Point2f(950, 0);
    points_after[2] = Point2f(950, 720);
    points_after[3] = Point2f(300, 720);

    Mat transform = getPerspectiveTransform(points_after, points);

    warpPerspective(src, dst, transform, Size(width, height), INTER_LINEAR);
}

void abs_sobel_thresh(const cv::Mat& src, cv::Mat& dst, const char& orient,
                      const int& thresh_min, const int& thresh_max) {
    cv::Mat src_gray, grad;
    cv::Mat abs_gray;
    cv::cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
    if (orient == 'x') {
        cv::Sobel(src_gray, grad, CV_64F, 1, 0);
        cv::convertScaleAbs(grad, abs_gray);
    }
    if (orient == 'y') {
        cv::Sobel(src_gray, grad, CV_64F, 0, 1);
        cv::convertScaleAbs(grad, abs_gray);
    }
    cv::inRange(abs_gray, thresh_min, thresh_max, dst);
}

void hls_select(const cv::Mat& src, cv::Mat& dst, const char& channel,
                const int& thresh_min, const int& thresh_max) {
    cv::Mat hls, grad;
    vector<cv::Mat> channels;
    cv::cvtColor(src, hls, cv::COLOR_RGB2HLS);
    cv::split(hls, channels);
    switch (channel) {
        case 'h':
            grad = channels.at(0);
            break;
        case 'l':
            grad = channels.at(1);
            break;
        case 's':
            grad = channels.at(2);
            break;
        default:
            break;
    }
    cv::inRange(grad, thresh_min, thresh_max, dst);
}

void luv_select(const cv::Mat& src, cv::Mat& dst, const char& channel,
                const int& thresh_min, const int& thresh_max) {
    cv::Mat luv, grad;
    vector<cv::Mat> channels;
    cv::cvtColor(src, luv, cv::COLOR_RGB2Luv);
    cv::split(luv, channels);
    switch (channel) {
        case 'l':
            grad = channels.at(0);
            break;
        case 'u':
            grad = channels.at(1);
            break;
        case 'v':
            grad = channels.at(2);
            break;
        default:
            break;
    }
    cv::inRange(grad, thresh_min, thresh_max, dst);
}

void mag_thresh(const cv::Mat& src, cv::Mat& dst, const int& sobel_kernel,
                const int& thresh_min, const int& thresh_max) {
    cv::Mat src_gray, gray_x, gray_y, grad;
    cv::Mat abs_gray_x, abs_gray_y;
    cv::cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
    cv::Sobel(src_gray, gray_x, CV_64F, 1, 0, sobel_kernel);
    cv::Sobel(src_gray, gray_y, CV_64F, 0, 1, sobel_kernel);
    cv::convertScaleAbs(gray_x, abs_gray_x);
    cv::convertScaleAbs(gray_y, abs_gray_y);
    cv::addWeighted(abs_gray_x, 0.5, abs_gray_y, 0.5, 0, grad);
    cv::inRange(grad, thresh_min, thresh_max, dst);
}