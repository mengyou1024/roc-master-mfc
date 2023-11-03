#pragma once

#include "RuitiePLC.h"
#include <iostream>

class PLCWnd : public CDuiWindowBase {
public:
    ~PLCWnd();
    virtual LPCTSTR GetWindowClassName() const {
        return _T("PLCWnd");
    }
    virtual CDuiString GetSkinFile() {
        return _T("Theme\\UI_PLCWnd.xml");
    }

    void ResumeSyncThread(void);

    virtual void InitWindow();
    virtual void Notify(TNotifyUI& msg);

private:
    bool   m_SyncUiExit       = false;
    HANDLE m_SyncThreadHandle = 0;
    string m_IpAddr           = "";

    static constexpr auto g_SaveParam = {
        "V1004",
        "V1016",
        "V1020",
        "V1024",
        "V1036",
        "V1040",
        "V1044",
        "V1056",
        "V1060",
        "V1064",
        "V1076",
        "V1080",
        "V1084",
        "V1088",
        "V1092",
        "V1096",
        "V1100",
        "V1104",
    };

    map<string, bool> m_SelfLockKeyMap = {
        {"M00", 0}, ///< 急停
        {"M01", 1}, ///< 手自开关
        {"M06", 0}, ///< 一键回原点
        {"M10", 0}, ///< 自动开始
        {"M20", 0}, ///< 卡盘张开
        {"M21", 0}, ///< 卡盘合拢
        {"M22", 0}, ///< 卡盘正转
        {"M23", 0}, ///< 卡盘反转
        {"M24", 0}, ///< 卡盘停止
        {"M25", 0}, ///< 卡盘变频器复位
        {"M26", 0}, ///< 上料盘升
        {"M27", 0}, ///< 上料盘降
        {"M30", 0}, ///< 横梁前进
        {"M31", 0}, ///< 横梁后退
        {"M32", 0}, ///< 横梁停止
        {"M33", 0}, ///< 左轴👈
        {"M34", 0}, ///< 左轴👉
        {"M42", 0}, ///< 左轴👆
        {"M43", 0}, ///< 左轴👇
        {"M47", 0}, ///< 右轴👈
        {"M50", 0}, ///< 右轴👉
        {"M55", 0}, ///< 右轴👆
        {"M56", 0}, ///< 右轴👇
        {"Q05", 1}, ///< 水箱电磁阀
    };

    static DWORD SyncUIThread(void* param);
};
