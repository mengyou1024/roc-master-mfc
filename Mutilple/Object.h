#pragma once

class Object
{
public:
	Object();
	virtual ~Object();

	BOOL Open(LPCTSTR lpFileName);
	BOOL Save(LPCTSTR lpFileName);

	/*��ʼ������Buf*/
	virtual void Resize(DWORD dwSize);

	/*��ȡ���л�����*/
	virtual DWORD GetSerializeLen(INT32 iType = -1) = 0;

	/*���л���������*/
	virtual BYTE* Serialize() = 0;

	/**�����л���������*/
	virtual BOOL Unserialize(BYTE* pData, DWORD dwLen) = 0;

	BOOL Compare(Object* pCompare);
protected:
	INT32 m_iVersion;						//��汾
	std::vector<BYTE> m_pStream;
};