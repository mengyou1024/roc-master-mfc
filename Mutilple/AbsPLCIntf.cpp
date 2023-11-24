#include "pch.h"

#include "AbsPLCIntf.h"
#include "snap7.h"
#include <cstdio>
#include <memory>
#include <mutex>

using std::lock_guard;
using std::make_unique;
using std::mutex;
using std::unique_ptr;

class _RuitiePLCInfo {
public:
    bool                   m_connected      = false;
    TS7Client*             m_client         = nullptr;
    std::string            m_addr           = {};
    int                    m_rack           = 0;
    int                    m_slot           = 0;
    int                    m_reconnectCount = 0;
    bool                   m_reconnectErr   = false;
    mutex                  m_lock           = {};
    static _RuitiePLCInfo* getInstance() {
        static _RuitiePLCInfo inst;
        return &inst;
    }
};

static bool reconnect() {
    string ip   = "";
    int    slot = 0;
    int    rack = 0;
    AbsPLCIntf::getConnectedInfo(&ip, &rack, &slot);
    if (ip.empty()) {
        return false;
    } else {
        if (_RuitiePLCInfo::getInstance()->m_client == nullptr) {
            _RuitiePLCInfo::getInstance()->m_client = new TS7Client;
        }
        int err = _RuitiePLCInfo::getInstance()->m_client->ConnectTo(ip.c_str(), rack, slot);
        if (err == 0) {
            _RuitiePLCInfo::getInstance()->m_addr      = string(ip.c_str());
            _RuitiePLCInfo::getInstance()->m_rack      = rack;
            _RuitiePLCInfo::getInstance()->m_slot      = slot;
            _RuitiePLCInfo::getInstance()->m_connected = true;
        } else {
            spdlog::warn("reconnect to :{} {} {} error: {%d}}\n", ip, rack, slot, err);
            return false;
        }
    }
    spdlog::warn("reconnect to :{} {} {} success\n", ip, rack, slot);
    return true;
}

static bool doWithReconnect(function<bool(void)> func) {
    lock_guard<mutex> lockGruard(_RuitiePLCInfo::getInstance()->m_lock);
    if (_RuitiePLCInfo::getInstance()->m_reconnectErr) {
        return false;
    }
    while (1) {
        if (!func()) {
            if (_RuitiePLCInfo::getInstance()->m_reconnectCount >= 2) {
                return false;
            }
        } else {
            _RuitiePLCInfo::getInstance()->m_reconnectCount = 0;
            break;
        }
        if (!reconnect()) {
            _RuitiePLCInfo::getInstance()->m_connected    = false;
            _RuitiePLCInfo::getInstance()->m_reconnectErr = true;
            spdlog::warn("reconnect error.");
            return false;
        }
        ++_RuitiePLCInfo::getInstance()->m_reconnectCount;
    }

    return true;
}

static uint16_t AbsPLCIntf::swaps(uint16_t sval) {
    return ((sval & 0xFF) << 8) | ((sval & 0xFF00) >> 8);
}

static uint32_t AbsPLCIntf::swapl(uint32_t lval) {
    return ((lval & 0xFF000000) >> 24) | ((lval & 0xFF0000) >> 8) | ((lval & 0xFF00) << 8) | ((lval & 0xFF) << 24);
}

static float AbsPLCIntf::swapf(float fval) {
    uint32_t temp = *(uint32_t*)&fval;
    temp          = swapl(temp);
    float ret     = *(float*)&temp;
    return ret;
}

bool AbsPLCIntf::isConnected() {
    return _RuitiePLCInfo::getInstance()->m_connected;
}

bool AbsPLCIntf::connectTo(const char* addr, int rack, int slot) {
    if (isConnected()) {
        spdlog::info("already connect to :{} {} {}", _RuitiePLCInfo::getInstance()->m_addr.c_str(), _RuitiePLCInfo::getInstance()->m_rack,
                     _RuitiePLCInfo::getInstance()->m_slot);
    } else {
        if (_RuitiePLCInfo::getInstance()->m_client == nullptr) {
            _RuitiePLCInfo::getInstance()->m_client = new TS7Client;
        }
        _RuitiePLCInfo::getInstance()->m_client->SetConnectionType(3);
        int err = _RuitiePLCInfo::getInstance()->m_client->ConnectTo(addr, rack, slot);
        if (err == 0) {
            _RuitiePLCInfo::getInstance()->m_addr           = string(addr);
            _RuitiePLCInfo::getInstance()->m_rack           = rack;
            _RuitiePLCInfo::getInstance()->m_slot           = slot;
            _RuitiePLCInfo::getInstance()->m_connected      = true;
            _RuitiePLCInfo::getInstance()->m_reconnectCount = 0;
            _RuitiePLCInfo::getInstance()->m_reconnectErr   = false;
        } else {
            spdlog::warn("connect to :{} {} {} error: {}", addr, rack, slot, err);
            return false;
        }
    }
    return true;
}

void AbsPLCIntf::disconnect(void) {
    if (_RuitiePLCInfo::getInstance()->m_client) {
        if (isConnected()) {
            _RuitiePLCInfo::getInstance()->m_client->Disconnect();
        }
        delete _RuitiePLCInfo::getInstance()->m_client;
        _RuitiePLCInfo::getInstance()->m_client    = nullptr;
        _RuitiePLCInfo::getInstance()->m_connected = false;
    }
}

void AbsPLCIntf::getConnectedInfo(std::string* addr, int* rack, int* slot) {
    if (addr) {
        *addr = _RuitiePLCInfo::getInstance()->m_addr;
    }
    if (rack) {
        *rack = _RuitiePLCInfo::getInstance()->m_rack;
    }
    if (slot) {
        *slot = _RuitiePLCInfo::getInstance()->m_slot;
    }
}

static unique_ptr<uint8_t[]> bitExpand(uint8_t* bytes, int bits) {
    auto ret = unique_ptr<uint8_t[]>(new uint8_t[bits]);
    for (int i = 0; i < bits; i++) {
        ret[i] = bytes[i / 8] & (1 << (i % 8)) ? 1 : 0;
    }
    return ret;
}

static bool readRegion(int Area, int start, int size, const char* format, function<void(string, uint8_t)> func) {
    if (!AbsPLCIntf::isConnected()) {
        return false;
    }

    auto data = shared_ptr<uint8_t[]>(new uint8_t[(size + 7) / 8]);

    auto ret = doWithReconnect([Area, start, size, &data]() -> bool {
        return _RuitiePLCInfo::getInstance()->m_client->ReadArea(Area, 0, start, (size + 7) / 8, S7WLByte, data.get()) == 0;
    });

    if (!ret) {
        _RuitiePLCInfo::getInstance()->m_connected = false;
        return false;
    }

    auto expand = bitExpand(data.get(), size);

    for (int i = start * 8; i < (start * 8 + size); i++) {
        char temp[10];
        snprintf(temp, sizeof(temp), format, i / 8, i % 8);
        func(temp, expand[i - static_cast<size_t>(start) * 8]);
    }
    return true;
}

int AbsPLCIntf::getVariable(string name, int) {
    if (!isConnected()) {
        return -1;
    }
    int  iStart = 0, iBit = 0, iArea = 0;
    char cArea = 0, cStart = 0, cBit = 0;
    if (sscanf(name.c_str(), "%c%c%c", &cArea, &cStart, &cBit) < 3) {
        return -1;
    }
    iStart = cStart - '0';
    iBit   = cBit - '0';

    switch (cArea) {
        case 'M': iArea = S7AreaMK; break;
        case 'I': iArea = S7AreaPE; break;
        case 'Q': iArea = S7AreaPA; break;
        default: return -1;
    }

    uint8_t temp = 0;
    auto    ret  = doWithReconnect([iArea, iStart, &temp]() -> bool {
        return _RuitiePLCInfo::getInstance()->m_client->ReadArea(iArea, 0, iStart, 1, S7WLByte, &temp) == 0;
    });
    if (!ret) {
        _RuitiePLCInfo::getInstance()->m_connected = false;
        return -1;
    }
    return (temp & (1 << iBit)) ? 1 : 0;
}

float AbsPLCIntf::getVariable(string name, float) {
    if (!isConnected()) {
        return -INFINITY;
    }
    char cArea = 0;
    int  iAddr = 0;
    if (sscanf(name.c_str(), "%c%d", &cArea, &iAddr) != 2) {
        return -INFINITY;
    }
    if (cArea != 'V') {
        return -INFINITY;
    }
    float ret = -INFINITY;
    if (!doWithReconnect([iAddr, &ret]() -> bool {
            return _RuitiePLCInfo::getInstance()->m_client->ReadArea(S7AreaDB, 1, iAddr, 1, S7WLReal, &ret) == 0;
        })) {
        return -INFINITY;
    }
#if PLC_SWAP_FLOAT
    ret = swapf(ret);
#endif
    return ret;
}

bool AbsPLCIntf::setVariable(string s, float var) {
    if (!isConnected()) {
        return false;
    }

    int  iAddr = 0, iArea = 0;
    char cArea = 0;
    if (sscanf(s.c_str(), "%c%d", &cArea, &iAddr) < 2) {
        return false;
    }

    if (cArea != 'V') {
        return false;
    }

#if PLC_SWAP_FLOAT
    var = swapf(var);
#endif
    auto ret = doWithReconnect(
        [iAddr, &var]() -> bool { return _RuitiePLCInfo::getInstance()->m_client->WriteArea(S7AreaDB, 1, iAddr, 1, S7WLReal, &var) == 0; });
    if (!ret) {
        _RuitiePLCInfo::getInstance()->m_connected = false;
        return false;
    }
    return true;
}

bool AbsPLCIntf::setVariable(string s, float* var, int count) {
    if (!isConnected()) {
        return false;
    }

    int  iAddr = 0, iArea = 0;
    char cArea = 0;
    if (sscanf(s.c_str(), "%c%d", &cArea, &iAddr) < 2) {
        return false;
    }

#if PLC_SWAP_FLOAT
    auto temp = shared_ptr<float[]>(new float[count]);
    for (int i = 0; i < count; i++) {
        temp[i] = swapf(var[i]);
    }
    auto ret = doWithReconnect([iAddr, count, &temp]() -> bool {
        return _RuitiePLCInfo::getInstance()->m_client->WriteArea(S7AreaDB, 1, iAddr, count, S7WLReal, temp.get()) == 0;
    });
#else
    auto ret = doWithReconnect([iAddr, count, var]() -> bool {
        return _RuitiePLCInfo::getInstance()->m_client->WriteArea(S7AreaDB, 1, iAddr, count, S7WLReal, var) == 0;
    });
#endif
    if (!ret) {
        _RuitiePLCInfo::getInstance()->m_connected = false;
        return false;
    }
    return true;
}

bool AbsPLCIntf::setVariable(string s, bool b) {
    if (!isConnected()) {
        return false;
    }

    int  iStart = 0, iBit = 0, iArea = 0;
    char cArea = 0, cStart = 0, cBit = 0;
    if (sscanf(s.c_str(), "%c%c%c", &cArea, &cStart, &cBit) < 3) {
        return false;
    }
    iStart = cStart - '0';
    iBit   = cBit - '0';

    switch (cArea) {
        case 'M': iArea = S7AreaMK; break;
        case 'I': iArea = S7AreaPE; break;
        case 'Q': iArea = S7AreaPA; break;
        default: return false;
    }

    uint8_t temp = 0;
    auto    ret  = doWithReconnect([iArea, iStart, &temp]() -> bool {
        return _RuitiePLCInfo::getInstance()->m_client->ReadArea(iArea, 0, iStart, 1, S7WLByte, &temp) == 0;
    });
    ;
    if (!ret) {
        _RuitiePLCInfo::getInstance()->m_connected = false;
        return false;
    }
    if (b) {
        temp |= 1 << iBit;
    } else {
        temp &= ~(1 << iBit);
    }
    ret = doWithReconnect([iArea, iStart, &temp]() -> bool {
        return _RuitiePLCInfo::getInstance()->m_client->WriteArea(iArea, 0, iStart, 1, S7WLByte, &temp) == 0;
    });
    if (!ret) {
        _RuitiePLCInfo::getInstance()->m_connected = false;
        return false;
    }
    return true;
}
