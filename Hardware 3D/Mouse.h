#pragma once

#include <queue>

class Mouse
{
	friend class Window;

public:
	class Event
	{
	public:
		enum class Type { LPress, LRelease, RPress, RRelease, WheelUp, WheelDown, Move, Invalid };

	public:
		Event() noexcept = default;
		Event(Type type, const Mouse& parent) noexcept;

		bool IsValid() const noexcept;
		bool LeftIsPressed() const noexcept;
		bool RightIsPressed() const noexcept;
		Type GetType() const noexcept;
		int GetPosX() const noexcept;
		int GetPosY() const noexcept;
		std::pair<int, int> GetPos() const noexcept;


	private:
		Type mType = Type::Invalid;
		bool mLeftIsPresed = false;
		bool mRightIsPressed = false;
		int mX = 0;
		int mY = 0;
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool IsEmpty() const noexcept;

	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	std::pair<int, int> GetPos() const noexcept;

	Mouse::Event Read() noexcept;
	void Flush() noexcept;

private:
	void OnMouseMove(int x, int y) noexcept;
	void OnLeftPresed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;

private:
	static constexpr unsigned int mBufferSize = 16u;
	std::queue<Event> mBuffer;

	int mX = 0;
	int mY = 0;
	bool mLeftIsPressed = false;
	bool mRightIsPressed = false;
};
