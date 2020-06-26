#ifndef _IMAGE_H
#define _IMAGE_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "raspicam_cv.h"
using namespace cv;
using namespace std;

int Divs = 50;
bool doTestSpeedOnly=false;

struct UserData
{
    Mat Image;
    vector<Point2f>Points;
};


Point3d PointAdd(Point3d p, Point3d q);
Point3d PointTimes(float c, Point3d p);
Point3d Bernstein(float u, Point3d* p);
void DrawBezier(Mat& Image, Point3d* pControls);
double Distance(Point p1, Point p2);
int Collinear(Point p1, Point p2, Point p3);
double Curvature(Point p1, Point p2, Point p3);
void Perspective(const cv::Mat& src, cv::Mat& dst, const int& width, const int& height);
void inverse_Perspective(const cv::Mat& src, cv::Mat& dst, const int& width, const int& height);


#endif
