#include "pch.h"

#include "Specimen.h"

bool Specimen::LoadTec(LPCTSTR lpName) {
    TCHAR pBuf[256] = {0};

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelUseModel"), _T("适用车型"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szWheelUseModel, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelType"), _T("车轮类型"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szWheelType, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelName"), _T("车轮名称"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szWheelName, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelModel"), _T("车轮型号"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szWheelModel, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelMaterial"), _T("车轮材质"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szWheelMaterial, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelNumber"), _T("15"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szWheelNumber, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szHeatNumber"), _T("23-7-01021"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szHeatNumber, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionStd"), _T("TB/T2995-2000"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szDetectionStd, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionContent"), _T("轮辋轴型径向探测Ф2mm平底孔当量"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szDetectionContent, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionArea"), _T("轮辋内部"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szDetectionArea, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionFact"), _T("苏州罗克莱管材探伤机"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szDetectionFact, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("szDeviceName"), _T("(水浸)车轮超声波探伤机"), pBuf, 256, lpName);
    StrCpy(m_WheelParam.szDeviceName, pBuf);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelHub"), _T("170"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelHub);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelRim"), _T("135"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelRim);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelInnerSideOffset"), _T("5"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelInnerSideOffset);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelHubInnerDiameter"), _T("177"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelHubInnerDiameter);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelHubOuterDiameter"), _T("250"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelHubOuterDiameter);
    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelRimlInnerDiameter"), _T("605"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelRimlInnerDiameter);
    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelRimOuterDiameter"), _T("920"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fWheelRimOuterDiameter);

    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fSideWidth"), _T("10.0"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fSideWidth);
    GetPrivateProfileString(_T("WHEEL_PAPA"), _T("fTreadWidth"), _T("10.0"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &m_WheelParam.fTreadWidth);

    return TRUE;
}

bool Specimen::SaveTec(LPCTSTR lpName) {
    CString strKey, strInfo;

    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelUseModel"), m_WheelParam.szWheelUseModel, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelType"), m_WheelParam.szWheelType, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelName"), m_WheelParam.szWheelName, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelModel"), m_WheelParam.szWheelModel, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelMaterial"), m_WheelParam.szWheelMaterial, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szWheelNumber"), m_WheelParam.szWheelNumber, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szHeatNumber"), m_WheelParam.szHeatNumber, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionStd"), m_WheelParam.szDetectionStd, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionContent"), m_WheelParam.szDetectionContent, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionArea"), m_WheelParam.szDetectionArea, lpName);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szDetectionFact"), m_WheelParam.szDetectionFact, lpName);

    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("szDeviceName"), m_WheelParam.szDeviceName, lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelHub);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelHub"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelRim);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelRim"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelInnerSideOffset);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelInnerSideOffset"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelHubInnerDiameter);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelHubInnerDiameter"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelHubOuterDiameter);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelHubOuterDiameter"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelRimlInnerDiameter);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelRimlInnerDiameter"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fWheelRimOuterDiameter);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fWheelRimOuterDiameter"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fSideWidth);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fSideWidth"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), m_WheelParam.fTreadWidth);
    WritePrivateProfileString(_T("WHEEL_PAPA"), _T("fTreadWidth"), strInfo.GetBuffer(0), lpName);

    return TRUE;
}