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
        User() = default;
        User(std::wstring _name, uint32_t _jobNumber, std::vector<char> _pswd = {}, std::wstring _rmaker = {}) :
        name(_name),
        jobNumber(_jobNumber),
        pswd(_pswd),
        rmaker(_rmaker) {}

        uint32_t          id        = {}; ///< id
        std::wstring      name      = {}; ///< ����
        uint32_t          jobNumber = {}; ///< ����
        std::vector<char> pswd      = {}; ///< ����
        std::wstring      rmaker    = {}; ///< ��ע

        bool isLogin = false; ///< �Ƿ��¼

        static auto storage(string name) {
            return make_storage(name, make_table("User", make_column("ID", &User::id, primary_key().autoincrement()),
                                                 make_column("NAME", &User::name, unique())));
        }

        static auto storage(void) {
            return storage(ORM_DB_NAME);
        }
    };

    class JobGroup {
    public:
        int          id        = {};
        std::wstring groupName = {}; ///< ������
        static auto  storage(string name) {
            return make_storage(name, make_table("JobGroup", make_column("ID", &JobGroup::id, primary_key().autoincrement()),
                                                  make_column("GROUP_NAME", &JobGroup::groupName, unique())));
        }

        static auto storage(void) {
            return storage(ORM_DB_NAME);
        }
    };
} // namespace ORM_Model
