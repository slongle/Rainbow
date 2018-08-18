#include "error.h"
#include "common.h"

RAINBOW_NAMESPACE_BEGIN


void Error(const std::string &st) {
	std::cerr << st << std::endl;
	exit(0);
}

RAINBOW_NAMESPACE_END