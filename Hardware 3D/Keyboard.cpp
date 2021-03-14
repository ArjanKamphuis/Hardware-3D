#include "Keyboard.h"

Keyboard::Event::Event() noexcept
	: mType(Type::Invalid), mCode(0u)
{
}

Keyboard::Event::Event(Type type, unsigned char code) noexcept
	: mType(type), mCode(code)
{
}

bool Keyboard::Event::IsPress() const noexcept
{
	return mType == Type::Press;
}

bool Keyboard::Event::IsRelease() const noexcept
{
	return mType == Type::Release;
}

bool Keyboard::Event::IsValid() const noexcept
{
	return mType == Type::Invalid;
}

unsigned char Keyboard::Event::GetCode() const noexcept
{
	return mCode;
}

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return mKeyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (mKeyBuffer.size() > 0)
	{
		Keyboard::Event e = mKeyBuffer.front();
		mKeyBuffer.pop();
		return e;
	}
	return Keyboard::Event();
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return mKeyBuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (mCharBuffer.size() > 0)
	{
		unsigned char charCode = mCharBuffer.front();
		mCharBuffer.pop();
		return charCode;
	}
	return 0;
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return mCharBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	mKeyBuffer = std::queue<Event>();
}

void Keyboard::FlushChar() noexcept
{
	mCharBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
	mAutorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	mAutorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return mAutorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	mKeyStates[keycode] = true;
	mKeyBuffer.emplace(Keyboard::Event::Type::Press, keycode);
	TrimBuffer(mKeyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	mKeyStates[keycode] = false;
	mKeyBuffer.emplace(Keyboard::Event::Type::Release, keycode);
	TrimBuffer(mKeyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	mCharBuffer.emplace(character);
	TrimBuffer(mCharBuffer);
}

void Keyboard::ClearState() noexcept
{
	mKeyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > mBufferSize)
		buffer.pop();
}
