#pragma once

class Scan {
public:
    ~Scan();
    // 成像每圈点数
    void SetSidePoints(int iPoints);
    int  GetSidePoint() {
        return m_iSidePoints;
    }
    float GetSideAngleStep() {
        return m_fSideAngleStep;
    }

    void SetTreadPoints(int iPoints);
    int  GetTreadPoint() {
        return m_iTreadPoints;
    }
    float GetTreadAngleStep() {
        return m_fTreadAngleStep;
    }

    // 扫查时，每圈半径，fRadius:第一圈半径
    void SetScanRadius(float fRadius);

    void InitScanData();

public:
    // 探头分布
    int m_pTreadProbe[6]{0, 1, 2, 3, 4, 5}; // 踏面6个三个弧度 三个直探头
    int m_pSideProbe[4]{6, 7, 8, 9};        // 侧面4个 2个双晶
    int m_pSideTrUT[2]{6, 7};               // 透声检查通道

    // 扫查时，侧面前进步进
    float m_fSideStep{1.0f};
    // 扫查时踏面前进步进
    //  float m_fTreadStep{1.0f };

    // 侧面/踏面，检测圈数   PLC的的 起始-结束 /步进
    union {
        int m_iSideSize{10};
        int m_iTreadSize;
    };

    int m_iDrawProbleSideSize;  // 侧面绘图两个探头之间的圈数
    int m_iDrawProbleTreadSize; // 踏面绘图两个探头之间的圈数

    int m_iDrawSideSize  = {0};  // 侧面绘图圈数   根据 检测范围 /步进      轮辋外径（7）-轮辋内径（8）
    int m_iDrawTreadSize = {0}; // 踏面绘图圈数 根据 检测范围/步进        踏面轮辋厚度（2）-轮缘厚度（8）

    // 侧面每圈检测半径
    std::vector<float> m_pSideRadius;

    // B扫图像数据 //A门最高波   //DAC 最高缺陷dB
    std::vector<UCHAR> m_pBscanData;
    std::vector<UCHAR> m_pBscanCH; // 缺陷通道 0 无缺陷 1-10 对应发现缺陷的通道 绘图时选对应的通道颜色

    //  std::vector<int> m_pBscanIndex;   //缺陷 对应的record中缺陷数据索引

    // C扫图像数据 //A门最高波
    std::vector<UCHAR> m_pCscanData;
    std::vector<UCHAR> m_pCscanCH; // 缺陷通道 0 无缺陷 1-10 对应发现缺陷的通道  绘图时选对应的通道颜色

    // std::vector<int> m_pCscanIndex;   //缺陷 对应的record中缺陷数据索引
private:
    // 侧面每圈检测点数
    int m_iSidePoints{72};
    // 踏面每圈检测点数
    int m_iTreadPoints{72};

    // 每点数角度步进
    float m_fSideAngleStep{5.0f};
    // 每点数角度步进
    float m_fTreadAngleStep{5.0f};

    //
};
