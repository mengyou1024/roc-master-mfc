#include "pch.h"

#include "MainProcess.h"
#include "Mutilple.h"
#include "RuitiePLC.h"

#include <iostream>
#include <Model/UserModel.h>
#include <Model/SystemConfig.h>

using namespace std;

MainProcess::MainProcess() {
    //_CrtSetBreakAlloc(303);
    ORM_Model::User::storage().sync_schema();
    ORM_Model::SystemConfig::storage().sync_schema();

    AllocConsole(); // 控制台
    #if _DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif 
    auto f = freopen("CONOUT$", "w", stdout);
    RuitiePLC::connectTo();
    //_CrtDumpMemoryLeaks();
}

MainProcess::~MainProcess() {
    RuitiePLC::disconnect();
    FreeConsole();
}

void MainProcess::Close() {
    CString strCfgFile;
    // strCfgFile.Format(_T("%sLastParam.ini"), theApp.m_pExePath);
    //   m_Techniques.SaveTec(strCfgFile);

    m_ConnectPLC.Close();
}

void MainProcess::Check(ICheckCarrier *pInitCheck) {
    // 自检，pInitCheck==nullptr时，后台进行检测
    if (pInitCheck) {
        pInitCheck->Begin();
        pInitCheck->SetResult(CHECK::DIR, CheckDir());
        pInitCheck->SetResult(CHECK::CALIB, CheckCalib());
        pInitCheck->SetResult(CHECK::SCAN, CheckScan());
        pInitCheck->SetResult(CHECK::ANALYSIS, CheckAnalysis());
        pInitCheck->SetResult(CHECK::PLC, CheckPLC());
        pInitCheck->End();
    } else {
        CheckDir();
        CheckCalib();
        CheckScan();
        CheckAnalysis();
    }
}

bool MainProcess::CheckDir() {
    CString strCfgFile;
    strCfgFile.Format(_T("%sLastParam.ini"), theApp.m_pExePath);
    m_Techniques.LoadTec(strCfgFile);

    // 显示效果
    Sleep(200);

    return true;
}

bool MainProcess::CheckCalib() {
    char p0[32] = {0}, p1[32] = {0};
    // if (m_HDBridge.IsOpened())
    //{
    //     m_HDBridge.Close();
    // }

    if (!m_HDBridge.Open(p0, {}, p1, {})) {
        return false;
    }
    //   Sleep(100);
    ////   UCHAR* pVersion = UNION_PORT_GetVersion(g_MainProcess.m_HDBridge.m_iNetwork);
    //   if (pVersion[0] == 0) {
    //       return false;
    //   }
    // 显示效果
    Sleep(200);

    return m_HDBridge.IsOpened();
}

bool MainProcess::CheckScan() {
    // 显示效果
    Sleep(200);

    return true;
}
bool MainProcess::CheckPLC() {
    bool res = m_ConnectPLC.isConnected();
    // 显示效果
    Sleep(200);

    return res;
}
bool MainProcess::CheckAnalysis() {
    // 显示效果
    Sleep(200);

    return true;
}

void MainProcess::StartScan() {
    m_ConnectPLC.Start();
    m_Techniques.Start();
}

void MainProcess::StopScan() {
    m_Techniques.Stop();
    m_ConnectPLC.Stop();
}

int MainProcess::GetChannelIndex() {
    return m_Techniques.m_iChannel;
}

Channel *MainProcess::GetCurChannel() {
    return &m_Techniques.m_pChannel[m_Techniques.m_iChannel];
}

Channel *MainProcess::GetChannel(int index) {
    return &m_Techniques.m_pChannel[index];
}
