#include "pca.h"

#include <wiringPi.h>

#include <cstdlib>

#include "../lib/pca9685.h"

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 60

Pca::Pca() {
    wiringPiSetup();

    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    pca9685PWMReset(fd);
    this->setSpeed(1.5);
    this->setDirect(1.5);
}

int Pca::calcTicks(float impulseMs, int hertz) {
    // cout << "impulse: " << impulseMs << endl;
    if (impulseMs > 2.0 || impulseMs < 1.0) {
        exit(1);
    } else {
        float cycleMs = 1.0 / hertz;
        return (int)(MAX_PWM * impulseMs * 0.001 / cycleMs);
    }
}

void Pca::setSpeed(float pulse) {
    // cout << "tick: " << calcTicks(pulse, HERTZ) << endl;
    pwmWrite(PIN_BASE + 15, calcTicks(pulse, HERTZ));
}

void Pca::setDirect(float pulse) {
    // cout << "tick: " << calcTicks(pulse, HERTZ) << endl;
    pwmWrite(PIN_BASE + 11, calcTicks(pulse, HERTZ));
}

void Pca::setRGB(int rv, int gv, int bv) {
    // RED
    pwmWrite(PIN_BASE + 0, rv);
    // GREEN
    pwmWrite(PIN_BASE + 1, gv);
    // BLUE
    pwmWrite(PIN_BASE + 2, bv);
}

void Pca::resetRGB() {
    pwmWrite(PIN_BASE + 0, 0);
    pwmWrite(PIN_BASE + 1, 0);
    pwmWrite(PIN_BASE + 2, 0);
}

Pca::~Pca() {}
