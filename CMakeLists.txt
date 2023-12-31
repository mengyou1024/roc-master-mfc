cmake_minimum_required(VERSION 3.16)

project(Roc-Master)

find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags
        OUTPUT_VARIABLE APP_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    if(NOT APP_VERSION)
        message(FATAL_ERROR "Git repository must have a tag , use `git tag <tag_name> -m <tag_message>` to create a tag.\n"
            "\te.g.: `git tag v0.0.1 -m \"init\"`\n"
            "the git describe is use for varible `APP_VERSION`"
        )
    else()
        message(STATUS "APP VERSION:" ${APP_VERSION})
    endif()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} remote
        OUTPUT_VARIABLE GIT_REMOTE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} remote get-url ${GIT_REMOTE}
        OUTPUT_VARIABLE GIT_REPOSITORY_URL
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    unset(GIT_REMOTE)
    message(STATUS "GIT_REPOSITORY_URL:${GIT_REPOSITORY_URL}")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} config user.name
        OUTPUT_VARIABLE GIT_USER_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    message(STATUS "GIT_USER_NAME:${GIT_USER_NAME}")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} config user.email
        OUTPUT_VARIABLE GIT_USER_EMAIL
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    message(STATUS "GIT_USER_EMAIL:${GIT_USER_EMAIL}")
else()
    message(WARNING "no git found, please install git: https://git-scm.com/")
endif()

# 搜索inno setup工具
find_program(ISCC_PATH ISCC)

if(ISCC_PATH)
    message(STATUS "Detected ISCC_PATH: ${ISCC_PATH}")
else(ISCC_PATH)
    message(WARNING "no ISCC path found, please install inno setup and add it to path\n see: https://jrsoftware.org/isinfo.php")
endif(ISCC_PATH)

file(GLOB _PACK_DLL_LIST ${CMAKE_SOURCE_DIR}/x64/Release/*.dll)
set(PACK_DLL_LIST)
foreach(PACK_DLL ${_PACK_DLL_LIST})
    set(PACK_DLL_LIST "${PACK_DLL_LIST}Source: \"${PACK_DLL}\"; DestDir: \"{app}\"\n")
endforeach(PACK_DLL ${_PACK_DLL_LIST})

configure_file(
    script/pack-installer.iss.in
    ${CMAKE_SOURCE_DIR}/pack-installer.iss
    @ONLY
)

configure_file(
    ProjectConfig.h.in
    ${CMAKE_SOURCE_DIR}/Mutilple/ProjectConfig.h
    @ONLY
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${CMAKE_SOURCE_DIR}/template"
    "${CMAKE_SOURCE_DIR}/x64/${CMAKE_BUILD_TYPE}/template"
)
