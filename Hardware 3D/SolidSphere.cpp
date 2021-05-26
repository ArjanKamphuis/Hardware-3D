#include "SolidSphere.h"

#include "BindableCommon.h"
#include "Sphere.h"

using namespace Bind;
using namespace DirectX;

SolidSphere::SolidSphere(const Graphics& gfx, float radius)
	: mRadius(radius)
{
	IndexedTriangleList model = Sphere::Make();
	model.Transform(XMMatrixScaling(mRadius, mRadius, mRadius));

	const std::wstring geoTag = L"$sphere." + std::to_wstring(mRadius);
	AddBind(VertexBuffer::Resolve(gfx, geoTag, model.Vertices));
	AddBind(IndexBuffer::Resolve(gfx, geoTag, model.Indices));

	AddBind(PixelShader::Resolve(gfx, L"SolidPS.cso"));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(PixelConstantBuffer<Material>::Resolve(gfx, mMaterial));

	std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso");
	AddBind(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	AddBind(std::move(pVS));

	AddBind(std::make_shared<TransformCBuf>(gfx, *this));
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
