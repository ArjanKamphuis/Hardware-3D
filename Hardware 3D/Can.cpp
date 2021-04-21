#include "Can.h"

#include "BindableBase.h"
#include "Cylinder.h"

using namespace DirectX;

Can::Can(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, std::uniform_int_distribution<int>& tdist)
	: TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
		StaticInitialize(gfx);

	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
	};

	const IndexedTriangleList<Vertex> model = Cylinder::MakeTesselatedIndependentCapNormals<Vertex>(tdist(rng));
	AddBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Can::StaticInitialize(const Graphics& gfx)
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	struct PSMaterialConstant
	{
		XMFLOAT3A Colors[6] =
		{
			{ 1.0f, 0.0f, 0.0f},
			{ 0.0f, 1.0f, 0.0f},
			{ 0.0f, 0.0f, 1.0f},
			{ 1.0f, 1.0f, 0.0f},
			{ 1.0f, 0.0f, 1.0f},
			{ 0.0f, 1.0f, 1.0f},
		};
		float SpecularIntensity = 0.6f;
		float SpecularPower = 30.0f;
	};

	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddStaticBind(std::move(vs));

	AddStaticBind(std::make_unique<PixelShader>(gfx, L"IndexedPhongPS.cso"));
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, PSMaterialConstant{}, 0u));
}
