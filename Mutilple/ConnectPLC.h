#pragma once

#include "ICoder.h"

class Techniques;
class ConnectPLC : public ICoderAngle
{
public:

    ConnectPLC();
    ~ConnectPLC();
    virtual bool isConnected();
    virtual void Close();

    //开始探伤
    virtual bool Start();
    //结束探伤
    virtual bool Stop();
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);
    float GetPLCSideX();
    float GetPLCSideY();
    float GetPLCTreadX();
    float GetPLCTreadY();


    //PLC的自动/手动状态
    bool SetPLCAuto(bool bAuto );
    bool GetPLCAuto();

    //自动开始

    bool SetPLCAutoStart();

    //自动状态
    bool GetPLCAutoState();
    //急停
    bool SetPLCStop();
    /*PLC 设置速度
    */
    bool SetPLC_Speed();

    /**自动下发命令参数 踏面移动(右轴)
fPosX1    VD 1060
fPosX2    VD 1064
fPosY1     VD1080
fPosY2     VD1084
fPosStep  VD1076
*/
    void SetPLC_TreadMove(float fPosX1, float fPosX2, float fPosY1, float fPosY2, float fPosStep);


   /**自动下发命令参数 侧面移动(左轴)
fPosX1    VD 1020
fPosX2    VD 1024
fPosY1    VD 1040
fPosY2    VD 1044
fPosStep  VD1036
*/
    void SetPLC_SideMove(float fPosX1, float fPosX2, float fPosY1,  float fPosStep);


    /**缺陷命令参数下发 

*/
    void SetPLC_DetectMove(float fSideX,float fTreadY,   float fWheelAngle);


    //获取缺陷定位状态 缺陷定位准备就绪
    int  GetDetectState();

    /* 一键原点
    */
    void SetAllZero();


    void GetAllFloatValue();
    //侧面缺陷定位
  
    //踏面缺陷定位
    
private:
    //循环读取PLC信息
    void _Read(); //扫差用
    void  _PLCStatus(); //非扫差用



public:
   

    Thread m_ThreadRead;
    Thread m_ThreadPLCStatus;
    PLC_SCAN_PAPA m_PlcDownParam;

    PLC_SPeed_PAPA mPLCSpeed;

    //PLC状态
    bool m_bPLCAuto;//PLC自动状态标识

    std::map<string, float> mPLCFloatValueMap;
};

