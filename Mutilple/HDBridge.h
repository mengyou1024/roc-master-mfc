#pragma once

#include "ICoder.h"
/*10ͨ������
#include "NetworkMulti.h"
#pragma comment(lib, "NetworkMulti.lib")
*/
//12ͨ������
#include "TOFDPort.h"
#pragma comment(lib, "TOFDPort.lib")
#define HEADER_POS 0									//��ͷλ��
#define HEADER_LEN 4									//��ͷ����
#define IDENTIFIER_POS (HEADER_POS+HEADER_LEN)			//����ʶλ��
#define IDENTIFIER_LEN 1								//����ʶ����
#define SEQUENCE_POS (IDENTIFIER_POS+IDENTIFIER_LEN)	//������λ��
#define SEQUENCE_LEN 2									//����ʶ����
#define LENGTH_POS (SEQUENCE_POS+SEQUENCE_LEN)			//����λ��
#define LENGTH_LEN 3									//��������
#define DATA_POS (LENGTH_POS+LENGTH_LEN)				//���ݶ�λ��
#define WAVE_POS (DATA_POS+1)							//���ݶ�λ��

class Techniques;
class HDBridge
{
public:
	HDBridge(void);
	~HDBridge(void);

	//���豸
	BOOL Open(char *pFPGA_IP, unsigned short iPortFPGA, char *pPC_IP, unsigned short iPortPC);
	BOOL IsOpened();

	//������ӱ�����
	void SetCoder(ICoderAngle* pCoder);

	//���ò���
	BOOL OnConfig(Techniques *pTechniques);
	//����IP
	//BOOL SetIP(NM_IP_CONFIG IP);
	//��ʼ����
	BOOL StartSequencer(BOOL bStart);
	//���ñ�����
	void ResetCoder();
	//��ȡ����
	BOOL ReadAscan();
	//�ر��豸
	void Close();
	//����FPS
	float FPS();

private:
	//���ζ�ȡ�߳�
	void _Read();

public:
	INT m_iNetwork;
	READ_DATA m_ReadData;
	Thread m_ThreadRead;
	CRITICAL_SECTION m_csData;

public:
	Techniques* m_pTechniques;
	ICoderAngle* m_pICoder;

	float m_fFPS;
	float m_fPacketLoss;
};

