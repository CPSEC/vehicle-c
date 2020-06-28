
/**
*/
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include "raspicam_cv.h"
using namespace std;

int main ( int argc,char **argv ) {
    raspicam::RaspiCam_Cv Camera;
    processCommandLine ( argc,argv,Camera );
    cout<<"Connecting to camera"<<endl;
    if ( !Camera.open() ) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }
    cout<<"Connected to camera ="<<Camera.getId() <<endl;
    
    //wait a while until camera stabilizes
    cout<<"Sleeping for 3 secs"<<endl;
    usleep(3*1000000);
    
    Camera.set (cv::CAP_PROP_FPS, 120);
    
    double time_=cv::getTickCount();
    
    for(int i=0; i<100; i++){
        Camera.grab();
        Camera.retrieve (image);
        cv::imwrite ("image"+std::to_string(i)+".ppm",image);
        Camera.release();
    }
}
//
