#include "BusyWnd.h"

BusyWnd::BusyWnd(std::function<void(void)> _func) {
    func = _func;
}

BusyWnd::~BusyWnd() {}

LPCTSTR BusyWnd::GetWindowClassName() const {
    return _T("BusyWnd");
}

CDuiString BusyWnd::GetSkinFile() {
    return _T("Theme\\UI_BusyWnd.xml");
}

void BusyWnd::InitWindow() {
    CDuiWindowBase::InitWindow();
    CenterWindow();
    std::thread t([this]() {
        std::invoke(func);
        Close();
    });
    t.detach();
}

void BusyWnd::OnLButtonDown(UINT nFlags, ::CPoint pt) {}

void BusyWnd::OnRButtonDown(UINT nFlags, ::CPoint pt) {}

void BusyWnd::OnLButtonUp(UINT nFlags, ::CPoint pt) {}

void BusyWnd::OnRButtonUp(UINT nFlags, ::CPoint pt) {}

void BusyWnd::OnLButtonDClick(UINT nFlags, ::CPoint pt) {}

void BusyWnd::OnMouseMove(UINT nFlags, ::CPoint pt) {}

void BusyWnd::OnMouseWheel(UINT nFlags, short zDelta, ::CPoint pt) {}
