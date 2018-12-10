#ifndef __TIMER_H
#define __TIMER_H

#include <iostream>
#include <chrono>

class Timer {
public:
    Timer() {
        Start();
    }

    void Start() {
        StartTime = std::chrono::system_clock::now();
    }

    void Finish() {
        FinishTime = std::chrono::system_clock::now();
    }

    void ShowTime() {
        Finish();
        int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(FinishTime - StartTime).count();
        int seconds = (milliseconds / 1000) % 60;
        int minutes = (milliseconds / 60000) % 60;
        int hours = milliseconds / 3600000;
        milliseconds %= 1000;

        std::cout << hours << " hours, " << minutes << " minutes, " << seconds << " seconds, "<<milliseconds<<" milliseconds\n";
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

private:
    std::chrono::system_clock::time_point StartTime, FinishTime;
};

#endif // !__TIMER_H
