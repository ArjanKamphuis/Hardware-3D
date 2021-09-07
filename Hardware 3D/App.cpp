#include "App.h"

#include "ChiliUtil.h"
#include "DynamicConstantBuffer.h"
#include "LayoutCodex.h"
#include "Material.h"
#include "Testing.h"

using namespace DirectX;
using namespace std::string_literals;

App::App(const std::wstring& commandLine)
    : mCommandLine(commandLine), mWnd(1280, 720, L"The Donkey Fart Box"), mScriptCommander(ChiliUtil::TokenizeQuoted(commandLine)), mLight(mWnd.Gfx())
{
	TestMaterialSystemLoading(mWnd.Gfx());
	mCube.SetPosition({ 4.0f, 0.0f, 0.0f });
	mCube2.SetPosition({ 0.0f, 4.0f, 0.0f });

	std::string path{ "Models/brick_wall/brick_wall.obj" };
	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	Material mat{ mWnd.Gfx(), *pScene->mMaterials[1], path };
	mLoadedMesh = std::make_unique<Mesh>(mWnd.Gfx(), mat, *pScene->mMeshes[0]);

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
	mLight.Submit(mFrameCommander);
	//mSponza.Draw(gfx);
	//mBluePlane.Draw(gfx);
	//mRedPlane.Draw(gfx);
	//mCube.Submit(mFrameCommander);
	//mCube2.Submit(mFrameCommander);
	mLoadedMesh->Submit(mFrameCommander, XMMatrixIdentity());

	mFrameCommander.Execute(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui(gfx);
	
	gfx.EndFrame();
	mFrameCommander.Reset();
}

void App::DoImGui(const Graphics& gfx) noexcept
{
	class Probe : public TechniqueProbe
	{
	protected:
		void OnSetTechnique() override
		{
			ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, ChiliUtil::ToNarrow(mTechnique->GetName()).c_str());
			bool active = mTechnique->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(mTechIdx)).c_str(), &active);
			mTechnique->SetActiveState(active);
		}
		bool OnVisitBuffer(Dcb::Buffer& buffer) override
		{
			float dirty = false;
			const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(mBufferIdx)](const char* label) mutable
			{
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (auto r = buffer[L"Scale"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Scale"), &r, 1.0f, 2.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
			if (auto r = buffer[L"MaterialColor"s]; r.Exists())
				dcheck(ImGui::ColorPicker3(tag("Mat. Color"), reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(r))));
			if (auto r = buffer[L"SpecularColor"s]; r.Exists())
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(r))));
			if (auto r = buffer[L"SpecularGloss"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &r, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
			if (auto r = buffer[L"SpecularWeight"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &r, 1.0f, 2.0f));
			if (auto r = buffer[L"UseNormalMap"s]; r.Exists())
				dcheck(ImGui::Checkbox(tag("Normal Map Enabled"), &r));
			if (auto r = buffer[L"NormalMapWeight"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &r, 0.0f, 2.0f));

			return dirty;
		}
	} probe;
	mLoadedMesh->Accept(probe);

	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	//mWall.ShowWindow(gfx, "Wall");
	//mPlane.SpawnControlWindow(gfx);
	//mNano.ShowWindow(gfx, "Nano");
	//mGobber.ShowWindow(gfx, "Gobber");
	//mSponza.ShowWindow(gfx, "Sponza");
	//mBluePlane.SpawnControlWindow(gfx, "Blue Plane");
	//mRedPlane.SpawnControlWindow(gfx, "Red Plane");
	//mCube.SpawnControlWindow(gfx, "Cube1");
	//mCube2.SpawnControlWindow(gfx, "Cube2");
}
