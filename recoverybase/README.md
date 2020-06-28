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

1. save a compulsive checkpoint after each cycle of each part
2. save a unit checkpoint when now-last > average time / 2
