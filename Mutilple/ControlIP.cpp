#include "pch.h"

#include "ControlIP.h"
#include <Windows.h>
#include <algorithm>
#include <codecvt>
#include <string.h>
#include <string>

namespace {
    // 数字字符串转unsigned char类型数字
    unsigned char StrToUchr(char* str) {
        unsigned char tmp = 0;
        while (*str) {
            tmp *= 10;
            tmp += (*str) - '0';
            ++str;
        }
        return tmp;
    }
    // ipv4字符串转4字节数据
    void StrIpToBitIp(unsigned char* bitIp, char* strIp) {
        int  i         = 0;
        int  iCur      = 0;
        char strCur[4] = {0};
        while (*strIp) {
            if (*strIp == '.') {
                strCur[iCur] = '\0';
                bitIp[i]     = StrToUchr(strCur);
                ++i;
                iCur = 0;
                ++strIp;
            }
            strCur[iCur++] = *strIp;
            ++strIp;
        }
        bitIp[i] = StrToUchr(strCur);
    }

    // 获取网卡名、ip地址
    void GetAdapterInfo(std::vector<ControlIP::Info>& vecInfo) {
        // PIP_ADAPTER_INFO结构体指针存储本机网卡信息
        PIP_ADAPTER_INFO adapter_info = new IP_ADAPTER_INFO();

        unsigned long stSize = sizeof(IP_ADAPTER_INFO);
        memset(adapter_info, 0, stSize);
        // 调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
        auto ret = GetAdaptersInfo(adapter_info, &stSize);
        if (ERROR_BUFFER_OVERFLOW == ret) {
            delete adapter_info;
            adapter_info = (PIP_ADAPTER_INFO) new BYTE[stSize];
            ret          = GetAdaptersInfo(adapter_info, &stSize);
        }
        if (ERROR_SUCCESS == ret) { // 可能有多网卡,因此通过循环去判断

            // 遍历获取适配器个数
            for (PIP_ADAPTER_INFO adapter = adapter_info; adapter; adapter = adapter->Next) {
                ControlIP::Info info;
                strcpy(info.strAdapterName, adapter_info->AdapterName);
                StrIpToBitIp(info.ip, adapter_info->IpAddressList.IpAddress.String);

                vecInfo.push_back(std::move(info));
            }
        }
        // 释放内存空间
        if (adapter_info) {
            delete adapter_info;
        }
    }

    // 获取连接名
    void GetAdapterAddresses(std::vector<ControlIP::Info>& vecInfo) {
        PIP_ADAPTER_ADDRESSES addresses  = nullptr;
        ULONG                 outBufLen  = 15000; // Allocate a 15 KB buffer to start with.
        ULONG                 Iterations = 0, ret = 0;
        do {
            addresses = (IP_ADAPTER_ADDRESSES*)new BYTE[outBufLen];
            if (addresses != nullptr) {
                // 0x07，Skip_UNICAST、Skip_ANYCAST、Skip_MULTICAST
                ret = GetAdaptersAddresses(AF_UNSPEC, 0x07, nullptr, addresses, &outBufLen);
                if (ret == NO_ERROR)
                    break;
                if (ret == ERROR_BUFFER_OVERFLOW) {
                    delete addresses;
                    addresses = nullptr;
                }
            }
            Iterations++;

        } while ((ret == ERROR_BUFFER_OVERFLOW) && (Iterations < 3));

        if (ret == NO_ERROR) {
            // 遍历
            for (PIP_ADAPTER_ADDRESSES cur_addresses = addresses; cur_addresses; cur_addresses = cur_addresses->Next) {
                // 通过 AdapterName 匹配两个相同适配器，并获取 FriendlyName
                for (int i = 0; i < vecInfo.size(); ++i) {
                    if (strcmp(vecInfo[i].strAdapterName, cur_addresses->AdapterName) == 0) {
                        wcscpy(vecInfo[i].wstrFriendlyName, cur_addresses->FriendlyName);
                    }
                }
            }
        }

        if (addresses)
            delete addresses;
    }
} // namespace

ControlIP::ControlIP() :
m_vecInfo() {
    GetAdapterInfo(m_vecInfo);
    GetAdapterAddresses(m_vecInfo);
}
ControlIP::~ControlIP() {
}

const unsigned char* ControlIP::GetIP(int iIndex) const {
    return m_vecInfo[iIndex].ip;
}

bool ControlIP::SetIP(unsigned char* ip, int iIndex) {
    Info& info = m_vecInfo[iIndex];
    char  strCommand[MAX_COMMAND_LENGTH];

    // 用于将宽字符转换为窄字符
    std::wstring wstrFriendlyName(info.wstrFriendlyName);
    std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>>
        converter(new std::codecvt<wchar_t, char, std::mbstate_t>("CHS"));

    //  拼接命令
    int a = ip[0], b = ip[1], c = ip[2], d = ip[3];
    sprintf(strCommand, "netsh interface ip set address name=\"%s\" source=static addr=%d.%d.%d.%d  gateway=%d.%d.%d.1",
            converter.to_bytes(wstrFriendlyName).c_str(),
            a, b, c, d,
            a, b, c);

    // printf("%s\n", strCommon);
    if (system(strCommand)) {
        // 执行失败
    } else {
        // 修改成功，将对象中的数据更新。
        memcpy(info.ip, ip, sizeof(info.ip));
    }

    return true;
}

int ControlIP::Size() const {
    return (int)m_vecInfo.size();
}
