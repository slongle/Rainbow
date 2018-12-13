#ifndef __MEMORY_H
#define __MEMORY_H

#include <memory>

#define ALLOCA(TYPE, COUNT) (TYPE*)alloca(COUNT * sizeof(TYPE))

#endif // __MEMORY_H