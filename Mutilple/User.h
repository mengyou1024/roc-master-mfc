#pragma once
#include "Define.h"
#include "sqlite3.h"
#include <vector>

class User {
public:
    User();
    ~User();
    // 添加用户
    static bool add(DB_USER_DATA &data);
    // 删除用户
    static bool del(long serial);
    // 读取用户（包括上次登录和列表）
    static bool loadUser(void);
    // 修改当前用户的数据
    static bool modify(DB_USER_DATA &data);
    static DB_USER_DATA findUser(long number);
    static DB_USER_DATA findUser(const wchar_t* name);
    static DB_USER_DATA findUserByNumber(const wchar_t*number);
    static bool         m_isLogin;

public:
    static DB_USER_DATA              mLogionUser; // 当前用户
    static std::vector<DB_USER_DATA> m_pUserList; // 用户列表
private:
    static int      m_dbRef;
    static sqlite3 *m_db;
};
