#include "pch.h"

#include "Scan.h"

void Scan::SetSidePoints(int iPoints) {
    m_iSidePoints    = iPoints;
    m_fSideAngleStep = 360.0f / iPoints;
}

void Scan::SetTreadPoints(int iPoints) {
    m_iTreadPoints    = iPoints;
    m_fTreadAngleStep = 360.0f / iPoints;
}

void Scan::SetScanRadius(float fRadius) {
    m_pSideRadius.resize(m_iDrawSideSize);
    for (int i = 0; i < m_iDrawSideSize; i++) {
        m_pSideRadius[i] = (i)*m_fSideStep; //-2
    }
}

void Scan::InitScanData() {
    // »æÍ¼Êý¾ÝÇå³ý
    m_pBscanData.resize(m_iTreadSize * m_iTreadPoints);
    m_pBscanCH.resize(m_iDrawTreadSize * m_iTreadPoints);

    m_pCscanData.resize(m_iSideSize * m_iSidePoints);
    m_pCscanCH.resize(m_iDrawSideSize * m_iSidePoints);

    memset(m_pBscanData.data(), 0, m_pBscanData.size());
    memset(m_pCscanData.data(), 0, m_pCscanData.size());

    memset(m_pBscanCH.data(), 0, m_pBscanCH.size());
    memset(m_pCscanCH.data(), 0, m_pCscanCH.size());
}

Scan::~Scan() {
    m_pBscanData.clear();
    m_pBscanCH.clear();

    m_pCscanData.clear();
    m_pCscanCH.clear();

    int test = 0;
}