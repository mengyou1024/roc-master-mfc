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

#include <bitset>
#include <cmath>
#include <functional>
#include <memory>
#include <stack>
#include <tuple>
#include <vector>

using std::bitset;
using std::invoke;
using std::make_shared;
using std::make_unique;
using std::max;
using std::min;
using std::pair;
using std::shared_ptr;
using std::stack;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

// OpenEW
#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib, "glew32s.lib")

// GL
#pragma comment(lib, "opengl32.lib")

// GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Thread.h"

// Duilib
#include "Define.h"
#include "DuiWindowBase.h"
#include "ToolFunc.h"
using ::CPoint;
using ::CSize;

#include <Version.h>
#include <curl/curl.h>
#include <json/json.h>
#include <spdlog/spdlog.h>
#include <sqlite_orm.h>

#ifndef DB_UNNAMED_GROUP
    #define DB_UNNAMED_GROUP "Î´ÃüÃû"
#endif

#ifndef DB_UNNAMED_USER
    #define DB_UNNAMED_USER "Î´ÃüÃû"
#endif

#ifndef SCAN_DATA_DIR_NAME
    #define SCAN_DATA_DIR_NAME "Data/"
#endif // !SCAN_DATA_DIR_NAME

#endif // PCH_H
