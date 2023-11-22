#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <sqlite_orm.h>

#ifndef ORM_DB_NAME
    #define ORM_DB_NAME "DetectInfo.db"
#endif // !ORM_DB_NAME

namespace ORM_Model {
    using namespace sqlite_orm;
    class DetectInfo {
    public:
        uint32_t id = {}; ///< id
        // ������Ϣ
        std::wstring customer               = {}; ///< �ͻ�
        std::wstring customerContractNumber = {}; ///< �ͻ���ͬ���
        std::wstring workOrder              = {}; ///< ������
        std::wstring reportNumber           = {}; ///< ������
        // ����
        std::wstring description       = {}; ///< ����
        std::wstring materialStandards = {}; ///< ���ʱ�׼
        std::wstring surfaceStates     = {}; ///< ����״̬
        std::wstring surveyedArea      = {}; ///< �������
        // ���ļ�����
        std::wstring unitType            = {}; ///< �豸�ͺ�
        std::wstring probeType           = {}; ///< ̽ͷ�ź�
        std::wstring referenceBlock      = {}; ///< �ο��Կ�
        std::wstring waveform            = {}; ///< ����
        std::wstring scanningSensitivity = {}; ///< ɨ��������
        std::wstring couplingAgent       = {}; ///< ��ϼ�
        // ̽ͷ�ͺ�
        std::wstring executiveStandard   = {}; ///< ִ�б�׼
        std::wstring acceptanceStandard  = {}; ///< ���ձ�׼
        std::wstring detectRatio         = {}; ///< ������
        std::wstring detectProcessNumber = {}; ///< ��⹤�ձ��

        static auto storage(std::string name) {
            return make_storage(
                name,
                make_table(
                    "DetectInfo", make_column("ID", &DetectInfo::id, primary_key().autoincrement()),
                    make_column("CUSTOMER", &DetectInfo::customer),
                    make_column("CUSTOMER_CONTRACT_NUMBER", &DetectInfo::customerContractNumber),
                    make_column("WORK_ORDER", &DetectInfo::workOrder), make_column("REPORT_NUMBER", &DetectInfo::reportNumber),
                    make_column("DESCRIPTION", &DetectInfo::description), make_column("MATERIAL_STANDARDS", &DetectInfo::materialStandards),
                    make_column("SURFACE_STATES", &DetectInfo::surfaceStates), make_column("SURVEYED_AREA", &DetectInfo::surveyedArea),
                    make_column("UNIT_TYPE", &DetectInfo::unitType), make_column("PROBE_TYPE", &DetectInfo::probeType),
                    make_column("REFERENCE_BLOCK", &DetectInfo::referenceBlock), make_column("WAVEFORM", &DetectInfo::waveform),
                    make_column("SCANNING_SENSITIVITY", &DetectInfo::scanningSensitivity),
                    make_column("COUPLING_AGENT", &DetectInfo::couplingAgent),
                    make_column("EXECUTIVE_STANDARD", &DetectInfo::executiveStandard),
                    make_column("ACCEPTANCE_STANDARD", &DetectInfo::acceptanceStandard),
                    make_column("DETECT_RATIO", &DetectInfo::detectRatio),
                    make_column("DETECT_PROCESS_NUMBER", &DetectInfo::detectProcessNumber)));
        }

        static auto storage(void) {
            return storage(ORM_DB_NAME);
        }
    };
} // namespace ORM_Model
