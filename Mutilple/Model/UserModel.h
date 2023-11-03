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
        std::wstring      name      = {}; ///< ÐÕÃû
        uint32_t          jobNumber = {}; ///< ¹¤ºÅ
        std::vector<char> pswd      = {}; ///< ÃÜÂë
        std::wstring      rmaker    = {}; ///< ±¸×¢

        bool isLogin = false; ///< ÊÇ·ñµÇÂ¼

        static auto storage(void) {
            return make_storage("data.db",
                                make_table("User", make_column("ID", &User::id, primary_key().autoincrement()),
                                           make_column("NAME", &User::name), make_column("JOB_NUMBER", &User::jobNumber, unique()),
                                           make_column("PASSWORD", &User::pswd), make_column("RMAKER", &User::rmaker, default_value(""))));
        }
    };
} // namespace ORM_Model
