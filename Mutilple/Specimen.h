#pragma once

class Specimen
{

public:
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);
public:
    ////��������
    //TCHAR m_pName[STR_LEN];
    ////�����ͺ�
    //TCHAR m_pModel[STR_LEN];
    ////��������
    //TCHAR m_pMaterial[STR_LEN];
    ////�������
    //TCHAR m_pNumber[STR_LEN];
    ////¯�� 
    //TCHAR m_pHeatNumber[STR_LEN];

    //����ֱ��
    //float m_fDiameter;

    //������Ϣ 
    WHEEL_PAPA m_WheelParam; //Ŀǰ�ݶ��ṹ�� �ӿ쿪������
};

