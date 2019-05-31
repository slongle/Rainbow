#pragma once
#ifndef __RAINBOW_STATISTIC_H
#define __RAINBOW_STATISTIC_H

#include <iostream>

class Statistic {
public:
    Statistic(){}

    void ShowStatistic(FILE* file) {
        fprintf(file, "\rZero Bounce Rate : %f%%\n", 100 * static_cast<double>(zeroPath) / pathNum);
        fprintf(file, "\rSum Bounce : %lu\n", sumBounce);
        fprintf(file, "\rAverage Bounce : %f\n", static_cast<double>(sumBounce) / pathNum);
        fprintf(file, "\rMax Bounce : %lu\n", maxBounce);
        fprintf(file, "\rMin Bounce : %lu\n", minBounce);
        //std::cout << "Zero Bounce Rate : " << 100 * static_cast<double>(zeroPath) / pathNum << "%" << std::endl;
        //std::cout << "Sum Bounce : " << sumBounce << std::endl;
        //std::cout << "Average Bounce : " << static_cast<double>(sumBounce) / pathNum << std::endl;
        //std::cout << "Max Bounce : " << maxBounce << std::endl;
        //std::cout << "Min Bounce : " << minBounce << std::endl;
    }

    unsigned long long zeroPath = 0;
    unsigned long long pathNum = 0;
    unsigned long long sumBounce = 0;
    unsigned long long maxBounce = 0;
    unsigned long long minBounce = 10000;
};

#endif //__RAINBOW_STATISTIC_H