#include "Sheet.h"

#include "BindableBase.h"
#include "Plane.h"
#include "Sampler.h"
#include "Surface.h"
#include "Texture.h"

using namespace DirectX;

Sheet::Sheet(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, const DirectX::XMFLOAT3& color)
	: TestObject(gfx, rng, adist, ddist, odist, rdist), mColor(color)
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<MaterialCBuf>(gfx, *this));
}

Drawable::Material Sheet::GetMaterial() const noexcept
{
	return { mColor };
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

	AddRequiredStaticBindings(gfx, L"TextureVS.cso", L"TexturePS.cso", ied, model);

	AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile(L"Images/kappa50.png")));
	AddStaticBind(std::make_unique<Sampler>(gfx));
}
