#pragma once
#include "core/Log.h"

#ifdef AR_ENABLE_ASSERTS
#define AR_ASSERT(x, ...) { if(!(x)) {AR_ERROR("Assertion failed ({0}, line {1}) : {2}", __FILE__, __LINE__, __VA_ARGS__); __debugbreak(); } }
#else
#define AR_ASSERT(x, ...)
#endif
