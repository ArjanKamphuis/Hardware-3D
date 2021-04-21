#include "App.h"

#include "Box.h"
#include "Can.h"
#include "Pyramid.h"
#include "SkinnedBox.h"

#include "Surface.h"
#include "GDIPlusManager.h"

#include "imgui/imgui.h"

GDIPlusManager gdipm;

using namespace DirectX;

App::App()
    : mWnd(800, 600, L"The Donkey Fart Box"), mLight(mWnd.Gfx())
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			: mGfx(gfx)
		{}

		std::unique_ptr<Drawable> operator()()
		{
			switch (mSdist(mRng))
			{
			case 0:
				const XMFLOAT3 material = { mCdist(mRng), mCdist(mRng) , mCdist(mRng) };
				return std::make_unique<Box>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mBdist, material);
			case 1:
				return std::make_unique<Can>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mBdist, mTdist);
			case 2:
				return std::make_unique<Pyramid>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mTdist);
			case 3:
				return std::make_unique<SkinnedBox>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist);
			default:
				assert(false && "Impossible drawable option in factory");
				return {};
			}
		}

	private:
		Graphics& mGfx;
		std::mt19937 mRng{ std::random_device{}() };
		std::uniform_int_distribution<int> mSdist{ 0, 3 };
		std::uniform_real_distribution<float> mAdist{ 0.0f, XM_2PI };
		std::uniform_real_distribution<float> mDdist{ 0.0f, XM_PI };
		std::uniform_real_distribution<float> mOdist{ 0.0f, XM_PI * 0.08f };
		std::uniform_real_distribution<float> mRdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> mBdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> mCdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> mTdist{ 3, 30 };
	};

	mDrawables.reserve(mNumDrawables);
	std::generate_n(std::back_inserter(mDrawables), mNumDrawables, Factory{ mWnd.Gfx() });
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
	const float dt = mTimer.Mark() * mSpeedFactor;

	mWnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	mWnd.Gfx().SetCamera(mCamera.GetMatrix());

	mLight.SetCameraPosition(mCamera.GetPosition());
	mLight.Bind(mWnd.Gfx());

	for (auto& d : mDrawables)
	{
		d->Update(mWnd.Keyboard.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(mWnd.Gfx());
	}

	mLight.Draw(mWnd.Gfx());

	if (mWnd.Gfx().IsImguiEnabled())
		DoImGui();
	
	mWnd.Gfx().EndFrame();
}

void App::DoImGui()
{
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &mSpeedFactor, 0.0f, 4.0f);
		const float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS", 1000.0f / frameRate, frameRate);
		ImGui::Text("Status: %s", mWnd.Keyboard.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}

	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();

	ImGui::End();
}
