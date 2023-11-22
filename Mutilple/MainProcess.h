#pragma once

#include <Model/SystemConfig.h>

class MainProcess {
public:
    MainProcess();
    ~MainProcess();

    void InitStroage();

public:
#if _DEBUG
    FILE *mFile = nullptr;
#endif
    MAINPROCESS_TYPE m_enumMainProType = {};
};
