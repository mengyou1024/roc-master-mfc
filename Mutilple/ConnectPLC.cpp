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

/**�Զ��·�������� ̤���ƶ�(����)
fPosX1    VD 1060
fPosX2    VD 1064
fPosY1     VD1080
fPosY2     VD1084
fPosStep  VD1076
*/
void ConnectPLC::SetPLC_TreadMove(float fPosX1, float fPosX2, float fPosY1, float fPosY2, float fPosStep) {
    RuitiePLC::setVariable("V1060", fPosX1);   // ��ˮƽ��X��Ե��
    RuitiePLC::setVariable("V1064", fPosX2);   // ��ˮƽ��X��Ե��
    RuitiePLC::setVariable("V1080", fPosY1);   // ��������Y��ʼ
    RuitiePLC::setVariable("V1084", fPosY2);   // ��������Y����
    RuitiePLC::setVariable("V1076", fPosStep); // ��Y����
}

/**�Զ��·�������� �����ƶ�(����)
fPosX1    VD 1020
fPosX2    VD 1024
fPosY1    VD 1040
fPosStep  VD1016
*/
void ConnectPLC::SetPLC_SideMove(float fPosX1, float fPosX2, float fPosY1, float fPosStep) {
    RuitiePLC::setVariable("V1020", fPosX1); // ��ˮƽ��һ�ζ�λλ��
    RuitiePLC::setVariable("V1024", fPosX2); // ��ˮƽ����ζ�λλ��
    RuitiePLC::setVariable("V1040", fPosY1); // ��������һ�ζ�λλ��

    RuitiePLC::setVariable("V1016", fPosStep); // ������������λ��SV
}

// �Զ���ʼ
bool ConnectPLC::SetPLCAutoStart() {
    return RuitiePLC::setVariable("M10", true);
}

// �Զ�״̬
bool ConnectPLC::GetPLCAutoState() {
    return RuitiePLC::getVariable("M11", 0);
}

/**ȱ����������·�
fTreadY        VD1096

fWheelAngle  VD1104
*/
void ConnectPLC::SetPLC_DetectMove(float fSideX, float fTreadY, float fWheelAngle) {
    RuitiePLC::setVariable("V1096", fSideX); // ��X

    RuitiePLC::setVariable("V1100", fTreadY);     // ��Y
    RuitiePLC::setVariable("V1104", fWheelAngle); // ���̻�ԭ�Ƕ�
}

int ConnectPLC::GetDetectState() {
    return RuitiePLC::getVariable("M90", true); // ȱ���ƶ�����ʼλ��
}

void ConnectPLC::SetAllZero() {
    RuitiePLC::setVariable("M00", false); // ��λ��ͣ �ڻ�ԭ��

    RuitiePLC::setVariable("M06", true); // һ��ԭ��
}

bool ConnectPLC::SetPLCStop() {
    SetPLCAuto(false);
    return RuitiePLC::setVariable("M00", true); // ��ͣ
}

bool ConnectPLC::SetPLCAuto(bool bAuto) {
    m_bPLCAuto = bAuto;

    RuitiePLC::setVariable("M01", m_bPLCAuto); // �����л�
    // �·�״̬
    return true;
}
bool ConnectPLC::GetPLCAuto() {
    m_bPLCAuto = RuitiePLC::getVariable("M01", 0); // �����л�
    // ��ȡ״̬
    return m_bPLCAuto;
}

bool ConnectPLC::Start() // ��ʼɨ��
{
    // �·���ز���
    m_ThreadPLCStatus.Close();

    while (m_ThreadPLCStatus.m_bWorking) {
    }
    SetCoder(0);
    if (m_ThreadRead.IsNull())
        m_ThreadRead.Create(&ConnectPLC::_Read, this);
    return true;
}

bool ConnectPLC::Stop() {
    m_ThreadRead.Close(); // ɨ���߳�ֹͣ

    // ̽�˽���ʱ��ɨ��ܻ�λ
    SetCoder(0);

    // if (m_ThreadPLCStatus.IsNull()) //����PLC״̬�߳�
    //     m_ThreadPLCStatus.Create(&ConnectPLC::_PLCStatus, this);
    return true;
}

// 100mm ��ȡһ��PLC״̬
void ConnectPLC::_PLCStatus() {
    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency(&nFrequency);
    LARGE_INTEGER time_start; /*��ʼʱ��*/
    QueryPerformanceCounter(&time_start);

    while (m_ThreadPLCStatus.m_bWorking) {
        LARGE_INTEGER time_end; /*��ʼʱ��*/
        QueryPerformanceCounter(&time_end);
        // ����ʱ������һ������Ƕ�
        static int i = 0;
        // float fAngle = float(((time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart) / 300 % 3600) / 10.0f;// /1 ����ɨ��Ŀ���
        //  printf("_PLCStatus:%.2f-%Id\n", fAngle,( (time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart));
        //   SetCoder(fAngle);

        Sleep(500);
    }
}
void ConnectPLC::_Read() {
    INT64 iDebugTime = GetTickCount64();

    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency(&nFrequency);
    LARGE_INTEGER time_start; /*��ʼʱ��*/
    QueryPerformanceCounter(&time_start);
    while (m_ThreadRead.m_bWorking) {
        LARGE_INTEGER time_end; /*��ʼʱ��*/
        QueryPerformanceCounter(&time_end);
        // ����ʱ������һ������Ƕ�
        static int i = 0;
        // float fAngle = float((GetTickCount64() - iDebugTime) % 3600) / 10.0f;

        if (RuitiePLC::isConnected()) { // PLC����
            GetAllFloatValue();
            Sleep(1);
        } else {                                                                                                               // δ����
            float fAngle = float(((time_end.QuadPart - time_start.QuadPart) * 1000 / nFrequency.QuadPart) / 4 % 3600) / 10.0f; // /1 ����ɨ��Ŀ���

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
    float fValue = mPLCFloatValueMap["V1008"]; // ��xλ��
    return fValue;
}
float ConnectPLC::GetPLCSideY() {
    float fValue = mPLCFloatValueMap["V1028"]; // ��Yλ��
    return fValue;
}
float ConnectPLC::GetPLCTreadX() {
    float fValue = mPLCFloatValueMap["V1048"]; // ��Xƽλ��
    return fValue;
}
float ConnectPLC::GetPLCTreadY() {
    float fValue = mPLCFloatValueMap["V1068"]; // ��Yƽλ��
    return fValue;
}
