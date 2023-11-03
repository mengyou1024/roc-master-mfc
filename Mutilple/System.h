#pragma once
class System  : public Object
{
public:
    /** 获取序列化长度*/
    virtual DWORD GetSerializeLen(INT32 iType = -1);

    /**序列化工件数据*/
    virtual BYTE* Serialize();

    /**反序列化工件数据*/
    virtual BOOL Unserialize(BYTE* pData, DWORD dwLen);

    //INI 工艺保存 读取
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);

public:
    INT32 m_iFrequency;				//重复频率
    INT32 m_iVoltage;				//电压
    INT32 m_iPulseWidth;			//脉冲宽度
    INT32 m_iTxFlag;				//发射标志
    INT32 m_iRxFlag;				//接收标志
    INT32 m_iChMode;				//通道工作模式
    INT32 m_iScanIncrement;			//扫查增量
    INT32 m_iResetCoder;			//编码器清零
    INT32 m_iLEDStatus;				//LED灯
    INT32 m_iWorkType;				//工作模式
    INT32 m_iControlTime;
    INT32 m_iAxleTime;
};

