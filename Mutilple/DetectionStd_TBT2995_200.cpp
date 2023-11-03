#include "pch.h"

#include "DetectionStd_TBT2995_200.h"
#include "Mutilple.h"

// TODO: ��ʱ���ξ���
#pragma warning( disable: 4267 4244 4552 )


bool DetectionStd_TBT2995_200::InitTechniques(Techniques* pTechniques) {
    float res = mDacCount.CoutdBOffsetByAmp(0.8f, 0.4f);
    int   a   = 0;

    return true;
}
// ����DAC  nShowdB ���ý���ʹ�õ�ǰ����  ɨ�����ʹ��̽������
void DetectionStd_TBT2995_200::CountDAC_Scan(Techniques* pTechniques, int ch, float nShowdB, float fGateAmpPos) {
    Channel pCurCh = pTechniques->m_pChannel[ch];
    m_DAC_RL_Scan[ch].clear(); // �з���
    DETECTION_PARAM2995_200 temDetetionParam2995_200;
    memcpy(&temDetetionParam2995_200, &mDetetionParam2995_200[ch], sizeof(DETECTION_PARAM2995_200));
    // ��̬����DAC ���ݱ��沨���� ��λ��
    for (size_t i = 0; i < mDetetionParam2995_200[ch].nUseHole; i++) {
        temDetetionParam2995_200.fDAC_HoleDepth[i] = fGateAmpPos + mDetetionParam2995_200[ch].fDAC_HoleDepth[i];
    }

    mDacCount.CoutDacCurve(&temDetetionParam2995_200, nShowdB + temDetetionParam2995_200.fDAC_LineRL, pCurCh.m_fDelay, pCurCh.m_fRange, m_DAC_RL_Scan[ch]);
}

void DetectionStd_TBT2995_200::CountDAC(Techniques* pTechniques, int ch, float nShowdB, float fGateAmpPos) {

    Channel pCurCh = pTechniques->m_pChannel[ch];
    m_DAC_M[ch].clear();
    m_DAC_RL[ch].clear(); // �з���
    DETECTION_PARAM2995_200 temDetetionParam2995_200;
    memcpy(&temDetetionParam2995_200, &mDetetionParam2995_200[ch], sizeof(DETECTION_PARAM2995_200));
    // ��̬����DAC ���ݱ��沨���� ��λ��
    for (size_t i = 0; i < mDetetionParam2995_200[ch].nUseHole; i++) {
        temDetetionParam2995_200.fDAC_HoleDepth[i] = fGateAmpPos + mDetetionParam2995_200[ch].fDAC_HoleDepth[i];
    }

    mDacCount.CoutDacCurve(&temDetetionParam2995_200, nShowdB + temDetetionParam2995_200.fDAC_LineRL, pCurCh.m_fDelay, pCurCh.m_fRange, m_DAC_RL[ch]);

    m_DAC_ED[ch].clear(); // ������
    mDacCount.CoutDacCurve(&temDetetionParam2995_200, nShowdB + temDetetionParam2995_200.fDAC_LineRD, pCurCh.m_fDelay, pCurCh.m_fRange, m_DAC_ED[ch]);
}

// �����߼�
//  1. A����߲�����5mm��̤�� �������0��ʼ��   �� B����߲� ǰ�� 5mm
// 2. ̤�� ��1-6 ��A�ſ���ˮ�㲨 ���ڼ��� ȱ��λ��
// 3.���� ��7-10��A �� ��������  b�� �����ز� ���ڼ��� ͸��

// �м����߲����Ӧ��DAC ���  ��¼ ȱ������
bool DetectionStd_TBT2995_200::Detection(Techniques* pTechniques, READ_DATA& Read) {
    float fAaxAmp_0 = 35; // ��ֵ������;��� �ݶ�10mm ����ĸ��ž��� ����ȡ Amen��ֵ�½���10%
    float f0_AaxAmp = 5;  // 0 dao  ��ֵ����;��� �ݶ�5 ����ĸ��ž���

    UCHAR* pCscanData = &pTechniques->m_Scan.m_pCscanData[pTechniques->m_iSideCirIndex * pTechniques->m_Scan.GetSidePoint()];
    UCHAR* pCscanCH   = &pTechniques->m_Scan.m_pCscanCH[pTechniques->m_iSideCirIndex * pTechniques->m_Scan.GetSidePoint()];
    for (int i = 0; i < 2; i++) {
        int size1 = pTechniques->m_Scan.m_pCscanCH.size();
        int size2 = pTechniques->m_iSideCirIndex + pTechniques->m_Scan.m_iDrawProbleSideSize;
        //	TRACE("%d_%d_%d\n", size1, size2, pTechniques->m_iSideCirIndex);
        if (pTechniques->m_iSideCirIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize < pTechniques->m_Scan.m_iDrawSideSize) {
            if (pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetSidePoint()] == 0) { // δ���ʱ������Ϊ���״̬
                pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetSidePoint()] = 11;   // �м����ȱ��Ϊ11
            }
        }
    }

    UCHAR* pBscanData = &pTechniques->m_Scan.m_pBscanData[pTechniques->m_iTreadCirIndex * pTechniques->m_Scan.GetTreadPoint()];
    UCHAR* pBscanCH   = &pTechniques->m_Scan.m_pBscanCH[pTechniques->m_iTreadCirIndex * pTechniques->m_Scan.GetTreadPoint()];
    //	printf("m_iTreadPointIndex%d\n", m_iTreadPointIndex);
    // if (pBscanCH[pTechniques->m_iTreadPointIndex] == 0) { //δ���ʱ������Ϊ���״̬
    //	pBscanCH[pTechniques->m_iTreadPointIndex] = 11; //�м����ȱ��Ϊ11
    //}
    // �ڶ���
    for (int i = 0; i < 3; i++) {
        if (pTechniques->m_iTreadCirIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize < pTechniques->m_Scan.m_iDrawTreadSize) {
            //	if (pTechniques->m_iTreadPointIndex +i* pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pBscanCH.size()) {
            if (pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] == 0) { // δ���ʱ������Ϊ���״̬
                pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = 11;   // �м����ȱ��Ϊ11
            }
        }
    }

    // memset(&recod_buffer, 0, sizeof(RECORD_DATA_RES));

    RECORD_DATA_RES* recod_buffer = new RECORD_DATA_RES;
    // if (abs(pTechniques->m_fLastAngle - Read.fAngle)<= EPS) { //�жϽǶ��Ƿ���� <0.00001
    // TRACE("%xxx0.2f_%0.2f\n", pTechniques->m_fLastAngle, Read.fAngle);
    if (pTechniques->m_nLastIndex == pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) {
        for (int i = 0; i < HD_CHANNEL_NUM; i++) {
            recod_buffer->bDefectType[i] = -1; // Ĭ��-1 ��ȱ��

            recod_buffer->pGatePos[i][GATE_A] = Read.pGatePos[i][GATE_A];
            recod_buffer->pGatePos[i][GATE_B] = Read.pGatePos[i][GATE_B];
            recod_buffer->pGateAmp[i][GATE_A] = Read.pGateAmp[i][GATE_A];
            recod_buffer->pGateAmp[i][GATE_B] = Read.pGateAmp[i][GATE_B];
            size_t size                       = Read.pAscan[i].size();
            recod_buffer->iAscanSize[i]       = size;
            recod_buffer->pAscan[i].resize(size);
            memcpy(recod_buffer->pAscan[i].data(), Read.pAscan[i].data(), size); // Aɨ����
        }

        pTechniques->m_pRecord_Buffer.push_back(recod_buffer);
    } else {
        delete recod_buffer;
        recod_buffer = NULL;
    }

    for (int i = 0; i < 6; i++) // ̤�� 6��
    {
        int      ichannel = pTechniques->m_Scan.m_pTreadProbe[i];
        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        //	if (pTechniques->m_iTreadPointIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() >= pTechniques->m_Scan.m_pBscanCH.size())
        if (pTechniques->m_iTreadCirIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize >= pTechniques->m_Scan.m_iDrawTreadSize) {
            continue; // ��ǰȦ������ ̽ͷ������Ȧ������������Ȧ������
        }
        if (i == 2) {
            int test = 0;
        }
        // ʵʱ���㵱ǰ֡��DAC
        CountDAC_Scan(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);
        //	CountDAC(pTechniques, ichannel, pChannel->m_fGain,0);
        // ���� A�ŷ�ֵ�½��� <10%��λ�� A�ŷ�ֵ>25% ��ȥ���½���10% ����ʹ��A�ŷ�ֵ
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25f) {                                                                                              // ����15%�����½��� <10��λ����Ϊ��ʼ����
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // ʵ�ʲ������Ǵ������� ��
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A����߲���֡�е�λ��
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // �������λ��ΪA���½���10%���� A�ŷ�ֵλ�ã�С���ܳ���
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }
        // A B ����ȷ�������ο�  A ��λ 0 �ſ�1   b ��λ 99 �ſ�1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f; // ����A �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��

        //	if (fStartPos > fStartAmpDown10)  //A�ŷ�ֵ����10mm�� Amen��ֵ�½���10%�Ƚ�
        fStartPos     = fStartAmpDown10;
        float fEndPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // ����B �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
        // A����߲�����B��λ��Ϊ�����
        fEndPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // ʵ�ʾ���mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL_Scan[ichannel].size(); // DAC �Ĵ�С������һ��

        // ���� fStartPos- fEndPos����߲�
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // ʵ�ʲ������Ǵ������� ��
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // ����ʵ�ʲ�������ʼ��
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // ����ʵ�ʲ�������ʼ��
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // ��������ݲŽ���ȱ�ݴ���
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // ��Aɨ����߲�����
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            int fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // �����������ߵ�����λ��

            float fDAC = 0;
            if (m_DAC_RL_Scan[ichannel].size() > fMaxAmpPos) {
                fDAC = m_DAC_RL_Scan[ichannel][fMaxAmpPos];
            }
            float fDB = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // �������ǰ���� ��DAC��dB��

            // ȱ�ݶ��ж��Ѿ����ݼ�¼
            if (fDB > 0) {                                  // Ϊȱ�� ���ϸ���nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // ��һ�η���ȱ�ݵ�֡ ԭ��Ϊfalse ״̬

                    pTechniques->mDB_DEFECT_DATA[ichannel].nIndex  = pTechniques->m_pDefect[ichannel].size();
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // ����Ϊȱ��

                if (pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset < fDB) { // �ж����dB �����������

                    pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset    = fDB;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCircleIndex = pTechniques->m_iTreadCirIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex   = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCH          = ichannel;
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szDetectionArea, _T("̤��"));
                    CString strName;
                    strName.Format(_T("̤��A%d"), ichannel + 1);
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szProbeMake, strName);

                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleYPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStart + g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep * pTechniques->m_iTreadCirIndex; // ̤��Y��ʼλ��+Ȧ��*���� nCH 0-5 Ϊ̤��λ��
                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleXPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadXPos;                                                                                           // ̤��Xλ��
                    //        nCH 0-5 Ϊ̤��λ��     6-9Ϊ����λ��
                    pTechniques->mDB_DEFECT_DATA[ichannel].nRadialDistance = fMaxAmpPos - (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f; // �������  //̤��̽ͷΪ ȱ����� (��߲�-A�ű��沨)
                    pTechniques->mDB_DEFECT_DATA[ichannel].nAxialDepth     = abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fTreadYStep * pTechniques->m_iTreadCirIndex) +
                                                                         (ichannel / 2) * g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fTreadWidth; // �������    //̤��̽ͷΪ ̽ͷλ�� ��������+�ڼ���̽ͷ*̽ͷ���ľ���
                    pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle = Read.fAngle;                                                                    ////ȱ�ݽǶ�

                    pTechniques->mDB_DEFECT_DATA[ichannel].nWaveHeight   = fMaxAmp;                                        // int  nWaveHeight;  //���� 0-255
                    pTechniques->mDB_DEFECT_DATA[ichannel].nSensitivity  = mDetetionParam2995_200[ichannel].fScanGain;     // ̽��������
                    pTechniques->mDB_DEFECT_DATA[ichannel].nTranslucency = mDetetionParam2995_200[ichannel].fScanTrUTGain; // ̽��������
                    pTechniques->mDB_DEFECT_DATA[ichannel].bDefectType   = 0;                                              // 0 ȱ�� 1. ͸������
                    if (recod_buffer != NULL) {
                        recod_buffer->fDb[ichannel]         = fDB;
                        recod_buffer->bDefectType[ichannel] = 0; // ����ȱ�ݴ�С
                    }
                }
            } else {
                if (pTechniques->m_bNewDEFECT[ichannel] &&
                    (pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) != pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex) { // ԭ����ȱ�� �����ȱ�� ����ͬһ���ط�
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA ��Ҫ�洢��voter �´ο�ʼ�ͷ�
                    memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // �������ݵ�
                    pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                    pTechniques->m_bNewDEFECT[ichannel] = false; // ȱ�����
                    printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                    memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // һ��ȱ�ݽ��� �ظ�Ĭ��
                } else {                                                                        // ̤�治�ж�͸��
                }
            }
        }

        // ̤���ͼ����
        if (pBscanData[pTechniques->m_iTreadPointIndex] < fMaxAmp) // ����Ƕȼ�¼�Ľ�֡�� ȱ��ֵС�� ��ǰ��ȡ��֡�� ȱ��ֵ ��������
        {
            pBscanData[pTechniques->m_iTreadPointIndex] = fMaxAmp; // ȱ�ݲ����� �������Բ������� ֱ������ȱ��

            if (pTechniques->m_bNewDEFECT[ichannel]) { // Ϊȱ��

                if (pTechniques->m_iTreadCirIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize < pTechniques->m_Scan.m_iDrawTreadSize)                           // ��ǰȦ������ ̽ͷ������Ȧ��С�ڳ�����Ȧ��
                {                                                                                                                                                                 // 2��̽ͷһ����ȱ�ݷ��õ���Ӧ��λ��
                    pBscanCH[pTechniques->m_iTreadPointIndex + (ichannel / 2) * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1; // 0 Ĭ��0 ��ʶ��ȱ��  ��ȱ����ʾͨ����
                }
            }
        }
        int iIndex = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
        //	if ( m_pRecord[iIndex].iAscanSize[ichannel].size()>0)//�����ݵ�������ж��Ƿ��и���λȱ�� ���񸲸�
        //{  //̤���¼����
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
        //	memcpy(pTechniques->m_pRecord[iIndex].pAscan[ichannel].data(), Read.pAscan[ichannel].data(), size); //Aɨ����
        //}
    }

    // �������ݴ���

    for (int i = 0; i < 4; i++) // �����ĸ�
    {
        int ichannel = pTechniques->m_Scan.m_pSideProbe[i];

        // ������ⷶΧ �������ж�
        //	if (pTechniques->m_iSidePointIndex + ((ichannel- pTechniques->m_Scan.m_pSideProbe[0]) / 2) * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetSidePoint() >= pTechniques->m_Scan.m_pCscanCH.size())
        if (pTechniques->m_iSideCirIndex + ((ichannel - pTechniques->m_Scan.m_pSideProbe[0])) / 2 * pTechniques->m_Scan.m_iDrawProbleSideSize >= pTechniques->m_Scan.m_iDrawSideSize) {
            continue; // ��ǰȦ������ ̽ͷ������Ȧ������������Ȧ������
        }

        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        // ������ʱ����Ҫ��̬DAC
        //	CountDAC(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);

        // ���� A�ŷ�ֵ�½��� <10%��λ�� A�ŷ�ֵ>25% ��ȥ���½���10% ����ʹ��A�ŷ�ֵ
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        float GatePosA        = pChannel->m_pGatePos[0] * pChannel->m_fRange;
        if (fStartAmpDown10 < GatePosA) {
        }

        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25) {                                                                                               // ����15%�����½��� <10��λ����Ϊ��ʼ����
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // ʵ�ʲ������Ǵ������� ��
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A����߲���֡�е�λ��
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // �������λ��ΪA���½���10%���� A�ŷ�ֵλ�ã�С���ܳ���
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }

        // A B ����ȷ�������ο�  A ��λ 0 �ſ�1   b ��λ 99 �ſ�1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + fAaxAmp_0; // ����A �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
        // fStartPos = fStartAmpDown10;

        float fEndPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // ����B �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
        fEndPos       = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // ʵ�ʾ���mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL[ichannel].size(); // DAC �Ĵ�С������һ��

        // ���� fStartPos- fEndPos����߲�
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // ʵ�ʲ������Ǵ������� ��
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // ����ʵ�ʲ�������ʼ��
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // ����ʵ�ʲ�������ʼ��
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // bool bSideMaxDefet = false;
        float fDB = 0;
        // ��������ݲŽ���ȱ�ݴ���
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // ��Aɨ����߲�����
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            float fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // �����������ߵ�����λ��
            float fDAC       = m_DAC_RL[ichannel][fMaxAmpPos];
            fDB              = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // �������ǰ���� ��DAC��dB��

            // ȱ�ݶ��ж��Ѿ����ݼ�¼
            if (fDB > 0) {                                  // Ϊȱ�� ���ϸ���nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // ��һ�η���ȱ�ݵ�֡ ԭ��Ϊfalse ״̬

                    pTechniques->mDB_DEFECT_DATA[ichannel].nIndex  = pTechniques->m_pDefect[ichannel].size();
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // ����Ϊȱ��

                if (pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset < fDB) { // �ж����dB �����������
                    pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset    = fDB;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCircleIndex = pTechniques->m_iTreadCirIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex   = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    pTechniques->mDB_DEFECT_DATA[ichannel].nCH          = ichannel;
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szDetectionArea, _T("����"));
                    CString strName;
                    strName.Format(_T("����B%d"), ichannel + 1);
                    StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szProbeMake, strName);

                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleYPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos;   // ����Yλ��
                    ;                                                                                                           // ����Y��ʼλ��+Ȧ��*���� nCH 0-5 Ϊ̤��λ��
                    pTechniques->mDB_DEFECT_DATA[ichannel].fProbleXPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart; // ����Xλ�ã���ʼλ��+Ȧ��*����
                    +g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex;
                    //        nCH 0-5 Ϊ̤��λ��     6-9Ϊ����λ��
                    pTechniques->mDB_DEFECT_DATA[ichannel].nRadialDistance = abs(g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex) +
                                                                             ((ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2) * g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth; // �������  //����̽ͷΪ  ̽ͷ ��������+�ڼ���*̽ͷ���ľ���
                    pTechniques->mDB_DEFECT_DATA[ichannel].nAxialDepth  = fMaxAmpPos;                                                                                                                // �������    //����̽ͷΪ  ȱ�����
                    pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle = Read.fAngle;                                                                                                               ////ȱ�ݽǶ�

                    pTechniques->mDB_DEFECT_DATA[ichannel].nWaveHeight   = fMaxAmp;                                        // int  nWaveHeight;  //���� 0-255
                    pTechniques->mDB_DEFECT_DATA[ichannel].nSensitivity  = mDetetionParam2995_200[ichannel].fScanGain;     // ̽��������
                    pTechniques->mDB_DEFECT_DATA[ichannel].nTranslucency = mDetetionParam2995_200[ichannel].fScanTrUTGain; // ̽��������
                    pTechniques->mDB_DEFECT_DATA[ichannel].bDefectType   = 0;                                              // 0 ȱ�� 1. ͸������
                    if (recod_buffer != NULL) {
                        recod_buffer->fDb[ichannel]         = fDB; // ����ȱ�ݴ�С
                        recod_buffer->bDefectType[ichannel] = 0;   // ����ȱ������
                    }
                }
            } else {
                if (pTechniques->m_bNewDEFECT[ichannel] &&
                    (pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) != pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex) { // ԭ����ȱ�� �����ȱ��
                    pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                    DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA ��Ҫ�洢��voter �´ο�ʼ�ͷ�
                    memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // �������ݵ�
                    pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                    pTechniques->m_bNewDEFECT[ichannel] = false; // ȱ�����
                    printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                           pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                    memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // һ��ȱ�ݽ��� �ظ�Ĭ��
                } else {                                                                        // ̤�治�ж�͸��
                }
            }
        }

        // ͸��ȱ���ж� ����ֱ̽ͷ�ж�͸�� false ������͸�����

        if ((ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[0] || ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[1]) && false) {
            int nGate_Bamp = Read.pGateAmp[ichannel][GATE_B];
            int nGate_BPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
            // ����͸���������µ��·�ֵ
            int nAmpTS = mDacCount.CoutDACNewPeek(mDetetionParam2995_200[ichannel].fScanGain + mDetetionParam2995_200[ichannel].fScanCompensate,
                                                  mDetetionParam2995_200[ichannel].fScanCompensate + mDetetionParam2995_200[ichannel].fScanTrUTCompensate,
                                                  nGate_Bamp); // ���ݵ�ǰ��̽������ ����� ͸�������µĲ�����

            // ͸��ȱ�ݵ��жϣ�
            // 1�����жϲ����Ƿ񽵵�50%��
            // 2.�ж��Ƿ��з�͸��ȱ��
            // 3.û���ж�͸��ȱ��
            // 4.���ַ�͸��ȱ�� ���̽���͸��ȱ���ж�
            //	if (nAmpTS < 255 * 0.5)// ��������50%��͸������
            {
                if (nAmpTS < 2)
                    nAmpTS = 2;                                               // ��Сֵ��Ϊ0
                float fdBTS = mDacCount.CoutdBOffsetByAmp(nAmpTS, 255 * 0.5); // �������50%������dB��

                if (!pTechniques->m_bNewDEFECT[ichannel]) { // ��ȱ������� ���ж��Ƿ���͸������
                    if (fdBTS < 0) {
                        if (!pTechniques->m_bNewDEFECT_TS[ichannel]) { // ��һ�η���ȱ�ݵ�֡ ԭ��Ϊfalse ״̬

                            pTechniques->mDB_DEFECT_DATA[ichannel].nIndex  = pTechniques->m_pDefect[ichannel].size();
                            pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                        }
                        pTechniques->m_bNewDEFECT_TS[ichannel] = true; // ����Ϊȱ��

                        if (pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset > fdBTS) { // ͸���ж����dB �����������
                            pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset    = fdBTS;
                            pTechniques->mDB_DEFECT_DATA[ichannel].nCircleIndex = pTechniques->m_iTreadCirIndex;
                            pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex   = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                            pTechniques->mDB_DEFECT_DATA[ichannel].nCH          = ichannel;
                            StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szDetectionArea, _T("����"));
                            CString strName;
                            strName.Format(_T("����B%d"), ichannel + 1);
                            StrCpy(pTechniques->mDB_DEFECT_DATA[ichannel].szProbeMake, strName);

                            pTechniques->mDB_DEFECT_DATA[ichannel].fProbleYPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideYPos;   // ����Yλ��
                            ;                                                                                                           // ����Y��ʼλ��+Ȧ��*���� nCH 0-5 Ϊ̤��λ��
                            pTechniques->mDB_DEFECT_DATA[ichannel].fProbleXPos = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStart; // ����Xλ�ã���ʼλ��+Ȧ��*����
                            +g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex;
                            //        nCH 0-5 Ϊ̤��λ��     6-9Ϊ����λ��
                            pTechniques->mDB_DEFECT_DATA[ichannel].nRadialDistance = g_MainProcess.m_ConnectPLC.m_PlcDownParam.fSideXStep * pTechniques->m_iTreadCirIndex + ((ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2) * g_MainProcess.m_Techniques.m_Specimen.m_WheelParam.fSideWidth; // �������  //����̽ͷΪ  ̽ͷ ��������
                            pTechniques->mDB_DEFECT_DATA[ichannel].nAxialDepth     = nGate_BPos;                                                                                                                                                                                                    // �������    //����̽ͷΪ  ȱ�����
                            pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle    = Read.fAngle;                                                                                                                                                                                                   ////ȱ�ݽǶ�

                            pTechniques->mDB_DEFECT_DATA[ichannel].nWaveHeight   = nAmpTS;                                         // int  nWaveHeight;  //���� 0-255
                            pTechniques->mDB_DEFECT_DATA[ichannel].nSensitivity  = mDetetionParam2995_200[ichannel].fScanGain;     // ̽��������
                            pTechniques->mDB_DEFECT_DATA[ichannel].nTranslucency = mDetetionParam2995_200[ichannel].fScanTrUTGain; // ̽��������
                            pTechniques->mDB_DEFECT_DATA[ichannel].bDefectType   = 1;                                              // 0 ȱ�� 1. ͸������
                            if (recod_buffer != NULL) {
                                recod_buffer->fDb[ichannel]         = fDB;
                                recod_buffer->bDefectType[ichannel] = 1; // ����ȱ�ݴ�С
                            }
                        }
                    } else {
                        if (pTechniques->m_bNewDEFECT_TS[ichannel] &&
                            (pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex) != pTechniques->mDB_DEFECT_DATA[ichannel].nScanIndex) { // ԭ����ȱ�� �����ȱ��
                            pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                            DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA ��Ҫ�洢��voter �´ο�ʼ�ͷ�
                            memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // �������ݵ�
                            pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // ȱ�����
                            printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d ͸������\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                            memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // һ��ȱ�ݽ��� �ظ�Ĭ��
                        }
                    }
                } else {
                    {                                                 // ���ַ�͸������ȱ�� ���浱ǰ�� ͸��ȱ��
                        if (pTechniques->m_bNewDEFECT_TS[ichannel]) { // ԭ����ȱ�� �����ȱ��
                            pTechniques->mDB_DEFECT_DATA[ichannel].nParam2 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                            DB_DEFECT_DATA* Defect_date                    = new DB_DEFECT_DATA();                // new DB_DEFECT_DATA ��Ҫ�洢��voter �´ο�ʼ�ͷ�
                            memcpy(Defect_date, &pTechniques->mDB_DEFECT_DATA[ichannel], sizeof(DB_DEFECT_DATA)); // �������ݵ�
                            pTechniques->m_pDefect[ichannel].push_back(Defect_date);
                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // ȱ�����
                            printf("nParam1-_nParam2:%d-%d_CH:%d_fDB:%.2f-nDefectAnglen:%.2f_SizeAscan:%d ͸������\n", pTechniques->mDB_DEFECT_DATA[ichannel].nParam1, pTechniques->mDB_DEFECT_DATA[ichannel].nParam2,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nCH,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDBOffset,
                                   pTechniques->mDB_DEFECT_DATA[ichannel].nDefectAngle, nSizeAscan);
                            memset(&pTechniques->mDB_DEFECT_DATA[ichannel], 0, sizeof(DB_DEFECT_DATA)); // һ��ȱ�ݽ��� �ظ�Ĭ��
                        }
                    }
                }
            }
        }

        // �����ͼ����
        if (pTechniques->m_bNewDEFECT_TS[ichannel] || pTechniques->m_bNewDEFECT[ichannel]) // ͸���������� ȱ��
        {
            pCscanData[pTechniques->m_iSidePointIndex] = fMaxAmp; // ȱ�ݲ����� �������Բ������� ֱ������ȱ��
            if (pTechniques->m_iSideCirIndex + ((ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2) * pTechniques->m_Scan.m_iDrawProbleSideSize < pTechniques->m_Scan.m_iDrawSideSize) {
                // 0 Ĭ��0 ��ʶ�޼�� ��ȱ��Ϊ11  ��ȱ����ʾͨ����
                int DrawIndex = 0;
                if (ichannel == 6 || ichannel == 8)
                    DrawIndex = 0; // ǰ��һ��̽ͷ
                if (ichannel == 7 || ichannel == 9)
                    DrawIndex = 1; // ����һ��̽ͷ ��ͼ����̽ͷ֮��ļ��Ȧ��
                pCscanCH[pTechniques->m_iTreadPointIndex + (DrawIndex)*pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1;
            }
        }
    }

    return true;
}
bool DetectionStd_TBT2995_200::ReViewDetection(Techniques* pTechniques, RECORD_DATA& Read, int nRecordIndex) {
    float  fAaxAmp_0  = 35; // ��ֵ������;��� �ݶ�30mm ����ĸ��ž���
    float  f0_AaxAmp  = 5;  // 0 dao  ��ֵ����;��� �ݶ�5 ����ĸ��ž���
    UCHAR* pBscanData = &pTechniques->m_Scan.m_pBscanData[0];
    UCHAR* pBscanCH   = &pTechniques->m_Scan.m_pBscanCH[0];
    printf("m_iTreadPointIndex%d\n", pTechniques->m_iTreadPointIndex);
    pBscanCH[pTechniques->m_iTreadPointIndex] = 0;

    bool bAllAscanSize = false;              // û������
    for (int i = 0; i < HD_CHANNEL_NUM; i++) // 6��
    {
        if (Read.iAscanSize[i] > 0) {
            bAllAscanSize = true;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pBscanCH.size()) {
            if (pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] == 0 && bAllAscanSize) { // δ���ʱ������Ϊ���״̬
                pBscanCH[pTechniques->m_iTreadPointIndex + i * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = 11;                    // �м����ȱ��Ϊ11
            }
        }
    }
    if (!bAllAscanSize) {
        return false;
    }

    for (int i = 0; i < 6; i++) // ̤�� 6��
    {
        int      ichannel = pTechniques->m_Scan.m_pTreadProbe[i];
        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        CountDAC_Scan(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);
        if (pTechniques->m_iTreadPointIndex + i / 2 * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() >= pTechniques->m_Scan.m_pBscanCH.size()) {
            continue;
        }
        // ���� A�ŷ�ֵ�½��� <10%��λ�� A�ŷ�ֵ>25% ��ȥ���½���10% ����ʹ��A�ŷ�ֵ
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25) {                                                                                               // ����15%�����½��� <10��λ����Ϊ��ʼ����
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // ʵ�ʲ������Ǵ������� ��
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A����߲���֡�е�λ��
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // �������λ��ΪA���½���10%���� A�ŷ�ֵλ�ã�С���ܳ���
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }
        // A B ����ȷ�������ο�  A ��λ 0 �ſ�1   b ��λ 99 �ſ�1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f; // ����A �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
        fStartPos       = fStartAmpDown10;
        float fEndPos   = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // ����B �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
                                                                                                                       //  A����߲�����B��λ��Ϊ�����
        fEndPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // ʵ�ʾ���mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL_Scan[ichannel].size(); // DAC �Ĵ�С������һ��

        // ���� fStartPos- fEndPos����߲�
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // ʵ�ʲ������Ǵ������� ��
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // ����ʵ�ʲ�������ʼ��
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // ����ʵ�ʲ�������ʼ��
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // ��������ݲŽ���ȱ�ݴ���
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // ��Aɨ����߲�����
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            float fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // �����������ߵ�����λ��

            float fDAC = m_DAC_RL_Scan[ichannel][fMaxAmpPos];
            float fDB  = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // �������ǰ���� ��DAC��dB��

            // ȱ�ݶ��ж��Ѿ����ݼ�¼
            if (fDB > 0) {                                  // Ϊȱ�� ���ϸ���nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // ��һ�η���ȱ�ݵ�֡ ԭ��Ϊfalse ״̬

                    // pTechniques->mDB_DEFECT_DATA[ichannel].nIndex = pTechniques->m_pDefect[ichannel].size();
                    //	pTechniques->mDB_DEFECT_DATA[ichannel].nParam1 = pTechniques->m_iTreadCirIndex * pTechniques->GetTreadPoint() + pTechniques->m_iSaveTreadPointIndex;
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // ����Ϊȱ��
            } else {
                if (pTechniques->m_bNewDEFECT[ichannel]) { // ԭ����ȱ�� �����ȱ��

                    pTechniques->m_bNewDEFECT[ichannel] = false; // ȱ�����

                } else { // ̤�治�ж�͸��
                }
            }
        }

        // ̤���ͼ����
        if (pBscanData[pTechniques->m_iTreadPointIndex] < fMaxAmp) // ����Ƕȼ�¼�Ľ�֡�� ȱ��ֵС�� ��ǰ��ȡ��֡�� ȱ��ֵ ��������
        {
            pBscanData[pTechniques->m_iTreadPointIndex] = fMaxAmp; // ȱ�ݲ����� �������Բ������� ֱ������ȱ��

            if (pTechniques->m_bNewDEFECT[ichannel]) { // Ϊȱ��
                if (pTechniques->m_iTreadPointIndex + i / 2 * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pBscanCH.size()) {
                    pBscanCH[pTechniques->m_iTreadPointIndex + i / 2 * pTechniques->m_Scan.m_iDrawProbleTreadSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1; // 0 Ĭ��0 ��ʶ��ȱ��  ��ȱ����ʾͨ����
                }
            }
        }
    }

    // �������ݴ���
    UCHAR* pCscanData = &pTechniques->m_Scan.m_pCscanData[0];
    UCHAR* pCscanCH   = &pTechniques->m_Scan.m_pCscanCH[0];

    for (int i = 0; i < 2; i++) {
        if (pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pCscanCH.size()) {
            if (pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] == 0) { // δ���ʱ������Ϊ���״̬
                pCscanCH[pTechniques->m_iSidePointIndex + i * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] = 11;   // �м����ȱ��Ϊ11
            }
        }
    }

    for (int i = 0; i < 4; i++) // �����ĸ�
    {
        int ichannel = pTechniques->m_Scan.m_pSideProbe[i];

        if ((pTechniques->m_iSidePointIndex + (ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2 * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint() >= pTechniques->m_Scan.m_pCscanCH.size())) {
            continue;
        }
        Channel* pChannel = &pTechniques->m_pChannel[ichannel];
        // CountDAC(pTechniques, ichannel, pChannel->m_fGain, (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_fDelay);

        // ���� A�ŷ�ֵ�½��� <10%��λ�� A�ŷ�ֵ>25% ��ȥ���½���10% ����ʹ��A�ŷ�ֵ
        float fStartAmpDown10 = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
        if (Read.pGateAmp[ichannel][GATE_A] / 255.0f > 0.25) {                                                                                               // ����15%�����½��� <10��λ����Ϊ��ʼ����
            int  nSizeAscan         = Read.pAscan[ichannel].size();                                                                                          // ʵ�ʲ������Ǵ������� ��
            int  GateAmpIndex       = nSizeAscan * (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f / pChannel->m_fRange; // A����߲���֡�е�λ��
            BYTE pGateAmpDown10     = 255 * 0.1;
            int  GateAmpDown10Index = GateAmpIndex;
            for (int i = GateAmpIndex; i < nSizeAscan; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp < pGateAmpDown10) {
                    GateAmpDown10Index = i;
                    break;
                }
            }
            if (GateAmpIndex < GateAmpDown10Index && GateAmpDown10Index < nSizeAscan) { // �������λ��ΪA���½���10%���� A�ŷ�ֵλ�ã�С���ܳ���
                fStartAmpDown10 = pChannel->m_fRange * GateAmpDown10Index / nSizeAscan;
            }
        }
        // A B ����ȷ�������ο�  A ��λ 0 �ſ�1   b ��λ 99 �ſ�1
        float fStartPos = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + fAaxAmp_0; // ����A �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
        // fStartPos = fStartAmpDown10;
        float fEndPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f - f0_AaxAmp; // ����B �з�ֵλ�� ��ʵ�ʾ�������ʱ�� ��ΪDAC�ж�ȱ�ݵ���ʼλ��
        fEndPos       = (Read.pGatePos[ichannel][GATE_A]) / 100.0f * (float)pChannel->m_iVelocity / 2000.0f + pChannel->m_pGatePos[GATE_B] * pChannel->m_fRange;

        float fRange = pChannel->m_fRange; // ʵ�ʾ���mm
        if (fStartPos > fRange)
            fStartPos = fRange;
        if (fEndPos < 0) {
            fEndPos = 0;
        }
        int DACSize = m_DAC_RL[ichannel].size(); // DAC �Ĵ�С������һ��

        // ���� fStartPos- fEndPos����߲�
        int  nSizeAscan  = Read.pAscan[ichannel].size();    // ʵ�ʲ������Ǵ������� ��
        int  nStartIndex = nSizeAscan * fStartPos / fRange; // ����ʵ�ʲ�������ʼ��
        int  nEndIndex   = nSizeAscan * fEndPos / fRange;   // ����ʵ�ʲ�������ʼ��
        BYTE fMaxAmp     = 0;
        int  nMaxIndex   = 0;
        // ��������ݲŽ���ȱ�ݴ���
        if (nStartIndex < nSizeAscan && nEndIndex < nSizeAscan) {
            fMaxAmp   = Read.pAscan[ichannel][nStartIndex];
            nMaxIndex = 0; // ��Aɨ����߲�����
            for (int i = nStartIndex; i < nEndIndex; i++) {
                BYTE btyeAmp = Read.pAscan[ichannel][i];
                if (btyeAmp >= fMaxAmp) {
                    fMaxAmp   = btyeAmp;
                    nMaxIndex = i;
                }
            }

            float fMaxAmpPos = fRange * nMaxIndex / nSizeAscan; // �����������ߵ�����λ��
            float fDAC       = m_DAC_RL[ichannel][fMaxAmpPos];
            float fDB        = mDacCount.CoutdBOffsetByAmp(fMaxAmp / 255.0, fDAC); // �������ǰ���� ��DAC��dB��

            // ȱ�ݶ��ж��Ѿ����ݼ�¼
            if (fDB > 0) {                                  // Ϊȱ�� ���ϸ���nDEFECTindex
                if (!pTechniques->m_bNewDEFECT[ichannel]) { // ��һ�η���ȱ�ݵ�֡ ԭ��Ϊfalse ״̬
                }
                pTechniques->m_bNewDEFECT[ichannel] = true; // ����Ϊȱ��

            } else {
                if (pTechniques->m_bNewDEFECT[ichannel]) { // ԭ����ȱ�� �����ȱ��

                    pTechniques->m_bNewDEFECT[ichannel] = false; // ȱ�����

                } else { // ̤�治�ж�͸��
                }
            }
        }

        // ͸��ȱ���ж� ����ֱ̽ͷ�ж�͸��
        if ((ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[0] || ichannel == g_MainProcess.m_Techniques.m_Scan.m_pSideTrUT[1]) && false) {
            int nGate_Bamp = Read.pGateAmp[ichannel][GATE_B];
            int nGate_BPos = Read.pGatePos[ichannel][GATE_B] / 100.0f * (float)pChannel->m_iVelocity / 2000.0f;
            // ����͸���������µ��·�ֵ
            int nAmpTS = mDacCount.CoutDACNewPeek(mDetetionParam2995_200[ichannel].fScanGain + mDetetionParam2995_200[ichannel].fScanCompensate,
                                                  mDetetionParam2995_200[ichannel].fScanCompensate + mDetetionParam2995_200[ichannel].fScanTrUTCompensate,
                                                  nGate_Bamp); // ���ݵ�ǰ��̽������ ����� ͸�������µĲ�����

            // ͸��ȱ�ݵ��жϣ�
            // 1�����жϲ����Ƿ񽵵�50%��
            // 2.�ж��Ƿ��з�͸��ȱ��
            // 3.û���ж�͸��ȱ��
            // 4.���ַ�͸��ȱ�� ���̽���͸��ȱ���ж�
            //	if (nAmpTS < 255 * 0.5)// ��������50%��͸������
            {
                if (nAmpTS < 2)
                    nAmpTS = 2;                                               // ��Сֵ��Ϊ0
                float fdBTS = mDacCount.CoutdBOffsetByAmp(nAmpTS, 255 * 0.5); // �������50%������dB��

                if (!pTechniques->m_bNewDEFECT[ichannel]) { // ��ȱ������� ���ж��Ƿ���͸������
                    if (fdBTS < 0) {
                        if (!pTechniques->m_bNewDEFECT_TS[ichannel]) { // ��һ�η���ȱ�ݵ�֡ ԭ��Ϊfalse ״̬
                        }
                        pTechniques->m_bNewDEFECT_TS[ichannel] = true; // ����Ϊȱ��
                    }

                    else {
                        if (pTechniques->m_bNewDEFECT_TS[ichannel]) { // ԭ����ȱ�� �����ȱ��

                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // ȱ�����
                        }
                    }
                } else {
                    {                                                 // ���ַ�͸������ȱ�� ���浱ǰ�� ͸��ȱ��
                        if (pTechniques->m_bNewDEFECT_TS[ichannel]) { // ԭ����ȱ�� �����ȱ��

                            pTechniques->m_bNewDEFECT_TS[ichannel] = false; // ȱ�����
                        }
                    }
                }
            }
        }

        if (pTechniques->m_iSidePointIndex > 100) {
            int test = 0;
        }
        // �����ͼ����
        if (pTechniques->m_bNewDEFECT_TS[ichannel] || pTechniques->m_bNewDEFECT[ichannel]) // ͸���������� ȱ��
        {
            pCscanData[pTechniques->m_iSidePointIndex] = fMaxAmp; // ȱ�ݲ����� �������Բ������� ֱ������ȱ��
            if (pTechniques->m_iSidePointIndex + (ichannel - pTechniques->m_Scan.m_pSideProbe[0]) / 2 * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint() < pTechniques->m_Scan.m_pCscanCH.size()) {
                int DrawIndex = 0;
                if (ichannel == 6 || ichannel == 8)
                    DrawIndex = 0; // ǰ��һ��̽ͷ
                if (ichannel == 7 || ichannel == 9)
                    DrawIndex = 1; // ����һ��̽ͷ ��ͼ����̽ͷ֮��ļ��Ȧ��

                pCscanCH[pTechniques->m_iSidePointIndex + DrawIndex * pTechniques->m_Scan.m_iDrawProbleSideSize * pTechniques->m_Scan.GetTreadPoint()] = ichannel + 1; // 0 Ĭ��0 ��ʶ�޼�� ��ȱ��Ϊ11  ��ȱ����ʾͨ����
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