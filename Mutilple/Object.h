#pragma once

class Object
{
public:
	Object();
	virtual ~Object();

	BOOL Open(LPCTSTR lpFileName);
	BOOL Save(LPCTSTR lpFileName);

	/*初始化数据Buf*/
	virtual void Resize(DWORD dwSize);

	/*获取序列化长度*/
	virtual DWORD GetSerializeLen(INT32 iType = -1) = 0;

	/*序列化对象数据*/
	virtual BYTE* Serialize() = 0;

	/**反序列化对象数据*/
	virtual BOOL Unserialize(BYTE* pData, DWORD dwLen) = 0;

	BOOL Compare(Object* pCompare);
protected:
	INT32 m_iVersion;						//类版本
	std::vector<BYTE> m_pStream;
};