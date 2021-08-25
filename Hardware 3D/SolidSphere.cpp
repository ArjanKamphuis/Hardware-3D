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
	mVertexBuffer = VertexBuffer::Resolve(gfx, geoTag, model.Vertices);
	mIndexBuffer = IndexBuffer::Resolve(gfx, geoTag, model.Indices);
	mTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Technique solid;
	Step only(0u);

	std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso");
	only.AddBindable(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	only.AddBindable(std::move(pVS));

	only.AddBindable(PixelShader::Resolve(gfx, L"SolidPS.cso"));
	only.AddBindable(PixelConstantBuffer<Material>::Resolve(gfx, mMaterial));
	only.AddBindable(std::make_shared<TransformCBuf>(gfx));

	only.AddBindable(Blender::Resolve(gfx, false));
	only.AddBindable(Rasterizer::Resolve(gfx, false));

	solid.AddStep(std::move(only));
	AddTechnique(std::move(solid));
}

void XM_CALLCONV SolidSphere::SetPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat3(&mPosition, position);
}

void XM_CALLCONV SolidSphere::SetColor(const Graphics& gfx, FXMVECTOR color) noexcept
{
	XMStoreFloat3(&mMaterial.Color, color);
	//QueryBindable<PixelConstantBuffer<Material>>()->Update(gfx, mMaterial);
}

XMMATRIX XM_CALLCONV SolidSphere::GetTransformMatrix() const noexcept
{
	return XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}
