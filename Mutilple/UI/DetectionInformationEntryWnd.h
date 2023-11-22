#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"

class DetectionInformationEntryWnd : public CDuiWindowBase {
public:
    ~DetectionInformationEntryWnd();

    virtual LPCTSTR    GetWindowClassName() const override;
    virtual CDuiString GetSkinFile() override;
    void               InitWindow() override;
    void               Notify(TNotifyUI& msg) override;

private:
};
