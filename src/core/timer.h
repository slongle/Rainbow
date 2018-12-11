#ifndef __TIMER_H
#define __TIMER_H

#include <iostream>
#include <chrono>

#include "ext/tinyformat/tinyformat.h"

class Timer {
public:
    Timer() {
        pauseTime = 0;
        pause = false;
        Start();
    }

    void Start() {
        StartTime = std::chrono::system_clock::now();
    }

    void Finish() {
        FinishTime = std::chrono::system_clock::now();
    }

    std::string LastTime() {
        if (!pause)
            Finish();
        milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(FinishTime - StartTime).count() - pauseTime;
        seconds = (milliseconds / 1000) % 60;
        minutes = (milliseconds / 60000) % 60;
        hours = milliseconds / 3600000;
        milliseconds %= 1000;

        return toString();        
        /* 
        if (hours != 0) {
            std::cout << hours << " hours, " << minutes << " minutes, " << seconds << " seconds\n";
        }
        else if (minutes != 0) {
            std::cout << minutes << " minutes, " << seconds << " seconds\n";
        }
        else if (seconds != 0) {
            std::cout << seconds << " seconds\n";
        }*/
    }



    std::string toString() {        
        return tfm::format("%dh %dm %ds", hours, minutes, seconds);
        return tfm::format("%dh %dm %ds %dms", hours, minutes, seconds, milliseconds);
    }

    void Stop() {
        Finish();
        pause = true;
    }

    void Continue() {
        pause = false;
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        pauseTime += milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - FinishTime).count();
    }

private:
    std::chrono::system_clock::time_point StartTime, FinishTime;
    int hours, minutes, seconds, milliseconds;
    
    int pauseTime;
    bool pause;
};

#endif // !__TIMER_H
