#include "pch.h"

#include "DacCount.h"
#include <algorithm>

// const float CALIB_BASE_AMP = 0.80F;

DacCount::DacCount() {}

// ��ͬ����  ��ͬ�׾��� ���� ���㹫ʽ      ��1�� ����  len1 ��ֵ peek1  ��2�ľ���len2
float DacCount::CoutNewPeekByDis(float len1, float peek1, float len2) {
    float offseGain = (float)(40.0f * log10(len1 / len2)); //
    return (float)(peek1 * pow(10, offseGain / 20.0));
}

// ���� ԭ��������ͷ�ֵ �����������µķ�ֵ
float DacCount ::CoutDACNewPeek(float OlddB, float NewdB, float OldPeek) {
    float peek   = OldPeek;
    float offset = NewdB - OlddB;
    peek         = (float)(OldPeek * pow(10, offset / 20.0));

    return peek;
}

// �������� ����֮��� dB��
float DacCount::CoutdBOffsetByAmp(float Amp1, float Amp2) {
    float dB = (float)(20.0f * log10(Amp1 / Amp2));
    return dB;
}
void PushLinePoint(float CALIB_BASE_AMP, float depth, float amp) {
    int re = (int)std::floorf(20.0F * log10(CALIB_BASE_AMP / amp));
}
/*
 * ̽������ + �з��ߵ� ����
 */
void DacCount::CoutDacCurve(DETECTION_PARAM2995_200* DACStruct, float gain, float delay, float range, vector<float>& DAC_M) {
    float fDelay        = delay;
    int   nDACHoleIndex = -1;
    int   verSize       = (int)std::floorf(range); // �������̲��ڼ�������
    for (int i = 0; i < DACStruct->nUseHole; i++) {
        if (DACStruct->fDAC_HoleDepth[i] >= fDelay) {
            nDACHoleIndex = i;
            break;
        }
    }

    float fBaseAmp = CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[0]); // ��һ���������Ϊ��������
    // �жϵ�ǰ����λ�� ����ʱ��Ӧ�Ŀ�
    for (int iDAC = 0; iDAC < DACStruct->nUseHole; iDAC++) {
        if (iDAC == 0) {
            int iLast = (int)std::floor(DACStruct->fDAC_HoleDepth[0] - fDelay); // ��һ����֮ǰ������һ��
            for (int i = 0; i < iLast; i++) {
                DAC_M.push_back(fBaseAmp); // ��һ����֮ǰ ��һ��
                if (DAC_M.size() > verSize) {
                    return;
                }
            }
        } else {
            float x1 = DACStruct->fDAC_HoleDepth[iDAC - 1];
            float x2 = DACStruct->fDAC_HoleDepth[iDAC];

            float fLdealAmp =
                CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC - 1] * (x1 / x2)); // ���� �ڶ�������ֵ
            //		float k = (DACStruct->fDAC_HolebAmp[iDAC - 1] - fIdealAmp) / (DACStruct->fDAC_HolebAmp[iDAC - 1] -
            //DACStruct->fDAC_HolebAmp[iDAC ]);  //���� ���� �� ʵ�ʵı�ֵ
            float frealityAmp   = CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC]); // ʵ�ʲ�����Db��
            float fAmpoffsetMax = frealityAmp - fLdealAmp;
            float dBoffset      = CoutdBOffsetByAmp(frealityAmp, fLdealAmp); // ʵ�������۵� Db��

            //	float BaseK = CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC - 1]) /
            //DACStruct->fDAC_HoleDepth[iDAC-1];
            // ��һ����Kֵ
            float fLdealK =
                DACStruct->fDAC_HoleDepth[iDAC - 1] / CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC - 1]);
            //		�ڶ�����Kֵ
            float frealityK =
                DACStruct->fDAC_HoleDepth[iDAC] / CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC]);
            float fKOffset = (frealityK - fLdealK);

            int iLast = (int)std::floor(DACStruct->fDAC_HoleDepth[iDAC]);

            // �ж���ʱ�� fDAC_HoleDepth[iDAC - 1] ʲôλ��
            int startPos = (int)std::floor(DACStruct->fDAC_HoleDepth[iDAC - 1] + 1.0f);

            for (int i = startPos; i < iLast; i++) {
                float xNow = (float)i;
                // ��ǰKֵ
                float curK = fLdealK + fKOffset / (x2 - x1) * (xNow - x1);

                float fIdealAmp_K = xNow / curK;

                if (i > fDelay) {
                    DAC_M.push_back(fIdealAmp_K);
                    if (DAC_M.size() > verSize) {
                        return;
                    }
                }
            }

            if (iDAC == DACStruct->nUseHole - 1) // ���һ���㵽 ȫ���������
            {
                int size     = DAC_M.size();
                int startPos = DACStruct->fDAC_HoleDepth[iDAC]; // ���㿪ʼ����DAC
                if (fDelay > startPos)
                    startPos = fDelay;
                for (int i = startPos; i < startPos + range - size; i++) //
                {
                    x1 = DACStruct->fDAC_HoleDepth[iDAC];
                    x2 = i;
                    //	if (i > fDelay) {
                    DAC_M.push_back(CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC]) * (x1 / x2));
                    //}
                    if (DAC_M.size() > verSize) {
                        return;
                    }
                }
            }
        }
    }
}
