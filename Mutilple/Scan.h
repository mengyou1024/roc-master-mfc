#pragma once

class Scan {
public:
    ~Scan();
    // ����ÿȦ����
    void SetSidePoints(int iPoints);
    int  GetSidePoint() {
        return m_iSidePoints;
    }
    float GetSideAngleStep() {
        return m_fSideAngleStep;
    }

    void SetTreadPoints(int iPoints);
    int  GetTreadPoint() {
        return m_iTreadPoints;
    }
    float GetTreadAngleStep() {
        return m_fTreadAngleStep;
    }

    // ɨ��ʱ��ÿȦ�뾶��fRadius:��һȦ�뾶
    void SetScanRadius(float fRadius);

    void InitScanData();

public:
    // ̽ͷ�ֲ�
    int m_pTreadProbe[6]{0, 1, 2, 3, 4, 5}; // ̤��6���������� ����ֱ̽ͷ
    int m_pSideProbe[4]{6, 7, 8, 9};        // ����4�� 2��˫��
    int m_pSideTrUT[2]{6, 7};               // ͸�����ͨ��

    // ɨ��ʱ������ǰ������
    float m_fSideStep{1.0f};
    // ɨ��ʱ̤��ǰ������
    //  float m_fTreadStep{1.0f };

    // ����/̤�棬���Ȧ��   PLC�ĵ� ��ʼ-���� /����
    union {
        int m_iSideSize{10};
        int m_iTreadSize;
    };

    int m_iDrawProbleSideSize;  // �����ͼ����̽ͷ֮���Ȧ��
    int m_iDrawProbleTreadSize; // ̤���ͼ����̽ͷ֮���Ȧ��

    int m_iDrawSideSize  = {0};  // �����ͼȦ��   ���� ��ⷶΧ /����      �����⾶��7��-�����ھ���8��
    int m_iDrawTreadSize = {0}; // ̤���ͼȦ�� ���� ��ⷶΧ/����        ̤��������ȣ�2��-��Ե��ȣ�8��

    // ����ÿȦ���뾶
    std::vector<float> m_pSideRadius;

    // Bɨͼ������ //A����߲�   //DAC ���ȱ��dB
    std::vector<UCHAR> m_pBscanData;
    std::vector<UCHAR> m_pBscanCH; // ȱ��ͨ�� 0 ��ȱ�� 1-10 ��Ӧ����ȱ�ݵ�ͨ�� ��ͼʱѡ��Ӧ��ͨ����ɫ

    //  std::vector<int> m_pBscanIndex;   //ȱ�� ��Ӧ��record��ȱ����������

    // Cɨͼ������ //A����߲�
    std::vector<UCHAR> m_pCscanData;
    std::vector<UCHAR> m_pCscanCH; // ȱ��ͨ�� 0 ��ȱ�� 1-10 ��Ӧ����ȱ�ݵ�ͨ��  ��ͼʱѡ��Ӧ��ͨ����ɫ

    // std::vector<int> m_pCscanIndex;   //ȱ�� ��Ӧ��record��ȱ����������
private:
    // ����ÿȦ������
    int m_iSidePoints{72};
    // ̤��ÿȦ������
    int m_iTreadPoints{72};

    // ÿ�����ǶȲ���
    float m_fSideAngleStep{5.0f};
    // ÿ�����ǶȲ���
    float m_fTreadAngleStep{5.0f};

    //
};
