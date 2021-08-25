#pragma once

class Drawable;
class Graphics;
class Step;

class Job
{
public:
	Job(const Step* pStep, const Drawable* pDrawable);
	void Execute(const Graphics& gfx) const noexcept(!IS_DEBUG);

private:
	const Drawable* mDrawable;
	const Step* mStep;
};
