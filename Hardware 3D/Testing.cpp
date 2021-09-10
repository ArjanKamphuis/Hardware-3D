#include "Testing.h"

#include "ChiliXM.h"
#include "DynamicConstantBuffer.h"
#include "LayoutCodex.h"
#include "Material.h"
#include "Mesh.h"
#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace DirectX;
using namespace std::string_literals;

void TestDynamicConstant()
{
	using namespace std::string_literals;
	// data roundtrip tests
	{
		Dcb::RawLayout s;
		s.Add(Dcb::Type::Struct, L"butts"s);
		s[L"butts"s].Add(Dcb::Type::Float3, L"pubes"s).Add(Dcb::Type::Float, L"dank"s);
		s.Add(Dcb::Type::Float, L"woot"s).Add(Dcb::Type::Array, L"arr"s);
		s[L"arr"s].Set(Dcb::Type::Struct, 4);
		s[L"arr"s].T().Add(Dcb::Type::Float3, L"twerk"s);
		s[L"arr"s].T().Add(Dcb::Type::Array, L"werk"s);
		s[L"arr"s].T()[L"werk"s].Set(Dcb::Type::Float, 6);
		s[L"arr"s].T().Add(Dcb::Type::Array, L"meta"s);
		s[L"arr"s].T()[L"meta"s].Set(Dcb::Type::Array, 6);
		s[L"arr"s].T()[L"meta"s].T().Set(Dcb::Type::Matrix, 4);
		s[L"arr"s].T().Add(Dcb::Type::Bool, L"booler"s);

		//s.Add(Dcb::Type::Bool, L"arr"s);
		//s.Add(Dcb::Type::Bool, L"69man"s);

		Dcb::Buffer b = Dcb::Buffer(std::move(s));
		//b[L"woot"s] = L"#"s;
		const std::wstring sig = b.GetLayoutRootElement().GetSignature();

		{
			float exp = 42.0f;
			b[L"woot"s] = exp;
			float act = b[L"woot"s];
			assert(act == exp);
		}
		{
			float exp = 420.0f;
			b[L"butts"s][L"dank"s] = exp;
			float act = b[L"butts"s][L"dank"s];
			assert(act == exp);
		}
		{
			float exp = 111.0f;
			b[L"arr"s][2][L"werk"s][5] = exp;
			float act = b[L"arr"s][2][L"werk"s][5];
			assert(act == exp);
		}
		{
			XMFLOAT3 exp = XMFLOAT3{ 69.0f, 0.0f, 0.0f };
			b[L"butts"s][L"pubes"s] = exp;
			XMFLOAT3 act = b[L"butts"s][L"pubes"s];
			assert(!std::memcmp(&exp, &act, sizeof(XMFLOAT3)));
		}
		{
			XMFLOAT4X4 exp;
			XMStoreFloat4x4(&exp, XMMatrixIdentity());
			b[L"arr"s][2][L"meta"s][5][3] = exp;
			XMFLOAT4X4 act = b[L"arr"s][2][L"meta"s][5][3];
			assert(!std::memcmp(&exp, &act, sizeof(XMFLOAT4X4)));
		}
		{
			bool exp = true;
			b[L"arr"s][2][L"booler"s] = exp;
			bool act = b[L"arr"s][2][L"booler"s];
			assert(act == exp);
		}
		{
			bool exp = false;
			b[L"arr"s][2][L"booler"s] = exp;
			bool act = b[L"arr"s][2][L"booler"s];
			assert(act == exp);
		}
		{
			assert(b[L"butts"s][L"pubes"s].Exists());
			assert(!b[L"butts"s][L"fubar"s].Exists());
			if (auto ref = b[L"butts"s][L"pubes"s]; ref.Exists())
			{
				XMFLOAT3 f = ref;
				assert(f.x == 69.0f);
			}
		}
		// set if exists
		{
			assert(b[L"butts"s][L"pubes"s].SetIfExists(XMFLOAT3{ 1.0f, 2.0f, 3.0f }));
			const XMFLOAT3& f3 = static_cast<const XMFLOAT3&>(b[L"butts"s][L"pubes"s]);
			assert(f3.x == 1.0f && f3.y == 2.0f && f3.z == 3.0f);
			assert(!b[L"butts"s][L"phubar"s].SetIfExists(XMFLOAT3{ 2.0f, 2.0f, 7.0f }));
		}

		const Dcb::Buffer& cb = b;
		{
			XMFLOAT4X4 act = cb[L"arr"s][2][L"meta"s][5][3];
			assert(act._11 == 1.0f);
			//cb[L"arr"s][2][L"booler"s] = true;
			//static_cast<bool&, cb[L"arr"s][2][L"booler"s]) = true;
			//cb[L"arr"s][200];
		}
	}
	// size test array of arrays
	{
		Dcb::RawLayout s;
		s.Add(Dcb::Type::Array, L"arr"s);
		s[L"arr"s].Set(Dcb::Type::Array, 6);
		s[L"arr"s].T().Set(Dcb::Type::Matrix, 4);
		Dcb::Buffer b = Dcb::Buffer(std::move(s));

		size_t act = b.GetSizeInBytes();
		assert(act == 16u * 4u * 4u * 6u);
	}
	// size test array of structs with padding
	{
		Dcb::RawLayout s;
		s.Add(Dcb::Type::Array, L"arr"s);
		s[L"arr"s].Set(Dcb::Type::Struct, 6);
		s[L"arr"s].T().Add(Dcb::Type::Float2, L"a"s);
		s[L"arr"s].T().Add(Dcb::Type::Float3, L"b"s);
		Dcb::Buffer b = Dcb::Buffer(std::move(s));

		size_t act = b.GetSizeInBytes();
		assert(act == 16u * 2u * 6u);
	}
	// size test array of primitive that needs padding
	{
		Dcb::RawLayout s;
		s.Add(Dcb::Type::Array, L"arr"s);
		s[L"arr"s].Set(Dcb::Type::Float3, 6);
		Dcb::Buffer b = Dcb::Buffer(std::move(s));

		size_t act = b.GetSizeInBytes();
		assert(act == 16u * 6u);
	}
	// testing CookedLayout
	{
		Dcb::RawLayout s;
		s.Add(Dcb::Type::Array, L"arr"s);
		s[L"arr"s].Set(Dcb::Type::Float3, 6);
		Dcb::CookedLayout cooked = Dcb::LayoutCodex::Resolve(std::move(s));

		s.Add(Dcb::Type::Float, L"arr"s);
		//cooked[L"arr"s].Add(Dcb::Type::Float, L"buttman"s);

		Dcb::Buffer b1 = Dcb::Buffer(cooked);
		b1[L"arr"s][0] = XMFLOAT3{ 69.0f, 0.0f, 0.0f };

		Dcb::Buffer b2 = Dcb::Buffer(cooked);
		b2[L"arr"s][0] = XMFLOAT3{ 420.0f, 0.0f, 0.0f };

		assert(static_cast<XMFLOAT3>(b1[L"arr"s][0]).x == 69.0f);
		assert(static_cast<XMFLOAT3>(b2[L"arr"s][0]).x == 420.0f);
	}
	// specific testing scenario (packing error)
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add(Dcb::Type::Float3, L"MaterialColor"s);
		pscLayout.Add(Dcb::Type::Float3, L"SpecularColor"s);
		pscLayout.Add(Dcb::Type::Float, L"SpecularWeight"s);
		pscLayout.Add(Dcb::Type::Float, L"SpecularGloss"s);
		Dcb::CookedLayout cooked = Dcb::LayoutCodex::Resolve(std::move(pscLayout));
		assert(cooked.GetSizeInBytes() == 48u);
	}
	// array non-packing
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add(Dcb::Type::Array, L"arr"s);
		pscLayout[L"arr"s].Set(Dcb::Type::Float, 10u);
		Dcb::CookedLayout cooked = Dcb::LayoutCodex::Resolve(std::move(pscLayout));
		assert(cooked.GetSizeInBytes() == 160u);
	}
	// array of struct w/ padding
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add(Dcb::Type::Array, L"arr"s);
		pscLayout[L"arr"s].Set(Dcb::Type::Struct, 10u);
		pscLayout[L"arr"s].T().Add(Dcb::Type::Float3, L"x"s);
		pscLayout[L"arr"s].T().Add(Dcb::Type::Float2, L"y"s);
		Dcb::CookedLayout cooked = Dcb::LayoutCodex::Resolve(std::move(pscLayout));
		assert(cooked.GetSizeInBytes() == 320u);
	}
	// testing pointer stuff
	{
		Dcb::RawLayout s;
		s.Add(Dcb::Type::Struct, L"butts"s);
		s[L"butts"s].Add(Dcb::Type::Float3, L"pubes"s);
		s[L"butts"s].Add(Dcb::Type::Float, L"dank"s);

		Dcb::Buffer b = Dcb::Buffer(std::move(s));
		const float exp = 696969.6969f;
		b[L"butts"s][L"dank"s] = 696969.6969f;
		assert(static_cast<float&>(b[L"butts"s][L"dank"s]) == exp);
		assert(*static_cast<float*>(&b[L"butts"s][L"dank"s]) == exp);
		const float exp2 = 42.424242f;
		*static_cast<float*>(&b[L"butts"s][L"dank"s]) = exp2;
		assert(static_cast<float&>(b[L"butts"s][L"dank"s]) == exp2);
	}
	// specific testing scenario (packing error)
	{
		Dcb::RawLayout pscLayout;
		pscLayout.Add(Dcb::Type::Bool, L"NormalMapEnabled"s);
		pscLayout.Add(Dcb::Type::Bool, L"SpecularMapEnabled"s);
		pscLayout.Add(Dcb::Type::Bool, L"HasGlossMap"s);
		pscLayout.Add(Dcb::Type::Float, L"SpecularPower"s);
		pscLayout.Add(Dcb::Type::Float3, L"SpecularColor"s);
		pscLayout.Add(Dcb::Type::Float, L"SpecularMapWeight"s);
		Dcb::CookedLayout cooked = Dcb::LayoutCodex::Resolve(std::move(pscLayout));
		assert(cooked.GetSizeInBytes() == 32u);
	}
}

void TestDynamicMeshLoading()
{
	using namespace Dvtx;
	using ElementType = VertexLayout::ElementType;

	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile("Models/brick_wall/brick_wall.obj"s, 
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	VertexLayout layout = VertexLayout{}
		.Append(ElementType::Position3D)
		.Append(ElementType::Normal)
		.Append(ElementType::Tangent)
		.Append(ElementType::BiTangent)
		.Append(ElementType::Texture2D);
	VertexBuffer buffer{ std::move(layout), *pScene->mMeshes[0] };

	for (size_t i = 0; i < buffer.Size(); i++)
	{
		const XMFLOAT3 a = buffer[i].Attr<ElementType::Position3D>();
		const XMFLOAT3 b = buffer[i].Attr<ElementType::Normal>();
		const XMFLOAT3 c = buffer[i].Attr<ElementType::Tangent>();
		const XMFLOAT3 d = buffer[i].Attr<ElementType::BiTangent>();
		const XMFLOAT2 e = buffer[i].Attr<ElementType::Texture2D>();
	}
}

void TestMaterialSystemLoading(const Graphics& gfx)
{
	std::string path{ "Models/brick_wall/brick_wall.obj"s };
	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	Material mat{ gfx, *pScene->mMaterials[1], path };
	Mesh mesh{ gfx, mat, *pScene->mMeshes[0] };
}

void TestScaleMatrixTranslation()
{
	using namespace ChiliXM;
	XMMATRIX translation = XMMatrixTranslation(20.0f, 30.0f, 40.0f);
	translation = ScaleTranslation(translation, 0.1f);
	XMFLOAT3 f3;
	XMStoreFloat3(&f3, ExtractTranslation(translation));
	assert(f3.x == 2.0f && f3.y == 3.0f && f3.z == 4.0f);
}
