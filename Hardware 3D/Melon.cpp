#include "Melon.h"

#include "BindableBase.h"
#include "Sphere.h"

using namespace DirectX;

Melon::Melon(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_int_distribution<int>& longDist, std::uniform_int_distribution<int>& latDist)
	: mRadius(rdist(rng)), mAngles({ 0.0f, 0.0f, 0.0f, adist(rng), adist(rng), adist(rng) }), mDelta({ ddist(rng), ddist(rng), ddist(rng), odist(rng), odist(rng), odist(rng) })
{
	if (!IsStaticInitialized())
		StaticInitialize(gfx);

	struct Vertex
	{
		XMFLOAT3 Position;
	};

	IndexedTriangleList<Vertex> model = Sphere::MakeTesselated<Vertex>(latDist(rng), longDist(rng));
	model.Transform(XMMatrixScaling(1.0, 1.0f, 1.2f));

	AddBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));
	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Melon::Update(float dt) noexcept
{
	mAngles.Update(mDelta, dt);
}

XMMATRIX Melon::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mAngles.Pitch, mAngles.Yaw, mAngles.Roll) * XMMatrixTranslation(mRadius, 0.0f, 0.0f) *
		XMMatrixRotationRollPitchYaw(mAngles.Theta, mAngles.Phi, mAngles.Chi) * XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}

void Melon::StaticInitialize(const Graphics& gfx)
{
	struct PixelShaderConstants
	{
		XMFLOAT4 FaceColors[8];
	};

	const PixelShaderConstants psData =
	{
		{
			{ 1.0f, 1.0f, 1.0f, 1.0f},
			{ 1.0f, 0.0f, 0.0f, 1.0f},
			{ 0.0f, 1.0f, 0.0f, 1.0f},
			{ 1.0f, 1.0f, 0.0f, 1.0f},
			{ 0.0f, 0.0f, 1.0f, 1.0f},
			{ 1.0f, 0.0f, 1.0f, 1.0f},
			{ 0.0f, 1.0f, 1.0f, 1.0f},
			{ 0.0f, 0.0f, 0.0f, 1.0f}
		}
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};


	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"ColorIndexVS.cso");
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddStaticBind(std::move(vs));

	AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPS.cso"));
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, psData));
}
