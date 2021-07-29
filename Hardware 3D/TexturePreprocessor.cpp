#include "TexturePreprocessor.h"

#include <filesystem>
#include "Mesh.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using Color = Surface::Color;

void TexturePreprocessor::RotateXAxis180(const std::wstring& pathIn)
{
	return RotateXAxis180(pathIn, pathIn);
}

void TexturePreprocessor::RotateXAxis180(const std::wstring& pathIn, const std::wstring& pathOut)
{
	const XMMATRIX rotation = XMMatrixRotationX(XM_PI);
	const auto ProcessNormal = [rotation](FXMVECTOR n) -> XMVECTOR
	{
		return XMVector3Transform(n, rotation);
	};
	TransformFile(pathIn, pathOut, ProcessNormal);
}

void TexturePreprocessor::FlipYAllNormalMapsInObj(const std::wstring& objPath)
{
	const std::wstring rootPath = std::filesystem::path{ objPath }.parent_path().generic_wstring() + L"/";

	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(std::filesystem::path{ objPath }.string(), 0u);
	if (pScene == nullptr)
		throw Model::Exception(__LINE__, __FILEW__, imp.GetErrorString());

	const XMVECTOR flipY = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	const auto ProcessNormal = [flipY](FXMVECTOR n) -> XMVECTOR
	{
		return XMVectorMultiply(n, flipY);
	};

	for (UINT i = 0u; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial& mat = *pScene->mMaterials[i];
		aiString texFileName;
		if (mat.GetTexture(aiTextureType_NORMALS, 0u, &texFileName) == aiReturn_SUCCESS)
		{
			std::string fileString = std::string(texFileName.C_Str());
			const std::wstring path = rootPath + std::wstring(fileString.begin(), fileString.end());
			TransformFile(path, path, ProcessNormal);
		}
	}
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
