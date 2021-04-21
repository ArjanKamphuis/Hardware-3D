#include "Box.h"

#include "BindableBase.h"
#include "Cube.h"

using namespace DirectX;

Box::Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, const DirectX::XMFLOAT3& color)
	: TestObject(gfx, rng, adist, ddist, odist, rdist), mScale(1.0f, 1.0f, bdist(rng)), mColor(color)
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<MaterialCBuf>(gfx, *this));
}

XMMATRIX Box::GetTransformMatrix() const noexcept
{
	return XMMatrixScalingFromVector(XMLoadFloat3(&mScale)) * TestObject::GetTransformMatrix();
}

Drawable::Material Box::GetMaterial() const noexcept
{
	return { mColor };
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
