#include "App.h"

#include "ChiliUtil.h"
#include "DynamicConstantBuffer.h"

using namespace DirectX;

void TestDynamicConstant()
{
	// data roundtrip tests
	{
		Dcb::Layout s;
		s.Add<Dcb::Struct>(L"butts");
		s[L"butts"].Add<Dcb::Float3>(L"pubes").Add<Dcb::Float>(L"dank");
		s.Add<Dcb::Float>(L"woot").Add<Dcb::Array>(L"arr");
		s[L"arr"].Set<Dcb::Struct>(4);
		s[L"arr"].T().Add<Dcb::Float3>(L"twerk");
		s[L"arr"].T().Add<Dcb::Array>(L"werk");
		s[L"arr"].T()[L"werk"].Set<Dcb::Float>(6);
		s[L"arr"].T().Add<Dcb::Array>(L"meta");
		s[L"arr"].T()[L"meta"].Set<Dcb::Array>(6);
		s[L"arr"].T()[L"meta"].T().Set<Dcb::Matrix>(4);
		s[L"arr"].T().Add<Dcb::Bool>(L"booler");
		Dcb::Buffer b(s);

		{
			float exp = 420.0f;
			b[L"butts"][L"dank"] = exp;
			float act = b[L"butts"][L"dank"];
			assert(act == exp);
		}
		{
			float exp = 111.0f;
			b[L"arr"][2][L"werk"][5] = exp;
			float act = b[L"arr"][2][L"werk"][5];
			assert(act == exp);
		}
		{
			XMFLOAT3 exp = XMFLOAT3{ 69.0f, 0.0f, 0.0f };
			b[L"butts"][L"pubes"] = exp;
			XMFLOAT3 act = b[L"butts"][L"pubes"];
			assert(!std::memcmp(&exp, &act, sizeof(XMFLOAT3)));
		}
		{
			XMFLOAT4X4 exp;
			XMStoreFloat4x4(&exp, XMMatrixIdentity());
			b[L"arr"][2][L"meta"][5][3] = exp;
			XMFLOAT4X4 act = b[L"arr"][2][L"meta"][5][3];
			assert(!std::memcmp(&exp, &act, sizeof(XMFLOAT4X4)));
		}
		{
			bool exp = true;
			b[L"arr"][2][L"booler"] = exp;
			bool act = b[L"arr"][2][L"booler"];
			assert(act == exp);
		}
		{
			bool exp = false;
			b[L"arr"][2][L"booler"] = exp;
			bool act = b[L"arr"][2][L"booler"];
			assert(act == exp);
		}
	}
	// size test array of arrays
	{
		Dcb::Layout s;
		s.Add<Dcb::Array>(L"arr");
		s[L"arr"].Set<Dcb::Array>(6);
		s[L"arr"].T().Set<Dcb::Matrix>(4);
		Dcb::Buffer b(s);

		size_t act = b.GetSizeInBytes();
		assert(act == 16u * 4u * 4u * 6u);
	}
	// size test array of structs with padding
	{
		Dcb::Layout s;
		s.Add<Dcb::Array>(L"arr");
		s[L"arr"].Set<Dcb::Struct>(6);
		s[L"arr"].T().Add<Dcb::Float2>(L"a");
		s[L"arr"].T().Add<Dcb::Float3>(L"b");
		Dcb::Buffer b(s);

		size_t act = b.GetSizeInBytes();
		assert(act == 16u * 2u * 6u);
	}
	// size test array of primitive that needs padding
	{
		Dcb::Layout s;
		s.Add<Dcb::Array>(L"arr");
		s[L"arr"].Set<Dcb::Float3>(6);
		Dcb::Buffer b(s);

		size_t act = b.GetSizeInBytes();
		assert(act == 16u * 6u);
	}
}

App::App(const std::wstring& commandLine)
    : mCommandLine(commandLine), mWnd(1280, 720, L"The Donkey Fart Box"), mScriptCommander(ChiliUtil::TokenizeQuoted(commandLine)), mLight(mWnd.Gfx())
{
	TestDynamicConstant();
	//mBluePlane.SetPosition(mCamera.GetPosition());
	//mRedPlane.SetPosition(mCamera.GetPosition());
	//mWall.SetRootTransform(XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//mPlane.SetPosition({ 12.0f, 0.0f, 0.0f });
	//mGobber.SetRootTransform(XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	//mNano.SetRootTransform(XMMatrixTranslation(0.0f, -7.0f, 6.0f));
}

App::~App()
{
}

int App::Go()
{
	std::optional<int> exitcode;
	while (!(exitcode = Window::ProcessMessages()))
	{
		const float dt = mTimer.Mark();
		HandleInput(dt);
		DoFrame(dt);
	}
	return *exitcode;
}

void App::HandleInput(float dt)
{
	while (const auto e = mWnd.Keyboard.ReadKey())
	{
		if (!e->IsPress())
			continue;

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_INSERT:
			if (mWnd.CursorEnabled())
			{
				mWnd.DisableCursor();
				mWnd.Mouse.EnableRaw();
			}
			else
			{
				mWnd.EnableCursor();
				mWnd.Mouse.DisableRaw();
			}
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

	if (!mWnd.CursorEnabled())
	{
		if (mWnd.Keyboard.KeyIsPressed('W'))
			mCamera.Translate({ 0.0f, 0.0f, dt });
		if (mWnd.Keyboard.KeyIsPressed('A'))
			mCamera.Translate({ -dt, 0.0f, 0.0f });
		if (mWnd.Keyboard.KeyIsPressed('S'))
			mCamera.Translate({ 0.0f, 0.0f, -dt });
		if (mWnd.Keyboard.KeyIsPressed('D'))
			mCamera.Translate({ dt, 0.0f, 0.0f });
		if (mWnd.Keyboard.KeyIsPressed('E'))
			mCamera.Translate({ 0.0f, dt, 0.0f });
		if (mWnd.Keyboard.KeyIsPressed('Q'))
			mCamera.Translate({ 0.0f, -dt, 0.0f });
	}

	while (const auto delta = mWnd.Mouse.ReadRawDelta())
	{
		if (!mWnd.CursorEnabled())
			mCamera.Rotate(static_cast<float>(delta->X), static_cast<float>(delta->Y));
	}
}

void App::DoFrame(float dt)
{
	Graphics& gfx = mWnd.Gfx();

	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(mCamera.GetMatrix());

	mLight.SetCameraPosition(mCamera.GetPosition());
	mLight.Bind(gfx);
	
	//mWall.Draw(gfx);
	//mPlane.Draw(gfx);
	//mNano.Draw(gfx);
	//mGobber.Draw(gfx);
	mLight.Draw(gfx);
	mSponza.Draw(gfx);
	//mBluePlane.Draw(gfx);
	//mRedPlane.Draw(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui(gfx);
	
	gfx.EndFrame();
}

void App::DoImGui(const Graphics& gfx) noexcept
{
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	//mWall.ShowWindow(gfx, "Wall");
	//mPlane.SpawnControlWindow(gfx);
	//mNano.ShowWindow(gfx, "Nano");
	//mGobber.ShowWindow(gfx, "Gobber");
	mSponza.ShowWindow(gfx, "Sponza");
	//mBluePlane.SpawnControlWindow(gfx, "Blue Plane");
	//mRedPlane.SpawnControlWindow(gfx, "Red Plane");
}
