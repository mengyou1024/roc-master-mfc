#pragma once

#include "Techniques.h"

enum { DETECTIONSTD_NONE, DETECTIONSTD_TBT2995_200 };

class DetectionStd
{
public:
    virtual ~DetectionStd();

    //��׼����
    virtual LPCTSTR GetName() = 0;

    //���ݱ�׼��ʼ������
    virtual bool InitTechniques(Techniques* pTechniques) = 0;

    //���ݱ�׼̽�ˣ���̽����ɺ��ȱ�ݵ��ж϶�Ӧ�÷���ÿ����׼��Detection������
    virtual bool Detection(Techniques* pTechniques, READ_DATA& Read) = 0;

    virtual  bool LoadTec(size_t ch, LPCTSTR lpName) = 0;
    virtual bool SaveTec(size_t ch, LPCTSTR lpName) = 0;
};

