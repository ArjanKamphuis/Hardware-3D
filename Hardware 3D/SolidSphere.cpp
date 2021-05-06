#include "SolidSphere.h"

#include "BindableCommon.h"
#include "Sphere.h"

using namespace Bind;
using namespace DirectX;

SolidSphere::SolidSphere(const Graphics& gfx, float radius)
	: mRadius(radius)
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<PixelConstantBuffer<Material>>(gfx, mMaterial));
}

void SolidSphere::Update(float dt) noexcept
{
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

void SolidSphere::StaticInitialize(const Graphics& gfx)
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

	AddRequiredStaticBindings(gfx, L"SolidVS.cso", L"SolidPS.cso", ied, model);
}
