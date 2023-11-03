#include "pch.h"

#include "Object.h"

Object::Object() :
m_iVersion(0) {
}

Object::~Object() {
    m_pStream.clear();
}

BOOL Object::Open(LPCTSTR lpFileName) {
    CFile pFile(lpFileName, CFile::modeRead | CFile::typeBinary);
    if (pFile.m_hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    int uLength = static_cast<int>(pFile.GetLength());
    if (uLength == 0)
        return FALSE;

    BYTE* pBuf = new BYTE[uLength];

    pFile.Read(pBuf, uLength);

    Unserialize(pBuf, uLength);

    delete[] pBuf;
    pBuf = NULL;

    return TRUE;
}

BOOL Object::Save(LPCTSTR lpFileName) {
    CFile pFile(lpFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
    if (pFile.m_hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    pFile.Write(Serialize(), GetSerializeLen());

    pFile.Close();

    return TRUE;
}

void Object::Resize(DWORD dwSize) {
    m_pStream.resize(dwSize, 0);
}

BOOL Object::Compare(Object* pCompare) {
    BOOL  bSame          = FALSE;
    DWORD dwSerializeLen = GetSerializeLen();
    if (dwSerializeLen != pCompare->GetSerializeLen())
        return bSame;

    BYTE* p  = Serialize();
    BYTE* pc = pCompare->Serialize();

    bSame = TRUE;
    for (DWORD i = 0; i < dwSerializeLen; i++) {
        if (*(p + i) != *(pc + i)) {
            bSame = FALSE;
            break;
        }
    }

    return bSame;
}