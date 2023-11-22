#include "DetectionInformationEntryWnd.h"
#include "ModelAScan.h"

DetectionInformationEntryWnd::~DetectionInformationEntryWnd() {}

LPCTSTR DetectionInformationEntryWnd::GetWindowClassName() const {
    return _T("DetectionInformationEntryWnd");
}

CDuiString DetectionInformationEntryWnd::GetSkinFile() {
    return _T(R"(Theme\UI_DetectionInformationEntryWnd.xml)");
}

void DetectionInformationEntryWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
}

void DetectionInformationEntryWnd::Notify(TNotifyUI& msg) {
    CDuiWindowBase::Notify(msg);
}