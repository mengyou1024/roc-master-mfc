#pragma once

#ifdef DLL_IMPLEMENT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef struct _tagNM_DATA
{
	__int32 iAxle;						//���
	__int32 iChannel;					//ͨ����
	__int32 iPackage;					//������
	__int32 iAScanSize;					//Aɨ����
	unsigned char *pAscan;				//Aɨ����
	__int32 pCoder[2];					//������ֵ
	__int32 pGatePos[2];				//����λ��
	unsigned char pGateAmp[2];			//���Ų���
	__int32 pAlarm[2];					//���ű���
}NM_DATA, *P_NM_DATA;

typedef struct _tagNM_IP_CONFIG
{
	unsigned char pIpFPGA[4];		//������IP,pIpFPGA[0]=192,pIpFPGA[1]=168,pIpFPGA[2]=1,pIpFPGA[3]=110
	unsigned int  iPortFPGA;		//������˿�,3500
	unsigned char pIpPC[4];			//PC IP,pIpPC[0]=192,pIpPC[1]=168,pIpPC[2]=1,pIpPC[3]=100
	unsigned int  iPortPC;			//PC �˿�,4200
	unsigned char pGateway[4];		//����,pGateway[0]=192,pGateway[1]=168,pGateway[2]=1,pGateway[3]=1
	unsigned char pSubnet[4];		//��������,pSubnet[0]=255,pSubnet[1]=255,pSubnet[2]=255,pSubnet[3]=0
	unsigned char pMAC[6];  		//������MAC��ַ,pMAC[0]=0,pMAC[1]=10,pMAC[2]=53,pMAC[3]=1,pMAC[4]=254,pMAC[5]=192
}NM_IP_CONFIG, *P_NM_IP_CONFIG;

/************************************************************************/
/*���Ӱ忨�����ð忨FPGA��PC����IP��ַ                                  */
/*���ذ忨��ţ�ʧ�ܷ���-1			                                    */
/************************************************************************/
extern "C" DLL_API int UNION_PORT_Open(char *pFPGA_IP = "192.168.1.110", unsigned short iPortFPGA = 3500, char *pPC_IP = "192.168.1.100", unsigned short iPortPC = 4200);

/************************************************************************/
/*�忨�Ƿ�����                                                          */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_IsOpen(int iIndex);

/************************************************************************/
/* �Ͽ��忨                                                             */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API void UNION_PORT_Close(int iIndex);

/************************************************************************/
/* �汾����Ϣ															*/
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/* 6Bytes,ǰ2Bytes��ʶ�汾�ţ���4Bytes��ʾ�޸�����                      */
/* ��: {0x01, 0x03, 0x14, 0x15, 0x0A, 0x19}                             */
/* Version:1.3                                                          */
/* �޸�����:2021-10-25(0x14*100+0x15 = 2021, 0x0A=10, 0x19=25)          */
/************************************************************************/
extern "C" DLL_API unsigned char * UNION_PORT_GetVersion(int iIndex);

/************************************************************************/
/* ���ó����忨ͨ��IP, ��������Ч                                       */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetIP(int iIndex, NM_IP_CONFIG IP);

/************************************************************************/
/* ��ȡ�����忨IP��ַ                                                   */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API P_NM_IP_CONFIG UNION_PORT_GetIP(int iIndex);

/************************************************************************/
/* ��ȡͨ��Aɨ����                                                      */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API unsigned int UNION_PORT_GetAScanSize(int iIndex, int iChannel);

/************************************************************************/
/*�����ظ�Ƶ��[50Hz, 5000Hz]                                            */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetFrequency(int iIndex, int iFrequency);

/************************************************************************/
/*���ø�ѹ.0:��ѹ��1:��ѹ��2:��ѹ                                       */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetVoltage(int iIndex, int iVoltage);

/************************************************************************/
/*����������.1:��(200),0:խ(100)                                      */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetPulseWidth(int iIndex, int iPulseWidth);

/************************************************************************/
/*����ͨ�������־.                                                     */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iTxFlag:bit0��bit9�ֱ����ͨ��0��ͨ��9��1��ʾѡ�У�0��ʾ����          */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetTxFlag(int iIndex, int iTxFlag);

/************************************************************************/
/*����ͨ�����ձ�־.                                                     */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iRxFlag:bit0��bit9�ֱ����ͨ��0��ͨ��9��1��ʾѡ�У�0��ʾ����          */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetRxFlag(int iIndex, int iRxFlag);

/************************************************************************/
/*����ͨ������ģʽ.                                                     */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iChMode:bit0��bit9�ֱ����ͨ��0��ͨ��9��1��ʾ˫��ģʽ��0��ʾ��ͨģʽ  */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetChMode(int iIndex, int iChMode);

/************************************************************************/
/* ɨ��������Ԥ��                                                       */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetScanIncrement(int iIndex, int iScanIncrement);

/************************************************************************/
/* ���������㣬0��������1������                                         */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_ResetCoder(int iIndex, int iResetCoder);

/************************************************************************/
/* ϵͳ����ģʽ:                                                        */ 
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/* 0:�ֶ�����ģʽ��ͨ��StartSequencer��StopSequencer���Ƴ����幤��״̬  */
/* 1:�Զ�����ģʽ���ɿ���ϵͳ�����Ĺ��������źźͼ����źſ���           */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetWorkType(int iIndex, int iWorkType);

/************************************************************************/
/* �����ź�����ʱ��:                                                    */ 
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/* iTimes: 0 - 255, ms													*/
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetControlTime(int iIndex, int iTimes);

/************************************************************************/
/* �����ź�����ʱ��:                                                    */ 
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/* iTimes: 0 - 255, ms													*/
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetAxleTime(int iIndex, int iTimes);

/************************************************************************/
/* ����ͨ����ʱ��[-10.24us, 1700us]                                     */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetDelay(int iIndex, int iChannel, float fDelay/*us*/);

/************************************************************************/
/* ����ͨ��������ȣ�[0us, 1700us]                                      */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetSampleDepth(int iIndex, int iChannel, float fSampleDepth/*us*/);

/************************************************************************/
/* ����ͨ���������ӣ�[1, 255]                                           */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetSampleFactor(int iIndex, int iChannel, int iSampleFactor);

/************************************************************************/
/* ����ͨ�����棬[0, 110dB]                                             */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetGain(int iIndex, int iChannel, float fGain/*dB*/);

/************************************************************************/
/* �����˲�Ƶ����0x00:���˲�, 0x01��Ƶ��2.5MHz, 0x02��Ƶ��5.0MHz        */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetFilter(int iIndex, int iChannel, int iFilter);

/************************************************************************/
/* ���ü첨��ʽ��0x00:��Ƶ, 0x01��ȫ��, 0x02�����벨��0x03:���벨       */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetDemodu(int iIndex, int iChannel, int iDemodu );

/************************************************************************/
/* ��λ��ת��0x00:����ת, 0x01����ת                                    */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iChannel:ͨ�����			                                            */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetPhaseReverse(int iIndex, int iChannel, int iPhaseReverse);

/************************************************************************/
/* ���ò�����Ϣ                                                         */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iChannel:ͨ�����			                                            */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetGateInfo(int iIndex, int iChannel, int iGate, int iActive, int iAlarmType, float fPos/*%*/, float fWidth/*%*/, float fHeight/*%*/);

/************************************************************************/
/* ���ò���2����, 0���̶���1:�����Բ���1��߻ز�Ϊ0��                   */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iChannel:ͨ�����			                                            */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_SetGate2Type(int iIndex, int iChannel,  int iType);

/************************************************************************/
/* �����·�����Ч                                                       */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_FlushSetting(int iIndex);

/************************************************************************/
/*��ȡ���ý��                                                          */
/*return:1:�ɹ���2:ʧ�ܣ�0:δ��ȡ�����                                 */
/************************************************************************/
extern "C" DLL_API int UNION_PORT_GetFlushResult(int iIndex);
extern "C" DLL_API bool UNION_PORT_ClearFlushResult(int iIndex);

/************************************************************************/
/* �ֶ�����ģʽ�¿�ʼ����                                               */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_StartSequencer(int iIndex);

/************************************************************************/
/* �ֶ�����ģʽ��ֹͣ����                                               */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API bool UNION_PORT_StopSequencer(int iIndex);

/************************************************************************/
/* ��ȡ������                                                           */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/*iChannel:ͨ�����			                                            */
/************************************************************************/
extern "C" DLL_API float UNION_PORT_GetPacketLoss(int iIndex, int iChannel);

/************************************************************************/
/* ��ȡ��������                                                         */
/*iIndex:�忨��ţ���UNION_PORT_Open���                                */
/************************************************************************/
extern "C" DLL_API NM_DATA *UNION_PORT_ReadDatas(int iIndex);
