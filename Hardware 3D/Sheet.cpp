#include "Sheet.h"

#include "BindableBase.h"
#include "Plane.h"
#include "Sampler.h"
#include "Surface.h"
#include "Texture.h"

using namespace DirectX;

Sheet::Sheet(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	: mRadius(rdist(rng)), mAngles({ 0.0f, 0.0f, 0.0f, adist(rng), adist(rng), adist(rng) }), mDelta({ ddist(rng), ddist(rng), ddist(rng), odist(rng), odist(rng), odist(rng) })
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Sheet::Update(float dt) noexcept
{
	mAngles.Update(mDelta, dt);
}

DirectX::XMMATRIX Sheet::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mAngles.Pitch, mAngles.Yaw, mAngles.Roll) * XMMatrixTranslation(mRadius, 0.0f, 0.0f) * XMMatrixRotationRollPitchYaw(mAngles.Theta, mAngles.Phi, mAngles.Chi);
}

void Sheet::StaticInitialize(const Graphics& gfx)
{
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT2 TexCoord;
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	IndexedTriangleList<Vertex> model = Plane::Make<Vertex>();

	model.Vertices[0].TexCoord = { 0.0f, 0.0f };
	model.Vertices[1].TexCoord = { 1.0f, 0.0f };
	model.Vertices[2].TexCoord = { 0.0f, 1.0f };
	model.Vertices[3].TexCoord = { 1.0f, 1.0f };

	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddStaticBind(std::move(vs));

	AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));

	AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));

	AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile(L"Images/kappa50.png")));
	AddStaticBind(std::make_unique<Sampler>(gfx));
}
