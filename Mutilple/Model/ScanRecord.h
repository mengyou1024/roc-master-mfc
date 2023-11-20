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
        uint32_t    id      = {}; ///< id
        int         channel = {}; ///< ȱ��ͨ����
        int         startID = {}; ///< ȱ�ݵ���ʼID
        int         endID   = {}; ///< ȱ�ݵĽ���ID

        static auto storage(std::string dbName) {
            return make_storage(dbName,
                                make_table("ScanRecord", make_column("ID", &ScanRecord::id, primary_key().autoincrement()),
                                           make_column("CHANNEL", &ScanRecord::channel), make_column("START_ID", &ScanRecord::startID),
                                           make_column("END_ID", &ScanRecord::endID)));
        }

        static auto storage(void) {
            return make_storage(ORM_DB_NAME);
        }
    };
} // namespace ORM_Model
