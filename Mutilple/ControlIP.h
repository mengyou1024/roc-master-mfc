#pragma once
#include <WinSock2.h>
#include <vector>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
class ControlIP
{
public:
	static const int MAX_FRIENDLY_NAME_LENGTH = 256;
	static const int MAX_COMMAND_LENGTH = 1024;
	struct Info {
		char strAdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
		WCHAR wstrFriendlyName[MAX_FRIENDLY_NAME_LENGTH];
		unsigned char ip[4];
	};

	ControlIP();
	~ControlIP();


	const unsigned char * GetIP(int iIndex=0)const;

	bool SetIP(unsigned char* ip, int iIndex=0);

	int Size()const;

public:
	std::vector<Info> m_vecInfo;


};
