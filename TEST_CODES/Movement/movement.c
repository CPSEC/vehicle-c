#include <pca9685.h>
#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 60

int pca_setup(){
    
    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0){
        printf("Error in setup\n");
        return fd;
    }
    pca9685PWMReset(fd);
    
    return 0;
}

int calcTicks(float impulseMs, int hertz)
{
    printf("impulse: %f\n", impulseMs);
    if (impulseMs>2.0 || impulseMs <1.0){
        exit(1);
    }
    else{
        float cycleMs = 1.0/ hertz;
        return (int)(MAX_PWM * impulseMs * 0.001 / cycleMs);
    }
}

int setSpeed(float pulse){
    printf("tick: %d\n",calcTicks(pulse, HERTZ));
    pwmWrite(PIN_BASE + 15, calcTicks(pulse, HERTZ));
    return 0;
}

int setDirect(float pulse){
    printf("tick: %d\n",calcTicks(pulse, HERTZ));
    pwmWrite(PIN_BASE + 11, calcTicks(pulse, HERTZ));
    return 0;
}

int main(void){
    wiringPiSetup();
    pca_setup();
    float x;
    while(1){
        scanf("%f", &x);
        setSpeed(x);
    }
}
