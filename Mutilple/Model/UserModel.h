#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

using namespace sqlite_orm;

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "User.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    class User {
    public:
        User(std::wstring _name, uint32_t _jobNumber, std::vector<char> _pswd = {}, std::wstring _rmaker = {}) :
        name(_name),
        jobNumber(_jobNumber),
        pswd(_pswd),
        rmaker(_rmaker) {}

        uint32_t          id        = {}; ///< id
        std::wstring      name      = {}; ///< 姓名
        uint32_t          jobNumber = {}; ///< 工号
        std::vector<char> pswd      = {}; ///< 密码
        std::wstring      rmaker    = {}; ///< 备注

        bool isLogin = false; ///< 是否登录

        static auto storage(void) {
            return make_storage(ORM_DB_NAME,
                                make_table("User", make_column("ID", &User::id, primary_key().autoincrement()),
                                           make_column("NAME", &User::name), make_column("JOB_NUMBER", &User::jobNumber, unique()),
                                           make_column("PASSWORD", &User::pswd), make_column("RMAKER", &User::rmaker, default_value(""))));
        }
    };

    class JobGroup {
    public:
        int          id        = {};
        std::wstring groupName = {}; ///< 班组名
        static auto  storage(void) {
            return make_storage(ORM_DB_NAME, make_table("JobGroup", make_column("ID", &JobGroup::id, primary_key().autoincrement()),
                                                         make_column("GROUP_NAME", &JobGroup::groupName)));
        }
    };
} // namespace ORM_Model
