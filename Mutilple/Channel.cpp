#include "pch.h"

#include "Channel.h"
DWORD Channel::GetSerializeLen(INT32 iType) {
    if (iType == -1) {};

    DWORD dwLen = sizeof(m_fRange) + sizeof(m_iVelocity) + sizeof(m_fDelay) + sizeof(m_fOffset) + sizeof(m_iSampleFactor) +
                  sizeof(m_fGain) + sizeof(m_iFilter) + sizeof(m_iDemodu) + sizeof(m_iPhaseReverse) + sizeof(m_pGateAlarmType[0]) +
                  sizeof(m_pGateAlarmType[1]) + sizeof(m_pGatePos[0]) + sizeof(m_pGatePos[1]) + sizeof(m_pGateWidth[0]) +
                  sizeof(m_pGateWidth[1]) + sizeof(m_pGateHeight[0]) + sizeof(m_pGateHeight[1]) + sizeof(m_iGateBType);

    return dwLen;
}

BYTE* Channel::Serialize() {
    DWORD dwSerializeLen = GetSerializeLen();
    Resize(dwSerializeLen);

    BYTE* pData = &m_pStream[0];

    int iIndex = 0;
    memcpy(pData + iIndex, &m_fRange, sizeof(m_fRange));
    iIndex += sizeof(m_fRange);

    memcpy(pData + iIndex, &m_iVelocity, sizeof(m_iVelocity));
    iIndex += sizeof(m_iVelocity);

    memcpy(pData + iIndex, &m_fDelay, sizeof(m_fDelay));
    iIndex += sizeof(m_fDelay);

    memcpy(pData + iIndex, &m_fOffset, sizeof(m_fOffset));
    iIndex += sizeof(m_fOffset);

    memcpy(pData + iIndex, &m_iSampleFactor, sizeof(m_iSampleFactor));
    iIndex += sizeof(m_iSampleFactor);

    memcpy(pData + iIndex, &m_fGain, sizeof(m_fGain));
    iIndex += sizeof(m_fGain);

    memcpy(pData + iIndex, &m_iFilter, sizeof(m_iFilter));
    iIndex += sizeof(m_iFilter);

    memcpy(pData + iIndex, &m_iDemodu, sizeof(m_iDemodu));
    iIndex += sizeof(m_iDemodu);

    memcpy(pData + iIndex, &m_iPhaseReverse, sizeof(m_iPhaseReverse));
    iIndex += sizeof(m_iPhaseReverse);

    memcpy(pData + iIndex, &m_pGateAlarmType[0], sizeof(m_pGateAlarmType[0]));
    iIndex += sizeof(m_pGateAlarmType[0]);

    memcpy(pData + iIndex, &m_pGateAlarmType[1], sizeof(m_pGateAlarmType[1]));
    iIndex += sizeof(m_pGateAlarmType[1]);

    memcpy(pData + iIndex, &m_pGatePos[0], sizeof(m_pGatePos[0]));
    iIndex += sizeof(m_pGatePos[0]);

    memcpy(pData + iIndex, &m_pGatePos[1], sizeof(m_pGatePos[1]));
    iIndex += sizeof(m_pGatePos[1]);

    memcpy(pData + iIndex, &m_pGateWidth[0], sizeof(m_pGateWidth[0]));
    iIndex += sizeof(m_pGateWidth[0]);

    memcpy(pData + iIndex, &m_pGateWidth[1], sizeof(m_pGateWidth[1]));
    iIndex += sizeof(m_pGateWidth[1]);

    memcpy(pData + iIndex, &m_pGateHeight[0], sizeof(m_pGateHeight[0]));
    iIndex += sizeof(m_pGateHeight[0]);

    memcpy(pData + iIndex, &m_pGateHeight[1], sizeof(m_pGateHeight[1]));
    iIndex += sizeof(m_pGateHeight[1]);

    memcpy(pData + iIndex, &m_iGateBType, sizeof(m_iGateBType));

    return pData;
}

BOOL Channel::Unserialize(BYTE* pData, DWORD dwLen) {
    DWORD dwSerializeLen = GetSerializeLen();
    if (dwSerializeLen != dwLen)
        return FALSE;

    int iIndex = 0;
    memcpy(&m_fRange, pData + iIndex, sizeof(m_fRange));
    iIndex += sizeof(m_fRange);

    int testVelocity = 0;
    memcpy(&testVelocity, pData + iIndex, sizeof(m_iVelocity));
    memcpy(&m_iVelocity, pData + iIndex, sizeof(m_iVelocity));
    iIndex += sizeof(m_iVelocity);

    memcpy(&m_fDelay, pData + iIndex, sizeof(m_fDelay));
    iIndex += sizeof(m_fDelay);

    memcpy(&m_fOffset, pData + iIndex, sizeof(m_fOffset));
    iIndex += sizeof(m_fOffset);

    memcpy(&m_iSampleFactor, pData + iIndex, sizeof(m_iSampleFactor));
    iIndex += sizeof(m_iSampleFactor);

    memcpy(&m_fGain, pData + iIndex, sizeof(m_fGain));
    iIndex += sizeof(m_fGain);

    memcpy(&m_iFilter, pData + iIndex, sizeof(m_iFilter));
    iIndex += sizeof(m_iFilter);

    memcpy(&m_iDemodu, pData + iIndex, sizeof(m_iDemodu));
    iIndex += sizeof(m_iDemodu);

    memcpy(&m_iPhaseReverse, pData + iIndex, sizeof(m_iPhaseReverse));
    iIndex += sizeof(m_iPhaseReverse);

    memcpy(&m_pGateAlarmType[0], pData + iIndex, sizeof(m_pGateAlarmType[0]));
    iIndex += sizeof(m_pGateAlarmType[0]);

    memcpy(&m_pGateAlarmType[1], pData + iIndex, sizeof(m_pGateAlarmType[1]));
    iIndex += sizeof(m_pGateAlarmType[1]);

    memcpy(&m_pGatePos[0], pData + iIndex, sizeof(m_pGatePos[0]));
    iIndex += sizeof(m_pGatePos[0]);

    memcpy(&m_pGatePos[1], pData + iIndex, sizeof(m_pGatePos[1]));
    iIndex += sizeof(m_pGatePos[1]);

    memcpy(&m_pGateWidth[0], pData + iIndex, sizeof(m_pGateWidth[0]));
    iIndex += sizeof(m_pGateWidth[0]);

    memcpy(&m_pGateWidth[1], pData + iIndex, sizeof(m_pGateWidth[1]));
    iIndex += sizeof(m_pGateWidth[1]);

    memcpy(&m_pGateHeight[0], pData + iIndex, sizeof(m_pGateHeight[0]));
    iIndex += sizeof(m_pGateHeight[0]);

    memcpy(&m_pGateHeight[1], pData + iIndex, sizeof(m_pGateHeight[1]));
    iIndex += sizeof(m_pGateHeight[1]);

    memcpy(&m_iGateBType, pData + iIndex, sizeof(m_iGateBType));
    iIndex += sizeof(m_iGateBType);
    return TRUE;
}
bool Channel::LoadTec(size_t ch, LPCTSTR lpName) {
    TCHAR   pBuf[256] = {0};
    CString strKey;
    strKey.Format(_T("Ch%lld"), ch);
    GetPrivateProfileStringW(strKey.GetBuffer(0), _T("Delay"), _T("0"), pBuf, 256, lpName);
    swscanf_s(pBuf, _T("%f"), &m_fDelay);
    GetPrivateProfileStringW(strKey.GetBuffer(0), _T("Offset"), _T("0"), pBuf, 256, lpName);
    swscanf_s(pBuf, _T("%f"), &m_fOffset);
    GetPrivateProfileStringW(strKey.GetBuffer(0), _T("Range"), _T("100"), pBuf, 256, lpName);
    swscanf_s(pBuf, _T("%f"), &m_fRange);
    GetPrivateProfileStringW(strKey.GetBuffer(0), _T("Velocity"), _T("5920"), pBuf, 256, lpName);
    swscanf_s(pBuf, _T("%d"), &m_iVelocity);
    m_iSampleFactor = GetPrivateProfileIntW(strKey.GetBuffer(0), _T("SampleFactor"), 9, lpName);
    GetPrivateProfileStringW(strKey.GetBuffer(0), _T("Gain"), _T("10.0"), pBuf, 256, lpName);
    swscanf_s(pBuf, _T("%f"), &m_fGain);

    m_iFilter       = GetPrivateProfileInt(strKey.GetBuffer(0), _T("Filter"), 0, lpName);
    m_iDemodu       = GetPrivateProfileInt(strKey.GetBuffer(0), _T("Demodu"), 0, lpName);
    m_iPhaseReverse = GetPrivateProfileInt(strKey.GetBuffer(0), _T("PhaseReverse"), 0, lpName);

    m_pGateAlarmType[GATE_A] = GetPrivateProfileInt(strKey.GetBuffer(0), _T("GateAlarmType[GATE_A]"), 0, lpName);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("GatePos[GATE_A]"), _T("0.75"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_pGatePos[GATE_A]);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("GateWidth[GATE_A]"), _T("0.10"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_pGateWidth[GATE_A]);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("GateHeight[GATE_A]"), _T("0.80"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_pGateHeight[GATE_A]);

    m_iGateBType             = GetPrivateProfileInt(strKey.GetBuffer(0), _T("GateBType"), 0, lpName);
    m_pGateAlarmType[GATE_B] = GetPrivateProfileInt(strKey.GetBuffer(0), _T("GateAlarmType[GATE_B]"), 0, lpName);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("GatePos[GATE_B]"), _T("0.75"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_pGatePos[GATE_B]);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("GateWidth[GATE_B]"), _T("0.10"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_pGateWidth[GATE_B]);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("GateHeight[GATE_B]"), _T("0.50"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_pGateHeight[GATE_B]);

    return TRUE;
}

bool Channel::SaveTec(size_t ch, LPCTSTR lpName) {
    CString strKey, strInfo;
    strKey.Format(_T("Ch%ull"), ch);
    strInfo.Format(_T("%f"), m_fDelay);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Delay"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_fOffset);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Offset"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_fRange);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Range"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iVelocity);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Velocity"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iSampleFactor);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("SampleFactor"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_fGain);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Gain"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iFilter);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Filter"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iDemodu);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("Demodu"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_iPhaseReverse);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("PhaseReverse"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_pGateAlarmType[GATE_A]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateAlarmType[GATE_A]"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_pGatePos[GATE_A]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GatePos[GATE_A]"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_pGateWidth[GATE_A]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateWidth[GATE_A]"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_pGateHeight[GATE_A]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateHeight[GATE_A]"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%d"), m_iGateBType);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateBType"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%d"), m_pGateAlarmType[GATE_B]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateAlarmType[GATE_B]"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_pGatePos[GATE_B]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GatePos[GATE_B]"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_pGateWidth[GATE_B]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateWidth[GATE_B]"), strInfo.GetBuffer(0), lpName);
    strInfo.Format(_T("%f"), m_pGateHeight[GATE_B]);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("GateHeight[GATE_B]"), strInfo.GetBuffer(0), lpName);

    return TRUE;
}

void Channel::CopyeChannel(Channel* pCH) {
    m_fRange            = pCH->m_fRange;
    m_iVelocity         = pCH->m_iVelocity;
    m_fDelay            = pCH->m_fDelay;
    m_fOffset           = pCH->m_fOffset;
    m_iSampleFactor     = pCH->m_iSampleFactor;
    m_fGain             = pCH->m_fGain;
    m_iFilter           = pCH->m_iFilter;
    m_iDemodu           = pCH->m_iDemodu;
    m_iPhaseReverse     = pCH->m_iPhaseReverse;
    m_pGateAlarmType[0] = pCH->m_pGateAlarmType[0];
    m_pGateAlarmType[1] = pCH->m_pGateAlarmType[1];
    m_pGatePos[0]       = pCH->m_pGatePos[0];
    m_pGatePos[1]       = pCH->m_pGatePos[1];
    m_pGateWidth[0]     = pCH->m_pGateWidth[0];
    m_pGateWidth[1]     = pCH->m_pGateWidth[1];
    m_pGateHeight[0]    = pCH->m_pGateHeight[0];
    m_pGateHeight[1]    = pCH->m_pGateHeight[1];
    m_iGateBType        = pCH->m_iGateBType;
}