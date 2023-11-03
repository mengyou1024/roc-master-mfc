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

    // ����̽�˱�׼
    void SetDetectionStd(int iDetectionStd);

    // ��������
    void Compute();

    // ��ʼ̽��
    virtual bool Start();
    // ����̽��
    virtual bool Stop();

    // ���ݶ�ȡ����
    virtual void Test();
    // �������ݴ���
    void Dealwith(READ_DATA& Read);

    // ���ձ���
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);
    // ���浱ǰ�����ַ�����"LastParam.ini"
    void LoadTecName(LPCTSTR lpName);
    void SaveTecName(LPCTSTR lpName);

    // �����Ʊ��湤�� �����ֳ����Խ��� ���ʺ�ini����
    bool LoadTecBinary(LPCTSTR lpName);
    bool SaveTecBinary(LPCTSTR lpName);
    // ������ݱ���
    bool LoadRecData(LPCTSTR lpName);
    bool SaveRecData(LPCTSTR lpName);
    // ��¼����ÿȦ����
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
    int       m_GroupScanOffset = 0; /// ����ɨ��ʱ��ƫ��

    bool      m_bDailyScan; // �Ƿ����ճ�У��
    int       m_nDailyType; // 0 ���� 1�깤
    SCAN_TYPE m_ScanType;
    TCHAR     m_pCurTechName[STR_LEN]; // ��ǰ��������
    // ����
    Specimen m_Specimen;

    //

    // ϵͳ����
    System m_System;
    // ͨ������
    std::vector<Channel> m_pChannel;
    // ɨ�����
    Scan m_Scan;

    // ̽�˱�׼
    int m_iDetectionStd;

    // ��ǰͨ��
    int m_iChannel;
    // ��ǰȦ��
    union {
        int m_iSideCirIndex;
        int m_iTreadCirIndex;
    };

    // ��ǰ�������
    int m_iSidePointIndex;
    int m_iTreadPointIndex;

    // ��ǰ���ݼ�¼����
    int m_iSaveSidePointIndex;
    int m_iSaveTreadPointIndex;

public:
    DRAW_BUFFER m_pDraw[2]; // ʵʱ�ɼ���ǰ֡����

    std::vector<RECORD_DATA> m_pRecord; // ��¼������

    // ��¼�����ݻ��� ���ڽǶȱ��������½�������߻������β�һ��֡�� ������¼ȱ�ݴ�С���ͣ�����ȡ���ȱ�����ݱ���
    std::vector<RECORD_DATA_RES*> m_pRecord_Buffer;

    std::vector<DB_DEFECT_DATA*> m_pDefect[HD_CHANNEL_NUM]; //   ȱ������

    DB_DEFECT_DATA mDB_DEFECT_DATA[HD_CHANNEL_NUM]; // ���ʱ��ǰȱ��

    DB_DEFECT_DATA mSelect_DB_DEFECT_DATA;          // ȱ�ݶ�λʱѡ�е�ȱ��
    bool           m_bNewDEFECT[HD_CHANNEL_NUM];    // ����ȱ�� ����true  ����ȱ��false;
    bool           m_bNewDEFECT_TS[HD_CHANNEL_NUM]; // ����͸������ȱ�� ����true  ����ȱ��false; ���浥��ֱ̽ͷ

    int   m_nLastIndex; // ���濪ʼ֡
    float m_fLastAngle; // ���濪ʼ�Ƕ�
    int   m_nLastCir;   // ���濪ʼȦ��

    // ��¼���ݲ���ÿȦ������
    int m_iSidePoints{360};
    // ��¼����̤��ÿȦ������
    int m_iTreadPoints{360};

    // ��¼����ÿ�����ǶȲ���
    float m_fSideAngleStep{1.0f};
    // ��¼����ÿ�����ǶȲ���
    float m_fTreadAngleStep{1.0f};

protected:
    int           _AngleSection;
    DetectionStd* _pDetectionStd;

    // �������ݶ�ȡ·�� ��󱣴�ɨ�������
    CString m_strLastSaveScanData;
};
