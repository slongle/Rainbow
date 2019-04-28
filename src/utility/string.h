#pragma once
#ifndef __UTILITY_STRING_H
#define __UTILITY_STRING_H

#include <string>

namespace utility {
    inline std::string GetExtension(const std::string& name) {
        return name.substr(name.find_last_of('.') + 1);
    }
}

#endif //__UTILITY_STRING_H