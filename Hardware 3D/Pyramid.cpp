#include "Pyramid.h"

#include "BindableBase.h"
#include "Cone.h"

using namespace DirectX;

Pyramid::Pyramid(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	: mRadius(rdist(rng)), mAngles({ 0.0f, 0.0f, 0.0f, adist(rng), adist(rng), adist(rng) }), mDelta({ ddist(rng), ddist(rng), ddist(rng), odist(rng), odist(rng), odist(rng) })
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Pyramid::Update(float dt) noexcept
{
	mAngles.Update(mDelta, dt);
}

XMMATRIX Pyramid::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mAngles.Pitch, mAngles.Yaw, mAngles.Roll) * XMMatrixTranslation(mRadius, 0.0f, 0.0f) *
		XMMatrixRotationRollPitchYaw(mAngles.Theta, mAngles.Phi, mAngles.Chi) * XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}

void Pyramid::StaticInitialize(const Graphics& gfx)
{
	struct Vertex
	{
		XMFLOAT3 Position;
		struct
		{
			unsigned char R;
			unsigned char G;
			unsigned char B;
			unsigned char A;
		} Color;
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	IndexedTriangleList<Vertex> model = Cone::MakeTesselated<Vertex>(4);
	model.Transform(XMMatrixScaling(1.0f, 1.0f, 0.7f));

	model.Vertices[0].Color = { 255, 255, 0 };
	model.Vertices[1].Color = { 255, 255, 0 };
	model.Vertices[2].Color = { 255, 255, 0 };
	model.Vertices[3].Color = { 255, 255, 0 };
	model.Vertices[4].Color = { 255, 255, 80 };
	model.Vertices[5].Color = { 255, 10, 0 };

	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"ColorBlendVS.cso");
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddStaticBind(std::move(vs));

	AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorBlendPS.cso"));
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));

	AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));
}
