#ifndef _IMAGE_H
#define _IMAGE_H

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>

#include "raspicam/src/raspicam_cv.h"
using namespace cv;
using namespace std;

struct UserData {
    Mat Image;
    vector<Point2f> Points;
};

Point3d PointAdd(Point3d p, Point3d q);
Point3d PointTimes(float c, Point3d p);
Point3d Bernstein(float u, Point3d* p);
void DrawBezier(Mat& Image, Point3d* pControls);
double Distance(Point p1, Point p2);
int Collinear(Point p1, Point p2, Point p3);
double Curvature(Point p1, Point p2, Point p3);
void Perspective(const cv::Mat& src, cv::Mat& dst, const int& width,
                 const int& height);
void inverse_Perspective(const cv::Mat& src, cv::Mat& dst, const int& width,
                         const int& height);
void abs_sobel_thresh(const cv::Mat& src, cv::Mat& dst, const char& orient,
                      const int& thresh_min, const int& thresh_max);
void hls_select(const cv::Mat& src, cv::Mat& dst, const char& channel,
                const int& thresh_min, const int& thresh_max);
void luv_select(const cv::Mat& src, cv::Mat& dst, const char& channel,
                const int& thresh_min, const int& thresh_max);
void mag_thresh(const cv::Mat& src, cv::Mat& dst, const int& sobel_kernel,
                const int& thresh_min, const int& thresh_max);

#endif