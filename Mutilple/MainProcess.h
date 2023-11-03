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

    //�Լ�
    void Check(ICheckCarrier* pInitCheck);
    //�Լ�: �ļ�Ŀ¼
    bool CheckDir();
    //�Լ�: У��
    bool CheckCalib();
    //�Լ�: ̽��
    bool CheckScan();
    //�Լ�: ���ݴ���
    bool CheckAnalysis();
    //PLC ����
    bool CheckPLC();
    void StartScan();
    void StopScan();

    //��ȡ��ǰͨ�����
    int GetChannelIndex();
    //��ȡ��ǰͨ��
    Channel* GetCurChannel();

    /**
     * @brief ��ȡͨ��
     * @param index 
     * @return ͨ��ָ��
    */
    Channel* GetChannel(int index);


public:
    Techniques m_Techniques = {};
    HDBridge   m_HDBridge   = {};
    ConnectPLC m_ConnectPLC = {};
    User       m_User       = {};

    MAINPROCESS_TYPE m_enumMainProType = {};
};

