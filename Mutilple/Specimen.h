#pragma once

class Specimen
{

public:
    bool LoadTec(LPCTSTR lpName);
    bool SaveTec(LPCTSTR lpName);
public:
    ////工件名称
    //TCHAR m_pName[STR_LEN];
    ////工件型号
    //TCHAR m_pModel[STR_LEN];
    ////工件材料
    //TCHAR m_pMaterial[STR_LEN];
    ////工件编号
    //TCHAR m_pNumber[STR_LEN];
    ////炉号 
    //TCHAR m_pHeatNumber[STR_LEN];

    //工件直径
    //float m_fDiameter;

    //车轮信息 
    WHEEL_PAPA m_WheelParam; //目前暂定结构体 加快开发进度
};

