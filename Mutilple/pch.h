// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H
#define _CRT_SECURE_NO_DEPRECATE 
// add headers that you want to pre-compile here
#include "framework.h"

#include <vector>
#include <memory>
#include <cmath>
#include <stack>
#include <bitset>
#include <functional>

using std::vector;
using std::string;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::max;
using std::min;
using std::pair;
using std::stack;
using std::bitset;
using std::invoke;

//OpenEW
#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib, "glew32s.lib")

// GL
#pragma comment (lib, "opengl32.lib")

//GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Thread.h"

//Duilib
#include "Define.h" 
#include "ToolFunc.h"
#include "DuiWindowBase.h"
#include "Object.h"
#endif //PCH_H

using ::CPoint;
using ::CSize;

#include <sqlite_orm.h>
#include <spdlog/spdlog.h>

