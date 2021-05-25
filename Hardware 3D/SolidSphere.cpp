#include "SolidSphere.h"

#include "BindableCommon.h"
#include "Sphere.h"

using namespace Bind;
using namespace DirectX;

SolidSphere::SolidSphere(const Graphics& gfx, float radius)
	: mRadius(radius)
{
	struct Vertex
	{
		XMFLOAT3 Position;
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	IndexedTriangleList<Vertex> model = Sphere::Make<Vertex>();
	model.Transform(XMMatrixScaling(mRadius, mRadius, mRadius));

	AddBind(std::make_shared<PixelShader>(gfx, L"SolidPS.cso"));
	AddBind(std::make_shared<VertexBuffer>(gfx, model.Vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, model.Indices));
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	std::shared_ptr<VertexShader> pVS = std::make_shared<VertexShader>(gfx, L"SolidVS.cso");
	AddBind(std::make_shared<InputLayout>(gfx, ied, pVS->GetByteCode()));
	AddBind(std::move(pVS));

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<PixelConstantBuffer<Material>>(gfx, mMaterial));
}

void SolidSphere::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	mPosition = position;
}

void SolidSphere::SetMaterial(const Graphics& gfx, const Drawable::Material& material) noexcept
{
	mMaterial = material;

	auto pMaterialBuf = QueryBindable<PixelConstantBuffer<Material>>();
	assert(pMaterialBuf != nullptr);
	pMaterialBuf->Update(gfx, mMaterial);
}

XMMATRIX SolidSphere::GetTransformMatrix() const noexcept
{
	return XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}
