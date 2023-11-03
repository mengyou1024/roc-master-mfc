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

    // 缺陷数据
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

    // 创建探伤标准
    switch (iDetectionStd) {
        case DETECTIONSTD_TBT2995_200: _pDetectionStd = new DetectionStd_TBT2995_200(); break;
    }

    if (_pDetectionStd != nullptr) {
        // 根据标准初始化工艺
        _pDetectionStd->InitTechniques(this);
        m_iDetectionStd = iDetectionStd;
    } else {
        m_iDetectionStd = DETECTIONSTD_NONE;
    }
}

void Techniques::Compute() {
    // 当前成像每圈固定360个点
    m_Scan.SetSidePoints(360);
    m_Scan.SetTreadPoints(360);
    m_Scan.m_iSideSize = g_MainProcess.m_ConnectPLC.m_PlcDownParam.nBestCirSize;

    // 侧面绘图两个探头之间的圈数
    m_Scan.m_iDrawProbleSideSize = m_Specimen.m_WheelParam.fSideWidth / abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);
    // 踏面绘图两个探头之间的圈数
    m_Scan.m_iDrawProbleTreadSize = m_Specimen.m_WheelParam.fTreadWidth / abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep);

    // 侧面绘图圈数   根据 检测范围 /步进      轮辋外径（7）-轮辋内径（8）
    auto _iDrawSizeSize = (m_Specimen.m_WheelParam.fWheelRimOuterDiameter - m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) / 2.0f /
                          abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep);

    // 踏面绘图圈数 根据 检测范围/步进 踏面轮辋厚度（2）-轮缘厚度（8）
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

    // 根据优化圈数计算步进
    m_Scan.m_fSideStep = _fSizeStep;

    // 轮辋外径-内径 为侧面扫差范围
    m_Scan.SetScanRadius((m_Specimen.m_WheelParam.fWheelRimOuterDiameter - m_Specimen.m_WheelParam.fWheelRimlInnerDiameter) / 2.0f);
    // m_Scan.m_iSideSize 为 旋转圈数  m_iDrawProbleSideSize为侧面两组探头之间的间隔圈数       m_iDrawSideSize 为侧面 三个探头总共需要的圈数

    // m_pRecordTread.resize(m_iTreadPoints * m_Scan.m_iTreadSize);
    g_MainProcess.m_Techniques.m_iSideCirIndex = 0;
    m_Scan.InitScanData();
}

bool Techniques::Start() {
    m_nLastIndex = 0;
    m_fLastAngle = 0.0f;
    m_nLastCir   = 0;
    m_pRecord_Buffer.clear(); // 清除缓存
    // 数据记录 轮规一圈 不超过3000mm      不漏检0.05mm的孔   按住 一圈360  一圈不低于 7200个点 暂定7200个点
    m_pRecord.clear(); // 清除记录数据

    m_pRecord.resize(m_iSidePoints * m_Scan.m_iSideSize); // 从新分配

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
    memset(m_bNewDEFECT, 0, HD_CHANNEL_NUM);    // 开始检测所有通道都不是缺陷
    memset(m_bNewDEFECT_TS, 0, HD_CHANNEL_NUM); // 开始检测所有通道都不是透声不良缺陷

    return true;
}
bool Techniques::CreateMultipleDirectory(LPCTSTR szPath) {
    CString strDir(szPath);                               // 存放要创建的目录字符串
    if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\')) // 确保以'\'结尾以创建最后一个目录
    {
        strDir.AppendChar(_T('\\'));
    }
    vector<CString> vPath;            // 存放每一层目录字符串
    CString         strTemp;          // 一个临时变量,存放目录字符串
    bool            bSuccess = false; // 成功标志
    // 遍历要创建的字符串
    for (int i = 0; i < strDir.GetLength(); ++i) {
        if (strDir.GetAt(i) != _T('\\')) { // 如果当前字符不是'\\'
            strTemp.AppendChar(strDir.GetAt(i));
        } else {                      // 如果当前字符是'\\'
            vPath.push_back(strTemp); // 将当前层的字符串添加到数组中
            strTemp.AppendChar(_T('\\'));
        }
    }
    // 遍历存放目录的数组,创建每层目录
    vector<CString>::const_iterator vIter;
    for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) {
        if (!PathIsDirectory(*vIter)) // 判断当前目录时候存在，不存在就创建
        {
            // 如果CreateDirectory执行成功,返回true,否则返回false
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

    // 数据保存
    SYSTEMTIME stm;
    GetLocalTime(&stm);

    CString stDir;
    if (m_bDailyScan) {
        if (m_nDailyType == 0) {
            stDir.Format(_T("%sData\\Daily\\%4d%02d\\%02d\\开工检_"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay);
        } else if (m_nDailyType == 1) {
            { stDir.Format(_T("%sData\\Daily\\%4d%02d\\%02d\\完工检_"), theApp.m_pExePath, stm.wYear, stm.wMonth, stm.wDay); }
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
                       stm.wSecond); // 保存数据根据需求可以添加轴型，编号，探伤工等数据
    g_MainProcess.m_Techniques.SaveRecData(stDir + strFileName);

    m_pDefect;

    return true;
}
// 切换实时采样以及数据分析
void Techniques::Test() {}

void Techniques::Dealwith(READ_DATA& Read) {
    // 未扫查时，直接退出
    if (m_ScanType != SCAN_SCAN) {
        for (int i = 0; i < HD_CHANNEL_NUM; i++) { // 结束时判断当前是否是缺陷
            if (m_bNewDEFECT[i]) {                 // 非透声缺陷
                mDB_DEFECT_DATA[i].nParam2  = m_iTreadCirIndex * GetTreadPoint() + m_iSaveTreadPointIndex;
                DB_DEFECT_DATA* Defect_date = new DB_DEFECT_DATA(); // new DB_DEFECT_DATA 需要存储到voter 下次开始释放
                memcpy(Defect_date, &mDB_DEFECT_DATA[i], sizeof(DB_DEFECT_DATA)); // 拷贝数据到
                m_pDefect[i].push_back(Defect_date);
                m_bNewDEFECT[i] = false; // 缺陷完成

            } else {
                if (m_bNewDEFECT_TS[i]) { // 透声缺陷
                    mDB_DEFECT_DATA[i].nParam2  = m_iTreadCirIndex * GetTreadPoint() + m_iSaveTreadPointIndex;
                    DB_DEFECT_DATA* Defect_date = new DB_DEFECT_DATA(); // new DB_DEFECT_DATA 需要存储到voter 下次开始释放
                    memcpy(Defect_date, &mDB_DEFECT_DATA[i], sizeof(DB_DEFECT_DATA)); // 拷贝数据到
                    m_pDefect[i].push_back(Defect_date);
                    m_bNewDEFECT_TS[i] = false;
                }
            }
        }
        return;
    }
    // printf("Dealwith%.2f\n", Read.fAngle);
    // 角度有变化时，圈数增加或减小
    int iAngleSection(0);
    if (Read.fAngle < ANGLE_LEFT)
        iAngleSection = -1;
    else if (Read.fAngle > ANGLE_RIGHT)
        iAngleSection = 1;
    else
        iAngleSection = 0;
    if (_AngleSection * iAngleSection < 0) // 经历了 -1 到1的变化
    {
        if (iAngleSection < 0) {
            m_iSideCirIndex++;
        } else if (iAngleSection > 0) {
            m_iSideCirIndex--;
        }
    }
    _AngleSection = iAngleSection;

    // 成像数据点
    m_iSidePointIndex  = int(Read.fAngle / m_Scan.GetSideAngleStep()) % m_Scan.GetSidePoint();
    m_iTreadPointIndex = int(Read.fAngle / m_Scan.GetTreadAngleStep()) % m_Scan.GetTreadPoint();
    // 数据记录
    m_iSaveSidePointIndex  = int(Read.fAngle / GetSideAngleStep()) % GetSidePoint();
    m_iSaveTreadPointIndex = int(Read.fAngle / GetTreadAngleStep()) % GetTreadPoint();
    if (m_iSaveSidePointIndex >= m_pRecord.size()) {
        int tem = 0;
    }
    if (m_iSaveTreadPointIndex >= m_pRecord.size()) {
        int tem = 0;
    }

    if (m_iSideCirIndex == 0 && Read.fAngle < 0.5f) { // 第一圈 角度大于0.5才表示转盘 盘启动
        return;
    }

    // 圈数开始 并且 小于最小圈数
    if (m_iSideCirIndex >= 0 && m_iSideCirIndex < m_Scan.m_iSideSize) // 扫差过程 未到最后一圈 m_iSideSize 与 m_iTreadSize 一样
    {
        int res    = _pDetectionStd->Detection(this, Read);                    // 进入判伤以及 成像数据处理
        int iIndex = m_iSideCirIndex * GetSidePoint() + m_iSaveSidePointIndex; // 缓存结束索引
        // if (abs(m_fLastAngle - Read.fAngle) > EPS) { //角度不同   将缓存帧数 取最大缺陷分配到 存储数据和成像中
        if ((iIndex - m_nLastIndex) > 0) {
            //     int iLastIndex = m_nLastCir* GetSidePoint()+ int(m_fLastAngle / GetSideAngleStep()) % GetSidePoint();//缓存开始索引

            TRACE("%d_%df_%d\n", m_nLastIndex, iIndex);
            float nBufferScale = 1.0f * m_pRecord_Buffer.size() / (iIndex - m_nLastIndex);
            for (int i = m_nLastIndex; i < iIndex; i++) // 对间隔缺少的帧数从缓存中补上
            {
                float maxDb = 0;
                int   maxj  = (i - m_nLastIndex) * nBufferScale;
                for (int j = (i - m_nLastIndex) * nBufferScale; j < (i - m_nLastIndex + 1) * nBufferScale; j++) { // 找缓存缺陷最大波

                    if (j < m_pRecord_Buffer.size()) {
                        for (int ichannel = 0; ichannel < HD_CHANNEL_NUM; ichannel++) { // 缺陷最大 db正值
                            if (m_pRecord_Buffer[j]->bDefectType[ichannel] == 0) {
                                if (m_pRecord_Buffer[j]->fDb[ichannel] > maxDb) {
                                    maxDb = m_pRecord_Buffer[j]->fDb[ichannel];
                                    maxj  = j;
                                }

                            } else if (m_pRecord_Buffer[j]->bDefectType[ichannel] == 1) { // 透声缺陷最大 dB为负值最小
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
                for (int ichannel = 0; ichannel < HD_CHANNEL_NUM; ichannel++) { // 记录数据
                    if (maxj < m_pRecord_Buffer.size()) {                       // 保存最高波

                        // if(mDB_DEFECT_DATA[ichannel].nDBOffset!=0) //更新缺陷索引
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
            //	m_pRecord_Buffer.clear();//清除缓存

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
            m_fLastAngle = Read.fAngle; // 更新上一次角度
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
        GetPrivateProfileString(_T("CURTECH"), _T("Name"), _T("无"), pBuf, 256, lpName);
        StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, pBuf);
        CString strFind;
        strFind.Format(_T("%sTechINI\\%s.tech"), theApp.m_pExePath, g_MainProcess.m_Techniques.m_pCurTechName);

        WIN32_FIND_DATA fileData;
        HANDLE          hFindFile = FindFirstFile(strFind, &fileData);
        if (INVALID_HANDLE_VALUE == hFindFile) { // 文件不存在初化通道
            // TODO: 文件不存在时使用默认参数
            // 默认系统参数
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
            // 默认通道参数
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
            // 其他参数
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

            //	printf("不存在该文件或目录！\n");
        } else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            //	printf("%s是一个目录！\n",fileData.cFileName);
        } else {
            //	printf("%s是一个文件！\n",fileData.cFileName);
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
    LoadTecName(lpName); // LastParam.ini 只保存工艺名称
    return TRUE;
}

bool Techniques::SaveTec(LPCTSTR lpName) {
    SaveTecName(lpName);
    return true; // ini 保存废弃
}
bool Techniques::LoadTecBinary(LPCTSTR lpName) {
    CFile* pFile = new CFile(lpName, CFile::shareDenyWrite | CFile::typeBinary);
    if (pFile == NULL) {
        return FALSE;
    }
    if (pFile->m_hFile == INVALID_HANDLE_VALUE) {
        // 默认系统参数
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
        // 默认通道参数
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
        while (pFile->Read(&dwKey, sizeof(dwKey))) { // 读取标志位

            pFile->Read(&dwRead, sizeof(dwRead));
            switch (dwKey) {
                case FK_DATA_SYSFILE: // 系统参数表
                {
                    BYTE* Sysbyte = new BYTE[dwRead];
                    pFile->Read(Sysbyte, dwRead);
                    //	System  SystemTest;
                    m_System.Unserialize(Sysbyte, dwRead);
                    delete[] Sysbyte;
                } break;
                case FK_DATA_CHFILE: // 通道参数
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
                case FK_DATA_SPECIFILE: // 工件参数
                {
                    // WHEEL_PAPA wheelParamTest;
                    //	m_Specimen.m_WheelParam

                    // 工件参数保存失败
                    pFile->Read(&m_Specimen.m_WheelParam, dwRead);

                } break;
                case FK_DATA_PLCFILE: // PLC
                {
                    // PLC_SCAN_PAPA plcScanTest;
                    pFile->Read(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwRead);
                } break;
                case FK_DATA_USERFILE: // 用户数据
                {
                    DB_USER_DATA userTest;
                    // pFile->Read(&g_MainProcess.m_User.mLogionUser, dwRead);
                    pFile->Read(&userTest, dwRead);

                } break;
                case FK_DATA_STDDETECTFILE: // 探伤数据
                {
                    dwKey = FK_DATA_STDDETECTFILE;
                    //		DETECTION_PARAM2995_200 detectParam[HD_CHANNEL_NUM];
                    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
                    pFile->Read(p->mDetetionParam2995_200, dwRead);

                    int test = 0;
                } break;
                case FK_DATA_PLCSPEED: // PLC速度
                {
                    // PLC_SCAN_PAPA plcScanTest;
                    pFile->Read(&g_MainProcess.m_ConnectPLC.mPLCSpeed, dwRead);
                }

                break;
                case FK_DATA_END: // 数据结束
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
    pFile->Write(&dwKey, sizeof(dwKey)); // 开始标记

    dwKey = FK_DATA_SYSFILE;             // System 参数
    pFile->Write(&dwKey, sizeof(dwKey)); // System标记
    dwWrite = m_System.GetSerializeLen();
    pFile->Write(&dwWrite, sizeof(dwWrite)); // System大小
    pFile->Write(m_System.Serialize(), m_System.GetSerializeLen());

    dwKey = FK_DATA_CHFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = m_pChannel.size(); // 通道个数
    pFile->Write(&dwWrite, sizeof(dwWrite));
    for (DWORD i = 0; i < dwWrite; i++) {
        DWORD dwVerWrite = m_pChannel[i].GetSerializeLen();
        pFile->Write(&dwVerWrite, sizeof(dwVerWrite));
        pFile->Write(m_pChannel[i].Serialize(), m_pChannel[i].GetSerializeLen());
    }

    // 车轮参数 m_WheelParam
    dwKey = FK_DATA_SPECIFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(WHEEL_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&m_Specimen.m_WheelParam, dwWrite);

    // PLC控制参数
    dwKey = FK_DATA_PLCFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(PLC_SCAN_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwWrite);

    // 登录用户参数
    dwKey = FK_DATA_USERFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DB_USER_DATA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_User.mLogionUser, dwWrite);

    // 标准判伤数据
    dwKey = FK_DATA_STDDETECTFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DETECTION_PARAM2995_200) * HD_CHANNEL_NUM; // 数据个数
    pFile->Write(&dwWrite, sizeof(dwWrite));
    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
    pFile->Write(p->mDetetionParam2995_200, dwWrite);

    // PLC控制参数
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
// 数据回放调用
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
        while (pFile->Read(&dwKey, sizeof(dwKey))) { // 读取标志位

            pFile->Read(&dwRead, sizeof(dwRead));
            switch (dwKey) {
                case FK_DATA_SYSFILE: // 系统参数表
                {
                    BYTE* Sysbyte = new BYTE[dwRead];
                    pFile->Read(Sysbyte, dwRead);
                    //	System  SystemTest;
                    m_System.Unserialize(Sysbyte, dwRead);
                    delete[] Sysbyte;
                } break;
                case FK_DATA_CHFILE: // 通道参数
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
                case FK_DATA_SPECIFILE: // 工件参数
                {
                    // WHEEL_PAPA wheelParamTest;
                    //	m_Specimen.m_WheelParam

                    // 工件参数保存失败
                    pFile->Read(&m_Specimen.m_WheelParam, dwRead);

                } break;
                case FK_DATA_PLCFILE: // PLC
                {
                    // PLC_SCAN_PAPA plcScanTest;
                    pFile->Read(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwRead);
                } break;
                case FK_DATA_USERFILE: // 用户数据
                {
                    // DB_USER_DATA userTest;
                    pFile->Read(&g_MainProcess.m_User.mLogionUser, dwRead);

                } break;
                case FK_DATA_STDDETECTFILE: // 探伤数据
                {
                    dwKey = FK_DATA_STDDETECTFILE;
                    //		DETECTION_PARAM2995_200 detectParam[HD_CHANNEL_NUM];
                    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
                    pFile->Read(p->mDetetionParam2995_200, dwRead);

                    int test = 0;
                } break;

                case FK_DATA_SCANFILE: // 扫差数据
                {
                    Compute(); // 初始 成像参数

                    m_pRecord.clear(); // 清除记录数据
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

                    // 情况缺陷数据
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
                case FK_DATA_DEFECTFILE: // 缺陷数据
                {
                    for (DWORD ch = 0; ch < dwRead; ch++) {
                        DWORD dwVerWrite = 0;
                        pFile->Read(&dwVerWrite, sizeof(dwVerWrite));
                        int size = dwVerWrite / sizeof(DB_DEFECT_DATA);
                        for (int i = 0; i < size; i++) {
                            DB_DEFECT_DATA* temDEFECT_DATA = new DB_DEFECT_DATA();
                            pFile->Read(temDEFECT_DATA, sizeof(DB_DEFECT_DATA)); // 数据读取
                            m_pDefect[ch].push_back(temDEFECT_DATA);
                        }
                        int test = 0;
                    }

                } break;
                case FK_DATA_END: // 数据结束
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
/*目前检测数据保存 格式位
开始标识位（FK_NEWFILE） +
系统参数 标识（FK_DATA_SYSFILE）+系统参数（大小）+System数据            （这System数据采样序列化 反序化读写）
通道参数 标识（FK_DATA_CHFILE）+通道个数 + 一个Channel的大小 +Channel 数据        （这Channel[10]数据采样序列化 反序化读写）
工件参数/车轮参数 标识 （FK_DATA_SPECIFILE）    +    WHEEL_PAPA 大小+         WHEEL_PAPA数据
PLC参数 标识
检测人员数据 标识

探伤数据 （DETECTION_PARAM2995_200） (FK_DATA_SCANFILE) + DETECTION_PARAM2995_200 *10 大小+ DETECTION_PARAM2995_200【10 】数据

扫差数据 标识
缺陷数据 标识

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
    pFile->Write(&dwKey, sizeof(dwKey)); // 开始标记

    dwKey = FK_DATA_SYSFILE;             // System 参数
    pFile->Write(&dwKey, sizeof(dwKey)); // System标记
    dwWrite = m_System.GetSerializeLen();
    pFile->Write(&dwWrite, sizeof(dwWrite)); // System大小
    pFile->Write(m_System.Serialize(), m_System.GetSerializeLen());

    dwKey = FK_DATA_CHFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = m_pChannel.size(); // 通道个数
    pFile->Write(&dwWrite, sizeof(dwWrite));
    for (DWORD i = 0; i < dwWrite; i++) {
        DWORD dwVerWrite = m_pChannel[i].GetSerializeLen();
        pFile->Write(&dwVerWrite, sizeof(dwVerWrite));
        pFile->Write(m_pChannel[i].Serialize(), m_pChannel[i].GetSerializeLen());
    }

    // 车轮参数 m_WheelParam
    dwKey = FK_DATA_SPECIFILE; // Channel [10]
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(WHEEL_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&m_Specimen.m_WheelParam, dwWrite);

    // PLC控制参数
    dwKey = FK_DATA_PLCFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(PLC_SCAN_PAPA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_ConnectPLC.m_PlcDownParam, dwWrite);

    // 登录用户参数
    dwKey = FK_DATA_USERFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DB_USER_DATA);
    pFile->Write(&dwWrite, sizeof(dwWrite));
    pFile->Write(&g_MainProcess.m_User.mLogionUser, dwWrite);

    // 标准判伤数据
    dwKey = FK_DATA_STDDETECTFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = sizeof(DETECTION_PARAM2995_200) * HD_CHANNEL_NUM; // 数据个数
    pFile->Write(&dwWrite, sizeof(dwWrite));
    DetectionStd_TBT2995_200* p = (DetectionStd_TBT2995_200*)(g_MainProcess.m_Techniques._pDetectionStd);
    pFile->Write(p->mDetetionParam2995_200, dwWrite);

    // 扫差查数据
    dwKey = FK_DATA_SCANFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = m_pRecord.size(); // 数据个数
    pFile->Write(&dwWrite, sizeof(dwWrite));

    for (const auto& record : m_pRecord) {
        pFile->Write(record.pGatePos, sizeof(__int32) * 10 * 2); // 写入波门位置
        pFile->Write(record.pGateAmp, sizeof(__int32) * 10 * 2); // 写入波门波幅
        pFile->Write(&record.iAscanSize, sizeof(__int64) * 10);  // 写入波形大小
        for (const auto& pAscan : record.pAscan) {
            const BYTE*  dataPtr  = pAscan.data();
            const size_t dataSize = pAscan.size();
            pFile->Write(dataPtr, dataSize); // 写入pAscan中的数据
        }
    }
    // 缺陷数据
    dwKey = FK_DATA_DEFECTFILE;
    pFile->Write(&dwKey, sizeof(dwKey));
    dwWrite = 10; // 10个通道
    pFile->Write(&dwWrite, sizeof(dwWrite));
    for (const auto& pDefect : m_pDefect) // 10个通道的缺陷
    {
        const size_t dataSize = pDefect.size();
        dwWrite               = sizeof(DB_DEFECT_DATA) * dataSize; // 数据个数
        pFile->Write(&dwWrite, sizeof(dwWrite));
        for (int i = 0; i < dataSize; i++) { // 数据保存
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