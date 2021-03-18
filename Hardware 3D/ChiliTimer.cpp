#include "ChiliTimer.h"

using namespace std::chrono;

ChiliTimer::ChiliTimer()
{
    mLast = steady_clock::now();
}

float ChiliTimer::Mark()
{
    const steady_clock::time_point old = mLast;
    mLast = steady_clock::now();
    const duration<float> frameTime = mLast - old;
    return frameTime.count();
}

float ChiliTimer::Peek() const
{
    return duration<float>(steady_clock::now() - mLast).count();
}
