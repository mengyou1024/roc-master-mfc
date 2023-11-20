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
        uint32_t id             = {}; ///< id
        bool     saveUniqueFile = 1;  ///< 扫查记录按时间和日期保存为单个文件
        bool     saveMergeFile  = 0;  ///< 扫查记录都保存为一个文件
        bool     checkUpdate    = 1;  ///< 软件检查最新版本

        static auto storage(void) {
            return make_storage(ORM_DB_NAME, make_table("SystemConfig", make_column("ID", &SystemConfig::id, primary_key().autoincrement()),
                                                        make_column("SAVE_UNIQUE", &SystemConfig::saveUniqueFile),
                                                        make_column("SAVE_MERGE", &SystemConfig::saveMergeFile),
                                                        make_column("CHECK_UPDATE", &SystemConfig::checkUpdate)));
        }
    };
} // namespace ORM_Model
