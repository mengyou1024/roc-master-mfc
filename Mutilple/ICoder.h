#pragma once

#include <shared_mutex>

template<class T>
class ICoder
{
public:
    inline void SetCoder(T Coder)
    {
        std::unique_lock<std::shared_mutex> lock(_mutex);
        _Coder = Coder;
    }

    inline T GetCoder()
    {
        std::shared_lock<std::shared_mutex> lock(_mutex);
        return _Coder;
    }

private:
    T _Coder;
    mutable std::shared_mutex _mutex;
};

typedef ICoder<float> ICoderAngle;