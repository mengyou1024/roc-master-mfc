#pragma once

#include "Techniques.h"

enum { DETECTIONSTD_NONE, DETECTIONSTD_TBT2995_200 };

class DetectionStd
{
public:
    virtual ~DetectionStd();

    //标准名称
    virtual LPCTSTR GetName() = 0;

    //根据标准初始化工艺
    virtual bool InitTechniques(Techniques* pTechniques) = 0;

    //依据标准探伤，如探伤完成后对缺陷的判断都应该放在每个标准的Detection函数中
    virtual bool Detection(Techniques* pTechniques, READ_DATA& Read) = 0;

    virtual  bool LoadTec(size_t ch, LPCTSTR lpName) = 0;
    virtual bool SaveTec(size_t ch, LPCTSTR lpName) = 0;
};

