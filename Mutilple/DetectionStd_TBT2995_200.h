#pragma once

#include "DetectionStd.h"
#include "DacCount.h"

#pragma warning( disable: 26495 )

const int MAX_DAC = 2;
class DetectionStd_TBT2995_200 : public DetectionStd
{
public:
    //标准名称
    virtual LPCTSTR GetName() { return _T("TB/T2995-200"); }

    //初始化工艺
    virtual bool InitTechniques(Techniques* pTechniques);

    //依据标准探伤
    virtual bool Detection(Techniques* pTechniques, READ_DATA& Read);
    //
     bool ReViewDetection(Techniques* pTechniques, RECORD_DATA& Read,int nRecordIndex);
    virtual  bool LoadTec(size_t ch, LPCTSTR lpName) ;
    virtual bool SaveTec(size_t ch, LPCTSTR lpName);

    DETECTION_PARAM2995_200  mDetetionParam2995_200[HD_CHANNEL_NUM];

    //当前增益下的DAC 曲线点
    DacCount mDacCount;
    vector<float> m_DAC_M[HD_CHANNEL_NUM]; //母线
    vector<float> m_DAC_RL[HD_CHANNEL_NUM]; //评定线 和 判废线
    vector<float> m_DAC_ED[HD_CHANNEL_NUM]; //评定线 和 判废线

    /*
    * 
    * fGateAmpPos 动态DAC的A门最高波，无数据使用A门的起始位置
    */
    void  CountDAC(Techniques* pTechniques, int ch, float nShowdB,float fGateAmpPos);


    //扫差时动态计算DAC 区分成像DAC数据  扫查时数据更新较快成像快 不区分容易软件奔溃
    void   CountDAC_Scan(Techniques* pTechniques, int ch, float nShowdB, float fGateAmpPos);

    vector<float> m_DAC_RL_Scan[HD_CHANNEL_NUM]; //评定线 和 判废线

};

