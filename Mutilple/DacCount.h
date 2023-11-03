#pragma once
#include "Define.h"
#include <vector>

class DacCount
{
public:

	DacCount();

	void CoutDacCurve(DETECTION_PARAM2995_200* DACStruct, float gain, float delay, float range, vector<float>& DAC_M);
	float CoutNewPeekByDis(float len1, float peek1, float len2);

	float CoutDACNewPeek(float OlddB, float NewdB, float OldPeek);

	float CoutdBOffsetByAmp(float Amp1, float Amp2);
};

