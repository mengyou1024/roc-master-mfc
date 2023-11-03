#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

using namespace sqlite_orm;

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "SystemConfig.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    class SystemConfig {
    public:
        uint32_t id = {}; ///< id

        static auto storage(void) {
            return make_storage(ORM_DB_NAME,
                                make_table("SystemConfig", make_column("ID", &SystemConfig::id, primary_key().autoincrement())));
        }
    };
} // namespace ORM_Model
