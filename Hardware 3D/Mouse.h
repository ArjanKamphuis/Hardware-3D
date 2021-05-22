#pragma once

#include <optional>
#include <queue>

class Mouse
{
	friend class Window;

public:
	struct RawDelta
	{
		int X = 0;
		int Y = 0;
	};

	class Event
	{
	public:
		enum class Type { LPress, LRelease, RPress, RRelease, WheelUp, WheelDown, Move, Enter, Leave, Invalid };

	public:
		Event(Type type, const Mouse& parent) noexcept;

		bool LeftIsPressed() const noexcept;
		bool RightIsPressed() const noexcept;
		Type GetType() const noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;
		std::pair<int, int> GetPos() const noexcept;

	private:
		Type mType;
		bool mLeftIsPresed;
		bool mRightIsPressed;
		int mX;
		int mY;
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool IsEmpty() const noexcept;
	bool IsInWindow() const noexcept;

	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	std::pair<int, int> GetPos() const noexcept;

	std::optional<Mouse::Event> Read() noexcept;
	std::optional<RawDelta> ReadRawDelta() noexcept;
	void Flush() noexcept;

	void EnableRaw() noexcept;
	void DisableRaw() noexcept;
	bool RawEnabled() const noexcept;

private:
	void OnMouseMove(int x, int y) noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, short delta) noexcept;
	void OnRawDelta(int dx, int dy) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;

private:
	static constexpr unsigned int mBufferSize = 16u;
	std::queue<Event> mBuffer;
	std::queue<RawDelta> mRawDeltaBuffer;

	int mX = 0;
	int mY = 0;
	int mWheelDeltaCarry = 0;
	bool mLeftIsPressed = false;
	bool mRightIsPressed = false;
	bool mIsInWindow = false;
	bool mRawEnabled = false;
};
