#include <bits/stdc++.h> 
#include <iostream>
#include <cstdlib>
#include <chrono>
using namespace std::chrono; 
#include "as5048a.h"
using namespace std; 

int main(){
    //clock_t start, end;
    int result1 = 0;
    int result2 = 0;
    int difference = 0;
    int difference_total = 0;
    double time_taken = 0.0;
    double time_total = 0.0;
    double speed = 0.0;
    
    
    /*!< 65536 = 3.814697260kHz on Rpi2, 6.1035156kHz on RPI3 */
    /*!< 32768 = 7.629394531kHz on Rpi2, 12.20703125kHz on RPI3 */
    /*!< 16384 = 15.25878906kHz on Rpi2, 24.4140625kHz on RPI3 */
    /*!< 8192 = 30.51757813kHz on Rpi2, 48.828125kHz on RPI3 */
    /*!< 4096 = 61.03515625kHz on Rpi2, 97.65625kHz on RPI3 */
    /*!< 2048 = 122.0703125kHz on Rpi2, 195.3125kHz on RPI3 */
    /*!< 1024 = 244.140625kHz on Rpi2, 390.625kHz on RPI3 */
    /*!< 512 = 488.28125kHz on Rpi2, 781.25kHz on RPI3 */
    /*!< 256 = 976.5625kHz on Rpi2, 1.5625MHz on RPI3 */
    /*!< 128 = 1.953125MHz on Rpi2, 3.125MHz on RPI3 */
    /*!< 64 = 3.90625MHz on Rpi2, 6.250MHz on RPI3 */
    /*!< 32 = 7.8125MHz on Rpi2, 12.5MHz on RPI3 */
    /*!< 16 = 15.625MHz on Rpi2, 25MHz on RPI3 */
    /*!< 8 = 31.25MHz on Rpi2, 50MHz on RPI3 */
    /*!< 4 = 62.5MHz on Rpi2, 100MHz on RPI3. Dont expect this speed to work reliably. */
    /*!< 2 = 125MHz on Rpi2, 200MHz on RPI3, fastest you can get. Dont expect this speed to work reliably.*/
    /*!< 1 = 3.814697260kHz on Rpi2, 6.1035156kHz on RPI3, same as 0/65536 */
    //128 tested, works fine, by default use 128
    
    As5048a chalega_kya(0,128);
    
    //Clear any Error on initialization
    result1 = chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
    result2 = chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
    sleep(1);
    
    //start read loop
    while(1){
    	difference_total = 0;
    	time_total = 0.0;
    	
	for(int i = 1; i <= 8; i++){
		//get current position for the first time, record the current time in start
		result1 = chalega_kya.Read(AS5048_CMD_ANGLE);
		auto start = high_resolution_clock::now();

		//get current position for the second time, record the current time in end
		result2 = chalega_kya.Read(AS5048_CMD_ANGLE);
		auto stop = high_resolution_clock::now();

		//filter negative results
		if(result1 < 0 || result2 < 0){
			i--;
			continue;
		}
		
		//get duration in microseconds
		auto duration = duration_cast<microseconds>(stop - start); 
		
		//time difference in second
		time_taken = double(duration.count())/1000000.0;

		//filter missed deadline result
		if(time_taken > 0.0025 || time_taken < 0.0){
          	i--;
          	continue;
          }
          //cout << "time taken: "<<time_taken << endl;


		//inverse degree difference
		difference = (result2-result1)*(-1);
		//cout<<"difference before modify: "<< difference << endl;

		//resolve passing 0 degree point
		if(difference < -10467) difference += 16383;
          else if(difference > 10467) difference -= 16383;

          //cout<< "difference after modify: " << difference << endl;

          //sum degree difference, time difference
		difference_total += difference;
		time_total += time_taken;
	}

	speed = double(difference_total)/16383.0/time_total;
	
	
        
        if(result1 == -1 || result2 == -1){
            //Clear Error
            result1 = chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
            result2 = chalega_kya.Read(AS5048_CMD_CLEAR_ERROR);
        }
        else{
            cout << "Speed: " << speed << " turn/second"<<endl;
            //sleep 
            usleep(50000);
            
        }

        
    }
    return 1;
}
