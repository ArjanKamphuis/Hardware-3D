#include "App.h"

#include "Box.h"

using namespace DirectX;

App::App()
    : mWnd(800, 600, L"The Donkey Fart Box")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, XM_2PI);
	std::uniform_real_distribution<float> ddist(0.0f, XM_2PI);
	std::uniform_real_distribution<float> odist(0.0f, XM_PI * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);

	for (int i = 0; i < 80; i++)
		mBoxes.push_back(std::make_unique<Box>(mWnd.Gfx(), rng, adist, ddist, odist, rdist));

	mWnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 0.75f, 0.5f, 40.0f));
}

App::~App()
{
}

int App::Go()
{
	std::optional<int> exitcode;
	while (!(exitcode = Window::ProcessMessages()))
		DoFrame();
	return *exitcode;
}

void App::DoFrame()
{
	const float dt = mTimer.Mark();
	mWnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	for (auto& b : mBoxes)
	{
		b->Update(dt);
		b->Draw(mWnd.Gfx());
	}
	mWnd.Gfx().EndFrame();
}
