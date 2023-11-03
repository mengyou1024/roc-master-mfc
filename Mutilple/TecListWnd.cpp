#include "pch.h"

#include "Mutilple.h"
#include "TecListWnd.h"
TecListWnd::TecListWnd(void) {
}

TecListWnd::~TecListWnd(void) {
}

void TecListWnd::InitWindow() {
    CDuiWindowBase::InitWindow();

    m_pEditTechName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("EditTech")));
    m_pEditTechName->OnNotify += MakeDelegate(this, &TecListWnd::OnEdit);

    m_pListTechFileList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("TechFileList")));
    m_pLayoutTechSave   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("LayoutTechSave")));
    m_pLayoutReadRead   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("LayoutReadRead")));
    if (m_TechType == TECH_SAVE) {
        m_pLayoutTechSave->SetVisible(true);
        m_pLayoutReadRead->SetVisible(false);

    } else if (m_TechType == TECH_READ) {
        m_pLayoutTechSave->SetVisible(false);
        m_pLayoutReadRead->SetVisible(true);
        m_pEditTechName->SetEnabled(false);
        m_pEditTechName->SetText(g_MainProcess.m_Techniques.m_pCurTechName);
    }
    //
    ShowListData();
}

void TecListWnd::Notify(TNotifyUI& msg) {
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        CDuiString strName = msg.pSender->GetName();

        if (strName == _T("IDOK")) {
            Close(IDOK);
        } else if (strName == _T("IDCANCEL1")) // 退出
        {
            Close(IDOK);
        } else if (strName == _T("BtnSaveTech")) // 保存
        {
            OnBtnSaveTech();

        } else if (strName == _T("BtnLoadTech")) // 调用
        {
            OnBtnLoadTech();
            StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, m_pEditTechName->GetText());
        } else if (strName == _T("BtnDeleteTech")) // 删除
        {
            OnBtnDelectTech();
        }
    } else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
        CDuiString strName = msg.pSender->GetName();
        if (strName == _T("TechFileList")) {
            OnSelectelistItem();
        }
    }
    CDuiWindowBase::Notify(msg);
}
void TecListWnd::OnBtnSaveTech() {
    CString strTechName = m_pEditTechName->GetText();

    if (strTechName.GetLength() <= 0) {
        DMessageBox(_T("请输入工艺名称！"), _T("工艺保存！"));
        return;
    }
    CString strCfgFile;
    strCfgFile.Format(_T("%sTechINI\\%s.tech"), theApp.m_pExePath, strTechName.GetBuffer(0));
    if (PathFileExists(strCfgFile)) {
        if (DMessageBox(_T("文件已存在,是否覆盖?"), _T("工艺保存！"), MB_YESNO) == IDYES) {
            StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, m_pEditTechName->GetText());
            g_MainProcess.m_Techniques.SaveTecBinary(strCfgFile);
        } else {
            return;
        }
    } else {
        StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, m_pEditTechName->GetText());
        g_MainProcess.m_Techniques.SaveTecBinary(strCfgFile);
    }

    if (PathFileExists(strCfgFile)) {
        DMessageBox(_T("工艺保存成功"), _T("工艺保存！"));
    }

    ShowListData();
}
// 调用工艺
void TecListWnd::OnBtnLoadTech() {
    CString strFind;
    CString strTechName = m_pEditTechName->GetText();
    StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, strTechName);
    // strFind.Format(_T("%sTechINI\\%s.ini"), theApp.m_pExePath, strTechName);
    // g_MainProcess.m_Techniques.LoadTec(strFind);

    strFind.Format(_T("%sTechINI\\%s.tech"), theApp.m_pExePath, strTechName.GetBuffer(0));
    g_MainProcess.m_Techniques.LoadTecBinary(strFind);
    Close(IDOK);
}
// 删除工艺
void TecListWnd::OnBtnDelectTech() {
    CString strFind;
    CString strTechName = m_pEditTechName->GetText();
    if (StrCmp(strTechName, g_MainProcess.m_Techniques.m_pCurTechName) == 0) {
        StrCpy(g_MainProcess.m_Techniques.m_pCurTechName, _T("无"));
    }
    strFind.Format(_T("%sTechINI\\%s.tech"), theApp.m_pExePath, strTechName.GetBuffer(0));
    if (PathFileExists(strFind)) {
        if (DMessageBox(_T("是否确认删除工艺?"), _T("工艺管理！"), MB_YESNO) == IDYES) {
            DeleteFile(strFind);
        }
        DMessageBox(_T("工艺已删除"), _T("工艺管理！"));
    }
    ShowListData();
}
void TecListWnd::UpdateTechList() {
    CString   strFind;
    CFileFind ff;
    BOOL      ret;

    m_ITechList.RemoveAll();

    strFind.Format(_T("%sTechINI\\*.tech"), theApp.m_pExePath);
    ret = ff.FindFile(strFind);
    while (ret) {
        ret = ff.FindNextFile();
        if (!ff.IsDirectory() && !ff.IsDots()) {
            FILE_INFO fi;
            memset(&fi, 0, sizeof(FILE_INFO));
            StrCpy(fi.pName, ff.GetFileTitle().GetBuffer(0));
            StrCpy(fi.pPath, ff.GetFilePath().GetBuffer(0));

            WIN32_FIND_DATA wfd;
            FindFirstFile(fi.pPath, &wfd);
            FILETIME ft;
            FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &ft);
            SYSTEMTIME st;
            FileTimeToSystemTime(&ft, &st);

            StrCpy(fi.pUser, _T("未记录"));
            _stprintf_s(fi.pDate, _T("%4d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

            m_ITechList.Add(fi);
        }
    }
}
void TecListWnd::ShowListData() {
    UpdateTechList();
    CListContainerElementUI* pElement = NULL;

    if (m_pListTechFileList != NULL) {
        m_pListTechFileList->RemoveAll();

        int iCount = static_cast<int>(m_ITechList.GetCount());

        for (int i = 0; i < iCount; i++) {
            pElement = new CListContainerElementUI();
            pElement->SetFixedHeight(70);
            m_pListTechFileList->AddAt(pElement, 0);

            CString strXml;
            strXml.Format(_T("Theme\\TecItem.xml"));
            CDialogBuilder       builder;
            CHorizontalLayoutUI* pLayout = static_cast<CHorizontalLayoutUI*>(builder.Create(strXml.GetBuffer(0), 0, NULL, &m_PaintManager));
            pElement->Add(pLayout);

            CLabelUI* pLabelName      = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileName")));
            CLabelUI* pLabelInfo      = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileInfo")));
            CLabelUI* pLabelInfoIndex = static_cast<COptionUI*>(pElement->FindSubControl(_T("LabelFileIndex")));

            pLabelName->SetText(m_ITechList[i].pName);
            CString strText;
            strText.Format(_T("修改时间：%s"), m_ITechList[i].pDate);
            pLabelInfo->SetText(strText.GetBuffer(0));
            //	strText.Format(_T("%d"),it);
            // pLabelInfoIndex->SetText(strText);
        }
    }
}
void TecListWnd::OnSelectelistItem() {
    INT                      iIndex   = m_pListTechFileList->GetCurSel();
    int                      iCount   = m_pListTechFileList->GetCount();
    CListContainerElementUI* pElement = (CListContainerElementUI*)(m_pListTechFileList->GetItemAt(iIndex));

    CLabelUI* pLabelName = static_cast<CLabelUI*>(pElement->FindSubControl(_T("LabelFileName")));

    CString strName = pLabelName->GetText();
    m_pEditTechName->SetText(strName.GetBuffer(0));
    // CString strIndex = pLabelInfoIndex->GetText();
}

bool TecListWnd::OnEdit(void* pv) {
    TNotifyUI* msg = (TNotifyUI*)pv;

    if (msg->sType == DUI_MSGTYPE_RETURN || msg->sType == DUI_MSGTYPE_KILLFOCUS) {
        CDuiString strName = msg->pSender->GetName();
        CDuiString strText = msg->pSender->GetText();

        if (strName == _T("EditTech")) {
            CString strName = strText.GetData();

            if (strName.GetLength() <= 0) {
                DMessageBox(_T("请输入工艺名称！"), _T("工艺保存！"));
                return false;
            }
        }
    }

    return true;
}