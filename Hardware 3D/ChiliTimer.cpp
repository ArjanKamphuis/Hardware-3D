#include "ChiliTimer.h"

using namespace std::chrono;

ChiliTimer::ChiliTimer() noexcept
{
    mLast = steady_clock::now();
}

float ChiliTimer::Mark() noexcept
{
    const steady_clock::time_point old = mLast;
    mLast = steady_clock::now();
    const duration<float> frameTime = mLast - old;
    return frameTime.count();
}

float ChiliTimer::Peek() const noexcept
{
    return duration<float>(steady_clock::now() - mLast).count();
}
