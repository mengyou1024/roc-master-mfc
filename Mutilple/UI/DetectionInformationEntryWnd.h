#pragma once
#include "Define.h"
#include "DuiWindowBase.h"
#include "OpenGL.h"
#include "Thread.h"
#include <Model/DetectInfo.h>
#include <Model/UserModel.h>

class DetectionInformationEntryWnd : public CDuiWindowBase {
public:
    ~DetectionInformationEntryWnd();

    virtual LPCTSTR              GetWindowClassName() const override;
    virtual CDuiString           GetSkinFile() override;
    void                         InitWindow() override;
    void                         Notify(TNotifyUI &msg) override;
    const ORM_Model::JobGroup   &GetJobGroup() const noexcept;
    const ORM_Model::User       &GetUser() const noexcept;
    const ORM_Model::DetectInfo &GetDetectInfo() const noexcept;
    bool                         GetResult() const noexcept;

private:
    void LoadUserGroupInfo();

    bool                  mResult     = false;
    ORM_Model::JobGroup   mJobGroup   = {};
    ORM_Model::User       mUser       = {};
    ORM_Model::DetectInfo mDetectinfo = {};
};
