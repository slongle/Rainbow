#ifndef __RAINBOW_H
#define __RAINBOW_H

#include<iostream>
#include<iomanip>
#include<vector>
#include<map>
#include<memory>
#include<functional>
#include<assert.h>
#include<fstream>

using std::cin;
using std::cout;
using std::endl;

#include "timer.h"
#include "logger.h"
#include "memory.h"

//#ifndef NDEBUG

#define Log(message) \
    do { \
		(tfm::format)(std::cerr,"%s", message); \
	} while(0)

#define Assert(cond, explanation) \
    do { \
		if (!(cond)) std::cerr<<std::endl<<"Assertion occoured at "<<std::endl<<__FUNCTION__<<" "<<__FILE__<<" "<<__LINE__<<" ( "<<explanation<<" )", throw std::runtime_error(explanation); \
	} while (0)

//#endif // NDEBUG

#endif // !__RAINBOW_H
