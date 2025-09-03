#pragma once

#include "core/Core.h"
#include "core/GLDebug.h"

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
#include "LogUtils.h"

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

#ifndef AR_GL_CHECK
#define AR_GL_CHECK() { \
    std::string error = CheckGLErrors(__FILE__, __LINE__); \
    if (!error.empty()) { \
        AR_ERROR("OpenGL Error: {0}", error);\
        __debugbreak();\
    } \
}
#endif

#ifndef AR_GL_FBO_CHECK
#define AR_GL_FBO_CHECK(fbo) { \
    std::string fboError = CheckGLFramebufferErrors(fbo); \
    if (!fboError.empty()) { \
        AR_ERROR("Framebuffer error: {0}", fboError);\
        __debugbreak();\
    } \
}
#endif

#define AR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
