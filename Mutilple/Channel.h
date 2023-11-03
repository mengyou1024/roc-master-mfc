#pragma once
class Channel : public Object
{
public:
    /** 获取序列化长度*/
    virtual DWORD GetSerializeLen(INT32 iType = -1);

    /**序列化工件数据*/
    virtual BYTE* Serialize();

    /**反序列化工件数据*/
    virtual BOOL Unserialize(BYTE* pData, DWORD dwLen);


   void    CopyeChannel(Channel*pCH);
public:
    FLOAT m_fRange;                 //声程
    INT32 m_iVelocity;              //声速
    FLOAT m_fDelay;  			    //延时
    FLOAT m_fOffset;      			//零点
    INT32 m_iSampleFactor;	     	//采样因子
    FLOAT m_fGain;					//增益
    INT32 m_iFilter;				//滤波
    INT32 m_iDemodu;				//检波方式
    INT32 m_iPhaseReverse;			//相位反转

    INT32 m_pGateAlarmType[2];		//波门报警类型
    FLOAT m_pGatePos[2];			//波门门位
    FLOAT m_pGateWidth[2];			//波门宽度
    FLOAT m_pGateHeight[2];			//波门高度

    INT32 m_iGateBType;				//波门2类型

    
        //INI 工艺保存 d
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

