#include "pch.h"

#include "AboutWnd.h"

AboutWnd::AboutWnd(LPCTSTR lpStr, LPCTSTR lpTitle, UINT iType) {
    m_pString = lpStr;
    m_pTitle  = lpTitle;
    m_iType   = iType;
}

AboutWnd::~AboutWnd(void) {
}

LRESULT AboutWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                Close(IDCANCEL);
                return 0;
            } else if (wParam == VK_RETURN) {
                if (m_iType == MB_YESNO)
                    Close(IDYES);
                else
                    Close(IDOK);
                return 0;
            }
        } break;
    }

    return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
}

void AboutWnd::Notify(TNotifyUI &msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("IDOK")) {
            Close(IDOK);
            return;
        } else if (strName == _T("IDYES")) {
            Close(IDYES);
            return;
        }
    }

    CDuiWindowBase::Notify(msg);
}

void AboutWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    m_pTabBtn = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl(_T("TabBtn")));

    if (m_iType == MB_YESNO)
        m_pTabBtn->SelectItem(1);
    else
        m_pTabBtn->SelectItem(0);

    m_pTextTitle  = static_cast<CTextUI *>(m_PaintManager.FindControl(_T("LabelTitle")));
    m_pTextString = static_cast<CTextUI *>(m_PaintManager.FindControl(_T("LabelString")));
    m_pTextTitle->SetText(m_pTitle);
    m_pTextString->SetText(m_pString);
}
