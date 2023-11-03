#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"

// TODO: 暂时屏蔽警告
#pragma warning( disable: 4267 4244 4552 )


bool DetectionStd_TBT2995_200::InitTechniques(Techniques* pTechniques) {
    float res = mDacCount.CoutdBOffsetByAmp(0.8f, 0.4f);
    int   a   = 0;

    return true;
}
// 计算DAC  nShowdB 设置界面使用当前增益  扫差界面使用探伤增益
void DetectionStd_TBT2995_200::CountDAC_Scan(Techniques* pTechniques, int ch, float nShowdB, float fGateAmpPos) {
    Channel pCurCh = pTechniques->m_pChannel[ch];
    m_DAC_RL_Scan[ch].clear(); // 判废线
    DETECTION_PARAM2995_200 temDetetionParam2995_200;
    memcpy(&temDetetionParam2995_200, &mDetetionParam2995_200[ch], sizeof(DETECTION_PARAM2995_200));
    // 动态更新DAC 根据表面波调整 孔位置
    for (size_t i = 0; i < mDetetionParam2995_200[ch].nUseHole; i++) {
        temDetetionParam2995_200.fDAC_HoleDepth[i] = fGateAmpPos + mDetetionParam2995_200[ch].fDAC_HoleDepth[i];
    }

    mDacCount.CoutDacCurve(&temDetetionParam2995_200, nShowdB + temDetetionParam2995_200.fDAC_LineRL, pCurCh.m_fDelay, pCurCh.m_fRange, m_DAC_RL_Scan[ch]);
}

void DetectionStd_TBT2995_200::CountDAC(Techniques* pTechniques, int ch, float nShowdB, float fGateAmpPos) {

    Channel pCurCh = pTechniques->m_pChannel[ch];
    m_DAC_M[ch].clear();
    m_DAC_RL[ch].clear(); // 判废线
    DETECTION_PARAM2995_200 temDetetionParam2995_200;
    memcpy(&temDetetionParam2995_200, &mDetetionParam2995_200[ch], sizeof(DETECTION_PARAM2995_200));
    // 动态更新DAC 根据表面波调整 孔位置
    for (size_t i = 0; i < mDetetionParam2995_200[ch].nUseHole; i++) {
        temDetetionParam2995_200.fDAC_HoleDepth[i] = fGateAmpPos + mDetetionParam2995_200[ch].fDAC_HoleDepth[i];
    }

    mDacCount.CoutDacCurve(&temDetetionParam2995_200, nShowdB + temDetetionParam2995_200.fDAC_LineRL, pCurCh.m_fDelay, pCurCh.m_fRange, m_DAC_RL[ch]);

    m_DAC_ED[ch].clear(); // 评定线
    mDacCount.CoutDacCurve(&temDetetionParam2995_200, nShowdB + temDetetionParam2995_200.fDAC_LineRD, pCurCh.m_fDelay, pCurCh.m_fRange, m_DAC_ED[ch]);
}

// 判伤逻辑
//  1. A门最高波后面5mm（踏面 ，侧面从0开始）   至 B门最高波 前面 5mm
// 2. 踏面 （1-6 ）A门卡主水层波 便于计算 缺陷位置
// 3.侧面 （7-10）A 门 暂无作用  b门 卡主地波 便于计算 透声

// 中间的最高波与对应的DAC 相比  记录 缺陷数据
bool DetectionStd_TBT2995_200::Detection(Techniques* pTechniques, READ_DATA& Read) {
    float fAaxAmp_0 = 35; // 峰值降到最低距离 暂定10mm 波宽的干扰距离 后面取 Amen峰值下降沿10%
    float f0_AaxAmp = 5;  // 0 dao  峰值到最低距离 暂定5 波宽的干扰距离

    UCHAR* pCscanData = &pTechniques->m_Scan.m_pCscanData[pTechniques->m_iSideCirIndex * pTechniques->m_Scan.GetSidePoint()];
    UCHAR* pCscanCH   = &pTechniques->m_Scan.m_pCscanCH[pTechniques->m_iSideCirIndex * pTechniques->m_Scan.GetSidePoint()];
    for (int i = 0; i < 2; i++) {
        int size1 = pTechniques->m_Scan.m_pCscanCH.size();
        int size2 = pTechniques->m_iSideCirIndex + pTechniques->m_Scan.m_iDrawProbleSideSize;
        //	TRACE("%d_%d_%d\n", size1, size2, pTechniques->m_iSideCirIndex);
        if (pTechniques->m_iSideCirIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize < pTechniques->m_Scan.m_iDrawSideSize) {
            if (pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetSidePoint()] == 0) { // 未检测时才设置为检测状态
                pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetSidePoint()] = 11;   // 有检测无缺陷为11
            }
        }
    }

    UCHAR* pBscanData = &pTechniques->m_Scan.m_pBscanData[pTechniques->m_iTreadCirIndex * pTechniques->m_Scan.GetTreadPoint()];
    UCHAR* pBscanCH   = &pTechniques->m_Scan.m_pBscanCH[pTechniques->m_iTreadCirIndex * pTechniques->m_Scan.GetTreadPoint()];
    //	printf("m_iTreadPointIndex%d\n", m_iTreadPointIndex);
    // if (pBscanCH[pTechniques->m_iTreadPointIndex] == 0) { //未检测时才设置为检测状态
    //	pBscanCH[pTechniques->m_iTreadPointIndex] = 11; //有检测无缺陷为11
    //}
    // 第二组
    for (int i = 0; i < 3; i++) {
        if (pTechniques->m_iTreadCirIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize < pTechniques->m_Scan.m_iDrawTreadSize) {
            //	if (pTechniques->m_iTreadPointIndex +i* pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pBscanCH.size()) {
            if (pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] == 0) { // 未检测时才设置为检测状态
                pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = 11;   // 有检测无缺陷为11
            }
        }
    }

    // memset(&recod_buffer, 0, sizeof(RECORD_DATA_RES));

    RECORD_DATA_RES* recod_buffer = new RECORD_DATA_RES;
    // if (abs(pTechniques->m_fLastAngle - Read.fAngle)<= EPS) { //判断角度是否相等 <0.00001
    // TRACE("%xxx0.2f_%0.2f\n", pTechniques->m_fLastAngle, Read.fAngle);
    if (pTechniques->m_nLastIndex == pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) {
        for (int i = 0; i < HD_CHANNEL_NUM; i++) {
            recod_buffer->bDefectType[i] = -1; // 默认-1 无缺陷

            recod_buffer->pGatePos[i][GATE_A] = Read.pGatePos[i][GATE_A];
            recod_buffer->pGatePos[i][GATE_B] = Read.pGatePos[i][GATE_B];
            recod_buffer->pGateAmp[i][GATE_A] = Read.pGateAmp[i][GATE_A];
            recod_buffer->pGateAmp[i][GATE_B] = Read.pGateAmp[i][GATE_B];
            size_t size                       = Read.pAscan[i].size();
            recod_buffer->iAscanSize[i]       = size;
            recod_buffer->pAscan[i].resize(size);
            memcpy(recod_buffer->pAscan[i].data(), Read.pAscan[i].data(), size); // A扫数据
        }

        pTechniques->m_pRecord_Buffer.push_back(recod_buffer);
    } else {
        delete recod_buffer;
        recod_buffer = NULL;
    }

    for (int i = 0; i < 6; i++) // 踏面 6个
    {
        int      ichannel = pTechniques->m_Scan.m_pTreadProbe[i];
        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        //	if (pTechniques->m_iTreadPointIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() >= pTechniques->m_Scan.m_pBscanCH.size())
        if (pTechniques->m_iTreadCirIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize >= pTechniques->m_Scan.m_iDrawTreadSize) {
            continue; // 当前圈数加上 探头组间隔的圈数超出成像总圈数跳过
        }
        if (i == 2) {
            int test = 0;
        }
        // 实时计算当前帧的DAC
        CountDAC_Scan(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);
        //	CountDAC(pTechniques, ichannel, pChannel->m_fGain,0);
        // 计算 A门峰值下降沿 <10%的位置 A门峰值>25% 才去找下降沿10% 否则使用A门峰值
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25f) {                                                                                              // 大于15%才找下降沿 <10的位置作为开始检测点
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // 实际采样点是大于声程 的
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A门最高波在帧中的位置
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // 计算出的位置为A门下降沿10%大于 A门峰值位置，小于总长度
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }
        // A B 门无确定波幅参考  A 门位 0 门宽1   b 门位 99 门宽1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f; // 波门A 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置

        //	if (fStartPos > fStartAmpDown10)  //A门峰值后面10mm与 Amen峰值下降沿10%比较
        fStartPos     = fStartAmpDown10;
        float fEndPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // 波门B 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
        // A门最高波加上B门位置为检查宽度
        fEndPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // 实际距离mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL_Scan[ichannel].size(); // DAC 的大小和声程一样

        // 查找 fStartPos- fEndPos的最高波
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // 实际采样点是大于声程 的
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // 计算实际采样的起始点
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // 计算实际采样的起始点
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // 合理的数据才进行缺陷处理
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // 在A扫的最高波点数
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            int fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // 计算出波幅最高的声程位置

            float fDAC = 0;
            if (m_DAC_RL_Scan[ichannel].size() > fMaxAmpPos) {
                fDAC = m_DAC_RL_Scan[ichannel][fMaxAmpPos];
            }
            float fDB = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // 计算出当前波幅 与DAC的dB差

            // 缺陷段判断已经数据记录
            if (fDB > 0) {                                  // 为缺陷 不断更新nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // 第一次发现缺陷的帧 原来为false 状态

                    pTechniques->mDB_DEFECT_DATA[ichannel].nIndex  = pTechniques->m_pDefect[ichannel].size();
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // 设置为缺陷

                if (pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset < fDB) { // 判断最高dB 更新相关数据

                    pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset    = fDB;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCircleIndex = pTechniques->m_iTreadCirIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex   = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCH          = ichannel;
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szDetectionArea, _T("踏面"));
                    CString strName;
                    strName.Format(_T("踏面A%d"), ichannel + 1);
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szProbeMake, strName);

                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleYPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart + g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep * pTechniques->m_iTreadCirIndex; // 踏面Y开始位置+圈数*步进 nCH 0-5 为踏面位置
                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleXPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos;                                                                                           // 踏面X位置
                    //        nCH 0-5 为踏面位置     6-9为侧面位置
                    pTechniques->mDB_DEFECT_DATA[ichannel].nRadialDistance = fMaxAmpPos - (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f; // 径向距离  //踏面探头为 缺陷深度 (最高波-A门表面波)
                    pTechniques->mDB_DEFECT_DATA[ichannel].nAxialDepth     = abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep * pTechniques->m_iTreadCirIndex) +
                                                                         (ichannel / 2) * g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth; // 轴向深度    //踏面探头为 探头位置 步进距离+第几组探头*探头中心距离
                    pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle = Read.fAngle;                                                                    ////缺陷角度

                    pTechniques->mDB_DEFECT_DATA[ichannel].nWaveHeight   = fMaxAmp;                                        // int  nWaveHeight;  //波高 0-255
                    pTechniques->mDB_DEFECT_DATA[ichannel].nSensitivity  = mDetetionParam2995_200[ichannel].fScanGain;     // 探伤灵敏度
                    pTechniques->mDB_DEFECT_DATA[ichannel].nTranslucency = mDetetionParam2995_200[ichannel].fScanTrUTGain; // 探伤灵敏度
                    pTechniques->mDB_DEFECT_DATA[ichannel].bDefectType   = 0;                                              // 0 缺陷 1. 透声不良
                    if (recod_buffer != NULL) {
                        recod_buffer->fDb[ichannel]         = fDB;
                        recod_buffer->bDefectType[ichannel] = 0; // 缓存缺陷大小
                    }
                }
            } else {
                if (pTechniques->m_bNewDEFECT[ichannel] &&
                    (pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) != pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex) { // 原来有缺陷 变成无缺陷 不是同一个地方
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA 需要存储到voter 下次开始释放
                    memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // 拷贝数据到
                    pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                    pTechniques->m_bNewDEFECT[ichannel] = false; // 缺陷完成
                    printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                    memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // 一个缺陷结束 回复默认
                } else {                                                                        // 踏面不判断透声
                }
            }
        }

        // 踏面绘图数据
        if (pBscanData[pTechniques->m_iTreadPointIndex] < fMaxAmp) // 成像角度记录的进帧数 缺陷值小于 当前读取的帧数 缺陷值 覆盖数据
        {
            pBscanData[pTechniques->m_iTreadPointIndex] = fMaxAmp; // 缺陷波数据 瑞铁不对波幅成像 直对有无缺陷

            if (pTechniques->m_bNewDEFECT[ichannel]) { // 为缺陷

                if (pTechniques->m_iTreadCirIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize < pTechniques->m_Scan.m_iDrawTreadSize)                           // 当前圈数加上 探头组间隔的圈数小于成像总圈数
                {                                                                                                                                                                 // 2个探头一组有缺陷放置到对应的位置
                    pBscanCH[pTechniques->m_iTreadPointIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1; // 0 默认0 标识无缺陷  有缺陷显示通道号
                }
            }
        }
        int iIndex = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
        //	if ( m_pRecord[iIndex].iAscanSize[ichannel].size()>0)//有数据的情况下判断是否有更大单位缺陷 有择覆盖
        //{  //踏面记录数据
        //
        //	printf("%d,%d\n", pTechniques->m_iTreadPointIndex, iIndex);
        //
        //	pTechniques->m_pRecord[iIndex].pGatePos[ichannel][GATE_A] = Read.pGatePos[ichannel][GATE_A];
        //	pTechniques->m_pRecord[iIndex].pGatePos[ichannel][GATE_B] = Read.pGatePos[ichannel][GATE_B];
        //	pTechniques->m_pRecord[iIndex].pGateAmp[ichannel][GATE_A] = Read.pGateAmp[ichannel][GATE_A];
        //	pTechniques->m_pRecord[iIndex].pGateAmp[ichannel][GATE_B] = Read.pGateAmp[ichannel][GATE_B];
        //	size_t size = Read.pAscan[ichannel].size();
        //	pTechniques->m_pRecord[iIndex].iAscanSize[ichannel] = size;
        //	pTechniques->m_pRecord[iIndex].pAscan[ichannel].resize(size);
        //	memcpy(pTechniques->m_pRecord[iIndex].pAscan[ichannel].data(), Read.pAscan[ichannel].data(), size); //A扫数据
        //}
    }

    // 侧面数据处理

    for (int i = 0; i < 4; i++) // 侧面四个
    {
        int ichannel = pTechniques->m_Scan.m_pSideProbe[i];

        // 超过检测范围 跳过不判断
        //	if (pTechniques->m_iSidePointIndex + ((ichannel- pTechniques->m_Scan.m_pSideProbe[0]) / 2) * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetSidePoint() >= pTechniques->m_Scan.m_pCscanCH.size())
        if (pTechniques->m_iSideCirIndex + ((ichannel - pTechniques->m_Scan.m_pSideProbe[0])) / 2 * pTechniques->m_Scan.m_iDrawProbleSideSize >= pTechniques->m_Scan.m_iDrawSideSize) {
            continue; // 当前圈数加上 探头组间隔的圈数超出成像总圈数跳过
        }

        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        // 侧面暂时不需要动态DAC
        //	CountDAC(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);

        // 计算 A门峰值下降沿 <10%的位置 A门峰值>25% 才去找下降沿10% 否则使用A门峰值
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        float GatePosA        = pChannel->m_pGatePos[0] * pChannel->m_fRange;
        if (fStartAmpDown10 < GatePosA) {
        }

        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25) {                                                                                               // 大于15%才找下降沿 <10的位置作为开始检测点
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // 实际采样点是大于声程 的
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A门最高波在帧中的位置
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // 计算出的位置为A门下降沿10%大于 A门峰值位置，小于总长度
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }

        // A B 门无确定波幅参考  A 门位 0 门宽1   b 门位 99 门宽1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + fAaxAmp_0; // 波门A 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
        // fStartPos = fStartAmpDown10;

        float fEndPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // 波门B 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
        fEndPos       = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // 实际距离mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL[ichannel].size(); // DAC 的大小和声程一样

        // 查找 fStartPos- fEndPos的最高波
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // 实际采样点是大于声程 的
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // 计算实际采样的起始点
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // 计算实际采样的起始点
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // bool bSideMaxDefet = false;
        float fDB = 0;
        // 合理的数据才进行缺陷处理
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // 在A扫的最高波点数
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            float fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // 计算出波幅最高的声程位置
            float fDAC       = m_DAC_RL[ichannel][fMaxAmpPos];
            fDB              = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // 计算出当前波幅 与DAC的dB差

            // 缺陷段判断已经数据记录
            if (fDB > 0) {                                  // 为缺陷 不断更新nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // 第一次发现缺陷的帧 原来为false 状态

                    pTechniques->mDB_DEFECT_DATA[ichannel].nIndex  = pTechniques->m_pDefect[ichannel].size();
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // 设置为缺陷

                if (pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset < fDB) { // 判断最高dB 更新相关数据
                    pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset    = fDB;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCircleIndex = pTechniques->m_iTreadCirIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex   = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCH          = ichannel;
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szDetectionArea, _T("侧面"));
                    CString strName;
                    strName.Format(_T("侧面B%d"), ichannel + 1);
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szProbeMake, strName);

                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleYPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos;   // 侧面Y位置
                    ;                                                                                                           // 侧面Y开始位置+圈数*步进 nCH 0-5 为踏面位置
                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleXPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart; // 侧面X位置，开始位置+圈数*步进
                    +g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex;
                    //        nCH 0-5 为踏面位置     6-9为侧面位置
                    pTechniques->mDB_DEFECT_DATA[ichannel].nRadialDistance = abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex) +
                                                                             ((ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2) * g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth; // 径向距离  //侧面探头为  探头 步进距离+第几组*探头中心距离
                    pTechniques->mDB_DEFECT_DATA[ichannel].nAxialDepth  = fMaxAmpPos;                                                                                                                // 轴向深度    //侧面探头为  缺陷深度
                    pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle = Read.fAngle;                                                                                                               ////缺陷角度

                    pTechniques->mDB_DEFECT_DATA[ichannel].nWaveHeight   = fMaxAmp;                                        // int  nWaveHeight;  //波高 0-255
                    pTechniques->mDB_DEFECT_DATA[ichannel].nSensitivity  = mDetetionParam2995_200[ichannel].fScanGain;     // 探伤灵敏度
                    pTechniques->mDB_DEFECT_DATA[ichannel].nTranslucency = mDetetionParam2995_200[ichannel].fScanTrUTGain; // 探伤灵敏度
                    pTechniques->mDB_DEFECT_DATA[ichannel].bDefectType   = 0;                                              // 0 缺陷 1. 透声不良
                    if (recod_buffer != NULL) {
                        recod_buffer->fDb[ichannel]         = fDB; // 缓存缺陷大小
                        recod_buffer->bDefectType[ichannel] = 0;   // 缓存缺陷类型
                    }
                }
            } else {
                if (pTechniques->m_bNewDEFECT[ichannel] &&
                    (pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) != pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex) { // 原来有缺陷 变成无缺陷
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA 需要存储到voter 下次开始释放
                    memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // 拷贝数据到
                    pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                    pTechniques->m_bNewDEFECT[ichannel] = false; // 缺陷完成
                    printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                    memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // 一个缺陷结束 回复默认
                } else {                                                                        // 踏面不判断透声
                }
            }
        }

        // 透声缺陷判断 侧面直探头判断透声 false 不开启透声检测

        if ((ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[0] || ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[1]) && false) {
            int nGate_Bamp = Read.pGateAmp[ichannel][GATE_B];
            int nGate_BPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
            // 计算透声灵敏度下的新峰值
            int nAmpTS = mDacCount.CoutDACNewPeek(mDetetionParam2995_200[ichannel].fScanGain + mDetetionParam2995_200[ichannel].fScanCompensate,
                                                  mDetetionParam2995_200[ichannel].fScanCompensate + mDetetionParam2995_200[ichannel].fScanTrUTCompensate,
                                                  nGate_Bamp); // 根据当前的探伤增益 计算出 透声增益下的波幅。

            // 透声缺陷的判断，
            // 1首先判断波幅是否降到50%下
            // 2.判断是否有非透声缺陷
            // 3.没有判断透声缺陷
            // 4.发现非透声缺陷 立刻结束透声缺陷判断
            //	if (nAmpTS < 255 * 0.5)// 波幅低于50%；透声不良
            {
                if (nAmpTS < 2)
                    nAmpTS = 2;                                               // 最小值不为0
                float fdBTS = mDacCount.CoutdBOffsetByAmp(nAmpTS, 255 * 0.5); // 计算出与50%波幅的dB差

                if (!pTechniques->m_bNewDEFECT[ichannel]) { // 无缺陷情况下 才判断是否是透声不良
                    if (fdBTS < 0) {
                        if (!pTechniques->m_bNewDEFECT_TS[ichannel]) { // 第一次发现缺陷的帧 原来为false 状态

                            pTechniques->mDB_DEFECT_DATA[ichannel].nIndex  = pTechniques->m_pDefect[ichannel].size();
                            pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                        }
                        pTechniques->m_bNewDEFECT_TS[ichannel] = true; // 设置为缺陷

                        if (pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset > fdBTS) { // 透声判断最低dB 更新相关数据
                            pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset    = fdBTS;
                            pTechniques->mDB_DEFECT_DATA[ichannel].nCircleIndex = pTechniques->m_iTreadCirIndex;
                            pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex   = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                            pTechniques->mDB_DEFECT_DATA[ichannel].nCH          = ichannel;
                            StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szDetectionArea, _T("侧面"));
                            CString strName;
                            strName.Format(_T("侧面B%d"), ichannel + 1);
                            StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szProbeMake, strName);

                            pTechniques->mDB_DEFECT_DATA[ichannel].fProbleYPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos;   // 侧面Y位置
                            ;                                                                                                           // 侧面Y开始位置+圈数*步进 nCH 0-5 为踏面位置
                            pTechniques->mDB_DEFECT_DATA[ichannel].fProbleXPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart; // 侧面X位置，开始位置+圈数*步进
                            +g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex;
                            //        nCH 0-5 为踏面位置     6-9为侧面位置
                            pTechniques->mDB_DEFECT_DATA[ichannel].nRadialDistance = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex + ((ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2) * g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth; // 径向距离  //侧面探头为  探头 步进距离
                            pTechniques->mDB_DEFECT_DATA[ichannel].nAxialDepth     = nGate_BPos;                                                                                                                                                                                                    // 轴向深度    //侧面探头为  缺陷深度
                            pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle    = Read.fAngle;                                                                                                                                                                                                   ////缺陷角度

                            pTechniques->mDB_DEFECT_DATA[ichannel].nWaveHeight   = nAmpTS;                                         // int  nWaveHeight;  //波高 0-255
                            pTechniques->mDB_DEFECT_DATA[ichannel].nSensitivity  = mDetetionParam2995_200[ichannel].fScanGain;     // 探伤灵敏度
                            pTechniques->mDB_DEFECT_DATA[ichannel].nTranslucency = mDetetionParam2995_200[ichannel].fScanTrUTGain; // 探伤灵敏度
                            pTechniques->mDB_DEFECT_DATA[ichannel].bDefectType   = 1;                                              // 0 缺陷 1. 透声不良
                            if (recod_buffer != NULL) {
                                recod_buffer->fDb[ichannel]         = fDB;
                                recod_buffer->bDefectType[ichannel] = 1; // 缓存缺陷大小
                            }
                        }
                    } else {
                        if (pTechniques->m_bNewDEFECT_TS[ichannel] &&
                            (pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) != pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex) { // 原来有缺陷 变成无缺陷
                            pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                            DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA 需要存储到voter 下次开始释放
                            memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // 拷贝数据到
                            pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // 缺陷完成
                            printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d 透声不良\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                            memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // 一个缺陷结束 回复默认
                        }
                    }
                } else {
                    {                                                 // 发现非透声不良缺陷 保存当前的 透声缺陷
                        if (pTechniques->m_bNewDEFECT_TS[ichannel]) { // 原来有缺陷 变成无缺陷
                            pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                            DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA 需要存储到voter 下次开始释放
                            memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // 拷贝数据到
                            pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // 缺陷完成
                            printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d 透声不良\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                            memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // 一个缺陷结束 回复默认
                        }
                    }
                }
            }
        }

        // 侧面绘图数据
        if (pTechniques->m_bNewDEFECT_TS[ichannel] || pTechniques->m_bNewDEFECT[ichannel]) // 透声不良或者 缺陷
        {
            pCscanData[pTechniques->m_iSidePointIndex] = fMaxAmp; // 缺陷波数据 瑞铁不对波幅成像 直接有无缺陷
            if (pTechniques->m_iSideCirIndex + ((ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2) * pTechniques->m_Scan.m_iDrawProbleSideSize < pTechniques->m_Scan.m_iDrawSideSize) {
                // 0 默认0 标识无检测 无缺陷为11  有缺陷显示通道号
                int DrawIndex = 0;
                if (ichannel == 6 || ichannel == 8)
                    DrawIndex = 0; // 前面一组探头
                if (ichannel == 7 || ichannel == 9)
                    DrawIndex = 1; // 后面一组探头 绘图加上探头之间的间隔圈数
                pCscanCH[pTechniques->m_iTreadPointIndex + (DrawIndex)*pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1;
            }
        }
    }

    return true;
}
bool DetectionStd_TBT2995_200::ReViewDetection(Techniques* pTechniques, RECORD_DATA& Read, int nRecordIndex) {
    float  fAaxAmp_0  = 35; // 峰值降到最低距离 暂定30mm 波宽的干扰距离
    float  f0_AaxAmp  = 5;  // 0 dao  峰值到最低距离 暂定5 波宽的干扰距离
    UCHAR* pBscanData = &pTechniques->m_Scan.m_pBscanData[0];
    UCHAR* pBscanCH   = &pTechniques->m_Scan.m_pBscanCH[0];
    printf("m_iTreadPointIndex%d\n", pTechniques->m_iTreadPointIndex);
    pBscanCH[pTechniques->m_iTreadPointIndex] = 0;

    bool bAllAscanSize = false;              // 没有数据
    for (int i = 0; i < HD_CHANNEL_NUM; i++) // 6个
    {
        if (Read.iAscanSize[i] > 0) {
            bAllAscanSize = true;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pBscanCH.size()) {
            if (pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] == 0 && bAllAscanSize) { // 未检测时才设置为检测状态
                pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = 11;                    // 有检测无缺陷为11
            }
        }
    }
    if (!bAllAscanSize) {
        return false;
    }

    for (int i = 0; i < 6; i++) // 踏面 6个
    {
        int      ichannel = pTechniques->m_Scan.m_pTreadProbe[i];
        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        CountDAC_Scan(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);
        if (pTechniques->m_iTreadPointIndex + i / 2 * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() >= pTechniques->m_Scan.m_pBscanCH.size()) {
            continue;
        }
        // 计算 A门峰值下降沿 <10%的位置 A门峰值>25% 才去找下降沿10% 否则使用A门峰值
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25) {                                                                                               // 大于15%才找下降沿 <10的位置作为开始检测点
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // 实际采样点是大于声程 的
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A门最高波在帧中的位置
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // 计算出的位置为A门下降沿10%大于 A门峰值位置，小于总长度
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }
        // A B 门无确定波幅参考  A 门位 0 门宽1   b 门位 99 门宽1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f; // 波门A 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
        fStartPos       = fStartAmpDown10;
        float fEndPos   = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // 波门B 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
                                                                                                                       //  A门最高波加上B门位置为检查宽度
        fEndPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // 实际距离mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL_Scan[ichannel].size(); // DAC 的大小和声程一样

        // 查找 fStartPos- fEndPos的最高波
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // 实际采样点是大于声程 的
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // 计算实际采样的起始点
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // 计算实际采样的起始点
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // 合理的数据才进行缺陷处理
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // 在A扫的最高波点数
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            float fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // 计算出波幅最高的声程位置

            float fDAC = m_DAC_RL_Scan[ichannel][fMaxAmpPos];
            float fDB  = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // 计算出当前波幅 与DAC的dB差

            // 缺陷段判断已经数据记录
            if (fDB > 0) {                                  // 为缺陷 不断更新nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // 第一次发现缺陷的帧 原来为false 状态

                    // pTechniques->mDB_DEFECT_DATA[ichannel].nIndex = pTechniques->m_pDefect[ichannel].size();
                    //	pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // 设置为缺陷
            } else {
                if (pTechniques->m_bNewDEFECT[ichannel]) { // 原来有缺陷 变成无缺陷

                    pTechniques->m_bNewDEFECT[ichannel] = false; // 缺陷完成

                } else { // 踏面不判断透声
                }
            }
        }

        // 踏面绘图数据
        if (pBscanData[pTechniques->m_iTreadPointIndex] < fMaxAmp) // 成像角度记录的进帧数 缺陷值小于 当前读取的帧数 缺陷值 覆盖数据
        {
            pBscanData[pTechniques->m_iTreadPointIndex] = fMaxAmp; // 缺陷波数据 瑞铁不对波幅成像 直对有无缺陷

            if (pTechniques->m_bNewDEFECT[ichannel]) { // 为缺陷
                if (pTechniques->m_iTreadPointIndex + i / 2 * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pBscanCH.size()) {
                    pBscanCH[pTechniques->m_iTreadPointIndex + i / 2 * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1; // 0 默认0 标识无缺陷  有缺陷显示通道号
                }
            }
        }
    }

    // 侧面数据处理
    UCHAR* pCscanData = &pTechniques->m_Scan.m_pCscanData[0];
    UCHAR* pCscanCH   = &pTechniques->m_Scan.m_pCscanCH[0];

    for (int i = 0; i < 2; i++) {
        if (pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pCscanCH.size()) {
            if (pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] == 0) { // 未检测时才设置为检测状态
                pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] = 11;   // 有检测无缺陷为11
            }
        }
    }

    for (int i = 0; i < 4; i++) // 侧面四个
    {
        int ichannel = pTechniques->m_Scan.m_pSideProbe[i];

        if ((pTechniques->m_iSidePointIndex + (ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2 * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint() >= pTechniques->m_Scan.m_pCscanCH.size())) {
            continue;
        }
        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        // CountDAC(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);

        // 计算 A门峰值下降沿 <10%的位置 A门峰值>25% 才去找下降沿10% 否则使用A门峰值
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25) {                                                                                               // 大于15%才找下降沿 <10的位置作为开始检测点
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // 实际采样点是大于声程 的
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A门最高波在帧中的位置
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // 计算出的位置为A门下降沿10%大于 A门峰值位置，小于总长度
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }
        // A B 门无确定波幅参考  A 门位 0 门宽1   b 门位 99 门宽1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + fAaxAmp_0; // 波门A 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
        // fStartPos = fStartAmpDown10;
        float fEndPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // 波门B 中峰值位置 （实际距离无延时） 作为DAC判断缺陷的起始位置
        fEndPos       = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // 实际距离mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL[ichannel].size(); // DAC 的大小和声程一样

        // 查找 fStartPos- fEndPos的最高波
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // 实际采样点是大于声程 的
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // 计算实际采样的起始点
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // 计算实际采样的起始点
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // 合理的数据才进行缺陷处理
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // 在A扫的最高波点数
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            float fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // 计算出波幅最高的声程位置
            float fDAC       = m_DAC_RL[ichannel][fMaxAmpPos];
            float fDB        = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // 计算出当前波幅 与DAC的dB差

            // 缺陷段判断已经数据记录
            if (fDB > 0) {                                  // 为缺陷 不断更新nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // 第一次发现缺陷的帧 原来为false 状态
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // 设置为缺陷

            } else {
                if (pTechniques->m_bNewDEFECT[ichannel]) { // 原来有缺陷 变成无缺陷

                    pTechniques->m_bNewDEFECT[ichannel] = false; // 缺陷完成

                } else { // 踏面不判断透声
                }
            }
        }

        // 透声缺陷判断 侧面直探头判断透声
        if ((ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[0] || ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[1]) && false) {
            int nGate_Bamp = Read.pGateAmp[ichannel][GATE_B];
            int nGate_BPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
            // 计算透声灵敏度下的新峰值
            int nAmpTS = mDacCount.CoutDACNewPeek(mDetetionParam2995_200[ichannel].fScanGain + mDetetionParam2995_200[ichannel].fScanCompensate,
                                                  mDetetionParam2995_200[ichannel].fScanCompensate + mDetetionParam2995_200[ichannel].fScanTrUTCompensate,
                                                  nGate_Bamp); // 根据当前的探伤增益 计算出 透声增益下的波幅。

            // 透声缺陷的判断，
            // 1首先判断波幅是否降到50%下
            // 2.判断是否有非透声缺陷
            // 3.没有判断透声缺陷
            // 4.发现非透声缺陷 立刻结束透声缺陷判断
            //	if (nAmpTS < 255 * 0.5)// 波幅低于50%；透声不良
            {
                if (nAmpTS < 2)
                    nAmpTS = 2;                                               // 最小值不为0
                float fdBTS = mDacCount.CoutdBOffsetByAmp(nAmpTS, 255 * 0.5); // 计算出与50%波幅的dB差

                if (!pTechniques->m_bNewDEFECT[ichannel]) { // 无缺陷情况下 才判断是否是透声不良
                    if (fdBTS < 0) {
                        if (!pTechniques->m_bNewDEFECT_TS[ichannel]) { // 第一次发现缺陷的帧 原来为false 状态
                        }
                        pTechniques->m_bNewDEFECT_TS[ichannel] = true; // 设置为缺陷
                    }

                    else {
                        if (pTechniques->m_bNewDEFECT_TS[ichannel]) { // 原来有缺陷 变成无缺陷

                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // 缺陷完成
                        }
                    }
                } else {
                    {                                                 // 发现非透声不良缺陷 保存当前的 透声缺陷
                        if (pTechniques->m_bNewDEFECT_TS[ichannel]) { // 原来有缺陷 变成无缺陷

                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // 缺陷完成
                        }
                    }
                }
            }
        }

        if (pTechniques->m_iSidePointIndex > 100) {
            int test = 0;
        }
        // 侧面绘图数据
        if (pTechniques->m_bNewDEFECT_TS[ichannel] || pTechniques->m_bNewDEFECT[ichannel]) // 透声不良或者 缺陷
        {
            pCscanData[pTechniques->m_iSidePointIndex] = fMaxAmp; // 缺陷波数据 瑞铁不对波幅成像 直对有无缺陷
            if (pTechniques->m_iSidePointIndex + (ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2 * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pCscanCH.size()) {
                int DrawIndex = 0;
                if (ichannel == 6 || ichannel == 8)
                    DrawIndex = 0; // 前面一组探头
                if (ichannel == 7 || ichannel == 9)
                    DrawIndex = 1; // 后面一组探头 绘图加上探头之间的间隔圈数

                pCscanCH[pTechniques->m_iSidePointIndex + DrawIndex * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1; // 0 默认0 标识无检测 无缺陷为11  有缺陷显示通道号
            }
        }

        // m_pRecordSide.resize(m_Scan.m_iSideSize * m_Scan.GetSidePoint());
        //	if (pCscanData[m_iSidePointIndex] < Read.pGateAmp[m_Scan.m_pSideProbe[i]][GATE_A])
    }
    return true;
}
bool DetectionStd_TBT2995_200::LoadTec(size_t ch, LPCTSTR lpName) {
    TCHAR   pBuf[256] = {0};
    CString strKey;
    strKey.Format(_T("TBT2995_200_CH%lld"), ch);
    GetPrivateProfileString(strKey.GetBuffer(0), _T("fScanGain"), _T("40"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fScanGain);

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fScanCompensate"), _T("4"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fScanCompensate);

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fScanTrUTGain"), _T("45"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fScanTrUTGain);

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fScanTrUTCompensate"), _T("2"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fScanTrUTCompensate);

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fAperture"), _T("2.0"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fAperture);

    mDetetionParam2995_200[ch].nUseHole = GetPrivateProfileInt(strKey.GetBuffer(0), _T("nUseHole"), 2, lpName);

    CString strValueKey;
    for (int i = 0; i < DAC_HOLE_NUM; i++) {
        strValueKey.Format(_T("fDAC_HoleDepth%d"), i);
        GetPrivateProfileString(strKey.GetBuffer(0), strValueKey, _T("10.0"), pBuf, 256, lpName);
        _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fDAC_HoleDepth[i]);

        strValueKey.Format(_T("fDAC_HolebAmp%d"), i);
        GetPrivateProfileString(strKey.GetBuffer(0), strValueKey, _T("0.5"), pBuf, 256, lpName);
        _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fDAC_HolebAmp[i]);
    }

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fDAC_BaseGain"), _T("30"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fDAC_BaseGain);

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fDAC_LineRD"), _T("-8"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fDAC_LineRD);

    GetPrivateProfileString(strKey.GetBuffer(0), _T("fDAC_LineRL"), _T("-4"), pBuf, 256, lpName);
    _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].fDAC_LineRL);

    for (int i = 0; i < 2; i++) {
        strValueKey.Format(_T("DACGatePos%d"), i);
        GetPrivateProfileString(strKey.GetBuffer(0), strValueKey, _T("0.1"), pBuf, 256, lpName);
        _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].nDACGatePos[i]);

        strValueKey.Format(_T("DACWidth%d"), i);
        GetPrivateProfileString(strKey.GetBuffer(0), strValueKey, _T("0.1"), pBuf, 256, lpName);
        _stscanf_s(pBuf, _T("%f"), &mDetetionParam2995_200[ch].nDACWidth[i]);
    }
    return true;
}
bool DetectionStd_TBT2995_200::SaveTec(size_t ch, LPCTSTR lpName) {
    CString strKey, strInfo;
    strKey.Format(_T("TBT2995_200_CH%lld"), ch);
    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fScanGain);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fScanGain"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fScanCompensate);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fScanCompensate"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fScanTrUTGain);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fScanTrUTGain"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fScanTrUTCompensate);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fScanTrUTCompensate"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fAperture);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fAperture"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%d"), mDetetionParam2995_200[ch].nUseHole);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("nUseHole"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fDAC_BaseGain);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fDAC_BaseGain"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fDAC_LineRD);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fDAC_LineRD"), strInfo.GetBuffer(0), lpName);

    strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fDAC_LineRL);
    WritePrivateProfileString(strKey.GetBuffer(0), _T("fDAC_LineRL"), strInfo.GetBuffer(0), lpName);

    CString strValueKey;
    for (int i = 0; i < DAC_HOLE_NUM; i++) {
        strValueKey.Format(_T("fDAC_HoleDepth%d"), i);

        strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fDAC_HoleDepth[i]);
        WritePrivateProfileString(strKey.GetBuffer(0), strValueKey, strInfo.GetBuffer(0), lpName);

        strValueKey.Format(_T("fDAC_HolebAmp%d"), i);
        strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].fDAC_HolebAmp[i]);
        WritePrivateProfileString(strKey.GetBuffer(0), strValueKey, strInfo.GetBuffer(0), lpName);
    }

    for (int i = 0; i < 2; i++) {
        strValueKey.Format(_T("DACGatePos%d"), i);

        strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].nDACGatePos[i]);
        WritePrivateProfileString(strKey.GetBuffer(0), strValueKey, strInfo.GetBuffer(0), lpName);

        strValueKey.Format(_T("DACWidth%d"), i);
        strInfo.Format(_T("%f"), mDetetionParam2995_200[ch].nDACWidth[i]);
        WritePrivateProfileString(strKey.GetBuffer(0), strValueKey, strInfo.GetBuffer(0), lpName);
    }
    return true;
}