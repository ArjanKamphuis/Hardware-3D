#include "TexturePreprocessor.h"

#include <filesystem>
#include <sstream>
#include "ChiliUtil.h"
#include "Mesh.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace ChiliUtil;
using Color = Surface::Color;

void TexturePreprocessor::RotateXAxis180(const std::wstring& pathIn)
{
	return RotateXAxis180(pathIn, pathIn);
}

void TexturePreprocessor::RotateXAxis180(const std::wstring& pathIn, const std::wstring& pathOut)
{
	const XMMATRIX rotation = XMMatrixRotationX(XM_PI);
	const auto ProcessNormal = [rotation](FXMVECTOR n, UINT x, UINT y) -> XMVECTOR
	{
		return XMVector3Transform(n, rotation);
	};
	TransformFile(pathIn, pathOut, ProcessNormal);
}

void TexturePreprocessor::FlipYAllNormalMapsInObj(const std::wstring& objPath)
{
	const std::wstring rootPath = std::filesystem::path{ objPath }.parent_path().generic_wstring() + L"/";

	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(ToNarrow(objPath).c_str(), 0u);
	if (pScene == nullptr)
		throw Model::Exception(__LINE__, __FILEW__, imp.GetErrorString());

	for (UINT i = 0u; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial& mat = *pScene->mMaterials[i];
		aiString texFileName;
		if (mat.GetTexture(aiTextureType_NORMALS, 0u, &texFileName) == aiReturn_SUCCESS)
		{
			const std::wstring path = rootPath + ToWide(texFileName.C_Str());
			FlipYNormalMap(path, path);
		}
	}
}

void TexturePreprocessor::FlipYNormalMap(const std::wstring& pathIn, const std::wstring& pathOut)
{
	const XMVECTOR flipY = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	const auto ProcessNormal = [flipY](FXMVECTOR n, UINT x, UINT y) -> XMVECTOR
	{
		return XMVectorMultiply(n, flipY);
	};

	TransformFile(pathIn, pathOut, ProcessNormal);
}

void TexturePreprocessor::ValidateNormalMap(const std::wstring& pathIn, float thresholdMin, float thresholdMax)
{
	OutputDebugString((L"Validating normal map [" + pathIn + L"]\n").c_str());

	XMVECTOR sum = XMVectorZero();
	const auto ProcessNormal = [thresholdMin, thresholdMax, &sum](FXMVECTOR n, UINT x, UINT y) -> XMVECTOR
	{
		const float length = XMVectorGetX(XMVector3Length(n));
		const float z = XMVectorGetZ(n);
		if (length < thresholdMin || length > thresholdMax)
		{
			XMFLOAT3 vec;
			XMStoreFloat3(&vec, n);
			std::wostringstream oss;
			oss << L"Bad normal length: " << length << L" at: (" << x << L"," << y << L") normal: (" << vec.x << L"," << vec.y << L"," << vec.z << L")\n";
			OutputDebugString(oss.str().c_str());
		}
		if (z < 0.0f)
		{
			XMFLOAT3 vec;
			XMStoreFloat3(&vec, n);
			std::wostringstream oss;
			oss << L"Bad normal Z direction at: (" << x << L"," << y << L") normal: (" << vec.x << L"," << vec.y << L"," << vec.z << L")\n";
			OutputDebugString(oss.str().c_str());
		}
		sum = XMVectorAdd(sum, n);
		return n;
	};

	Surface surface = Surface::FromFile(pathIn);
	TransformSurface(surface, ProcessNormal);

	XMFLOAT2 sumv;
	XMStoreFloat2(&sumv, sum);
	std::wostringstream oss;
	oss << L"Normal map biases: (" << sumv.x << L"," << sumv.y << L")\n";
	OutputDebugString(oss.str().c_str());
}

void TexturePreprocessor::MakeStripes(const std::wstring& pathOut, size_t size, size_t stripeWidth)
{
	double power = log2(size);
	assert(modf(power, &power) == 0.0);
	assert(stripeWidth < size / 2);

	Surface s(size, size);
	const Color white{ UINT32_MAX };
	const Color color{ 0xFFFFD700 };
	for (size_t y = 0; y < size; y++)
		for (size_t x = 0; x < size; x++)
			s.PutPixel(x, y, (x / stripeWidth) % 2 == 0 ? color : white);

	s.Save(pathOut);
}

// Map Color[0,1] to Normal[-1,-1]
XMVECTOR XM_CALLCONV TexturePreprocessor::MapColorToNormal(Color c)
{
	return XMVectorSubtract(XMVectorMultiply(XMLoadColor(&c), XMVectorReplicate(2.0f)), XMVectorReplicate(1.0f));
}

//Map Normal[-1,1] to Color[0,1]
Color XM_CALLCONV TexturePreprocessor::MapNormalToColor(FXMVECTOR n)
{
	XMCOLOR xmc;
	XMStoreColor(&xmc, XMVectorDivide(XMVectorAdd(n, XMVectorReplicate(1.0f)), XMVectorReplicate(2.0f)));
	return xmc;
}
