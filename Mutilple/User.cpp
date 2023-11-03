#include "pch.h"

#include "User.h"
#include "User.h"

bool                      User::m_isLogin   = false;
sqlite3                  *User::m_db        = NULL;
int                       User::m_dbRef     = 0;
DB_USER_DATA              User::mLogionUser = {}; // 当前用户
std::vector<DB_USER_DATA> User::m_pUserList = {}; // 用户列表

User::User() {
    if (m_db != nullptr) {
        m_dbRef++;
        return;
    }
    int ret = sqlite3_open("User.db", &m_db);
    if (ret != SQLITE_OK) {
        printf("User.db open failed.");
        return;
    }
    const char *sql    = "CREATE TABLE IF NOT EXISTS USER("
                         "ID       INTEGER     PRIMARY KEY AUTOINCREMENT,"
                         "NAME     TEXT    NOT NULL,"
                         "NUMBER   TEXT    NOT NULL,"
                         "PWD      TEXT    NOT NULL,"
                         "LEVEL    INTEGER     NOT NULL,"
                         "REMARK   TEXT);";
    char       *errMsg = nullptr;
    int         err    = sqlite3_exec(m_db, sql, NULL, 0, &errMsg);
    if (err != SQLITE_OK) {
        printf("error: %s \n", errMsg);
        sqlite3_free(errMsg);
        return;
    }
    sql    = "CREATE TABLE IF NOT EXISTS LAST_LOGIN("
             "NUMBER   TEXT    NOT NULL,"
             "TIME   TIMESTAMP NOT NULL DEFAULT (datetime('now', 'localtime')));";
    errMsg = nullptr;
    err    = sqlite3_exec(m_db, sql, NULL, 0, &errMsg);
    if (err != SQLITE_OK) {
        printf("error: %s \n", errMsg);
        sqlite3_free(errMsg);
        return;
    }
    m_dbRef++;
}

User::~User() {
    if (m_db != nullptr) {
        if (--m_dbRef == 0) {
            char  sql[256] = {0};
            char *errStr;
            sprintf_s(sql, sizeof(sql), "DELETE FROM LAST_LOGIN;INSERT INTO LAST_LOGIN(NUMBER) VALUES(%d);", mLogionUser.lSerial);
            if (sqlite3_exec(m_db, sql, NULL, NULL, &errStr) != SQLITE_OK) {
                printf("error: %s\n", errStr);
                sqlite3_free(errStr);
            }
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }
}

bool User::add(DB_USER_DATA &data) {
    char  sql[256] = {};
    bool  isExsist = false;
    char *ErrStr   = NULL;
    sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE NUMBER = '%s'", StringFromLPCTSTR(data.strNumber).c_str());
    if (sqlite3_exec(
            m_db, sql,
            [](void *param, int argc, char **, char **) -> int {
                if (argc != 0) {
                    if (*(bool *)param != true) {
                        *(bool *)param = true;
                    }
                }
                return 0;
            },
            &isExsist, &ErrStr) != SQLITE_OK) {
        printf("error: %s\n", ErrStr);
        sqlite3_free(ErrStr);
    }
    sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE NAME = '%s'", StringFromLPCTSTR(data.strName).c_str());
    if (sqlite3_exec(
            m_db, sql,
            [](void *param, int argc, char **, char **) -> int {
                if (argc != 0) {
                    if (*(bool *)param != true) {
                        *(bool *)param = true;
                    }
                }
                return 0;
            },
            &isExsist, &ErrStr) != SQLITE_OK) {
        printf("error: %s\n", ErrStr);
        sqlite3_free(ErrStr);
    }
    if (!isExsist) {
        string name   = StringFromLPCTSTR(data.strName);
        string number = StringFromLPCTSTR(data.strNumber);
        string pwd    = StringFromLPCTSTR(data.strPwd);
        string remark = StringFromLPCTSTR(data.strRemark);
        sprintf_s(sql, sizeof(sql), "INSERT INTO USER(NAME, NUMBER, PWD, LEVEL, REMARK) VALUES('%s', '%s', '%s', %d, '%s')", name.c_str(),
                  number.c_str(), pwd.c_str(), data.lLevel, remark.c_str());
        if (sqlite3_exec(m_db, sql, NULL, &isExsist, &ErrStr) == SQLITE_OK) {
            return true;
        } else {
            printf("error: %s\n", ErrStr);
            sqlite3_free(ErrStr);
        }
    }
    return false;
}

bool User::del(long serial) {
    char sql[128] = {};
    sprintf_s(sql, sizeof(sql), "DELETE FROM USER WHERE ID = %d", serial);
    if (sqlite3_exec(m_db, sql, NULL, NULL, NULL) == SQLITE_OK) {
        return true;
    }
    return false;
}

bool User::loadUser(void) {
    if (m_pUserList.size() != 0) {
        m_pUserList.clear();
    }
    sqlite3_exec(
        m_db, "SELECT * FROM USER;",
        [](void *param, int argc, char **argv, char **colName) -> int {
            auto         list = (std::vector<DB_USER_DATA> *)param;
            DB_USER_DATA user = {};
            user.lSerial      = atol(argv[0]);
            char temp[128]    = {};
            Utf8ToGB2312(argv[1], temp);
            wnsprintf(user.strName, sizeof(user.strName) / 2, _T("%s"), CString(temp).GetString());

            Utf8ToGB2312(argv[2], temp);
            wnsprintf(user.strNumber, sizeof(user.strNumber) / 2, _T("%s"), CString(temp).GetString());

            wnsprintf(user.strPwd, sizeof(user.strPwd) / 2, _T("%s"), CString(argv[3]).GetString());
            user.lLevel = atol(argv[4]);
            Utf8ToGB2312(argv[5], temp);
            wnsprintf(user.strRemark, sizeof(user.strRemark) / 2, _T("%s"), CString(temp).GetString());
            list->push_back(user);
            return 0;
        },
        &m_pUserList, NULL);
    static bool isLoadLast = false;
    if (!isLoadLast) {
        mLogionUser.lSerial = -1;
        sqlite3_exec(
            m_db, "SELECT * FROM LAST_LOGIN;",
            [](void *param, int argc, char **argv, char **colName) -> int {
                auto user     = (DB_USER_DATA *)param;
                user->lSerial = atol(argv[0]);
                return 0;
            },
            &mLogionUser, NULL);
        if (mLogionUser.lSerial != -1) {
            char sql[128] = {};
            sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE ID = %d;", mLogionUser.lSerial);
            if (sqlite3_exec(
                    m_db, sql,
                    [](void *param, int argc, char **argv, char **colName) -> int {
                        auto user      = (DB_USER_DATA *)param;
                        user->lSerial  = atol(argv[0]);
                        char temp[128] = {};
                        Utf8ToGB2312(argv[1], temp);
                        wnsprintf(user->strName, sizeof(user->strName) / 2, _T("%s"), CString(temp).GetString());

                        Utf8ToGB2312(argv[2], temp);
                        wnsprintf(user->strNumber, sizeof(user->strNumber) / 2, _T("%s"), CString(temp).GetString());

                        wnsprintf(user->strPwd, sizeof(user->strPwd) / 2, _T("%s"), CString(argv[3]).GetString());
                        user->lLevel = atol(argv[4]);
                        Utf8ToGB2312(argv[5], temp);
                        wnsprintf(user->strRemark, sizeof(user->strRemark) / 2, _T("%s"), CString(temp).GetString());
                        return 0;
                    },
                    &mLogionUser, NULL) != SQLITE_OK) {
                return false;
            }
        }
        isLoadLast = true;
    }
    return true;
}

DB_USER_DATA User::findUser(long number) {
    DB_USER_DATA user     = {};
    char         sql[256] = {};
    sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE NUMBER = %d;", number);
    sqlite3_exec(
        m_db, sql,
        [](void *param, int argc, char **argv, char **colName) -> int {
            auto user      = (DB_USER_DATA *)param;
            user->lSerial  = atol(argv[0]);
            char temp[128] = {};
            Utf8ToGB2312(argv[1], temp);
            wnsprintf(user->strName, sizeof(user->strName) / 2, _T("%s"), CString(temp).GetString());

            Utf8ToGB2312(argv[2], temp);
            wnsprintf(user->strNumber, sizeof(user->strNumber) / 2, _T("%s"), CString(temp).GetString());

            wnsprintf(user->strPwd, sizeof(user->strPwd) / 2, _T("%s"), CString(argv[3]).GetString());
            user->lLevel = atol(argv[4]);
            Utf8ToGB2312(argv[5], temp);
            wnsprintf(user->strRemark, sizeof(user->strRemark) / 2, _T("%s"), CString(temp).GetString());
            return 0;
        },
        &user, NULL);
    return user;
}

DB_USER_DATA User::findUser(const wchar_t *name) {
    DB_USER_DATA user     = {};
    char         sql[256] = {};
    sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE NAME = '%s';", StringFromLPCTSTR(name).c_str());
    sqlite3_exec(
        m_db, sql,
        [](void *param, int argc, char **argv, char **colName) -> int {
            auto user      = (DB_USER_DATA *)param;
            user->lSerial  = atol(argv[0]);
            char temp[128] = {};
            Utf8ToGB2312(argv[1], temp);
            wnsprintf(user->strName, sizeof(user->strName) / 2, _T("%s"), CString(temp).GetString());

            Utf8ToGB2312(argv[2], temp);
            wnsprintf(user->strNumber, sizeof(user->strNumber) / 2, _T("%s"), CString(temp).GetString());

            wnsprintf(user->strPwd, sizeof(user->strPwd) / 2, _T("%s"), CString(argv[3]).GetString());
            user->lLevel = atol(argv[4]);
            Utf8ToGB2312(argv[5], temp);
            wnsprintf(user->strRemark, sizeof(user->strRemark) / 2, _T("%s"), CString(temp).GetString());
            return 0;
        },
        &user, NULL);
    return user;
}

DB_USER_DATA User::findUserByNumber(const wchar_t *number) {
    DB_USER_DATA user     = {};
    char         sql[256] = {};
    sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE NUMBER = '%s';", StringFromLPCTSTR(number).c_str());
    sqlite3_exec(
        m_db, sql,
        [](void *param, int argc, char **argv, char **colName) -> int {
            auto user      = (DB_USER_DATA *)param;
            user->lSerial  = atol(argv[0]);
            char temp[128] = {};
            Utf8ToGB2312(argv[1], temp);
            wnsprintf(user->strName, sizeof(user->strName) / 2, _T("%s"), CString(temp).GetString());

            Utf8ToGB2312(argv[2], temp);
            wnsprintf(user->strNumber, sizeof(user->strNumber) / 2, _T("%s"), CString(temp).GetString());

            wnsprintf(user->strPwd, sizeof(user->strPwd) / 2, _T("%s"), CString(argv[3]).GetString());
            user->lLevel = atol(argv[4]);
            Utf8ToGB2312(argv[5], temp);
            wnsprintf(user->strRemark, sizeof(user->strRemark) / 2, _T("%s"), CString(temp).GetString());
            return 0;
        },
        &user, NULL);
    return user;
}

bool User::modify(DB_USER_DATA &data) {
    char sql[256] = {};
    bool isExsist = false;
    sprintf_s(sql, sizeof(sql), "SELECT * FROM USER WHERE ID = %d;", data.lSerial);
    sqlite3_exec(
        m_db, sql,
        [](void *param, int argc, char **, char **) -> int {
            if (argc != 0) {
                *(bool *)param = true;
            }
            return 0;
        },
        &isExsist, NULL);
    if (isExsist) {
        string name   = StringFromLPCTSTR(data.strName);
        string number = StringFromLPCTSTR(data.strNumber);
        string pwd    = StringFromLPCTSTR(data.strPwd);
        string remark = StringFromLPCTSTR(data.strRemark);
        sprintf_s(sql, sizeof(sql), "UPDATE USER SET NAME = '%s', NUMBER = '%s', PWD = '%s',LEVEL =  %d, REMARK = '%s' WHERE ID = %d;",
                  name.c_str(), number.c_str(), pwd.c_str(), data.lLevel, remark.c_str(), data.lSerial);
        char *errStr;
        if (sqlite3_exec(m_db, sql, NULL, NULL, &errStr) == SQLITE_OK) {
            return true;
        } else {
            printf("error: %s\n", errStr);
            sqlite3_free(errStr);
        }
    }
    return false;
}
