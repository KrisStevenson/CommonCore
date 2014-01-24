/* 
The MIT License (MIT)

Copyright (c) 2010-2014 KrisStevenson

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ 


#ifndef TIMER_H
#define TIMER_H

#include <CommonCore.git/GenericDefines.h>

//#if defined(LINUX)
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
//#endif

#ifdef _WIN32
//#include <ctime>
 #include <windows.h>
 #include <winbase.h>
#endif

#ifdef MAC
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#include "stdio.h"

#include <CommonCore.git/GenericDefines.h>

class Timer {
private:

    double totalTime;
    bool started;
    int runs;


#ifdef MAC
    double lastStartTime;

    double getTime() { return mach_absolute_time(); }

    double subtractTimes(uint64_t endTime, uint64_t startTime) {
        uint64_t difference = endTime - startTime;
        static double conversion = 0.0;

        if (conversion == 0.0) {
            mach_timebase_info_data_t info;
            kern_return_t err = mach_timebase_info( &info );

            //Convert the timebase into seconds
            if (err == 0) conversion = 1e-9 * (double) info.numer / (double) info.denom;
        }

        return conversion * (double) difference;
    }
#endif
#ifdef _WIN32
    LARGE_INTEGER lastStartTime;

    LARGE_INTEGER getTime() { LARGE_INTEGER result; QueryPerformanceCounter(&result); return result; }

    double subtractTimes(LARGE_INTEGER endTime, LARGE_INTEGER startTime) {
        LARGE_INTEGER ticksPerSecond;
        if (!QueryPerformanceFrequency(&ticksPerSecond)) printf("\tQueryPerformance not present");

        LARGE_INTEGER cputime;
        cputime.QuadPart = endTime.QuadPart - startTime.QuadPart;

        return ((double) cputime.QuadPart / (double) ticksPerSecond.QuadPart);
    }
#endif
//#ifdef LINUX
    struct timeval lastStartTime;

    struct timeval getTime() { struct timeval result; gettimeofday(&result, NULL); return result; }

    double subtractTimes(struct timeval end, struct timeval start) {
        long seconds, useconds;

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;

        return ((seconds) + 1E-6*useconds);
    }
//#endif

public:
    Timer() : totalTime(0), started(false), runs(0) { }

    void resume() { lastStartTime = getTime(); started = true; }
    void suspend() { addRun(subtractTimes(getTime(), lastStartTime)); started = false; }

    void addRun(double runTime) { totalTime += runTime; runs++; }

    double getTotalTime() { return totalTime; }
    double getAverageTime() { return totalTime/runs; }
    int getRuns() { return runs; }
};

#endif // TIMER_H
