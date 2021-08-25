#include "TestCube.h"

#include "BindableCommon.h"
#include "Cube.h"
#include "imgui/imgui.h"
#include "TransformCBufDouble.h"

using namespace Bind;
using namespace DirectX;

TestCube::TestCube(const Graphics& gfx, float size)
{
	IndexedTriangleList model = Cube::MakeIndependentTextured();
	model.Transform(XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const std::wstring geoTag = L"$cube." + std::to_wstring(size);

	mVertexBuffer = VertexBuffer::Resolve(gfx, geoTag, model.Vertices);
	mIndexBuffer = IndexBuffer::Resolve(gfx, geoTag, model.Indices);
	mTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique standard;
		Step only(0u);

		only.AddBindable(Texture::Resolve(gfx, L"Images/brickwall.jpg"));
		only.AddBindable(Sampler::Resolve(gfx));

		std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVS.cso");
		only.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
		only.AddBindable(std::move(pVS));
		only.AddBindable(PixelShader::Resolve(gfx, L"PhongPS.cso"));

		only.AddBindable(std::make_shared<PixelConstantBuffer<Material>>(gfx, mMaterial));
		only.AddBindable(std::make_shared<TransformCBuf>(gfx));

		standard.AddStep(std::move(only));
		AddTechnique(std::move(standard));
	}

	{
		Technique outline;
		{
			Step mask(1u);

			std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso");
			mask.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
			mask.AddBindable(std::move(pVS));

			mask.AddBindable(std::make_shared<TransformCBuf>(gfx));

			outline.AddStep(std::move(mask));
		}

		{
			Step draw(2u);

			std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso");
			draw.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
			draw.AddBindable(std::move(pVS));
			draw.AddBindable(PixelShader::Resolve(gfx, L"SolidPS.cso"));

			class TransformCbufScaling : public TransformCBuf
			{
			public:
				using TransformCBuf::TransformCBuf;
				void Bind(const Graphics& gfx) noexcept override
				{
					const XMMATRIX scale = XMMatrixScaling(1.04, 1.04f, 1.04f);
					Transforms xf = GetTransforms(gfx);
					xf.World = xf.World * scale;
					xf.WVP = xf.WVP * scale;
					UpdateBindImpl(gfx, xf);
				}
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

		/*ImGui::Text("Shading");
		bool changed0 = ImGui::ColorPicker3("Spec. Color", reinterpret_cast<float*>(&mMaterial.SpecularColor));
		bool changed1 = ImGui::SliderFloat("Spec. Power", &mMaterial.SpecularPower, 0.0f, 100.0f);

		if (changed0 || changed1)
			QueryBindable<PixelConstantBuffer<Material>>()->Update(gfx, mMaterial);*/
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
