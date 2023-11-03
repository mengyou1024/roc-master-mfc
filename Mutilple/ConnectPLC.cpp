#include "pch.h"

#include "ConnectPLC.h"
#include "RuitiePLC.h"

ConnectPLC::ConnectPLC() {
    if (m_ThreadPLCStatus.IsNull())
        m_ThreadPLCStatus.Create(&ConnectPLC::_PLCStatus, this);
    int size = (1104 - 1000) / 4 + 1;
}
ConnectPLC::~ConnectPLC() {
    m_ThreadPLCStatus.Close();
}

bool ConnectPLC::isConnected() {
    return RuitiePLC::isConnected();
}

void ConnectPLC::Close() {
    m_ThreadRead.Close();
}

/**自动下发命令参数 踏面移动(右轴)
fPosX1    VD 1060
fPosX2    VD 1064
fPosY1     VD1080
fPosY2     VD1084
fPosStep  VD1076
*/
void ConnectPLC::SetPLC_TreadMove(float fPosX1, float fPosX2, float fPosY1, float fPosY2, float fPosStep) {
    RuitiePLC::setVariable("V1060", fPosX1);   // 右水平轴X轮缘外
    RuitiePLC::setVariable("V1064", fPosX2);   // 右水平轴X轮缘内
    RuitiePLC::setVariable("V1080", fPosY1);   // 右上下轴Y开始
    RuitiePLC::setVariable("V1084", fPosY2);   // 右上下轴Y结束
    RuitiePLC::setVariable("V1076", fPosStep); // 右Y步进
}

/**自动下发命令参数 侧面移动(左轴)
fPosX1    VD 1020
fPosX2    VD 1024
fPosY1    VD 1040
fPosStep  VD1016
*/
void ConnectPLC::SetPLC_SideMove(float fPosX1, float fPosX2, float fPosY1, float fPosStep) {
    RuitiePLC::setVariable("V1020", fPosX1); // 左水平轴一次定位位置
    RuitiePLC::setVariable("V1024", fPosX2); // 左水平轴二次定位位置
    RuitiePLC::setVariable("V1040", fPosY1); // 左上下轴一次定位位置

    RuitiePLC::setVariable("V1016", fPosStep); // 左上下轴增量位置SV
}

// 自动开始
bool ConnectPLC::SetPLCAutoStart() {
    return RuitiePLC::setVariable("M10", true);
}

// 自动状态
bool ConnectPLC::GetPLCAutoState() {
    return RuitiePLC::getVariable("M11", 0);
}

/**缺陷命令参数下发
fTreadY        VD1096

fWheelAngle  VD1104
*/
void ConnectPLC::SetPLC_DetectMove(float fSideX, float fTreadY, float fWheelAngle) {
    RuitiePLC::setVariable("V1096", fSideX); // 左X

    RuitiePLC::setVariable("V1100", fTreadY);     // 右Y
    RuitiePLC::setVariable("V1104", fWheelAngle); // 卡盘还原角度
}

int ConnectPLC::GetDetectState() {
    return RuitiePLC::getVariable("M90", true); // 缺陷移动到起始位置
}

void ConnectPLC::SetAllZero() {
    RuitiePLC::setVariable("M00", false); // 复位急停 在回原点

    RuitiePLC::setVariable("M06", true); // 一键原点
}

bool ConnectPLC::SetPLCStop() {
    SetPLCAuto(false);
    return RuitiePLC::setVariable("M00", true); // 急停
}

bool ConnectPLC::SetPLCAuto(bool bAuto) {
    m_bPLCAuto = bAuto;

    RuitiePLC::setVariable("M01", m_bPLCAuto); // 手自切换
    // 下发状态
    return true;
}
bool ConnectPLC::GetPLCAuto() {
    m_bPLCAuto = RuitiePLC::getVariable("M01", 0); // 手自切换
    // 读取状态
    return m_bPLCAuto;
}

bool ConnectPLC::Start() // 开始扫差
{
    // 下发相关参数
    m_ThreadPLCStatus.Close();

    while (m_ThreadPLCStatus.m_bWorking) {
    }
    SetCoder(0);
    if (m_ThreadRead.IsNull())
        m_ThreadRead.Create(&ConnectPLC::_Read, this);
    return true;
}

bool ConnectPLC::Stop() {
    m_ThreadRead.Close(); // 扫差线程停止

    // 探伤结束时，扫查架回位
    SetCoder(0);

    // if (m_ThreadPLCStatus.IsNull()) //开启PLC状态线程
    //     m_ThreadPLCStatus.Create(&ConnectPLC::_PLCStatus, this);
    return true;
}

// 100mm 读取一次PLC状态
void ConnectPLC::_PLCStatus() {
    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency(&nFrequency);
    LARGE_INTEGER time_start; /*开始时间*/
    QueryPerformanceCounter(&time_start);

    while (m_ThreadPLCStatus.m_bWorking) {
        LARGE_INTEGER time_end; /*开始时间*/
        QueryPerformanceCounter(&time_end);
        // 根据时间生成一组虚拟角度
        static int i = 0;
        // float fAngle = float(((time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart) / 300 % 3600) / 10.0f;// /1 调节扫差的快慢
        //  printf("_PLCStatus:%.2f-%Id\n", fAngle,( (time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart));
        //   SetCoder(fAngle);

        Sleep(500);
    }
}
void ConnectPLC::_Read() {
    INT64 iDebugTime = GetTickCount64();

    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency(&nFrequency);
    LARGE_INTEGER time_start; /*开始时间*/
    QueryPerformanceCounter(&time_start);
    while (m_ThreadRead.m_bWorking) {
        LARGE_INTEGER time_end; /*开始时间*/
        QueryPerformanceCounter(&time_end);
        // 根据时间生成一组虚拟角度
        static int i = 0;
        // float fAngle = float((GetTickCount64() - iDebugTime) % 3600) / 10.0f;

        if (RuitiePLC::isConnected()) { // PLC连接
            GetAllFloatValue();
            Sleep(1);
        } else {                                                                                                               // 未连接
            float fAngle = float(((time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart) / 4 % 3600) / 10.0f; // /1 调节扫差的快慢

            // printf("Read:%.2f-%Id\n", fAngle,( (time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart));
            SetCoder(fAngle);
            Sleep(20);
        }
    }
}

bool ConnectPLC::LoadTec(LPCTSTR lpName) {
    TCHAR pBuf[256] = {0};

    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideYPos"), _T("10.1"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fSideYPos);
    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideXStart"), _T("50.1"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fSideXStart);
    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideXEnd"), _T("150.2"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fSideXEnd);
    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideXStep"), _T("10.3"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fSideXStep);

    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadXPos"), _T("20.4"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fTreadXPos);
    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadYStart"), _T("60.5"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fTreadYStart);
    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadYEnd"), _T("160.6"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fTreadYEnd);
    GetPrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadYStep"), _T("10.7"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_PlcDownParam.fTreadYStep);

    m_PlcDownParam.nBestCirSize = GetPrivateProfileInt(_T("PLC_SCAN_PAPA"), _T("nBestCirSize"), 5, lpName);
    return true;
}
bool ConnectPLC::SaveTec(LPCTSTR lpName) {
    CString strKey, strInfo;
    strInfo.Format(_T("%f"), m_PlcDownParam.fSideYPos);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideYPos"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fSideXStart);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideXStart"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fSideXEnd);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideXEnd"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fSideXStep);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fSideXStep"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fTreadYStart);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadYStart"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fTreadXPos);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadXPos"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fTreadYEnd);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadYEnd"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_PlcDownParam.fTreadYStep);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("fTreadYStep"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%d"), m_PlcDownParam.nBestCirSize);
    WritePrivateProfileString(_T("PLC_SCAN_PAPA"), _T("nBestCirSize"), strInfo.GetBuffer(0), lpName);

    return true;
}

void ConnectPLC::GetAllFloatValue() {
    // mPLCFloatValue.insert()
    int size = (1104 - 1000) / 4 + 1;

    auto _lSyncGetValue = [this](string name, float val) {
        mPLCFloatValueMap[name] = val;
    };
    RuitiePLC::getVariable(_lSyncGetValue);
    SetCoder(mPLCFloatValueMap["V1000"]);
}

bool ConnectPLC::SetPLC_Speed() {
    if (RuitiePLC::isConnected()) {
        RuitiePLC::setVariable("V1108", mPLCSpeed.fSideXSpeed1);  //
        RuitiePLC::setVariable("V1112", mPLCSpeed.fSideYSpeed1);  //
        RuitiePLC::setVariable("V1116", mPLCSpeed.fTreadXSpeed1); //
        RuitiePLC::setVariable("V1120", mPLCSpeed.fTreadYSpeed1); //
        RuitiePLC::setVariable("V1004", mPLCSpeed.fRotateSpeed1); //

        RuitiePLC::setVariable("V1124", mPLCSpeed.fSideXSpeed2);  //
        RuitiePLC::setVariable("V1128", mPLCSpeed.fSideYSpeed2);  //
        RuitiePLC::setVariable("V1132", mPLCSpeed.fTreadXSpeed2); //
        RuitiePLC::setVariable("V1136", mPLCSpeed.fTreadYSpeed2); //
        return true;
    } else {
        return false;
    }
}

float ConnectPLC::GetPLCSideX() {
    float fValue = mPLCFloatValueMap["V1008"]; // 左x位置
    return fValue;
}
float ConnectPLC::GetPLCSideY() {
    float fValue = mPLCFloatValueMap["V1028"]; // 左Y位置
    return fValue;
}
float ConnectPLC::GetPLCTreadX() {
    float fValue = mPLCFloatValueMap["V1048"]; // 右X平位置
    return fValue;
}
float ConnectPLC::GetPLCTreadY() {
    float fValue = mPLCFloatValueMap["V1068"]; // 左Y平位置
    return fValue;
}
