#pragma once
#include "Define.h"
#include "sqlite3.h"
#include <vector>

class User {
public:
    User();
    ~User();
    // ����û�
    static bool add(DB_USER_DATA &data);
    // ɾ���û�
    static bool del(long serial);
    // ��ȡ�û��������ϴε�¼���б�
    static bool loadUser(void);
    // �޸ĵ�ǰ�û�������
    static bool modify(DB_USER_DATA &data);
    static DB_USER_DATA findUser(long number);
    static DB_USER_DATA findUser(const wchar_t* name);
    static DB_USER_DATA findUserByNumber(const wchar_t*number);
    static bool         m_isLogin;

public:
    static DB_USER_DATA              mLogionUser; // ��ǰ�û�
    static std::vector<DB_USER_DATA> m_pUserList; // �û��б�
private:
    static int      m_dbRef;
    static sqlite3 *m_db;
};
