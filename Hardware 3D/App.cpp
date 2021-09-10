#include "App.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "imgui/imgui.h"

#include "ChiliUtil.h"
#include "ChiliXM.h"
#include "DynamicConstantBuffer.h"
#include "LayoutCodex.h"
#include "Material.h"
#include "Mesh.h"
#include "ModelProbe.h"
#include "Node.h"
#include "Testing.h"

using namespace ChiliXM;
using namespace DirectX;
using namespace std::string_literals;

App::App(const std::wstring& commandLine)
    : mCommandLine(commandLine), mWnd(1280, 720, L"The Donkey Fart Box"), mScriptCommander(ChiliUtil::TokenizeQuoted(commandLine)), mLight(mWnd.Gfx())
{
	//TestScaleMatrixTranslation();

	//mCube.SetPosition({ 4.0f, 0.0f, 0.0f });
	//mCube2.SetPosition({ 0.0f, 4.0f, 0.0f });
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
	//mGobber.Submit(mFrameCommander);
	mLight.Submit(mFrameCommander);
	//mCube.Submit(mFrameCommander);
	mSponza.Submit(mFrameCommander);
	//mBluePlane.Draw(gfx);
	//mRedPlane.Draw(gfx);
	//mCube2.Submit(mFrameCommander);

	mFrameCommander.Execute(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui(gfx);
	
	gfx.EndFrame();
	mFrameCommander.Reset();
}

void App::DoImGui(const Graphics& gfx) noexcept
{
	class TP : public TechniqueProbe
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
			if (auto r = buffer[L"Offset"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Offset"), &r, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
			if (auto r = buffer[L"MaterialColor"s]; r.Exists())
				dcheck(ImGui::ColorPicker3(tag("Mat. Color"), reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(r))));
			if (auto r = buffer[L"SpecularColor"s]; r.Exists())
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(r))));
			if (auto r = buffer[L"SpecularGloss"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &r, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
			if (auto r = buffer[L"SpecularWeight"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &r, 1.0f, 2.0f));
			if (auto r = buffer[L"UseSpecularMap"s]; r.Exists())
				dcheck(ImGui::Checkbox(tag("Spec. Map Enabled"), &r));
			if (auto r = buffer[L"UseNormalMap"s]; r.Exists())
				dcheck(ImGui::Checkbox(tag("Normal Map Enabled"), &r));
			if (auto r = buffer[L"NormalMapWeight"s]; r.Exists())
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &r, 0.0f, 2.0f));

			return dirty;
		}
	};

	class MP : public ModelProbe
	{
	private:
		struct TransformParameters
		{
			DirectX::XMFLOAT3 Position = {};
			DirectX::XMFLOAT3 Orientation = {};
		};
	public:
		void SpawnWindow(Model& model)
		{
			ImGui::Begin("Model");
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if (mSelectedNode != nullptr)
			{
				bool dirty = false;
				const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };
				TransformParameters& tf = ResolveTransform();

				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Translation");
				dcheck(ImGui::SliderFloat("X", &tf.Position.x, -60.0f, 60.0f));
				dcheck(ImGui::SliderFloat("Y", &tf.Position.y, -60.0f, 60.0f));
				dcheck(ImGui::SliderFloat("Z", &tf.Position.z, -60.0f, 60.0f));
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Orientation");
				dcheck(ImGui::SliderAngle("X-rotation", &tf.Orientation.x, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Y-rotation", &tf.Orientation.y, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Z-rotation", &tf.Orientation.z, -180.0f, 180.0f));

				if (dirty)
					mSelectedNode->SetAppliedTransform(
						XMMatrixRotationX(tf.Orientation.x) * XMMatrixRotationY(tf.Orientation.y) * XMMatrixRotationZ(tf.Orientation.z) * 
						XMMatrixTranslation(tf.Position.x, tf.Position.y, tf.Position.z));

				TP probe;
				mSelectedNode->Accept(probe);
			}
			ImGui::End();
		}
		bool PushNode(Node& node) override
		{
			const int selectedId = mSelectedNode == nullptr ? -1 : mSelectedNode->GetId();
			const int nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			const bool expanded = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(node.GetId())), nodeFlags, node.GetName().c_str());

			if (ImGui::IsItemClicked())
			{
				struct HighlightProbe : public TechniqueProbe
				{
				protected:
					void OnSetTechnique() override
					{
						if (mTechnique->GetName() == L"Outline"s)
							mTechnique->SetActiveState(Highlighted);
					}
				public:
					bool Highlighted = false;
				} probe;

				if (mSelectedNode != nullptr)
					mSelectedNode->Accept(probe);

				probe.Highlighted = true;
				node.Accept(probe);

				mSelectedNode = &node;
			}

			return expanded;
		}
		void PopNode(Node& node) override
		{
			ImGui::TreePop();
		}
	private:
		TransformParameters& ResolveTransform() noexcept
		{
			const int id = mSelectedNode->GetId();
			auto it = mTransformParameters.find(id);
			return it == mTransformParameters.end() ? LoadTransform(id) : it->second;
		}
		TransformParameters& LoadTransform(int id) noexcept
		{
			const XMMATRIX& applied = mSelectedNode->GetAppliedTransform();
			XMFLOAT3 angles, translation;
			XMStoreFloat3(&angles, ChiliXM::ExtractEulerAngles(applied));
			XMStoreFloat3(&translation, ChiliXM::ExtractTranslation(applied));
			return mTransformParameters.insert({ id, { translation,  angles } }).first->second;
		}
	private:
		Node* mSelectedNode = nullptr;
		std::unordered_map<int, TransformParameters> mTransformParameters;
	};
	static MP modelProbe;

	modelProbe.SpawnWindow(mSponza);
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
