#include "error.h"
#include "common.h"

RAINBOW_NAMESPACE_BEGIN

/* Print error message and halt program*/
void Error(const std::string &error) {
	tfm::format(std::cerr, "%s\n", error);
	exit(0);
}

RAINBOW_NAMESPACE_END