#pragma once
#ifndef __RAINBOW_STATISTIC_H
#define __RAINBOW_STATISTIC_H

#include <iostream>

class Statistic {
public:
    Statistic(){}

    void ShowStatistic() {
        std::cout << "Average Bounce : " << static_cast<double>(pathBounce) / pathNum << std::endl;
    }

    unsigned long long pathNum = 0;
    unsigned long long pathBounce = 0;
};

#endif //__RAINBOW_STATISTIC_H