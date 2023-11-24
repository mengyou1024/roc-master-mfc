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

/// ����float���ͱ������ֽ���
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
     * @brief ��ȡint���͵ı��� �ڶ���������0
     *
     * @param name ������ �� I00��Q00��M00
     * @return
     */
    int getVariable(string name, int);

    /**
     * @brief ��ȡfloat���͵ı��� �ڶ���������0.0f
     *
     * @param name ������ �� `V1000`
     * @return
     */
    float getVariable(string name, float);

    /**
     * @brief ����float����(V��)�ı���
     *
     * @param s ������ �� `V1000`
     * @param var
     * @return true
     * @return false
     */
    bool setVariable(string s, float var);

    /**
     * @brief ��������float����(V��)�ı���
     *
     * @param s ��ʼ������ �� `V1000`
     * @param var float���������ַ
     * @param size float��������ĸ���
     * @return true
     * @return false
     */
    bool setVariable(string s, float* var, int count);

    /**
     * @brief ����bool���͵ı���
     *
     * @param s ������ �� `M00`��`I00`��`Q00`
     * @param b
     * @return true
     * @return false
     */
    bool setVariable(string s, bool b);

}; // namespace AbsPLCIntf
