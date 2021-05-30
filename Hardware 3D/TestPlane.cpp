#include "TestPlane.h"

#include "BindableCommon.h"
#include "Plane.h"

using namespace Bind;
using namespace DirectX;

TestPlane::TestPlane(const Graphics& gfx, float size)
{
	IndexedTriangleList model = Plane::Make();
	model.Transform(XMMatrixScaling(size, size, 1.0f));
	const std::wstring geoTag = L"$plane." + std::to_wstring(size);
	
	AddBind(VertexBuffer::Resolve(gfx, geoTag, model.Vertices));
	AddBind(IndexBuffer::Resolve(gfx, geoTag, model.Indices));

	AddBind(PixelShader::Resolve(gfx, L"PhongPS.cso"));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(PixelConstantBuffer<Material>::Resolve(gfx, Material{}));

	std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVS.cso");
	AddBind(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	AddBind(std::move(pVS));

	AddBind(Texture::Resolve(gfx, L"Images/brickwall.jpg"));
	AddBind(Sampler::Resolve(gfx));

	AddBind(std::make_shared<TransformCBuf>(gfx, *this));
}

void XM_CALLCONV TestPlane::SetPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat3(&mPosition, position);
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	mRoll = roll;
	mPitch = pitch;
	mYaw = yaw;
}

XMMATRIX XM_CALLCONV TestPlane::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll) * XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}
