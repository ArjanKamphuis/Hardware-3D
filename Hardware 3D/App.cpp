#include "App.h"

#include "AssTest.h"
#include "Box.h"
#include "Can.h"
#include "Pyramid.h"
#include "SkinnedBox.h"
#include "VertexLayout.h"

#include "Surface.h"
#include "GDIPlusManager.h"

#include "imgui/imgui.h"

GDIPlusManager gdipm;

using namespace DirectX;

void f()
{
	using ElementType = VertexLayout::ElementType;

	VertexBufferExp vb(std::move(VertexLayout{}
		.Append<ElementType::Position3D>()
		.Append<ElementType::Normal>()
		.Append<ElementType::Texture2D>()
	));

	vb.EmplaceBack(XMFLOAT3{ 1.0f, 1.0f, 5.0f }, XMFLOAT3{ 2.0f, 1.0f, 4.0f }, XMFLOAT2{ 6.0f, 9.0f });
	vb.EmplaceBack(XMFLOAT3{ 6.0f, 9.0f, 6.0f }, XMFLOAT3{ 9.0f, 6.0f, 9.0f }, XMFLOAT2{ 4.2f, 0.0f });

	auto& pos = vb[0].Attr<ElementType::Position3D>();
	auto& nor = vb[0].Attr<ElementType::Normal>();
	auto& tex = vb[1].Attr<ElementType::Texture2D>();
	
	vb.Back().Attr<ElementType::Position3D>().z = 420.0f;
	pos = vb.Back().Attr<ElementType::Position3D>();
}

App::App()
    : mWnd(800, 600, L"The Donkey Fart Box"), mLight(mWnd.Gfx())
{
	f();

	class Factory
	{
	public:
		Factory(Graphics& gfx)
			: mGfx(gfx)
		{}

		std::unique_ptr<Drawable> operator()()
		{
			const XMFLOAT3 material = { mCdist(mRng), mCdist(mRng) , mCdist(mRng) };
			
			switch (mSdist(mRng))
			{
			case 0:
				return std::make_unique<Box>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mBdist, material);
			case 1:
				return std::make_unique<Can>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mBdist, mTdist);
			case 2:
				return std::make_unique<Pyramid>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mTdist);
			case 3:
				return std::make_unique<SkinnedBox>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist);
			case 4:
				return std::make_unique<AssTest>(mGfx, mRng, mAdist, mDdist, mOdist, mRdist, mScaledist, material);
			default:
				assert(false && "Impossible drawable option in factory");
				return {};
			}
		}

	private:
		Graphics& mGfx;
		std::mt19937 mRng{ std::random_device{}() };
		std::uniform_int_distribution<int> mSdist{ 0, 4 };
		std::uniform_real_distribution<float> mAdist{ 0.0f, XM_2PI };
		std::uniform_real_distribution<float> mDdist{ 0.0f, XM_PI };
		std::uniform_real_distribution<float> mOdist{ 0.0f, XM_PI * 0.08f };
		std::uniform_real_distribution<float> mRdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> mBdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> mCdist{ 0.0f, 1.0f };
		std::uniform_real_distribution<float> mScaledist{ 0.5f, 2.0f };
		std::uniform_int_distribution<int> mTdist{ 3, 30 };
	};

	mDrawables.reserve(mNumDrawables);
	std::generate_n(std::back_inserter(mDrawables), mNumDrawables, Factory{ mWnd.Gfx() });

	for (const std::unique_ptr<Drawable>& drawable : mDrawables)
		if (Box* box = dynamic_cast<Box*>(drawable.get()))
			mBoxes.push_back(box);
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

	while (const auto e = mWnd.Mouse.Read())
	{
		switch (e->GetType())
		{
		case Mouse::Event::Type::WheelDown:
			mCamera.ZoomOut();
			break;
		case Mouse::Event::Type::WheelUp:
			mCamera.ZoomIn();
			break;
		}
	}
}

void App::DoFrame()
{
	const float dt = mTimer.Mark() * mSpeedFactor;
	Graphics& gfx = mWnd.Gfx();

	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(mCamera.GetMatrix());

	mLight.SetCameraPosition(mCamera.GetPosition());
	mLight.Bind(gfx);

	for (auto& d : mDrawables)
	{
		d->Update(mWnd.Keyboard.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(gfx);
	}

	mLight.Draw(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui();
	
	gfx.EndFrame();
}

void App::DoImGui() noexcept
{
	SpawnSimulationWindow();
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();
}

void App::SpawnSimulationWindow() noexcept
{
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &mSpeedFactor, 0.0f, 4.0f);
		const float frameRate = ImGui::GetIO().Framerate;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS", 1000.0f / frameRate, frameRate);
		ImGui::Text("Status: %s", mWnd.Keyboard.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();
}

void App::SpawnBoxWindowManagerWindow() noexcept
{
	if (ImGui::Begin("Boxes"))
	{
		const auto preview = mComboBoxIndex ? std::to_string(*mComboBoxIndex) : "Choose a box...";
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < mBoxes.size(); i++)
			{
				const bool selected = mComboBoxIndex && (*mComboBoxIndex == i);
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
					mComboBoxIndex = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && mComboBoxIndex)
		{
			mBoxControlIds.insert(*mComboBoxIndex);
			mComboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows() noexcept
{
	for (auto it = mBoxControlIds.begin(); it != mBoxControlIds.end();)
	{
		if (!mBoxes[*it]->SpawnControlWindow(mWnd.Gfx(), *it))
			it = mBoxControlIds.erase(it);
		else
			++it;
	}		
}
