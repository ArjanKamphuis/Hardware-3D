#include "Pyramid.h"

#include <array>
#include "BindableBase.h"
#include "Cone.h"

using namespace DirectX;

Pyramid::Pyramid(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_int_distribution<int>& tdist)
	: TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
		StaticInitialize(gfx);

	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		std::array<UCHAR, 4> Color;
	};

	const int tesselation = tdist(rng);
	IndexedTriangleList<Vertex> model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);
	model.Transform(XMMatrixScaling(1.0f, 1.0f, 0.7f));
	model.SetNormalsIndependentFlat();

	for (Vertex& v : model.Vertices)
		v.Color = { static_cast<UCHAR>(10), static_cast<UCHAR>(10), static_cast<UCHAR>(255) };
	for (size_t i = 0; i < tesselation; i++)
		model.Vertices[i * 3].Color = { static_cast<UCHAR>(255), static_cast<UCHAR>(10), static_cast<UCHAR>(10) };

	AddBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Pyramid::StaticInitialize(const Graphics& gfx)
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "COLOR", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 0u, 24u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"BlendedPhongVS.cso");
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddStaticBind(std::move(vs));

	AddStaticBind(std::make_unique<PixelShader>(gfx, L"BlendedPhongPS.cso"));
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddStaticBind(std::make_unique<MaterialCBuf>(gfx, *this));
}
