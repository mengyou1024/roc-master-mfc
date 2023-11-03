#pragma once

#include "ICheckCarrier.h"
#include "Techniques.h"
#include "HDBridge.h"
#include "ConnectPLC.h"
#include "User.h"
class MainProcess
{
public:
    MainProcess();
    ~MainProcess();

    void Close();

    //自检
    void Check(ICheckCarrier* pInitCheck);
    //自检: 文件目录
    bool CheckDir();
    //自检: 校验
    bool CheckCalib();
    //自检: 探伤
    bool CheckScan();
    //自检: 数据处理
    bool CheckAnalysis();
    //PLC 连接
    bool CheckPLC();
    void StartScan();
    void StopScan();

    //获取当前通道编号
    int GetChannelIndex();
    //获取当前通道
    Channel* GetCurChannel();

    /**
     * @brief 获取通道
     * @param index 
     * @return 通道指针
    */
    Channel* GetChannel(int index);


public:
    Techniques m_Techniques = {};
    HDBridge   m_HDBridge   = {};
    ConnectPLC m_ConnectPLC = {};
    User       m_User       = {};

    MAINPROCESS_TYPE m_enumMainProType = {};
};

