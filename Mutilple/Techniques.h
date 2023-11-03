#pragma once

#include "Channel.h"
#include "Scan.h"
#include "Specimen.h"
#include "System.h"

class DetectionStd;
class DetectionStd_TBT2995_200;

class Techniques {
public:
    Techniques();
    ~Techniques();

    // 设置探伤标准
    void SetDetectionStd(int iDetectionStd);

    // 参数计算
    void Compute();

    // 开始探伤
    virtual bool Start();
    // 结束探伤
    virtual bool Stop();

    // 数据读取测试
    virtual void Test();
    // 超声数据处理
    void Dealwith(READ_DATA& Read);

    // 工艺保存
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);
    // 保存当前工艺字符串到"LastParam.ini"
    void LoadTecName(LPCTSTR lpName);
    void SaveTecName(LPCTSTR lpName);

    // 二进制保存工艺 由于现场电脑较慢 不适合ini保存
    bool LoadTecBinary(LPCTSTR lpName);
    bool SaveTecBinary(LPCTSTR lpName);
    // 检测数据保存
    bool LoadRecData(LPCTSTR lpName);
    bool SaveRecData(LPCTSTR lpName);
    // 记录数据每圈点数
    void SetSidePoints(int iPoints) noexcept {
        m_iSidePoints = iPoints;
    }

    const int GetSidePoint() const {
        return m_iSidePoints;
    }

    const float GetSideAngleStep() const {
        return m_fSideAngleStep;
    }

    void SetTreadPoints(int iPoints) noexcept {
        m_iTreadPoints = iPoints;
    }

    const int GetTreadPoint() const {
        return m_iTreadPoints;
    }

    const float GetTreadAngleStep() const {
        return m_fTreadAngleStep;
    }

    bool Techniques::CreateMultipleDirectory(LPCTSTR szPath);

    DetectionStd_TBT2995_200* GetDetectionStd();

public:
    int       m_GroupScanOffset = 0; /// 分组扫查时的偏移

    bool      m_bDailyScan; // 是否是日常校验
    int       m_nDailyType; // 0 开工 1完工
    SCAN_TYPE m_ScanType;
    TCHAR     m_pCurTechName[STR_LEN]; // 当前工艺名称
    // 工件
    Specimen m_Specimen;

    //

    // 系统参数
    System m_System;
    // 通道参数
    std::vector<Channel> m_pChannel;
    // 扫查参数
    Scan m_Scan;

    // 探伤标准
    int m_iDetectionStd;

    // 当前通道
    int m_iChannel;
    // 当前圈数
    union {
        int m_iSideCirIndex;
        int m_iTreadCirIndex;
    };

    // 当前成像点数
    int m_iSidePointIndex;
    int m_iTreadPointIndex;

    // 当前数据记录点数
    int m_iSaveSidePointIndex;
    int m_iSaveTreadPointIndex;

public:
    DRAW_BUFFER m_pDraw[2]; // 实时采集当前帧数据

    std::vector<RECORD_DATA> m_pRecord; // 记录的数据

    // 记录的数据缓存 由于角度编码器更新较慢，这边缓存两次不一样帧数 ，并记录缺陷大小类型，便于取最大缺陷数据保存
    std::vector<RECORD_DATA_RES*> m_pRecord_Buffer;

    std::vector<DB_DEFECT_DATA*> m_pDefect[HD_CHANNEL_NUM]; //   缺陷数据

    DB_DEFECT_DATA mDB_DEFECT_DATA[HD_CHANNEL_NUM]; // 检测时当前缺陷

    DB_DEFECT_DATA mSelect_DB_DEFECT_DATA;          // 缺陷定位时选中的缺陷
    bool           m_bNewDEFECT[HD_CHANNEL_NUM];    // 发现缺陷 设置true  不是缺陷false;
    bool           m_bNewDEFECT_TS[HD_CHANNEL_NUM]; // 发现透声不良缺陷 设置true  不是缺陷false; 侧面单晶直探头

    int   m_nLastIndex; // 缓存开始帧
    float m_fLastAngle; // 缓存开始角度
    int   m_nLastCir;   // 缓存开始圈数

    // 记录数据侧面每圈检测点数
    int m_iSidePoints{360};
    // 记录数据踏面每圈检测点数
    int m_iTreadPoints{360};

    // 记录数据每点数角度步进
    float m_fSideAngleStep{1.0f};
    // 记录数据每点数角度步进
    float m_fTreadAngleStep{1.0f};

protected:
    int           _AngleSection;
    DetectionStd* _pDetectionStd;

    // 测试数据读取路径 最后保存扫差的数据
    CString m_strLastSaveScanData;
};
