#pragma once
class System  : public Object
{
public:
    /** ��ȡ���л�����*/
    virtual DWORD GetSerializeLen(INT32 iType = -1);

    /**���л���������*/
    virtual BYTE* Serialize();

    /**�����л���������*/
    virtual BOOL Unserialize(BYTE* pData, DWORD dwLen);

    //INI ���ձ��� ��ȡ
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);

public:
    INT32 m_iFrequency;				//�ظ�Ƶ��
    INT32 m_iVoltage;				//��ѹ
    INT32 m_iPulseWidth;			//������
    INT32 m_iTxFlag;				//�����־
    INT32 m_iRxFlag;				//���ձ�־
    INT32 m_iChMode;				//ͨ������ģʽ
    INT32 m_iScanIncrement;			//ɨ������
    INT32 m_iResetCoder;			//����������
    INT32 m_iLEDStatus;				//LED��
    INT32 m_iWorkType;				//����ģʽ
    INT32 m_iControlTime;
    INT32 m_iAxleTime;
};

