#include "Box.h"

#include "BindableBase.h"

using namespace DirectX;

Box::Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
	, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	: mRadius(rdist(rng)), mAngles({ 0.0f, 0.0f, 0.0f, adist(rng), adist(rng), adist(rng) }), mDelta({ ddist(rng), ddist(rng), ddist(rng), odist(rng), odist(rng), odist(rng)})
{
	struct Vertex
	{
		struct
		{
			float X;
			float Y;
			float Z;
		} Position;
	};

	const std::vector<Vertex> vertices =
	{
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }
	};

	const std::vector<USHORT> indices =
	{
		0, 2, 1,	2, 3, 1,
		1, 3, 5,	3, 7, 5,
		2, 6, 3,	3, 6, 7,
		4, 5, 7,	4, 7, 6,
		0, 4, 2,	2, 4, 6,
		0, 1, 4,	1, 5, 4
	};

	struct ColorConstantBuffer
	{
		struct
		{
			float R;
			float G;
			float B;
			float A;
		} FaceColors[6];
	};

	const ColorConstantBuffer colorData =
	{
		{
			{ 1.0f, 0.0f, 1.0f},
			{ 1.0f, 0.0f, 0.0f},
			{ 0.0f, 1.0f, 0.0f},
			{ 0.0f, 0.0f, 1.0f},
			{ 1.0f, 1.0f, 0.0f},
			{ 0.0f, 1.0f, 1.0f}
		}
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
	AddBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
	AddBind(std::move(vs));

	AddBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));
	AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_unique<IndexBuffer>(gfx, indices));
	AddBind(std::make_unique<VertexBuffer>(gfx, vertices));
	AddBind(std::make_unique<PixelConstantBuffer<ColorConstantBuffer>>(gfx, colorData));
	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
}

void Box::Update(float dt) noexcept
{
	mAngles.Update(mDelta, dt);
}

DirectX::XMMATRIX Box::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mAngles.Pitch, mAngles.Yaw, mAngles.Roll) * XMMatrixTranslation(mRadius, 0.0f, 0.0f) *
		XMMatrixRotationRollPitchYaw(mAngles.Theta, mAngles.Phi, mAngles.Chi) * XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}

void Box::AngularParameters::Update(const AngularParameters& delta, float dt)
{
	Roll = XMScalarModAngle(Roll + delta.Roll * dt);
	Pitch = XMScalarModAngle(Pitch + delta.Pitch * dt);
	Yaw = XMScalarModAngle(Yaw + delta.Yaw * dt);
	Theta = XMScalarModAngle(Theta + delta.Theta * dt);
	Phi = XMScalarModAngle(Phi + delta.Phi * dt);
	Chi = XMScalarModAngle(Chi + delta.Chi * dt);
}
