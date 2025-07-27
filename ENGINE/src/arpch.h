#pragma once

#include <iostream>
#include <stdexcept>
#include <memory>
#include <utility>
#include <numeric>
#include <algorithm>
#include <functional>
#include <core/stb_image.h>
#include <entt/entt.hpp>

#include "core/Log.h"
#include "core/GLDebug.h"

// Data structures
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "matrix_types.h"
#include "vector_types.h"
#include "quaternion.h"

// macros
#define BIT(x) (1 << x)

#ifdef AR_ENABLE_ASSERTS
	#define AR_ASSERT(x, ...) { if(!(x)) {AR_ERROR("Assertion failed ({0}, line {1}) : {2}", __FILE__, __LINE__, __VA_ARGS__); __debugbreak(); } }
#else
	#define AR_ASSERT(x, ...)
#endif

#define AR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
