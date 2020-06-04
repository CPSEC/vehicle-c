#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

struct UserData
{
	Mat Image;
	vector<Point2f>Points;
};

int Divs = 50;

/**********************************************************************************************************/

Point3d PointAdd(Point3d p, Point3d q);
Point3d PointTimes(float c, Point3d p);
Point3d Bernstein(float u, Point3d* p);
void DrawBezier(Mat& Image, Point3d* pControls);
double Distance(Point p1, Point p2);
int Collinear(Point p1, Point p2, Point p3);
double Curvature(Point p1, Point p2, Point p3);
void Perspective(const cv::Mat& src, cv::Mat& dst, const int& width, const int& height);
void inverse_Perspective(const cv::Mat& src, cv::Mat& dst, const int& width, const int& height);

//����������ӣ�p=p+q
Point3d PointAdd(Point3d p,Point3d q)
{
	p.x += q.x;
	p.y += q.y;
	p.z += q.z;
	return p;
}

//�����ͱ������ p=c*p
Point3d PointTimes(float c,Point3d p)
{
	p.x *= c;
	p.y *= c;
	p.z *= c;
	return p;
}

//���㱴�������̵�ֵ
//����u�ķ�Χ��0-1֮��
//P1*t^3 + P2*3*t^2*(1-t) + P3*3*t*(1-t)^2 + P4*(1-t)^3 = Pnew 
Point3d Bernstein(float u,Point3d *p)
{
	Point3d a, b, c, d, r;
	a = PointTimes(pow(u,3),p[0]);
	b = PointTimes(3*pow(u,2)*(1-u),p[1]);
	c = PointTimes(3*u*pow((1-u),2),p[2]);
	d = PointTimes(pow((1-u),3),p[3]);

	r = PointAdd(PointAdd(a,b),PointAdd(c,d));
	return r;
}

//����Bezier����
void DrawBezier(Mat &Image,Point3d *pControls)
{
	Point NowPt, PrePt;
	for (int i = 0; i <=Divs; i++)
	{
		float u = (float)i / Divs;
		Point3d NewPt = Bernstein(u,pControls);

		NowPt.x = (int)NewPt.x;
		NowPt.y = (int)NewPt.y;
		if (i>0)
		{
			line(Image,NowPt,PrePt,Scalar(255),2,LINE_AA,0);
		}
		PrePt = NowPt;
	}
}

/*******************************************************************************************************/
double Distance(Point p1, Point p2)
{
	double Dis; //�����ľ���
	double x2, y2;
	x2 = (p1.x - p2.x)*(p1.x - p2.x);
	y2 = (p1.y - p2.y)*(p1.y - p2.y);
	Dis = sqrt(x2 + y2); //��ƽ����
	return Dis;
}

int Collinear(Point p1, Point p2, Point p3)  //�ж�3���Ƿ��ߣ����߷���1
{
	double k1, k2;
	double kx1, ky1, kx2, ky2;
	if (p1.x == p2.x && p2.x == p3.x) //��������궼��ȣ�����
	{
		return 1;
	}
	else
	{
		kx1 = p2.x - p1.x;
		kx2 = p2.x - p3.x;
		ky1 = p2.y - p1.y;
		ky2 = p2.y - p3.y;
		k1 = ky1 / kx1;
		k2 = ky2 / kx2;
		if (k1 == k2)   //AB��BCб����ȣ�����
		{
			return 1;
		}
		else
		{
			return 0;   //������
		}
	}
}

double Curvature(Point p1, Point p2, Point p3)
{
	double Cur; //��õ�����
	double Radius = 0.0; //���ʰ뾶
	cv::Point p0;
	if (1 == Collinear(p1, p2, p3)) //�ж������Ƿ���
	{
		Cur = 0.0;//���㹲��ʱ��������Ϊĳ��ֵ 0
	}
	else
	{
		double a = p1.x - p2.x;
		double b = p1.y - p2.y;
		double c = p1.x - p3.x;
		double d = p1.y - p3.y;
		double e = ((p1.x * p1.x - p2.x * p2.x) + (p1.y * p1.y - p2.y * p2.y)) / 2.0;
		double f = ((p1.x * p1.x - p3.x * p3.x) + (p1.y * p1.y - p3.y * p3.y)) / 2.0;
		double det = b * c - a * d;

		p0.x = -(d * e - b * f) / det;
		p0.y = -(a * f - c * e) / det;
		Radius = Distance(p0, p1);
	}
	return Radius;
}

void Perspective(const cv::Mat& src, cv::Mat& dst, const int& width, const int& height) {
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

void inverse_Perspective(const cv::Mat& src, cv::Mat& dst, const int& width, const int& height) {
	vector<Point2f> points(4), points_after(4);
	points[0] = Point2f(577, 460);
	points[1] = Point2f(700, 460);
	points[2] = Point2f(1112, 720);
	points[3] = Point2f(232, 720);

	points_after[0] = Point2f(300, 0);
	points_after[1] = Point2f(950, 0);
	points_after[2] = Point2f(950, 720);
	points_after[3] = Point2f(300, 720);

	Mat transform = getPerspectiveTransform(points_after,points );

	warpPerspective(src, dst, transform, Size(width, height), INTER_LINEAR);
}

void abs_sobel_thresh(const cv::Mat& src, cv::Mat& dst, const char& orient, const int& thresh_min, const int& thresh_max) {
	cv::Mat src_gray, grad;
	cv::Mat abs_gray;
	//ת����Ϊ�Ҷ�ͼƬ
	cv::cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
	//ʹ��cv::Sobel()����x�����y����ĵ�
	if (orient == 'x') {
		cv::Sobel(src_gray, grad, CV_64F, 1, 0);
		cv::convertScaleAbs(grad, abs_gray);
	}
	if (orient == 'y') {
		cv::Sobel(src_gray, grad, CV_64F, 0, 1);
		cv::convertScaleAbs(grad, abs_gray);
	}
	//��ֵ��
	cv::inRange(abs_gray, thresh_min, thresh_max, dst);
}

void hls_select(const cv::Mat& src, cv::Mat& dst, const char& channel, const int& thresh_min, const int& thresh_max) {
	cv::Mat hls, grad;
	vector<cv::Mat> channels;
	cv::cvtColor(src, hls, cv::COLOR_RGB2HLS);
	//����ͨ��
	cv::split(hls, channels);
	//ѡ��ͨ��
	switch (channel)
	{
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
	//��ֵ��
	cv::inRange(grad, thresh_min, thresh_max, dst);
}

void luv_select(const cv::Mat& src, cv::Mat& dst, const char& channel, const int& thresh_min, const int& thresh_max) {
	cv::Mat luv, grad;
	vector<cv::Mat> channels;
	cv::cvtColor(src, luv, cv::COLOR_RGB2Luv);
	//����ͨ��
	cv::split(luv, channels);
	//ѡ��ͨ��
	switch (channel)
	{
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
	//��ֵ��
	cv::inRange(grad, thresh_min, thresh_max, dst);
}

void mag_thresh(const cv::Mat& src, cv::Mat& dst, const int& sobel_kernel, const int& thresh_min, const int& thresh_max) {
	cv::Mat src_gray, gray_x, gray_y, grad;
	cv::Mat abs_gray_x, abs_gray_y;
	//ת����Ϊ�Ҷ�ͼƬ
	cv::cvtColor(src, src_gray, cv::COLOR_RGB2GRAY);
	//ʹ��cv::Sobel()����x�����y����ĵ�
	cv::Sobel(src_gray, gray_x, CV_64F, 1, 0, sobel_kernel);
	cv::Sobel(src_gray, gray_y, CV_64F, 0, 1, sobel_kernel);
	//ת����CV_8U
	cv::convertScaleAbs(gray_x, abs_gray_x);
	cv::convertScaleAbs(gray_y, abs_gray_y);
	//�ϲ�x��y������ݶ�
	cv::addWeighted(abs_gray_x, 0.5, abs_gray_y, 0.5, 0, grad);
	//��ֵ��
	cv::inRange(grad, thresh_min, thresh_max, dst);
}

/************************************************************************************/





int main(void)
{
	VideoCapture Capture("./project_video.mp4");
	Mat Frame, TempImage, ImageDest, LabImage, HlsImage, ThreshImage, ThreshImagez, ThreshImagey, FinalDisp, H, Hinv;
	Mat DispImage = Mat::zeros(720, 1280, CV_8UC1);
	Mat WinSlip = Mat::zeros(720, 1280, CV_8UC1);  //��ʾ������
	Mat absm, mag, hls, luv, lab, dst, persp, blur;

	vector<vector<Point>> Contours;
	vector<Vec4i> Hierarchy;
	vector<Point2f> DestSrc;
	vector<Mat> Channels;

	UserData Data;

	int Array[1280] = { 0 };
	int LeftBaseTemp = 0;
	int RightBaseTemp = 0;
	int LeftBase = 10000, Thresh = 720, RightBase = 10000;
	int Lock = 0;

	double lxPositon = -1, rxPositon = -1, lNum = 0, rNum = 0, Cur = 0.0, distance = 0.0, r = 0.0;
	Point3d ControlPts[40];
	Point3d ControlPtsR[40];
	Point Cura, Curb, Curc;  //���������ڼ������ʰ뾶
	uchar* CurrRowP;


	while (Capture.read(Frame))
	{
		TempImage = Frame.clone();
		Data.Image = TempImage;

		//��ֵ����
		abs_sobel_thresh(Frame, absm, 'x', 55, 200);
		mag_thresh(Frame, mag, 3, 45, 150);
		hls_select(Frame, hls, 's', 120, 255);
		luv_select(Frame, luv, 'l', 180, 255);
		TempImage = (absm & mag & luv) | (hls & luv);  //��ֵ��������ҳ����ߺϲ�
		Perspective(TempImage, ThreshImage, Frame.cols, Frame.rows);  //͸�ӱ任
		//���ͼ����ʾ
		ImageDest = Mat::zeros(Frame.size(), CV_8UC3);
		WinSlip = Mat::zeros(720, 1280, CV_8UC1);
		Mat Test = Mat::zeros(720, 1280, CV_8UC1);


		//ͳ��ÿ�а׵�ĸ���
		if (0 == Lock)  //��һ�ν���
		{
			Lock = 1; //ֻ�ڼ����һ֡ͼ��ʱʹ��
			for (int i = 0; i < 1280; i++) //ͳ����������
			{
				Array[i] = 0;
			}
			for (int row = 0; row < Frame.rows; row++)     //ɨ����
			{
				for (int col = 0; col < Frame.cols; col++) //ɨ����
				{
					if (ThreshImage.at<uchar>(row, col) == 255) //ͳ��ÿ�а׵�ĸ���
					{
						Array[col]++;
					}
				}
			}

			for (int col = 0; col < Frame.cols; col++) //ɨ����
			{
				line(DispImage, Point(col, 720 - Array[col]), Point(col + 1, 720 - Array[col + 1]), Scalar(255), 2, LINE_8); //����ֱ��ͼ
			}

			//����ֱ��ͼ��ֵ��Ӧ���У�����¼����
			for (int row = 0; row < Frame.rows; row++)
			{
				for (int col = 0; col < Frame.cols / 2; col++)
				{
					if (DispImage.at<uchar>(row, col) == 255)
					{
						if (row < Thresh)
						{
							Thresh = row;
							LeftBase = col;
						}
					}
				}
			}

			Thresh = 720;  //���¸���һ��Ĭ��ֵ,ȥ����һ�γ���ֵ������Ӱ��
			for (int row = 0; row < Frame.rows; row++)
			{
				for (int col = Frame.cols / 2; col < Frame.cols; col++)
				{
					if (DispImage.at<uchar>(row, col) == 255)
					{
						if (row < Thresh)
						{
							Thresh = row;
							RightBase = col;
						}
					}
				}
			}

			ControlPts[0].x = LeftBase;  //��һ֡ͼ������ֱ��ͼ�ҵ��󳵵��ߵĴ���λ��
			ControlPts[0].y = 720;
			ControlPts[0].z = 0;

			ControlPtsR[0].x = RightBase;//��һ֡ͼ������ֱ��ͼ�ҵ��ҳ����ߵĴ���λ��
			ControlPtsR[0].y = 720;
			ControlPtsR[0].z = 0;
		}
		else
		{
			ControlPts[0].x = LeftBaseTemp; //����һ֡��,�Ժ�ÿ֡ͼ������·��Ļ������Ļ�׼�������һ֡ͼ�����·��ĵڶ����������Ļ�׼��Ϊ׼����Ϊÿ��֮֡�䳵���߲���ͻ�䣬���������󣬼�⳵���߸����ȶ���
			ControlPts[0].y = 720;
			ControlPts[0].z = 0;

			ControlPtsR[0].x = RightBaseTemp;
			ControlPtsR[0].y = 720;
			ControlPtsR[0].z = 0;

			LeftBase = LeftBaseTemp;
			RightBase = RightBaseTemp;
			line(WinSlip, Point(ControlPts[0].x, ControlPts[0].y), Point(ControlPtsR[0].x, ControlPtsR[0].y), Scalar(255), 2, LINE_AA);
		}
			cv::cvtColor(ThreshImage, ThreshImage, COLOR_GRAY2BGR); //��ֵ��ͼ��ת��Ϊ3ͨ��

			for (int i = 0; i < 12; i++)
			{
				for (int WinRow = 720 - 60 * (i + 1); WinRow < 720 - 60 * (i); WinRow++)
				{
					CurrRowP = ThreshImage.ptr<uchar>(WinRow);
					CurrRowP += ((LeftBase - 75) * 3);  //ָ�򴰿�����
					for (int lWinCol = LeftBase - 75; lWinCol < LeftBase + 75; lWinCol++)
					{
						if (((*CurrRowP) != 0) || ((*(CurrRowP + 1)) != 0) || ((*(CurrRowP + 2)) != 0))
						{
							lxPositon += lWinCol;
							lNum++;
						}
						CurrRowP += 3;
					}


					CurrRowP = ThreshImage.ptr<uchar>(WinRow) + ((RightBase - 75) * 3);  //ָ�򴰿�����
					for (int rWinCol = RightBase - 75; rWinCol < RightBase + 75; rWinCol++)
					{
						if (((*CurrRowP) != 0) || ((*(CurrRowP + 1)) != 0) || ((*(CurrRowP + 2)) != 0))
						{
							rxPositon += rWinCol;
							rNum++;
						}
						CurrRowP += 3;
					}
				}
				//���ƻ�����
				if (lNum > 0)
				{
					LeftBase = (lxPositon / lNum);
					lNum = 0;
					lxPositon = 0;
				}

				if (rNum > 0)
				{
					RightBase = (rxPositon / rNum);
					rNum = 0;
					rxPositon = 0;
				}

				//�������������������ѡ��
				if (0 == i)  //�����֡ͼƬ�ĵ����ڶ���������������������Xֵ
				{
					LeftBaseTemp = LeftBase;
					RightBaseTemp = RightBase;
				}
				if (i == 4)
				{
					ControlPts[1].x = LeftBase;
					ControlPts[1].y = (720 - 60 * (i + 2));
					ControlPts[1].z = 0;

					ControlPtsR[1].x = RightBase;
					ControlPtsR[1].y = (720 - 60 * (i + 2));
					ControlPtsR[1].z = 0;
				}
				if (i == 7)
				{
					ControlPts[2].x = LeftBase;
					ControlPts[2].y = (720 - 60 * (i + 2));
					ControlPts[2].z = 0;

					ControlPtsR[2].x = RightBase;
					ControlPtsR[2].y = (720 - 60 * (i + 2));
					ControlPtsR[2].z = 0;
				}
				if (i == 10)
				{
					ControlPts[3].x = LeftBase;
					ControlPts[3].y = (720 - 60 * (i + 2));
					ControlPts[3].z = 0;

					ControlPtsR[3].x = RightBase;
					ControlPtsR[3].y = (720 - 60 * (i + 2));
					ControlPtsR[3].z = 0;
					line(WinSlip, Point(ControlPts[3].x, ControlPts[3].y), Point(ControlPtsR[3].x, ControlPtsR[3].y), Scalar(255), 2, LINE_AA);
				}
			}

			DrawBezier(WinSlip, ControlPts);
			DrawBezier(WinSlip, ControlPtsR);
			//��������
			cv::findContours(WinSlip, Contours, Hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
			cv::cvtColor(WinSlip, WinSlip, COLOR_GRAY2BGR);
			for (int i = 0; i < Contours.size(); i++)
			{
				drawContours(WinSlip, Contours, i, Scalar(0, 255, 0), -1, LINE_8);
			}
			inverse_Perspective(WinSlip, TempImage, Frame.cols, Frame.rows);  //͸�ӱ任
			//��������
			Cura.x = (ControlPts[0].x + ControlPtsR[0].x) / 2;
			Cura.y = (ControlPts[0].y + ControlPtsR[0].y) / 2;
			Curb.x = (ControlPts[2].x + ControlPtsR[2].x) / 2;
			Curb.y = (ControlPts[2].y + ControlPtsR[2].y) / 2;
			Curc.x = (ControlPts[3].x + ControlPtsR[3].x) / 2;
			Curc.y = (ControlPts[3].y + ControlPtsR[3].y) / 2;
			Cur = Curvature(Cura, Curb, Curc);
			cv::putText(Frame, format("Cur:%f m", Cur), Point(20, 100), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2, LINE_8);
			
			distance = (Curb.x - Frame.cols / 2) * 3.75 / (ControlPtsR[3].x - ControlPts[3].x);
			cv::putText(Frame, format("Distance from center:%f m", distance), Point(20, 200), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(255, 255, 255), 2, LINE_8);
			//*/
			FinalDisp = Frame * 0.8 + TempImage * 0.2;
			
			cv::imshow("0", FinalDisp);
			//cv::imshow("Lane Line Detection", FinalDisp);
			cv::waitKey(1);
		}
		return 1;
	}
