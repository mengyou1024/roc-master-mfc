#ifndef _TOFDPORT_H_
#define _TOFDPORT_H_

#ifdef DLL_IMPLEMENT  
#define Dll_API _declspec(dllexport)
#else  
#define Dll_API _declspec(dllimport)  
#endif  

namespace TOFDPort
{
	typedef struct _tagNM_DATA
	{
		int iChannel;					//ͨ����
		int iPackage;					//������
		int iAScanSize;					//Aɨ����
		unsigned char* pAscan;			//Aɨ����
		int pCoder[2];					//������ֵ
		int pCoderZF[2];				//Zֵ��һȦ
		int pCoderZC[2];				//ZֵȦ��
		int pCoderZ[2];					//Zֵ
		int pGatePos[2];				//����λ��
		unsigned char pGateAmp[2];		//���Ų���
		int pAlarm[2];					//���ű���
	}NM_DATA, * P_NM_DATA;

	//���ӳ����忨
	Dll_API bool TOFD_PORT_OpenDevice();
	//�Ͽ������ӵĳ�����
	Dll_API bool TOFD_PORT_CloseDevice();
	//�жϳ�����������Ƿ�����
	Dll_API bool TOFD_PORT_IsOpen();
	//�жϳ�����������Ƿ�����
	Dll_API bool TOFD_PORT_IsDeviceExist();

	//�������ظ�Ƶ��
	Dll_API bool TOFD_PORT_SetFrequency(int iFrequency);
	//���ø�ѹ
	Dll_API bool TOFD_PORT_SetVoltage(int iVoltage);
	//����ͨ����־
	Dll_API bool TOFD_PORT_SetChannelFlag(int iChannelFlag);
	//����ɨ������
	Dll_API bool TOFD_PORT_SetScanIncrement(int iScanIncrement);
	//���ñ����������־λ
	Dll_API bool TOFD_PORT_SetResetCoder(int iResetCoder);
	//����ϵͳָʾ��
	Dll_API bool TOFD_PORT_SetLED(int iLEDStatus);
	//���ù���ͨ���̵���
	Dll_API bool TOFD_PORT_SetDamperFlag(int iDamperFlag);
	//���ñ�������������
	Dll_API bool TOFD_PORT_SetEncoderPulse(int iEncoderPulse);

	//����ͨ��������
	Dll_API bool TOFD_PORT_SetPulseWidth(int iChannel, float fPulseWidth/*ns*/);
	//����ͨ����ʱ
	Dll_API bool TOFD_PORT_SetDelay(int iChannel, float fDelay/*us*/);
	//����ͨ���������
	Dll_API bool TOFD_PORT_SetSampleDepth(int iChannel, float fSampleDepth/*us*/);
	//����ͨ����������
	Dll_API bool TOFD_PORT_SetSampleFactor(int iChannel, int iSampleFactor);
	//����ͨ������
	Dll_API bool TOFD_PORT_SetGain(int iChannel, float fGain/*dB*/);
	//�����˲�Ƶ��
	Dll_API bool TOFD_PORT_SetFilter(int iChannel, int iFilter);
	//���ü첨��ʽ
	Dll_API bool TOFD_PORT_SetDemodu(int iChannel, int iDemodu);
	//����Aɨ�ź���λ
	Dll_API bool TOFD_PORT_SetPhaseReverse(int iChannel, int iPhaseReverse);


	//���ò�����Ϣ
	Dll_API bool TOFD_PORT_SetGateInfo(int iChannel, int iGate, int iActive, int iAlarmType, float fPos/*%*/, float fWidth/*%*/, float fHeight/*%*/);
	//���ò���2����
	Dll_API bool TOFD_PORT_SetGate2Type(int iChannel, int iType);
	//�·�����
	Dll_API bool TOFD_PORT_FlushSetting();

	//��ȡ����
	Dll_API bool TOFD_PORT_ReadDatas(unsigned char* pBuff, unsigned int iSize);
	//��ȡNM_DATA���ݰ�
	Dll_API NM_DATA* TOFD_PORT_ReadDatasFormat();
	//�ͷ�NM_DATA����
	Dll_API void TOFD_PORT_Free_NM_DATA(NM_DATA* pData);
	//��ȡ������ֵ
	Dll_API bool TOFD_PORT_GetCoderValue(int* pCoder0, int* pCoder1);
	//��ȡ������ֵ
	Dll_API bool TOFD_PORT_GetCoderValueZ(int* pCoderZ0, int* pCoderZ1, int* pCoderF0, int* pCoderF1, int* pCoderC0, int* pCoderC1);

	//����������
	Dll_API bool TOFD_PORT_ResetCoder_Immediate();
}


//��չ��

namespace TOFDPortExtensions
{
	//��̬���ʼ��һ��
	Dll_API void ITS_init();
	//usb�Ƿ����
	Dll_API bool ITS_IsExist();
	//usb�Ƿ��Ѵ�
	Dll_API bool ITS_IsOpen();
	//��USB
	Dll_API bool ITS_OpenUSB();
	//�ر�USB
	Dll_API void ITS_CloseUSB();

	//����ͨ��
	Dll_API void ITS_SetCh(unsigned int ch_left_s, unsigned int ch_left_r, unsigned int ch_right_s, unsigned int ch_right_r);
	//����ͨ��Ƶ�� [��ǰȡ��������ֵ�ĺ� hz_left+hz_right Ϊ���ظ�Ƶ��)
	Dll_API void ITS_SetHZ(unsigned int hz_left, unsigned int hz_right);
	//����Ӳ����� [In �������� 10nsһ����]
	Dll_API void ITS_SetHard_Delayns(unsigned int delayns);

	//���������� [In ��λ10ns  ����plus_left = 16 ��ʾ160ns]
	Dll_API void ITS_SetPlusWidth(unsigned int plus_left, unsigned int plus_right);
	//�����ӳ� [In �������� 10nsһ����]
	Dll_API void ITS_SetXmove(unsigned int xmove_left, unsigned int xmove_right);
	//�������� [In ��λ0.1db ����dB_left = 312 ��ʾ31.2dB]
	Dll_API void ITS_SetdB(unsigned int dB_left, unsigned int dB_right);
	//�������� [In �������� 10nsһ����]
	Dll_API void ITS_SetDis(unsigned int dis_left, unsigned int dis_right);


	//��������(���ù���ͨ��)
	Dll_API void ITS_Selfcheck(unsigned int ch_left, unsigned int ch_right);
	//����ˮƽ����λ��
	Dll_API void ITS_SetZeroLeavel(unsigned int zl_left, unsigned int  zl_right);
	//���ó������Խӿ�DA
	Dll_API void ITS_TestSetDB(unsigned int  dB_A1, unsigned int  dB_A2, unsigned int  dB_A3, unsigned int  dB_B1, unsigned int  dB_B2, unsigned int  dB_B3);
	//��ȡ�������Խӿ�DA
	Dll_API void ITS_TestGetDB(unsigned int* dB_A1, unsigned int* dB_A2, unsigned int* dB_A3, unsigned int* dB_B1, unsigned int* dB_B2, unsigned int* dB_B3);

	//����ѹ���� [���β����� �����1024�ı�]
	Dll_API void ITS_SetZip(unsigned int zip_left, unsigned int zip_right);
	//����ѹ����
	Dll_API unsigned int ITS_GetZip(unsigned int ch);


	//���ñ�����
	Dll_API void ITS_ReSetEncoder(unsigned int ch, unsigned int flag);
	//��ȡ��������ֵ
	Dll_API signed int ITS_GetEncoder(unsigned int ch, unsigned int mode);

	//�����ɼ�
	Dll_API bool ITS_Start(unsigned char* buf_left, unsigned char* buf_right);

	///(�忨�������ù���)

	//���ñ�����Ƶ��
	Dll_API void ITS_SetEncoder_frequency(unsigned int ch, unsigned int freq);
	//���ñ�����Ƶ��
	Dll_API unsigned int ITS_GetWorkID();

	///������ ��1.ͨ��Ƶ�� 2.��ȡ������

}

#endif