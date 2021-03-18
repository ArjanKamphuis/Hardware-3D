#include "Mouse.h"

Mouse::Event::Event(Type type, const Mouse& parent) noexcept
    : mType(type), mLeftIsPresed(parent.mLeftIsPressed), mRightIsPressed(parent.mRightIsPressed), mX(parent.mX), mY(parent.mY)
{
}

bool Mouse::Event::IsValid() const noexcept
{
    return mType != Type::Invalid;
}

bool Mouse::Event::LeftIsPressed() const noexcept
{
    return mLeftIsPresed;
}

bool Mouse::Event::RightIsPressed() const noexcept
{
    return mRightIsPressed;
}

Mouse::Event::Type Mouse::Event::GetType() const noexcept
{
    return mType;
}

int Mouse::Event::GetPosX() const noexcept
{
    return mX;
}

int Mouse::Event::GetPosY() const noexcept
{
    return mY;
}

std::pair<int, int> Mouse::Event::GetPos() const noexcept
{
    return { mX, mY };
}

bool Mouse::LeftIsPressed() const noexcept
{
    return mLeftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
    return mRightIsPressed;
}

bool Mouse::IsEmpty() const noexcept
{
    return mBuffer.empty();
}

bool Mouse::IsInWindow() const noexcept
{
    return false;
}

int Mouse::GetPosX() const noexcept
{
    return mX;
}

int Mouse::GetPosY() const noexcept
{
    return mY;
}

std::pair<int, int> Mouse::GetPos() const noexcept
{
    return { mX, mY };
}

Mouse::Event Mouse::Read() noexcept
{
    if (mBuffer.size() > 0)
    {
        Mouse::Event e = mBuffer.front();
        mBuffer.pop();
        return e;
    }
    return Mouse::Event();
}

void Mouse::Flush() noexcept
{
    mBuffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
    mX = x;
    mY = y;

    mBuffer.emplace(Mouse::Event::Type::Move, *this);
    TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
    mLeftIsPressed = true;
    mBuffer.emplace(Mouse::Event::Type::LPress, *this);
    TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
    mLeftIsPressed = false;
    mBuffer.emplace(Mouse::Event::Type::LRelease, *this);
    TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
    mRightIsPressed = true;
    mBuffer.emplace(Mouse::Event::Type::RPress, *this);
    TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
    mRightIsPressed = false;
    mBuffer.emplace(Mouse::Event::Type::RRelease, *this);
    TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
    mBuffer.emplace(Mouse::Event::Type::WheelUp, *this);
    TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
    mBuffer.emplace(Mouse::Event::Type::WheelDown, *this);
    TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
    mIsInWindow = true;
    mBuffer.emplace(Mouse::Event::Type::Enter, *this);
    TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
    mIsInWindow = false;
    mBuffer.emplace(Mouse::Event::Type::Leave, *this);
    TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
    while (mBuffer.size() > mBufferSize)
        mBuffer.pop();
}
