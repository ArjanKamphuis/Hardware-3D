#include "Box.h"

#include "BindableBase.h"
#include "Cube.h"

using namespace DirectX;

Box::Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist
	, const DirectX::XMFLOAT3& color)
	: mRadius(rdist(rng)), mAngles({ 0.0f, 0.0f, 0.0f, adist(rng), adist(rng), adist(rng) }), mDelta({ ddist(rng), ddist(rng), ddist(rng), odist(rng), odist(rng), odist(rng)})
	, mColor(color), mScale(1.0f, 1.0f, bdist(rng))
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<MaterialCBuf>(gfx, *this));
}

void Box::Update(float dt) noexcept
{
	mAngles.Update(mDelta, dt);
}

XMMATRIX Box::GetTransformMatrix() const noexcept
{
	return XMMatrixScalingFromVector(XMLoadFloat3(&mScale)) * XMMatrixRotationRollPitchYaw(mAngles.Pitch, mAngles.Yaw, mAngles.Roll) * 
		XMMatrixTranslation(mRadius, 0.0f, 0.0f) *	XMMatrixRotationRollPitchYaw(mAngles.Theta, mAngles.Phi, mAngles.Chi);
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
