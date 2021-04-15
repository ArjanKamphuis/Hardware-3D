#include "SolidSphere.h"

#include "BindableBase.h"
#include "Sphere.h"

using namespace DirectX;

SolidSphere::SolidSphere(const Graphics& gfx, float radius)
	: mRadius(radius)
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept
{
}

void SolidSphere::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	mPosition = position;
}

XMMATRIX SolidSphere::GetTransformMatrix() const noexcept
{
	return XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}

void SolidSphere::StaticInitialize(const Graphics& gfx)
{
	struct Vertex
	{
		XMFLOAT3 Position;
	};

	struct PixelShaderConstants
	{
		XMFLOAT3 Color = { 1.0f, 1.0f, 1.0f };
		float Pad = 0.0f;
	} psConstant;

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	IndexedTriangleList<Vertex> model = Sphere::Make<Vertex>();
	model.Transform(XMMatrixScaling(mRadius, mRadius, mRadius));

	AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));
	AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));

	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddStaticBind(std::move(vs));

	AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidPS.cso"));
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, psConstant));
}
