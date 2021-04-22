#include "Box.h"

#include "BindableBase.h"
#include "Cube.h"
#include "imgui/imgui.h"

using namespace DirectX;

Box::Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, const DirectX::XMFLOAT3& color)
	: TestObject(gfx, rng, adist, ddist, odist, rdist), mScale(1.0f, 1.0f, bdist(rng)), mMaterial({ color, 0.6f, 30.0f })
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<PixelConstantBuffer<Material>>(gfx, mMaterial));
}

XMMATRIX Box::GetTransformMatrix() const noexcept
{
	return XMMatrixScalingFromVector(XMLoadFloat3(&mScale)) * TestObject::GetTransformMatrix();
}

bool Box::SpawnControlWindow(const Graphics& gfx, int id) noexcept
{
	bool dirty = false;
	bool open = true;

	if (ImGui::Begin(("Box " + std::to_string(id)).c_str(), &open))
	{
		const bool cd = ImGui::ColorEdit3("Material Color", &mMaterial.Color.x);
		const bool sid = ImGui::SliderFloat("Specular Intensity", &mMaterial.SpecularIntensity, 0.05f, 4.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		const bool spd = ImGui::SliderFloat("Specular Power", &mMaterial.SpecularPower, 1.0f, 200.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		dirty = cd || sid || spd;
	}
	ImGui::End();

	if (dirty)
		SyncMaterial(gfx);

	return open;
}

void Box::StaticInitialize(const Graphics& gfx)
{
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	IndexedTriangleList<Vertex> model = Cube::MakeIndependent<Vertex>();
	model.SetNormalsIndependentFlat();

	AddRequiredStaticBindings(gfx, L"PhongVS.cso", L"PhongPS.cso", ied, model);
}

void Box::SyncMaterial(const Graphics& gfx) noexcept(!IS_DEBUG)
{
	auto pMaterialBuf = QueryBindable<PixelConstantBuffer<Material>>();
	assert(pMaterialBuf != nullptr);
	pMaterialBuf->Update(gfx, mMaterial);
}
