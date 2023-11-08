#include "pch.h"

#include "System.h"
DWORD System::GetSerializeLen(INT32 iType) {
    if (iType == -1) {};

    DWORD dwLen = sizeof(m_iFrequency) + sizeof(m_iVoltage) + sizeof(m_iPulseWidth) + sizeof(m_iTxFlag) + sizeof(m_iRxFlag) + sizeof(m_iChMode) +
                  sizeof(m_iScanIncrement) + sizeof(m_iResetCoder) + sizeof(m_iLEDStatus) + sizeof(m_iWorkType) + sizeof(m_iControlTime) + sizeof(m_iAxleTime);

    return dwLen;
}

BYTE* System::Serialize() {
    DWORD dwSerializeLen = GetSerializeLen();
    Resize(dwSerializeLen);

    BYTE* pData = &m_pStream[0];

    int iIndex = 0;
    memcpy(pData + iIndex, &m_iFrequency, sizeof(m_iFrequency));
    iIndex += sizeof(m_iFrequency);

    memcpy(pData + iIndex, &m_iVoltage, sizeof(m_iVoltage));
    iIndex += sizeof(m_iVoltage);

    memcpy(pData + iIndex, &m_iPulseWidth, sizeof(m_iPulseWidth));
    iIndex += sizeof(m_iPulseWidth);

    memcpy(pData + iIndex, &m_iTxFlag, sizeof(m_iTxFlag));
    iIndex += sizeof(m_iTxFlag);

    memcpy(pData + iIndex, &m_iRxFlag, sizeof(m_iRxFlag));
    iIndex += sizeof(m_iRxFlag);

    memcpy(pData + iIndex, &m_iChMode, sizeof(m_iChMode));
    iIndex += sizeof(m_iChMode);

    memcpy(pData + iIndex, &m_iScanIncrement, sizeof(m_iScanIncrement));
    iIndex += sizeof(m_iScanIncrement);

    memcpy(pData + iIndex, &m_iResetCoder, sizeof(m_iResetCoder));
    iIndex += sizeof(m_iResetCoder);

    memcpy(pData + iIndex, &m_iLEDStatus, sizeof(m_iLEDStatus));
    iIndex += sizeof(m_iLEDStatus);

    memcpy(pData + iIndex, &m_iWorkType, sizeof(m_iWorkType));
    iIndex += sizeof(m_iWorkType);

    memcpy(pData + iIndex, &m_iControlTime, sizeof(m_iControlTime));
    iIndex += sizeof(m_iControlTime);

    memcpy(pData + iIndex, &m_iAxleTime, sizeof(m_iAxleTime));

    return pData;
}

BOOL System::Unserialize(BYTE* pData, DWORD dwLen) {
    DWORD dwSerializeLen = GetSerializeLen();
    if (dwSerializeLen != dwLen)
        return FALSE;

    int iIndex = 0;
    memcpy(&m_iFrequency, pData + iIndex, sizeof(m_iFrequency));
    iIndex += sizeof(m_iFrequency);

    int testVoltage = 0;
    memcpy(&testVoltage, pData + iIndex, sizeof(m_iVoltage));
    memcpy(&m_iVoltage, pData + iIndex, sizeof(m_iVoltage));
    iIndex += sizeof(m_iVoltage);

    memcpy(&m_iPulseWidth, pData + iIndex, sizeof(m_iPulseWidth));
    iIndex += sizeof(m_iPulseWidth);

    memcpy(&m_iTxFlag, pData + iIndex, sizeof(m_iTxFlag));
    iIndex += sizeof(m_iTxFlag);

    memcpy(&m_iRxFlag, pData + iIndex, sizeof(m_iRxFlag));
    iIndex += sizeof(m_iRxFlag);

    memcpy(&m_iChMode, pData + iIndex, sizeof(m_iChMode));
    //if (m_iChMode != 0x05ff0aff)
    //    m_iChMode = 0x05ff0aff;
    iIndex += sizeof(m_iChMode);

    memcpy(&m_iScanIncrement, pData + iIndex, sizeof(m_iScanIncrement));
    iIndex += sizeof(m_iScanIncrement);

    memcpy(&m_iResetCoder, pData + iIndex, sizeof(m_iResetCoder));
    iIndex += sizeof(m_iResetCoder);

    memcpy(&m_iLEDStatus, pData + iIndex, sizeof(m_iLEDStatus));
    iIndex += sizeof(m_iLEDStatus);

    memcpy(&m_iWorkType, pData + iIndex, sizeof(m_iWorkType));
    iIndex += sizeof(m_iWorkType);

    memcpy(&m_iControlTime, pData + iIndex, sizeof(m_iControlTime));
    iIndex += sizeof(m_iControlTime);

    memcpy(&m_iAxleTime, pData + iIndex, sizeof(m_iAxleTime));
    iIndex += sizeof(m_iAxleTime);
    return TRUE;
}

bool System::LoadTec(LPCTSTR lpName) {
    TCHAR pBuf[256] = {0};

    m_iFrequency     = GetPrivateProfileInt(_T("System"), _T("Frequency"), 1000, lpName);
    m_iVoltage       = GetPrivateProfileInt(_T("System"), _T("Voltage"), 1, lpName);
    m_iPulseWidth    = GetPrivateProfileInt(_T("System"), _T("PulseWidth"), 0, lpName);
    m_iTxFlag        = GetPrivateProfileInt(_T("System"), _T("TxFlag"), 0x03FF, lpName);
    m_iRxFlag        = GetPrivateProfileInt(_T("System"), _T("RxFlag"), 0x03FF, lpName);
    m_iChMode        = GetPrivateProfileInt(_T("System"), _T("ChMode"), 0x00000300, lpName); // ÈðÌú 9.10Í¨µÀË«¾§ÉèÖÃ¡£
    m_iScanIncrement = GetPrivateProfileInt(_T("System"), _T("ScanIncrement"), 0, lpName);
    m_iResetCoder    = GetPrivateProfileInt(_T("System"), _T("ResetCoder"), 0, lpName);
    m_iLEDStatus     = GetPrivateProfileInt(_T("System"), _T("LEDStatus"), 0, lpName);
    m_iWorkType      = GetPrivateProfileInt(_T("System"), _T("WorkType"), 0, lpName);
    m_iControlTime   = GetPrivateProfileInt(_T("System"), _T("ControlTime"), 1, lpName);
    m_iAxleTime      = GetPrivateProfileInt(_T("System"), _T("AxleTime"), 1, lpName);
    return TRUE;
}

bool System::SaveTec(LPCTSTR lpName) {
    CString strKey, strInfo;

    strInfo.Format(_T("%d"), m_iFrequency);
    WritePrivateProfileString(_T("System"), _T("Frequency"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iVoltage);
    WritePrivateProfileString(_T("System"), _T("Voltage"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iPulseWidth);
    WritePrivateProfileString(_T("System"), _T("PulseWidth"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iTxFlag);
    WritePrivateProfileString(_T("System"), _T("TxFlag"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iRxFlag);
    WritePrivateProfileString(_T("System"), _T("RxFlag"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iChMode);
    WritePrivateProfileString(_T("System"), _T("ChMode"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iScanIncrement);
    WritePrivateProfileString(_T("System"), _T("ScanIncrement"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iResetCoder);
    WritePrivateProfileString(_T("System"), _T("ResetCoder"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iLEDStatus);
    WritePrivateProfileString(_T("System"), _T("LEDStatus"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iWorkType);
    WritePrivateProfileString(_T("System"), _T("WorkType"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iControlTime);
    WritePrivateProfileString(_T("System"), _T("ControlTime"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iAxleTime);
    WritePrivateProfileString(_T("System"), _T("AxleTime"), strInfo.GetBuffer(0), lpName);

    return TRUE;
}