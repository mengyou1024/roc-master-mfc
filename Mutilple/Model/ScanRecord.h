#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

using namespace sqlite_orm;

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "ScanRecord.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    class ScanRecord {
    public:
        uint32_t    id   = {}; ///< id
        std::string time = {};

        static auto storage(void) {
            return make_storage(ORM_DB_NAME, make_table("ScanRecord", make_column("ID", &ScanRecord::id, primary_key().autoincrement()),
                                                        make_column("TIME", &ScanRecord::time, unique())));
        }
    };
} // namespace ORM_Model
