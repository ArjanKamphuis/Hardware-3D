#include "App.h"

#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"

#include "Surface.h"
#include "GDIPlusManager.h"

GDIPlusManager gdipm;

using namespace DirectX;

App::App()
    : mWnd(800, 600, L"The Donkey Fart Box")
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			: mGfx(gfx)
		{}

		std::unique_ptr<Drawable> operator()()
		{
			switch (mTypeDist(mRng))
			{
			case 0:	return std::make_unique<Pyramid>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist);
			case 1: return std::make_unique<Box>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mBdist);
			case 2:	return std::make_unique<Melon>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mLongDist, mLatDist);
			case 3: return std::make_unique<Sheet>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist);
			default:
				assert(false && "Bad drawable type in factory");
				return {};
			}
		}

	private:
		Graphics& mGfx;
		std::mt19937 mRng{ std::random_device{}() };
		std::uniform_real_distribution<float> mAdist{ 0.0f, XM_2PI };
		std::uniform_real_distribution<float> mDdist{ 0.0f, XM_PI };
		std::uniform_real_distribution<float> mOdist{ 0.0f, XM_PI * 0.08f };
		std::uniform_real_distribution<float> mRdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> mBdist{ 0.4f, 3.0f };
		std::uniform_int_distribution<int> mLatDist{ 5, 20 };
		std::uniform_int_distribution<int> mLongDist{ 10, 40 };
		std::uniform_int_distribution<int> mTypeDist{ 0, 3 };
	};

	Factory f(mWnd.Gfx());
	mDrawables.reserve(mNumDrawables);
	std::generate_n(std::back_inserter(mDrawables), mNumDrawables, f);
}

App::~App()
{
}

int App::Go()
{
	std::optional<int> exitcode;
	while (!(exitcode = Window::ProcessMessages()))
	{
		HandleInput();
		DoFrame();
	}
	return *exitcode;
}

void App::HandleInput()
{
	while (const auto e = mWnd.Keyboard.ReadKey())
	{
		if (!e->IsPress())
			return;

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}
}

void App::DoFrame()
{
	const float dt = mTimer.Mark();
	mWnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	for (auto& d : mDrawables)
	{
		d->Update(mWnd.Keyboard.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(mWnd.Gfx());
	}
	mWnd.Gfx().EndFrame();
}
