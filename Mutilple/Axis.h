#pragma once

class CAxis
{
public:
	CAxis(void);
	~CAxis(void);

	void Set( int iLen, int iCount, float fMin, float fMax );
	void Set( int iLen, float fMin, float fMax);
	void Set( int iLen, int iAxisScale, int iZero );

	size_t GetCount(){ return m_pAxis.size(); }
	float& operator[]( size_t iIndex );

public:
	vector<FLOAT> m_pAxis;			//�̶�
	vector<int> m_pAxisType;		//0:��ͨ�̶ȣ�1:С�̶ȣ� 2����̶�
	vector<FLOAT> m_pAxis_value;	//�̶�ֵ
};