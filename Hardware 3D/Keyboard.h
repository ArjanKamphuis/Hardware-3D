#pragma once

#include <bitset>
#include <optional>
#include <queue>

class Keyboard
{
	friend class Window;

public:
	class Event
	{
	public:
		enum class Type { Press, Release, Invalid };

	public:
		Event(Type type, unsigned char code) noexcept;

		bool IsPress() const noexcept;
		bool IsRelease() const noexcept;
		unsigned char GetCode() const noexcept;

	private:
		Type mType;
		unsigned char mCode;
	};

public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	bool KeyIsPressed(unsigned char keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;

	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;

	void FlushKey() noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;

private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int mNumKeys = 256u;
	static constexpr unsigned int mBufferSize = 16u;
	bool mAutorepeatEnabled = false;

	std::bitset<mNumKeys> mKeyStates;
	std::queue<Event> mKeyBuffer;
	std::queue<char> mCharBuffer;

};
