#include "SkinnedBox.h"

#include "BindableBase.h"
#include "Cube.h"
#include "GraphicsThrowMacros.h"
#include "Sampler.h"
#include "Surface.h"
#include "Texture.h"

using namespace DirectX;

SkinnedBox::SkinnedBox(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	: TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void SkinnedBox::StaticInitialize(const Graphics& gfx)
{
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
	};

	struct PSMaterialConstant
	{
		float SpecularIntensity = 0.6f;
		float SpecularPower = 30.0f;
		float Padding[2] = {};
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0u, 24u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	IndexedTriangleList<Vertex> model = Cube::MakeIndependentTextured<Vertex>();
	model.SetNormalsIndependentFlat();

	AddRequiredStaticBindings(gfx, L"TexturedPhongVS.cso", L"TexturedPhongPS.cso", ied, model);
	AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, PSMaterialConstant{}, 0u));

	AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile(L"Images/kappa50.png")));
	AddStaticBind(std::make_unique<Sampler>(gfx));
}
