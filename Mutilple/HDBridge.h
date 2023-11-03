#pragma once

#include "ICoder.h"
/*10通道高速
#include "NetworkMulti.h"
#pragma comment(lib, "NetworkMulti.lib")
*/
//12通道板子
#include "TOFDPort.h"
#pragma comment(lib, "TOFDPort.lib")
#define HEADER_POS 0									//包头位置
#define HEADER_LEN 4									//包头长度
#define IDENTIFIER_POS (HEADER_POS+HEADER_LEN)			//包标识位置
#define IDENTIFIER_LEN 1								//包标识长度
#define SEQUENCE_POS (IDENTIFIER_POS+IDENTIFIER_LEN)	//包序列位置
#define SEQUENCE_LEN 2									//包标识长度
#define LENGTH_POS (SEQUENCE_POS+SEQUENCE_LEN)			//包长位置
#define LENGTH_LEN 3									//包长长度
#define DATA_POS (LENGTH_POS+LENGTH_LEN)				//数据段位置
#define WAVE_POS (DATA_POS+1)							//数据段位置

class Techniques;
class HDBridge
{
public:
	HDBridge(void);
	~HDBridge(void);

	//打开设备
	BOOL Open(char *pFPGA_IP, unsigned short iPortFPGA, char *pPC_IP, unsigned short iPortPC);
	BOOL IsOpened();

	//设置外接编码器
	void SetCoder(ICoderAngle* pCoder);

	//配置参数
	BOOL OnConfig(Techniques *pTechniques);
	//设置IP
	//BOOL SetIP(NM_IP_CONFIG IP);
	//开始采样
	BOOL StartSequencer(BOOL bStart);
	//重置编码器
	void ResetCoder();
	//读取波形
	BOOL ReadAscan();
	//关闭设备
	void Close();
	//计算FPS
	float FPS();

private:
	//波形读取线程
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

