# README.md

This project change previous thread model to process model.
The change is caused by that we want to use criu to dump/restore process state to simulate recovery.

## Some key changes are as following

1. use process instead of thread
2. use shared mmory to support communication for different parts(process)
   1. a state class keep every variable which need to share
3. use map<PartType,pid_t> to indicate each part(each part has an unique pid)
4. abstract class part has attribute: PartType, method: Run()
5. each part process will be started at cat Inint();
6. replace Pca into state to do setfunction();
7. only 4 parts left: Camera, ThrottlePID, ServoPID, Speed
8. change to use gettimeofday() rather than chrono
9. RunAll is a wrapper function to call Run() and other utility function

## Simulation

1. we have 2 situation:
   1. need compulsive checkpoint + unit checkpoint
   2. need unit checkpoint only
2. to achieve these two cases we use a MARCO call C to control the situation:
   1. if #define C , means we run for case 1
      1. every part save a compulsive checkpoint after its one cycle
      2. car(manager) do unit checkpoint by time interval -- average run time /2 for each part(process)
   2. else run for case 2
      1. no compulsive checkpoint
      2. unit checkpoint time interval == average run time /4
3. we run key path part -- camera 30 times totally and make fault each 3 times and restore
4. other detials:
   1. we use sleep to simulate process' run time -- camera 40000 servopid 1000 htrottlepid 1000 speed 8000us
   2. data dependency
      1. camera: null
      2. servopid: direction\_ or target_direction_
      3. throttlepid: speed\_ or target_speed_
      4. speed: null
   3. since servopid and throttlepid need output data of camera(target direction/speed) or speed(real direction/speed),the average cycle time of servopid andthrottlepid will be $\min\{camera_{avgtime},speed_{avgtime}\}$

```bash
# config: MAX_TIMES 30 FAULT_STEP 3

# with C
start ...

total run time 1296179 us 1.29618 s
total run time 1395178 us 1.39518 s
total run time 1249326 us 1.24933 s
total run time 1350776 us 1.35078 s
total run time 1293032 us 1.29303 s

#without C
start ...

total run time 1280575 us 1.28058 s
total run time 1369984 us 1.36998 s
total run time 1439444 us 1.43944 s
total run time 1319033 us 1.31903 s
total run time 1258678 us 1.25868 s
```
