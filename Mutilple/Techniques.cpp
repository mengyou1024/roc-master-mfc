#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"
#include "Techniques.h"

Techniques::Techniques() :
m_iChannel(0),
m_iSideCirIndex(0),
m_iSidePointIndex(0),
m_iTreadPointIndex(0),
_pDetectionStd(nullptr),
m_iDetectionStd(DETECTIONSTD_NONE),
_AngleSection(0),
m_ScanType(SCAN_NORMAL) {
    memset(&m_Specimen.m_WheelParam, 0, sizeof(m_Specimen.m_WheelParam));
    for (int i = 0; i < 2; i++) {
        m_pDraw[i].hReady = CreateEvent(NULL, TRUE, FALSE, NULL);
    }
}

Techniques::~Techniques() {
    for (int i = 0; i < 2; i++) {
        if (m_pDraw[i].hReady != INVALID_HANDLE_VALUE) {
            CloseHandle(m_pDraw[i].hReady);
            m_pDraw[i].hReady = INVALID_HANDLE_VALUE;
            for (auto& ptr : m_pDraw[i].pData)
                ptr.clear();
        }
    }

    if (_pDetectionStd) {
        delete _pDetectionStd;
        _pDetectionStd = nullptr;
    }

    // ȱ������
    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        std::vector<DB_DEFECT_DATA*>::iterator it = m_pDefect[i].begin();
        while (it != m_pDefect[i].end()) {
            delete *it;
            *it = NULL;
            it++;
        }
        m_pDefect[i].clear();
    }
    int test = 0;
}

void Techniques::SetDetectionStd(int iDetectionStd) {
    if (_pDetectionStd) {
        delete _pDetectionStd;
        _pDetectionStd = nullptr;
    }

    // ����̽�˱�׼
    switch (iDetectionStd) {
        case DETECTIONSTD_TBT2995_200: _pDetectionStd = new DetectionStd_TBT2995_200(); break;
    }

    if (_pDetectionStd != nullptr) {
        // ���ݱ�׼��ʼ������
        _pDetectionStd->InitTechniques(this);
        m_iDetectionStd = iDetectionStd;
    } else {
        m_iDetectionStd = DETECTIONSTD_NONE;
    }
}

void Techniques::Compute() {
    // ��ǰ����ÿȦ�̶�360����
    m_Scan.SetSidePoints(360);
    m_Scan.SetTreadPoints(360);
    m_Scan.m_iSideSize = g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize;

    // �����ͼ����̽ͷ֮���Ȧ��
    m_Scan.m_iDrawProbleSideSize = m_Specimen.m_WheelParam.fSideWidth / abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    // ̤���ͼ����̽ͷ֮���Ȧ��
    m_Scan.m_iDrawProbleTreadSize = m_Specimen.m_WheelParam.fTreadWidth / abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);

    // �����ͼȦ��   ���� ��ⷶΧ /����      �����⾶��7��-�����ھ���8��
    auto _iDrawSizeSize = (m_Specimen.m_WheelParam.fWheelRimOuterDiameter - m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) / 2.0f /
                          abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);

    // ̤���ͼȦ�� ���� ��ⷶΧ/���� ̤��������ȣ�2��-��Ե��ȣ�8��
    auto _iDrawTreadSize = (m_Specimen.m_WheelParam.fWheelRim - m_Specimen.m_WheelParam.fWheelFlangeHeight) /
                           abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);

    auto _fSizeStep =
        (m_Specimen.m_WheelParam.fWheelRimOuterDiameter - m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) / 2.0f / m_Scan.m_iDrawSideSize;

    if (isnan(_iDrawSizeSize)) {
        _iDrawSizeSize = 17;
    }
    if (isnan(_iDrawTreadSize)) {
        _iDrawTreadSize = 21;
    }
    if (isnan(_fSizeStep)) {
        _fSizeStep = 10;
    }

    m_Scan.m_iDrawSideSize  = _iDrawSizeSize;
    m_Scan.m_iDrawTreadSize = _iDrawTreadSize;

    // �����Ż�Ȧ�����㲽��
    m_Scan.m_fSideStep = _fSizeStep;

    // �����⾶-�ھ� Ϊ����ɨ�Χ
    m_Scan.SetScanRadius((m_Specimen.m_WheelParam.fWheelRimOuterDiameter - m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) / 2.0f);
    // m_Scan.m_iSideSize Ϊ ��תȦ��  m_iDrawProbleSideSizeΪ��������̽ͷ֮��ļ��Ȧ��       m_iDrawSideSize Ϊ���� ����̽ͷ�ܹ���Ҫ��Ȧ��

    // m_pRecordTread.resize(m_iTreadPoints * m_Scan.m_iTreadSize);
    g_MainProcess.m_Techniques.m_iSideCirIndex = 0;
    m_Scan.InitScanData();
}

bool Techniques::Start() {
    m_nLastIndex = 0;
    m_fLastAngle = 0.0f;
    m_nLastCir   = 0;
    m_pRecord_Buffer.clear(); // �������
    // ���ݼ�¼ �ֹ�һȦ ������3000mm      ��©��0.05mm�Ŀ�   ��ס һȦ360  һȦ������ 7200���� �ݶ�7200����
    m_pRecord.clear(); // �����¼����

    m_pRecord.resize(m_iSidePoints * m_Scan.m_iSideSize); // ���·���

    m_Scan.InitScanData();

    _AngleSection    = 0;
    m_iSideCirIndex  = 0;
    m_iTreadCirIndex = 0;

    m_ScanType = SCAN_SCAN;

    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
        std::vector<DB_DEFECT_DATA*>::iterator it = m_pDefect[i].begin();
        while (it != m_pDefect[i].end()) {
            delete *it;
            *it = NULL;
            it++;
        }
        m_pDefect[i].clear();
    }
    memset(m_bNewDEFECT, 0, HD_CHANNEL_NUM);    // ��ʼ�������ͨ��������ȱ��
    memset(m_bNewDEFECT_TS, 0, HD_CHANNEL_NUM); // ��ʼ�������ͨ��������͸������ȱ��

    return true;
}
bool Techniques::CreateMultipleDirectory(LPCTSTR szPath) {
    CString strDir(szPath);                               // ���Ҫ������Ŀ¼�ַ���
    if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\')) // ȷ����'\'��β�Դ������һ��Ŀ¼
    {
        strDir.AppendChar(_T('\\'));
    }
    vector<CString> vPath;            // ���ÿһ��Ŀ¼�ַ���
    CString         strTemp;          // һ����ʱ����,���Ŀ¼�ַ���
    bool            bSuccess = false; // �ɹ���־
    // ����Ҫ�������ַ���
    for (int i = 0; i < strDir.GetLength(); ++i) {
        if (strDir.GetAt(i) != _T('\\')) { // �����ǰ�ַ�����'\\'
            strTemp.AppendChar(strDir.GetAt(i));
        } else {                      // �����ǰ�ַ���'\\'
            vPath.push_back(strTemp); // ����ǰ����ַ�����ӵ�������
            strTemp.AppendChar(_T('\\'));
        }
    }
    // �������Ŀ¼������,����ÿ��Ŀ¼
    vector<CString>::const_iterator vIter;
    for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) {
        if (!PathIsDirectory(*vIter)) // �жϵ�ǰĿ¼ʱ����ڣ������ھʹ���
        {
            // ���CreateDirectoryִ�гɹ�,����true,���򷵻�false
            bSuccess = CreateDirectory(*vIter, NULL) ? true : false;
        }
    }
    return bSuccess;
}
bool Techniques::Stop() {
    m_ScanType = SCAN_NORMAL;

    if (m_pRecord_Buffer.size() > 0) {
        {
            std::vector<RECORD_DATA_RES*>::iterator it = m_pRecord_Buffer.begin();
            while (it != m_pRecord_Buffer.end()) {
                for (int i = 0; i < HD_CHANNEL_NUM; i++) {
                    RECORD_DATA_RES* tem = *it;

                    tem->pAscan[i].clear();
                }
                delete *it;
                *it = NULL;
                it++;
            }
            m_pRecord_Buffer.clear();
        }
    }

    // ���ݱ���
    SYSTEMTIME stm;
    GetLocalTime(&stm);

    CString stDir;
    if (m_bDailyScan) {
        if (m_nDailyType == 0) {
            stDir.Format(_T("%sData\\Daily\\%4d%02d\\%02d\\������_"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay);
        } else if (m_nDailyType == 1) {
            { stDir.Format(_T("%sData\\Daily\\%4d%02d\\%02d\\�깤��_"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay); }
        }
    } else {
        stDir.Format(_T("%sData\\Scan\\%4d%02d\\%02d\\"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay);
    }

    if (!PathIsDirectory(stDir)) {
        if (!CreateMultipleDirectory(stDir)) {
            return false;
        }
    }
    CString strFileName;

    strFileName.Format(_T("%s_%s_%02d%02d%02d.scdat"), g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelModel,
                       g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.szWheelNumber, stm.wHour, stm.wMinute,
                       stm.wSecond); // �������ݸ����������������ͣ���ţ�̽�˹�������
    g_MainProcess.m_Techniques.SaveRecData(stDir + strFileName);

    m_pDefect;

    return true;
}
// �л�ʵʱ�����Լ����ݷ���
void Techniques::Test() {}

void Techniques::Dealwith(READ_DATA& Read) {
    // δɨ��ʱ��ֱ���˳�
    if (m_ScanType != SCAN_SCAN) {
        for (int i = 0; i < HD_CHANNEL_NUM; i++) { // ����ʱ�жϵ�ǰ�Ƿ���ȱ��
            if (m_bNewDEFECT[i]) {                 // ��͸��ȱ��
                mDB_DEFECT_DATA[i].nParam2  = m_iTreadCirIndex * GetTreadPoint() + m_iSaveTreadPointIndex;
                DB_DEFECT_DATA* Defect_date = new DB_DEFECT_DATA(); // new DB_DEFECT_DATA ��Ҫ�洢��voter �´ο�ʼ�ͷ�
                memcpy(Defect_date, &mDB_DEFECT_DATA[i], sizeof(DB_DEFECT_DATA)); // �������ݵ�
                m_pDefect[i].push_back(Defect_date);
                m_bNewDEFECT[i] = false; // ȱ�����

            } else {
                if (m_bNewDEFECT_TS[i]) { // ͸��ȱ��
                    mDB_DEFECT_DATA[i].nParam2  = m_iTreadCirIndex * GetTreadPoint() + m_iSaveTreadPointIndex;
                    DB_DEFECT_DATA* Defect_date = new DB_DEFECT_DATA(); // new DB_DEFECT_DATA ��Ҫ�洢��voter �´ο�ʼ�ͷ�
                    memcpy(Defect_date, &mDB_DEFECT_DATA[i], sizeof(DB_DEFECT_DATA)); // �������ݵ�
                    m_pDefect[i].push_back(Defect_date);
                    m_bNewDEFECT_TS[i] = false;
                }
            }
        }
        return;
    }
    // printf("Dealwith%.2f\n", Read.fAngle);
    // �Ƕ��б仯ʱ��Ȧ�����ӻ��С
    int iAngleSection(0);
    if (Read.fAngle < ANGLE_LEFT)
        iAngleSection = -1;
    else if (Read.fAngle > ANGLE_RIGHT)
        iAngleSection = 1;
    else
        iAngleSection = 0;
    if (_AngleSection * iAngleSection < 0) // ������ -1 ��1�ı仯
    {
        if (iAngleSection < 0) {
            m_iSideCirIndex++;
        } else if (iAngleSection > 0) {
            m_iSideCirIndex--;
        }
    }
    _AngleSection = iAngleSection;

    // �������ݵ�
    m_iSidePointIndex  = int(Read.fAngle / m_Scan.GetSideAngleStep()) % m_Scan.GetSidePoint();
    m_iTreadPointIndex = int(Read.fAngle / m_Scan.GetTreadAngleStep()) % m_Scan.GetTreadPoint();
    // ���ݼ�¼
    m_iSaveSidePointIndex  = int(Read.fAngle / GetSideAngleStep()) % GetSidePoint();
    m_iSaveTreadPointIndex = int(Read.fAngle / GetTreadAngleStep()) % GetTreadPoint();
    if (m_iSaveSidePointIndex >= m_pRecord.size()) {
        int tem = 0;
    }
    if (m_iSaveTreadPointIndex >= m_pRecord.size()) {
        int tem = 0;
    }

    if (m_iSideCirIndex == 0 && Read.fAngle < 0.5f) { // ��һȦ �Ƕȴ���0.5�ű�ʾת�� ������
        return;
    }

    // Ȧ����ʼ ���� С����СȦ��
    if (m_iSideCirIndex >= 0 && m_iSideCirIndex < m_Scan.m_iSideSize) // ɨ����� δ�����һȦ m_iSideSize �� m_iTreadSize һ��
    {
        int res    = _pDetectionStd->Detection(this, Read);                    // ���������Լ� �������ݴ���
        int iIndex = m_iSideCirIndex * GetSidePoint() + m_iSaveSidePointIndex; // �����������
        // if (abs(m_fLastAngle - Read.fAngle) > EPS) { //�ǶȲ�ͬ   ������֡�� ȡ���ȱ�ݷ��䵽 �洢���ݺͳ�����
        if ((iIndex - m_nLastIndex) > 0) {
            //     int iLastIndex = m_nLastCir* GetSidePoint()+ int(m_fLastAngle / GetSideAngleStep()) % GetSidePoint();//���濪ʼ����

            TRACE("%d_%df_%d\n", m_nLastIndex, iIndex);
            float nBufferScale = 1.0f * m_pRecord_Buffer.size() / (iIndex - m_nLastIndex);
            for (int i = m_nLastIndex; i < iIndex; i++) // �Լ��ȱ�ٵ�֡���ӻ����в���
            {
                float maxDb = 0;
                int   maxj  = (i - m_nLastIndex) * nBufferScale;
                for (int j = (i - m_nLastIndex) * nBufferScale; j < (i - m_nLastIndex + 1) * nBufferScale; j++) { // �һ���ȱ�����

                    if (j < m_pRecord_Buffer.size()) {
                        for (int ichannel = 0; ichannel < HD_CHANNEL_NUM; ichannel++) { // ȱ����� db��ֵ
                            if (m_pRecord_Buffer[j]->bDefectType[ichannel] == 0) {
                                if (m_pRecord_Buffer[j]->fDb[ichannel] > maxDb) {
                                    maxDb = m_pRecord_Buffer[j]->fDb[ichannel];
                                    maxj  = j;
                                }

                            } else if (m_pRecord_Buffer[j]->bDefectType[ichannel] == 1) { // ͸��ȱ����� dBΪ��ֵ��С
                                if (maxDb == 0) {
                                    if (m_pRecord_Buffer[j]->fDb[ichannel] < maxDb) {
                                        maxDb = m_pRecord_Buffer[j]->fDb[ichannel];
                                        maxj  = j;
                                    }
                                }
                            } else {
                            }
                        }
                    }
                }
                for (int ichannel = 0; ichannel < HD_CHANNEL_NUM; ichannel++) { // ��¼����
                    if (maxj < m_pRecord_Buffer.size()) {                       // ������߲�

                        // if(mDB_DEFECT_DATA[ichannel].nDBOffset!=0) //����ȱ������
                        // mDB_DEFECT_DATA[ichannel].nScanIndex = i;

                        m_pRecord[i].pGatePos[ichannel][GATE_A] = m_pRecord_Buffer[maxj]->pGatePos[ichannel][GATE_A];
                        m_pRecord[i].pGatePos[ichannel][GATE_B] = m_pRecord_Buffer[maxj]->pGatePos[ichannel][GATE_B];
                        m_pRecord[i].pGateAmp[ichannel][GATE_A] = m_pRecord_Buffer[maxj]->pGateAmp[ichannel][GATE_A];
                        m_pRecord[i].pGateAmp[ichannel][GATE_B] = m_pRecord_Buffer[maxj]->pGateAmp[ichannel][GATE_B];
                        size_t size                             = m_pRecord_Buffer[maxj]->pAscan[ichannel].size();
                        m_pRecord[i].iAscanSize[ichannel]       = size;
                        m_pRecord[i].pAscan[ichannel].resize(size);
                        std::memcpy(m_pRecord[i].pAscan[ichannel].data(), m_pRecord_Buffer[maxj]->pAscan[ichannel].data(), size);
                        int test = 0;
                    } else {
                        m_pRecord[i].pGatePos[ichannel][GATE_A] = Read.pGatePos[ichannel][GATE_A];
                        m_pRecord[i].pGatePos[ichannel][GATE_B] = Read.pGatePos[ichannel][GATE_B];
                        m_pRecord[i].pGateAmp[ichannel][GATE_A] = Read.pGateAmp[ichannel][GATE_A];
                        m_pRecord[i].pGateAmp[ichannel][GATE_B] = Read.pGateAmp[ichannel][GATE_B];
                        size_t size                             = Read.pAscan[ichannel].size();
                        m_pRecord[i].iAscanSize[ichannel]       = size;
                        m_pRecord[i].pAscan[ichannel].resize(size);
                        std::memcpy(m_pRecord[i].pAscan[ichannel].data(), Read.pAscan[ichannel].data(), size);
                    }
                }
            }

            TRACE("%0.2f_%0.2f_%d\n", m_fLastAngle, Read.fAngle, m_pRecord_Buffer.size());
            //	m_pRecord_Buffer.clear();//�������

            {
                std::vector<RECORD_DATA_RES*>::iterator it = m_pRecord_Buffer.begin();
                while (it != m_pRecord_Buffer.end()) {
                    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
                        RECORD_DATA_RES* tem = *it;
                        tem->pAscan[i].clear();
                    }
                    delete *it;
                    *it = NULL;
                    it++;
                }
                m_pRecord_Buffer.clear();
            }

            m_nLastIndex = iIndex;
            m_fLastAngle = Read.fAngle; // ������һ�νǶ�
            m_nLastCir   = m_iSideCirIndex;
        } else {
            //	TRACE("%0.2f_%0.2f\n", m_fLastAngle, Read.fAngle);
        }
    }
}
void Techniques::LoadTecName(LPCTSTR lpName) {
    CString strName = lpName;
    int     res     = strName.Find(_T("LastParam"));
    if (res > 0) {
        TCHAR pBuf[256] = {0};
        GetPrivateProfileString(_T("CURTECH"), _T("Name"), _T("��"), pBuf, 256, lpName);
        StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, pBuf);
        CString strFind;
        strFind.Format(_T("%sTechINI\\%s.tech"), theApp.m_pExePath, g_MainProcess.m_Techniques.m_pCurTechName);

        WIN32_FIND_DATA fileData;
        HANDLE          hFindFile = FindFirstFile(strFind, &fileData);
        if (INVALID_HANDLE_VALUE == hFindFile) { // �ļ������ڳ���ͨ��
            // TODO: �ļ�������ʱʹ��Ĭ�ϲ���
            // Ĭ��ϵͳ����
            m_System.m_iFrequency     = 0x4b0;
            m_System.m_iVoltage       = 1;
            m_System.m_iPulseWidth    = 0xd2;
            m_System.m_iTxFlag        = 0x3ff;
            m_System.m_iRxFlag        = 0x3ff;
            m_System.m_iChMode        = 0x05ff0aff;
            m_System.m_iScanIncrement = 0;
            m_System.m_iLEDStatus     = 0;
            m_System.m_iWorkType      = 0;
            m_System.m_iControlTime   = 1;
            m_System.m_iAxleTime      = 1;
            // Ĭ��ͨ������
            m_pChannel.resize(HD_CHANNEL_NUM);
            for (size_t i = 0; i < m_pChannel.size(); i++) {
                m_pChannel[i].m_fRange         = 200;
                m_pChannel[i].m_iVelocity      = 0x172a;
                m_pChannel[i].m_fDelay         = 140;
                m_pChannel[i].m_fOffset        = 0.868325114f;
                m_pChannel[i].m_iSampleFactor  = 0xd;
                m_pChannel[i].m_fGain          = 46.f;
                m_pChannel[i].m_iFilter        = 3;
                m_pChannel[i].m_iDemodu        = 1;
                m_pChannel[i].m_pGatePos[1]    = {0.23f};
                m_pChannel[i].m_pGateWidth[0]  = 0.3;
                m_pChannel[i].m_pGateWidth[1]  = 0.3;
                m_pChannel[i].m_pGateHeight[0] = 0.8;
                m_pChannel[i].m_pGateHeight[1] = 0.5;
                m_pChannel[i].m_iGateBType     = 1;
            }
            // ��������
            m_Specimen.m_WheelParam.fWheelHub               = 300.f;
            m_Specimen.m_WheelParam.fWheelRim               = 130.f;
            m_Specimen.m_WheelParam.fWheelInnerSideOffset   = 15.f;
            m_Specimen.m_WheelParam.fWheelHubInnerDiameter  = 200.f;
            m_Specimen.m_WheelParam.fWheelHubOuterDiameter  = 450.f;
            m_Specimen.m_WheelParam.fWheelRimlInnerDiameter = 740.f;
            m_Specimen.m_WheelParam.fWheelRimOuterDiameter  = 840.f;
            m_Specimen.m_WheelParam.fTreadWidth             = 25.f;
            m_Specimen.m_WheelParam.fWheelFlangeHeight      = 60.f;

            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos    = 457.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart  = 447.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXEnd    = 420.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep   = -3.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos   = 361.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart = 463.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYEnd   = 433.f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep  = -3.3f;
            g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadLyXPos = 340.f;

            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideXSpeed1  = 8.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideXSpeed2  = 25.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideYSpeed1  = 8.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fSideYSpeed2  = 25.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadXSpeed1 = 8.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadXSpeed2 = 25.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadYSpeed1 = 8.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fTreadYSpeed2 = 25.f;
            g_MainProcess.m_ConnectPLC.mPLCSpeed.fRotateSpeed1 = 2.f;

            //	printf("�����ڸ��ļ���Ŀ¼��\n");
        } else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //	printf("%s��һ��Ŀ¼��\n",fileData.cFileName);
        } else {
            //	printf("%s��һ���ļ���\n",fileData.cFileName);
            g_MainProcess.m_Techniques.LoadTecBinary(strFind);
        }
        FindClose(hFindFile);
    }
}
void Techniques::SaveTecName(LPCTSTR lpName) {
    CString strName = lpName;
    int     res     = strName.Find(_T("LastParam"));
    if (res > 0) {
        WritePrivateProfileString(_T("CURTECH"), _T("Name"), g_MainProcess.m_Techniques.m_pCurTechName, lpName);
    }
}

bool Techniques::LoadTec(LPCTSTR lpName) {
    LoadTecName(lpName); // LastParam.ini ֻ���湤������
    return TRUE;
}

bool Techniques::SaveTec(LPCTSTR lpName) {
    SaveTecName(lpName);
    return true; // ini �������
}
bool Techniques::LoadTecBinary(LPCTSTR lpName) {
    CFile* pFile = new CFile(lpName, CFile::shareDenyWrite | CFile::typeBinary);
    if (pFile == NULL) {
        return FALSE;
    }
    if (pFile->m_hFile == INVALID_HANDLE_VALUE) {
        // Ĭ��ϵͳ����
        m_System.m_iFrequency     = 0x4b0;
        m_System.m_iVoltage       = 1;
        m_System.m_iPulseWidth    = 0xd2;
        m_System.m_iTxFlag        = 0x3ff;
        m_System.m_iRxFlag        = 0x3ff;
        m_System.m_iChMode        = 0x05ff0aff;
        m_System.m_iScanIncrement = 0;
        m_System.m_iLEDStatus     = 0;
        m_System.m_iWorkType      = 0;
        m_System.m_iControlTime   = 1;
        m_System.m_iAxleTime      = 1;
        // Ĭ��ͨ������
        m_pChannel.resize(HD_CHANNEL_NUM);
        for (size_t i = 0; i < m_pChannel.size(); i++) {
            m_pChannel[i].m_fRange         = 200;
            m_pChannel[i].m_iVelocity      = 0x172a;
            m_pChannel[i].m_fDelay         = 140;
            m_pChannel[i].m_fOffset        = 0.868325114f;
            m_pChannel[i].m_iSampleFactor  = 0xd;
            m_pChannel[i].m_fGain          = 46.f;
            m_pChannel[i].m_iFilter        = 3;
            m_pChannel[i].m_iDemodu        = 1;
            m_pChannel[i].m_pGatePos[1]    = {0.23f};
            m_pChannel[i].m_pGateWidth[0]  = 0.3;
            m_pChannel[i].m_pGateWidth[1]  = 0.3;
            m_pChannel[i].m_pGateHeight[0] = 0.8;
            m_pChannel[i].m_pGateHeight[1] = 0.5;
            m_pChannel[i].m_iGateBType     = 1;
        }
        delete pFile;
        return TRUE;
    }

    pFile->SeekToBegin();
    DWORD dwKey = 0, dwRead = 0;
    pFile->Read(&dwKey, sizeof(dwKey));
    if (dwKey == FK_NEWFILE) {
        while (pFile->Read(&dwKey, sizeof(dwKey))) { // ��ȡ��־λ

            pFile->Read(&dwRead, sizeof(dwRead));
            switch (dwKey) {
                case FK_DATA_SYSFILE: // ϵͳ������
                {
                    BYTE* Sysbyte = new BYTE[dwRead];
                    pFile->Read(Sysbyte, dwRead);
                    //	System  SystemTest;
                    m_System.Unserialize(Sysbyte, dwRead);
                    delete[] Sysbyte;
                } break;
                case FK_DATA_CHFILE: // ͨ������
                {
                    //	std::vector<Channel> pChannelTest;
                    m_pChannel.resize(dwRead);
                    for (DWORD i = 0; i < dwRead; i++) {
                        DWORD dwVerWrite = 0;
                        pFile->Read(&dwVerWrite, sizeof(dwVerWrite));
                        BYTE* CHByte = new BYTE[dwVerWrite];

                        pFile->Read(CHByte, dwVerWrite);
                        m_pChannel[i].Unserialize(CHByte, dwVerWrite);

                        delete[] CHByte;
                    }
                } break;
                case FK_DATA_SPECIFILE: // ��������
                {
                    // WHEEL_PAPA wheelParamTest;
                    //	m_Specimen.m_WheelParam

                    // ������������ʧ��
                    pFile->Read(&m_Specimen.m_WheelParam, dwRead);

                } break;
                case FK_DATA_PLCFILE: // PLC
                {
                    // PLC_SCAN_PAPA plcScanTest;
                    pFile->Read(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwRead);
                } break;
                case FK_DATA_USERFILE: // �û�����
                {
                    DB_USER_DATA userTest;
                    // pFile->Read(&g_MainProcess.m_User.mLogionUser, dwRead);
                    pFile->Read(&userTest, dwRead);

                } break;
                case FK_DATA_STDDETECTFILE: // ̽������
                {
                    dwKey = FK_DATA_STDDETECTFILE;
                    //		DETECTION_PARAM2995_200 detectParam[HD_CHANNEL_NUM];
                    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
                    pFile->Read(p->mDetetionParam2995_200, dwRead);

                    int test = 0;
                } break;
                case FK_DATA_PLCSPEED: // PLC�ٶ�
                {
                    // PLC_SCAN_PAPA plcScanTest;
                    pFile->Read(&g_MainProcess.m_ConnectPLC.mPLCSpeed, dwRead);
                }

                break;
                case FK_DATA_END: // ���ݽ���
                {
                } break;
                default: {
                    pFile->Seek(dwRead, CFile::current);
                } break;
            }
        }
    }

    pFile->Close();
    delete pFile;

    return TRUE;
}

bool Techniques::SaveTecBinary(LPCTSTR lpName) {
    CFile* pFile = new CFile(lpName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
    if (pFile == NULL) {
        return FALSE;
    }
    if (pFile->m_hFile == INVALID_HANDLE_VALUE) {
        delete pFile;
        return FALSE;
    }
    pFile->SeekToBegin();
    DWORD dwWrite = 0;
    // UTPA
    DWORD dwKey = FK_NEWFILE;            //
    pFile->Write(&dwKey, sizeof(dwKey)); // ��ʼ���

    dwKey = FK_DATA_SYSFILE;             // System ����
    pFile->Write(&dwKey, sizeof(dwKey)); // System���
    dwWrite = m_System.GetSerializeLen();
    pFile->Write(&dwWrite, sizeof(dwWrite)); // System��С
    pFile->Write(m_System.Serialize(), m_System.GetSerializeLen());

    dwKey = FK_DATA_CHFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = m_pChannel.size(); // ͨ������
    pFile->Write(&dwWrite, sizeof(dwWrite));
    for (DWORD i = 0; i < dwWrite; i++) {
        DWORD dwVerWrite = m_pChannel[i].GetSerializeLen();
        pFile->Write(&dwVerWrite, sizeof(dwVerWrite));
        pFile->Write(m_pChannel[i].Serialize(), m_pChannel[i].GetSerializeLen());
    }

    // ���ֲ��� m_WheelParam
    dwKey = FK_DATA_SPECIFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(WHEEL_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&m_Specimen.m_WheelParam, dwWrite);

    // PLC���Ʋ���
    dwKey = FK_DATA_PLCFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(PLC_SCAN_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwWrite);

    // ��¼�û�����
    dwKey = FK_DATA_USERFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DB_USER_DATA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_User.mLogionUser, dwWrite);

    // ��׼��������
    dwKey = FK_DATA_STDDETECTFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DETECTION_PARAM2995_200) * HD_CHANNEL_NUM; // ���ݸ���
    pFile->Write(&dwWrite, sizeof(dwWrite));
    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
    pFile->Write(p->mDetetionParam2995_200, dwWrite);

    // PLC���Ʋ���
    dwKey = FK_DATA_PLCSPEED;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(PLC_SPeed_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_ConnectPLC.mPLCSpeed, dwWrite);

    dwKey = FK_DATA_END;
    pFile->Write(&dwKey, sizeof(dwKey));
    pFile->Close();
    delete pFile;
    return true;
}
// ���ݻطŵ���
bool Techniques::LoadRecData(LPCTSTR lpName) {
    m_ScanType   = SCAN_REPLAY;
    CFile* pFile = new CFile(lpName, CFile::shareDenyWrite | CFile::typeBinary);
    if (pFile == NULL) {
        return FALSE;
    }
    if (pFile->m_hFile == INVALID_HANDLE_VALUE) {
        delete pFile;
        return FALSE;
    }

    pFile->SeekToBegin();
    DWORD dwKey = 0, dwRead = 0;
    pFile->Read(&dwKey, sizeof(dwKey));
    if (dwKey == FK_NEWFILE) {
        while (pFile->Read(&dwKey, sizeof(dwKey))) { // ��ȡ��־λ

            pFile->Read(&dwRead, sizeof(dwRead));
            switch (dwKey) {
                case FK_DATA_SYSFILE: // ϵͳ������
                {
                    BYTE* Sysbyte = new BYTE[dwRead];
                    pFile->Read(Sysbyte, dwRead);
                    //	System  SystemTest;
                    m_System.Unserialize(Sysbyte, dwRead);
                    delete[] Sysbyte;
                } break;
                case FK_DATA_CHFILE: // ͨ������
                {
                    //	std::vector<Channel> pChannelTest;
                    m_pChannel.resize(dwRead);
                    for (DWORD i = 0; i < dwRead; i++) {
                        DWORD dwVerWrite = 0;
                        pFile->Read(&dwVerWrite, sizeof(dwVerWrite));
                        BYTE* CHByte = new BYTE[dwVerWrite];

                        pFile->Read(CHByte, dwVerWrite);
                        m_pChannel[i].Unserialize(CHByte, dwVerWrite);

                        delete[] CHByte;
                    }
                } break;
                case FK_DATA_SPECIFILE: // ��������
                {
                    // WHEEL_PAPA wheelParamTest;
                    //	m_Specimen.m_WheelParam

                    // ������������ʧ��
                    pFile->Read(&m_Specimen.m_WheelParam, dwRead);

                } break;
                case FK_DATA_PLCFILE: // PLC
                {
                    // PLC_SCAN_PAPA plcScanTest;
                    pFile->Read(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwRead);
                } break;
                case FK_DATA_USERFILE: // �û�����
                {
                    // DB_USER_DATA userTest;
                    pFile->Read(&g_MainProcess.m_User.mLogionUser, dwRead);

                } break;
                case FK_DATA_STDDETECTFILE: // ̽������
                {
                    dwKey = FK_DATA_STDDETECTFILE;
                    //		DETECTION_PARAM2995_200 detectParam[HD_CHANNEL_NUM];
                    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
                    pFile->Read(p->mDetetionParam2995_200, dwRead);

                    int test = 0;
                } break;

                case FK_DATA_SCANFILE: // ɨ������
                {
                    Compute(); // ��ʼ �������

                    m_pRecord.clear(); // �����¼����
                    //	std::vector<RECORD_DATA> pRecordSideTest;
                    for (DWORD i = 0; i < dwRead; i++) {
                        RECORD_DATA temRecord;
                        pFile->Read(temRecord.pGatePos, sizeof(__int32) * 2 * 10);
                        pFile->Read(temRecord.pGateAmp, sizeof(__int32) * 2 * 10);
                        pFile->Read(temRecord.iAscanSize, sizeof(__int64) * 10);

                        for (int i = 0; i < HD_CHANNEL_NUM; i++) {
                            temRecord.pAscan[i].resize(temRecord.iAscanSize[i]);
                            pFile->Read(temRecord.pAscan[i].data(), temRecord.iAscanSize[i]);

                            //  std::vector<BYTE> dataBuffer(temRecord.iAscanSize[i]);
                            //     UINT bytesRead = pFile->Read(dataBuffer.data(), temRecord.iAscanSize[i]);
                            // temRecord.  pAscan[i].assign(dataBuffer.begin() , dataBuffer.begin()  + temRecord.iAscanSize[i]);
                        }
                        m_pRecord.push_back(temRecord);
                    }

                    // ���ȱ������
                    for (int i = 0; i < HD_CHANNEL_NUM; i++) {
                        std::vector<DB_DEFECT_DATA*>::iterator it = m_pDefect[i].begin();
                        while (it != m_pDefect[i].end()) {
                            delete *it;
                            *it = NULL;
                            it++;
                        }
                        m_pDefect[i].clear();
                    }

                    int test = 0;
                }

                break;
                case FK_DATA_DEFECTFILE: // ȱ������
                {
                    for (DWORD ch = 0; ch < dwRead; ch++) {
                        DWORD dwVerWrite = 0;
                        pFile->Read(&dwVerWrite, sizeof(dwVerWrite));
                        int size = dwVerWrite / sizeof(DB_DEFECT_DATA);
                        for (int i = 0; i < size; i++) {
                            DB_DEFECT_DATA* temDEFECT_DATA = new DB_DEFECT_DATA();
                            pFile->Read(temDEFECT_DATA, sizeof(DB_DEFECT_DATA)); // ���ݶ�ȡ
                            m_pDefect[ch].push_back(temDEFECT_DATA);
                        }
                        int test = 0;
                    }

                } break;
                case FK_DATA_END: // ���ݽ���
                {
                } break;
                default: {
                    pFile->Seek(dwRead, CFile::current);
                } break;
            }
        }
    }

    pFile->Close();
    delete pFile;
    return TRUE;
}
/*Ŀǰ������ݱ��� ��ʽλ
��ʼ��ʶλ��FK_NEWFILE�� +
ϵͳ���� ��ʶ��FK_DATA_SYSFILE��+ϵͳ��������С��+System����            ����System���ݲ������л� ���򻯶�д��
ͨ������ ��ʶ��FK_DATA_CHFILE��+ͨ������ + һ��Channel�Ĵ�С +Channel ����        ����Channel[10]���ݲ������л� ���򻯶�д��
��������/���ֲ��� ��ʶ ��FK_DATA_SPECIFILE��    +    WHEEL_PAPA ��С+         WHEEL_PAPA����
PLC���� ��ʶ
�����Ա���� ��ʶ

̽������ ��DETECTION_PARAM2995_200�� (FK_DATA_SCANFILE) + DETECTION_PARAM2995_200 *10 ��С+ DETECTION_PARAM2995_200��10 ������

ɨ������ ��ʶ
ȱ������ ��ʶ

*/
bool Techniques::SaveRecData(LPCTSTR lpName) {
    m_strLastSaveScanData = lpName;
    CFile* pFile          = new CFile(lpName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
    if (pFile == NULL) {
        return FALSE;
    }
    if (pFile->m_hFile == INVALID_HANDLE_VALUE) {
        delete pFile;
        return FALSE;
    }
    pFile->SeekToBegin();
    DWORD dwWrite = 0;
    // UTPA
    DWORD dwKey = FK_NEWFILE;            //
    pFile->Write(&dwKey, sizeof(dwKey)); // ��ʼ���

    dwKey = FK_DATA_SYSFILE;             // System ����
    pFile->Write(&dwKey, sizeof(dwKey)); // System���
    dwWrite = m_System.GetSerializeLen();
    pFile->Write(&dwWrite, sizeof(dwWrite)); // System��С
    pFile->Write(m_System.Serialize(), m_System.GetSerializeLen());

    dwKey = FK_DATA_CHFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = m_pChannel.size(); // ͨ������
    pFile->Write(&dwWrite, sizeof(dwWrite));
    for (DWORD i = 0; i < dwWrite; i++) {
        DWORD dwVerWrite = m_pChannel[i].GetSerializeLen();
        pFile->Write(&dwVerWrite, sizeof(dwVerWrite));
        pFile->Write(m_pChannel[i].Serialize(), m_pChannel[i].GetSerializeLen());
    }

    // ���ֲ��� m_WheelParam
    dwKey = FK_DATA_SPECIFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(WHEEL_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&m_Specimen.m_WheelParam, dwWrite);

    // PLC���Ʋ���
    dwKey = FK_DATA_PLCFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(PLC_SCAN_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwWrite);

    // ��¼�û�����
    dwKey = FK_DATA_USERFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DB_USER_DATA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_User.mLogionUser, dwWrite);

    // ��׼��������
    dwKey = FK_DATA_STDDETECTFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DETECTION_PARAM2995_200) * HD_CHANNEL_NUM; // ���ݸ���
    pFile->Write(&dwWrite, sizeof(dwWrite));
    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
    pFile->Write(p->mDetetionParam2995_200, dwWrite);

    // ɨ�������
    dwKey = FK_DATA_SCANFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = m_pRecord.size(); // ���ݸ���
    pFile->Write(&dwWrite, sizeof(dwWrite));

    for (const auto& record : m_pRecord) {
        pFile->Write(record.pGatePos, sizeof(__int32) * 10 * 2); // д�벨��λ��
        pFile->Write(record.pGateAmp, sizeof(__int32) * 10 * 2); // д�벨�Ų���
        pFile->Write(&record.iAscanSize, sizeof(__int64) * 10);  // д�벨�δ�С
        for (const auto& pAscan : record.pAscan) {
            const BYTE*  dataPtr  = pAscan.data();
            const size_t dataSize = pAscan.size();
            pFile->Write(dataPtr, dataSize); // д��pAscan�е�����
        }
    }
    // ȱ������
    dwKey = FK_DATA_DEFECTFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = 10; // 10��ͨ��
    pFile->Write(&dwWrite, sizeof(dwWrite));
    for (const auto& pDefect : m_pDefect) // 10��ͨ����ȱ��
    {
        const size_t dataSize = pDefect.size();
        dwWrite               = sizeof(DB_DEFECT_DATA) * dataSize; // ���ݸ���
        pFile->Write(&dwWrite, sizeof(dwWrite));
        for (int i = 0; i < dataSize; i++) { // ���ݱ���
            pFile->Write(pDefect[i], sizeof(DB_DEFECT_DATA));
        }
    }

    dwKey = FK_DATA_END;
    pFile->Write(&dwKey, sizeof(dwKey));
    pFile->Close();
    delete pFile;

    return TRUE;
}

DetectionStd_TBT2995_200* Techniques ::GetDetectionStd() {
    if (m_iDetectionStd == DETECTIONSTD_TBT2995_200) {
        return (DetectionStd_TBT2995_200*)_pDetectionStd;
    } else {
        return NULL;
    }
}