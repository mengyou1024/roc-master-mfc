#pragma once
class Channel : public Object
{
public:
    /** ��ȡ���л�����*/
    virtual DWORD GetSerializeLen(INT32 iType = -1);

    /**���л���������*/
    virtual BYTE* Serialize();

    /**�����л���������*/
    virtual BOOL Unserialize(BYTE* pData, DWORD dwLen);


   void    CopyeChannel(Channel*pCH);
public:
    FLOAT m_fRange;                 //����
    INT32 m_iVelocity;              //����
    FLOAT m_fDelay;  			    //��ʱ
    FLOAT m_fOffset;      			//���
    INT32 m_iSampleFactor;	     	//��������
    FLOAT m_fGain;					//����
    INT32 m_iFilter;				//�˲�
    INT32 m_iDemodu;				//�첨��ʽ
    INT32 m_iPhaseReverse;			//��λ��ת

    INT32 m_pGateAlarmType[2];		//���ű�������
    FLOAT m_pGatePos[2];			//������λ
    FLOAT m_pGateWidth[2];			//���ſ��
    FLOAT m_pGateHeight[2];			//���Ÿ߶�

    INT32 m_iGateBType;				//����2����

    
        //INI ���ձ��� d
    bool LoadTec(size_t ch, LPCTSTR lpName);
    bool SaveTec(size_t ch, LPCTSTR lpName);
public:
    int m_pCoder[2];
    struct _gate_data
    {
        float fAmp;
        float fPos;
    }m_pGateData[MAX_GATE_NUM];
    float m_fGateBDepth;
};

