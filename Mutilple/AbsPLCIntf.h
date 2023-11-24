#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::function;
using std::make_pair;
using std::map;
using std::pair;
using std::string;
using std::vector;

/// 交换float类型变量的字节序
#define PLC_SWAP_FLOAT 1

namespace AbsPLCIntf {

    /**
     * @brief swap 16bit
     *
     * @param sval
     * @return uint16_t
     */
    uint16_t swaps(uint16_t sval);

    /**
     * @brief swap 32bit
     *
     * @param val
     * @return uint32_t
     */
    uint32_t swapl(uint32_t lval);

    /**
     * @brief swap float
     *
     * @param fval
     * @return float
     */
    float swapf(float fval);

    /**
     * @brief is PLC connected
     *
     * @return true
     * @return false
     */
    bool isConnected();

    /**
     * @brief connected to plc
     *
     * @param addr plc address
     * @param rack plc rack
     * @param slot plc slot
     */
    bool connectTo(const char* addr = "192.168.2.1", int rack = 0, int slot = 1);

    /**
     * @brief disconnect
     *
     */
    void disconnect(void);

    /**
     * @brief Get the Connected Info object
     *
     * @param addr plc address
     * @param rack plc rack
     * @param slot plc slot
     */
    void getConnectedInfo(std::string* addr = nullptr, int* rack = nullptr, int* slot = nullptr);

    /**
     * @brief 获取int类型的变量 第二个参数填0
     *
     * @param name 变量名 如 I00、Q00、M00
     * @return
     */
    int getVariable(string name, int);

    /**
     * @brief 获取float类型的变量 第二个参数填0.0f
     *
     * @param name 变量名 如 `V1000`
     * @return
     */
    float getVariable(string name, float);

    /**
     * @brief 设置float类型(V区)的变量
     *
     * @param s 变量名 如 `V1000`
     * @param var
     * @return true
     * @return false
     */
    bool setVariable(string s, float var);

    /**
     * @brief 批量设置float类型(V区)的变量
     *
     * @param s 起始变量名 如 `V1000`
     * @param var float类型数组地址
     * @param size float类型数组的个数
     * @return true
     * @return false
     */
    bool setVariable(string s, float* var, int count);

    /**
     * @brief 设置bool类型的变量
     *
     * @param s 变量名 如 `M00`、`I00`、`Q00`
     * @param b
     * @return true
     * @return false
     */
    bool setVariable(string s, bool b);

}; // namespace AbsPLCIntf
