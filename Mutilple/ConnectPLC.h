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

    //��ʼ̽��
    virtual bool Start();
    //����̽��
    virtual bool Stop();
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);
    float GetPLCSideX();
    float GetPLCSideY();
    float GetPLCTreadX();
    float GetPLCTreadY();


    //PLC���Զ�/�ֶ�״̬
    bool SetPLCAuto(bool bAuto );
    bool GetPLCAuto();

    //�Զ���ʼ

    bool SetPLCAutoStart();

    //�Զ�״̬
    bool GetPLCAutoState();
    //��ͣ
    bool SetPLCStop();
    /*PLC �����ٶ�
    */
    bool SetPLC_Speed();

    /**�Զ��·�������� ̤���ƶ�(����)
fPosX1    VD 1060
fPosX2    VD 1064
fPosY1     VD1080
fPosY2     VD1084
fPosStep  VD1076
*/
    void SetPLC_TreadMove(float fPosX1, float fPosX2, float fPosY1, float fPosY2, float fPosStep);


   /**�Զ��·�������� �����ƶ�(����)
fPosX1    VD 1020
fPosX2    VD 1024
fPosY1    VD 1040
fPosY2    VD 1044
fPosStep  VD1036
*/
    void SetPLC_SideMove(float fPosX1, float fPosX2, float fPosY1,  float fPosStep);


    /**ȱ����������·� 

*/
    void SetPLC_DetectMove(float fSideX,float fTreadY,   float fWheelAngle);


    //��ȡȱ�ݶ�λ״̬ ȱ�ݶ�λ׼������
    int  GetDetectState();

    /* һ��ԭ��
    */
    void SetAllZero();


    void GetAllFloatValue();
    //����ȱ�ݶ�λ
  
    //̤��ȱ�ݶ�λ
    
private:
    //ѭ����ȡPLC��Ϣ
    void _Read(); //ɨ����
    void  _PLCStatus(); //��ɨ����



public:
   

    Thread m_ThreadRead;
    Thread m_ThreadPLCStatus;
    PLC_SCAN_PAPA m_PlcDownParam;

    PLC_SPeed_PAPA mPLCSpeed;

    //PLC״̬
    bool m_bPLCAuto;//PLC�Զ�״̬��ʶ

    std::map<string, float> mPLCFloatValueMap;
};

