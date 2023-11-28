#pragma once
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
        uint32_t     id          = {}; ///< id
        bool         checkUpdate = {}; ///< ���������°汾
        std::wstring groupName   = {}; ///< ����
        std::wstring userName    = {}; ///< �û�����
        bool         enableProxy = {}; ///< ����
        std::wstring httpProxy   = {}; ///< http����
        static auto  storage(void) {
            return make_storage(ORM_DB_NAME, make_table("SystemConfig", make_column("ID", &SystemConfig::id, primary_key().autoincrement()),
                                                         make_column("CHECK_UPDATE", &SystemConfig::checkUpdate),
                                                         make_column("GROUP_NAME", &SystemConfig::userName),
                                                         make_column("USER_NAME", &SystemConfig::groupName),
                                                         make_column("ENABLE_PROXY", &SystemConfig::enableProxy),
                                                         make_column("HTTP_PROXY", &SystemConfig::httpProxy)));
        }
    };
} // namespace ORM_Model
