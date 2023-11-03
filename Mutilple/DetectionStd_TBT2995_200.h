#pragma once

#include "DetectionStd.h"
#include "DacCount.h"

#pragma warning( disable: 26495 )

const int MAX_DAC = 2;
class DetectionStd_TBT2995_200 : public DetectionStd
{
public:
    //��׼����
    virtual LPCTSTR GetName() { return _T("TB/T2995-200"); }

    //��ʼ������
    virtual bool InitTechniques(Techniques* pTechniques);

    //���ݱ�׼̽��
    virtual bool Detection(Techniques* pTechniques, READ_DATA& Read);
    //
     bool ReViewDetection(Techniques* pTechniques, RECORD_DATA& Read,int nRecordIndex);
    virtual  bool LoadTec(size_t ch, LPCTSTR lpName) ;
    virtual bool SaveTec(size_t ch, LPCTSTR lpName);

    DETECTION_PARAM2995_200  mDetetionParam2995_200[HD_CHANNEL_NUM];

    //��ǰ�����µ�DAC ���ߵ�
    DacCount mDacCount;
    vector<float> m_DAC_M[HD_CHANNEL_NUM]; //ĸ��
    vector<float> m_DAC_RL[HD_CHANNEL_NUM]; //������ �� �з���
    vector<float> m_DAC_ED[HD_CHANNEL_NUM]; //������ �� �з���

    /*
    * 
    * fGateAmpPos ��̬DAC��A����߲���������ʹ��A�ŵ���ʼλ��
    */
    void  CountDAC(Techniques* pTechniques, int ch, float nShowdB,float fGateAmpPos);


    //ɨ��ʱ��̬����DAC ���ֳ���DAC����  ɨ��ʱ���ݸ��½Ͽ����� �����������������
    void   CountDAC_Scan(Techniques* pTechniques, int ch, float nShowdB, float fGateAmpPos);

    vector<float> m_DAC_RL_Scan[HD_CHANNEL_NUM]; //������ �� �з���

};

