#pragma once

enum class CHECK{ DIR, CALIB, SCAN, ANALYSIS,PLC };
class ICheckCarrier
{
public:
    virtual void Begin() = 0;
    virtual void SetResult(CHECK check, bool bResult) = 0;
    virtual void End() = 0;
}; 