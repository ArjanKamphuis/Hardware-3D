#include "TestCube.h"

#include "BindableCommon.h"
#include "ChiliUtil.h"
#include "ConstantBuffersEx.h"
#include "Cube.h"
#include "DynamicConstantBuffer.h"
#include "imgui/imgui.h"
#include "TechniqueProbe.h"
#include "TransformCBufDouble.h"

using namespace Bind;
using namespace DirectX;
using namespace std::string_literals;

TestCube::TestCube(const Graphics& gfx, float size)
{
	IndexedTriangleList model = Cube::MakeIndependentTextured();
	model.Transform(XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const std::wstring geoTag = L"$cube."s + std::to_wstring(size);

	mVertexBuffer = VertexBuffer::Resolve(gfx, geoTag, model.Vertices);
	mIndexBuffer = IndexBuffer::Resolve(gfx, geoTag, model.Indices);
	mTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique shade(L"Shade"s);
		Step only(0u);

		only.AddBindable(Texture::Resolve(gfx, L"Images/brickwall.jpg"s));
		only.AddBindable(Sampler::Resolve(gfx));

		std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVS.cso"s);
		only.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
		only.AddBindable(std::move(pVS));
		only.AddBindable(PixelShader::Resolve(gfx, L"PhongPS.cso"s));

		Dcb::RawLayout layout;
		layout.Add(Dcb::Type::Float3, L"SpecularColor"s);
		layout.Add(Dcb::Type::Float, L"SpecularPower"s);
		Dcb::Buffer buffer(std::move(layout));
		buffer[L"SpecularColor"s] = XMFLOAT3{ 0.1f, 0.1f, 0.1f };
		buffer[L"SpecularPower"s] = 20.0f;
		only.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buffer));
		only.AddBindable(std::make_shared<TransformCBuf>(gfx));

		shade.AddStep(std::move(only));
		AddTechnique(std::move(shade));
	}

	{
		Technique outline(L"Outline"s);
		{
			Step mask(1u);

			std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso"s);
			mask.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
			mask.AddBindable(std::move(pVS));

			mask.AddBindable(std::make_shared<TransformCBuf>(gfx));

			outline.AddStep(std::move(mask));
		}

		{
			Step draw(2u);

			std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso"s);
			draw.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
			draw.AddBindable(std::move(pVS));
			draw.AddBindable(PixelShader::Resolve(gfx, L"SolidPS.cso"s));

			Dcb::RawLayout layout;
			layout.Add(Dcb::Type::Float4, L"MaterialColor"s);
			Dcb::Buffer buffer(std::move(layout));
			buffer[L"MaterialColor"s] = XMFLOAT4{ 1.0f, 0.4f, 0.4f, 1.0f };
			draw.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buffer));

			class TransformCbufScaling : public TransformCBuf
			{
			public:
				TransformCbufScaling(const Graphics& gfx, float scale = 1.04f)
					: TransformCBuf(gfx), mBuffer(MakeLayout())
				{
					mBuffer[L"Scale"s] = scale;
				}
				void Accept(TechniqueProbe& probe) override
				{
					probe.VisitBuffer(mBuffer);
				}
				void Bind(const Graphics& gfx) noexcept override
				{
					const float scale = mBuffer[L"Scale"s];
					const XMMATRIX scaleMatrix = XMMatrixScaling(scale, scale, scale);
					Transforms xf = GetTransforms(gfx);
					xf.World = xf.World * scaleMatrix;
					xf.WVP = xf.WVP * scaleMatrix;
					UpdateBindImpl(gfx, xf);
				}
			private:
				static Dcb::RawLayout MakeLayout()
				{
					Dcb::RawLayout layout;
					layout.Add(Dcb::Type::Float, L"Scale"s);
					return layout;
				}
			private:
				Dcb::Buffer mBuffer;
			};

			draw.AddBindable(std::make_shared<TransformCbufScaling>(gfx));

			outline.AddStep(std::move(draw));
		}

		AddTechnique(std::move(outline));
	}
}

void TestCube::SpawnControlWindow(const Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &mPosition.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &mPosition.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &mPosition.z, -80.0f, 80.0f, "%.1f");

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &mRoll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &mPitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &mYaw, -180.0f, 180.0f);

		class Probe : public TechniqueProbe
		{
		protected:
			void OnSetTechnique() override
			{
				const std::string techName = ChiliUtil::ToNarrow(mTechnique->GetName());
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, techName.c_str());
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

				if (Dcb::ElementRef v = buffer[L"Scale"s]; v.Exists())
					dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
				if (Dcb::ElementRef v = buffer[L"MaterialColor"s]; v.Exists())
					dcheck(ImGui::ColorPicker4(tag("Mat. Color"), reinterpret_cast<float*>(&static_cast<XMFLOAT4&>(v))));
				if (Dcb::ElementRef v = buffer[L"SpecularColor"s]; v.Exists())
					dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(v))));
				if (Dcb::ElementRef v = buffer[L"SpecularPower"s]; v.Exists())
					dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));

				return dirty;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
}

void XM_CALLCONV TestCube::SetPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat3(&mPosition, position);
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	mRoll = roll;
	mPitch = pitch;
	mYaw = yaw;
}

XMMATRIX XM_CALLCONV TestCube::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll) * XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}
