#include "pch.h"

#include "MainProcess.h"
#include "Mutilple.h"
#include "RuitiePLC.h"

#include <HDBridge/TOFDPort.h>
#include <HDBridge/Utils.h>
#include <Model/ScanRecord.h>
#include <Model/SystemConfig.h>
#include <Model/UserModel.h>
#include <Model/DetectInfo.h>
#include <iostream>
#include <spdlog/sinks/rotating_file_sink.h>
#include <curl/curl.h>
#include <filesystem>
#include <duckx.hpp>
#include <rttr/type.h>

namespace fs = std::filesystem;

using namespace std;

MainProcess::MainProcess() {
    TCHAR cPath[_MAX_FNAME];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR ExePath[_MAX_FNAME];
    GetModuleFileName(NULL, cPath, _MAX_FNAME);
    _tsplitpath_s(cPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
    _stprintf_s(ExePath, _T("%s%s"), drive, dir);
    SetCurrentDirectory(ExePath);
    InitStroage();
#if _DEBUG
    // _CrtSetBreakAlloc(1739);
    AllocConsole();
    spdlog::set_level(spdlog::level::debug);
    mFile = freopen("CONOUT$", "w", stdout);
#else 
    spdlog::set_default_logger(spdlog::rotating_logger_st("Mutilple", "log/log.txt", static_cast<size_t>(1024 * 1024 * 5), 5));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    spdlog::flush_on(spdlog::level::warn);
    spdlog::set_level(spdlog::level::info);
#endif
    RuitiePLC::connectTo();
}

MainProcess::~MainProcess() {
    RuitiePLC::disconnect();
#if _DEBUG
    FreeConsole();
    fclose(mFile);
#endif
    spdlog::drop_all();
}

void MainProcess::InitStroage() {
    HD_Utils::storage().sync_schema();
    TOFDUSBPort::storage().sync_schema();
    ORM_Model::User::storage().sync_schema();
    ORM_Model::SystemConfig::storage().sync_schema();
    ORM_Model::ScanRecord::storage().sync_schema();
    ORM_Model::DetectInfo::storage().sync_schema();
}
