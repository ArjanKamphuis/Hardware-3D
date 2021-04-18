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
	AddBind(std::make_unique<MaterialCBuf>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept
{
}

void SolidSphere::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	mPosition = position;
}

void SolidSphere::SetMaterialColor(const DirectX::XMFLOAT3& color) noexcept
{
	mMaterial.Color = color;
}

XMMATRIX SolidSphere::GetTransformMatrix() const noexcept
{
	return XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}

Drawable::Material SolidSphere::GetMaterial() const noexcept
{
	return mMaterial;
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
