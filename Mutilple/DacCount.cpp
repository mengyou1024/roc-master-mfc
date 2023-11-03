#include "pch.h"

#include "DacCount.h"
#include <algorithm>

// const float CALIB_BASE_AMP = 0.80F;

DacCount::DacCount() {}

// 不同距离  相同孔径的 波幅 计算公式      孔1的 距离  len1 峰值 peek1  孔2的距离len2
float DacCount::CoutNewPeekByDis(float len1, float peek1, float len2) {
    float offseGain = (float)(40.0f * log10(len1 / len2)); //
    return (float)(peek1 * pow(10, offseGain / 20.0));
}

// 根据 原来的增益和峰值 计算新增益下的峰值
float DacCount ::CoutDACNewPeek(float OlddB, float NewdB, float OldPeek) {
    float peek   = OldPeek;
    float offset = NewdB - OlddB;
    peek         = (float)(OldPeek * pow(10, offset / 20.0));

    return peek;
}

// 计算两个 波高之间的 dB差
float DacCount::CoutdBOffsetByAmp(float Amp1, float Amp2) {
    float dB = (float)(20.0f * log10(Amp1 / Amp2));
    return dB;
}
void PushLinePoint(float CALIB_BASE_AMP, float depth, float amp) {
    int re = (int)std::floorf(20.0F * log10(CALIB_BASE_AMP / amp));
}
/*
 * 探伤增益 + 判废线的 增益
 */
void DacCount::CoutDacCurve(DETECTION_PARAM2995_200* DACStruct, float gain, float delay, float range, vector<float>& DAC_M) {
    float fDelay        = delay;
    int   nDACHoleIndex = -1;
    int   verSize       = (int)std::floorf(range); // 超出声程不在计算数据
    for (int i = 0; i < DACStruct->nUseHole; i++) {
        if (DACStruct->fDAC_HoleDepth[i] >= fDelay) {
            nDACHoleIndex = i;
            break;
        }
    }

    float fBaseAmp = CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[0]); // 第一个点的增加为基本增益
    // 判断当前声程位置 即延时对应的孔
    for (int iDAC = 0; iDAC < DACStruct->nUseHole; iDAC++) {
        if (iDAC == 0) {
            int iLast = (int)std::floor(DACStruct->fDAC_HoleDepth[0] - fDelay); // 第一个点之前波幅都一样
            for (int i = 0; i < iLast; i++) {
                DAC_M.push_back(fBaseAmp); // 第一个点之前 都一样
                if (DAC_M.size() > verSize) {
                    return;
                }
            }
        } else {
            float x1 = DACStruct->fDAC_HoleDepth[iDAC - 1];
            float x2 = DACStruct->fDAC_HoleDepth[iDAC];

            float fLdealAmp =
                CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC - 1] * (x1 / x2)); // 计算 第二孔理论值
            //		float k = (DACStruct->fDAC_HolebAmp[iDAC - 1] - fIdealAmp) / (DACStruct->fDAC_HolebAmp[iDAC - 1] -
            //DACStruct->fDAC_HolebAmp[iDAC ]);  //两点 理论 与 实际的比值
            float frealityAmp   = CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC]); // 实际测量的Db差
            float fAmpoffsetMax = frealityAmp - fLdealAmp;
            float dBoffset      = CoutdBOffsetByAmp(frealityAmp, fLdealAmp); // 实际与理论的 Db差

            //	float BaseK = CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC - 1]) /
            //DACStruct->fDAC_HoleDepth[iDAC-1];
            // 第一个点K值
            float fLdealK =
                DACStruct->fDAC_HoleDepth[iDAC - 1] / CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC - 1]);
            //		第二个点K值
            float frealityK =
                DACStruct->fDAC_HoleDepth[iDAC] / CoutDACNewPeek(DACStruct->fDAC_BaseGain, gain, DACStruct->fDAC_HolebAmp[iDAC]);
            float fKOffset = (frealityK - fLdealK);

            int iLast = (int)std::floor(DACStruct->fDAC_HoleDepth[iDAC]);

            // 判断延时在 fDAC_HoleDepth[iDAC - 1] 什么位置
            int startPos = (int)std::floor(DACStruct->fDAC_HoleDepth[iDAC - 1] + 1.0f);

            for (int i = startPos; i < iLast; i++) {
                float xNow = (float)i;
                // 当前K值
                float curK = fLdealK + fKOffset / (x2 - x1) * (xNow - x1);

                float fIdealAmp_K = xNow / curK;

                if (i > fDelay) {
                    DAC_M.push_back(fIdealAmp_K);
                    if (DAC_M.size() > verSize) {
                        return;
                    }
                }
            }

            if (iDAC == DACStruct->nUseHole - 1) // 最后一个点到 全部声程最后
            {
                int size     = DAC_M.size();
                int startPos = DACStruct->fDAC_HoleDepth[iDAC]; // 最后点开始计算DAC
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
