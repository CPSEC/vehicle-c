#include "camera.h"
#include "car.h"
#include "image.h"
#include <iostream>
using namespace std;

Camera::Camera(Car*car,bool is_thread) : Part(car,is_thread) {
    //
    name_ = "Camera thread";
    car_ = car;
    is_thread_ = true;
    
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


    raspicam::RaspiCam_Cv Camera;
    //processCommandLine ( argc,argv,Camera );
    cout<<"Connecting to camera"<<endl;
//    if ( !Camera.open() ) {
//        cerr<<"Error opening camera"<<endl;
//        return -1;
//    }
    cout<<"Connected to camera ="<<Camera.getId() <<endl;
    //cv::Mat image;

    //wait a while until camera stabilizes
    cout<<"Sleeping for 3 secs"<<endl;


    Camera.set (cv::CAP_PROP_FPS, 120);

    double time_=cv::getTickCount();

}

void Camera::Run(int test) {
    //
    int times = 1;
    chrono::system_clock::time_point start;
    chrono::system_clock::time_point end;


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


//    raspicam::RaspiCam_Cv Camera;
//    processCommandLine ( argc,argv,Camera );
//    cout<<"Connecting to camera"<<endl;
//    if ( !Camera.open() ) {
//        cerr<<"Error opening camera"<<endl;
//        return -1;
//    }
//    cout<<"Connected to camera ="<<Camera.getId() <<endl;
//    //cv::Mat image;
//
//    //wait a while until camera stabilizes
//    cout<<"Sleeping for 3 secs"<<endl;
//
//
//    Camera.set (cv::CAP_PROP_FPS, 120);
//
//    double time_=cv::getTickCount();
//

    while(1)
        //while (Capture.read(Frame))
    {
        Camera.grab();
        Camera.retrieve (Frame);
        Camera.release();

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


        cout<<"Curve: " << Cur << endl;
        //cv::imshow("0", FinalDisp);
        //cv::imshow("Lane Line Detection", FinalDisp);
        //cv::waitKey(1);
    }
  
    
    
    
//    //____________________________________
//    
//    
    
//
    while (times) {
        start = chrono::system_clock::now();
        cout << test << endl;
        car_->test+=1;
        this_thread::sleep_for(chrono::seconds(1));
        end = chrono::system_clock::now();
        cout << chrono::duration_cast<chrono::milliseconds>(end - start).count()
             << endl;
        //test
        cout << "target_direction: " << car_->target_direction << endl;

//        if (car_->target_direction == 2){
//            car_->target_direction = 1.0;
//        }
//        else{
//            if (car_->target_direction == 1.5){
//                car_->target_direction = 2.0;
//            }
//
//            if (car_->target_direction == 1){
//                car_->target_direction = 2.0;
//            }
//        }
    }
}

void Camera::StartSeq() {
    //
}

void Camera::StartThread() {
    //
    cout << name_ << endl;
    thread_ = new thread(&Camera::Run, this, 123);
    thread_->detach();
}

Camera::~Camera() {
    //
}
