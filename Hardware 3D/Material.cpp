#include "Material.h"

#include "ChiliUtil.h"
#include "ConstantBuffersEx.h"

using namespace Bind;
using namespace DirectX;
using namespace std::string_literals;
using ChiliUtil::ToWide;

Material::Material(const Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG)
	: mModelPath(path.wstring())
{
	const auto rootPath = path.parent_path().wstring() + L"/"s;
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		mName = ToWide(tempName.C_Str());
	}

	// phong technique
	{
		Technique phong{ L"Phong"s };
		Step step{ 0u };
		std::wstring shaderCode = L"Phong"s;
		aiString texFileName;

		// common (pre)
		mVertexLayout.Append(Dvtx::VertexLayout::ElementType::Position3D);
		mVertexLayout.Append(Dvtx::VertexLayout::ElementType::Normal);
		Dcb::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0u, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += L"Dif"s;
				mVertexLayout.Append(Dvtx::VertexLayout::ElementType::Texture2D);
				auto pTex = Texture::Resolve(gfx, rootPath + ToWide(texFileName.C_Str()));
				if (pTex->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += L"Msk"s;
				}
				step.AddBindable(std::move(pTex));
			}
			else
				pscLayout.Add(Dcb::Type::Float3, L"MaterialColor"s);

			step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0u, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += L"Spc"s;
				mVertexLayout.Append(Dvtx::VertexLayout::ElementType::Texture2D);
				auto pTex = Texture::Resolve(gfx, rootPath + ToWide(texFileName.C_Str()), 1u);
				hasGlossAlpha = pTex->HasAlpha();
				step.AddBindable(std::move(pTex));
				pscLayout.Add(Dcb::Type::Bool, L"UseGlossAlpha"s);
				pscLayout.Add(Dcb::Type::Bool, L"UseSpecularMap"s);
			}
			pscLayout.Add(Dcb::Type::Float3, L"SpecularColor");
			pscLayout.Add(Dcb::Type::Float, L"SpecularWeight");
			pscLayout.Add(Dcb::Type::Float, L"SpecularGloss");
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0u, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += L"Nrm"s;
				mVertexLayout.Append(Dvtx::VertexLayout::ElementType::Texture2D);
				mVertexLayout.Append(Dvtx::VertexLayout::ElementType::Tangent);
				mVertexLayout.Append(Dvtx::VertexLayout::ElementType::BiTangent);
				step.AddBindable(Texture::Resolve(gfx, rootPath + ToWide(texFileName.C_Str()), 2u));
				pscLayout.Add(Dcb::Type::Bool, L"UseNormalMap"s);
				pscLayout.Add(Dcb::Type::Float, L"NormalMapWeight"s);
			}
		}
		// common (post)
		{
			step.AddBindable(std::make_shared<TransformCBuf>(gfx));
			step.AddBindable(Blender::Resolve(gfx, false));
			auto pVS = VertexShader::Resolve(gfx, shaderCode + L"_VS.cso"s);
			step.AddBindable(InputLayout::Resolve(gfx, mVertexLayout, pVS->GetByteCode()));
			step.AddBindable(std::move(pVS));
			step.AddBindable(PixelShader::Resolve(gfx, shaderCode + L"_PS.cso"s));
			if (hasTexture) step.AddBindable(Sampler::Resolve(gfx));

			Dcb::Buffer buffer{ std::move(pscLayout) };
			if (Dcb::ElementRef r = buffer[L"MaterialColor"s]; r.Exists())
			{
				aiColor3D color;
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<XMFLOAT3&>(color);
			}
			if (Dcb::ElementRef r = buffer[L"SpecularColor"s]; r.Exists())
			{
				aiColor3D color;
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<XMFLOAT3&>(color);
			}
			if (Dcb::ElementRef r = buffer[L"SpecularGloss"]; r.Exists())
			{
				float gloss;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buffer[L"UseGlossAlpha"s].SetIfExists(hasGlossAlpha);
			buffer[L"SpecularWeight"s].SetIfExists(1.0f);
			buffer[L"UseSpecularMap"s].SetIfExists(true);
			buffer[L"UseNormalMap"s].SetIfExists(true);
			buffer[L"NormalMapWeight"s].SetIfExists(1.0f);

			step.AddBindable(std::make_unique<CachingPixelConstantBufferEx>(gfx, std::move(buffer)));
		}

		phong.AddStep(std::move(step));
		mTechniques.push_back(std::move(phong));
	}
	// outline technique
	{
		Technique outline{ L"Outline"s, false };
		{
			Step mask{ 1u };
			auto pVS = VertexShader::Resolve(gfx, L"Solid_VS.cso"s);
			mask.AddBindable(InputLayout::Resolve(gfx, mVertexLayout, pVS->GetByteCode()));
			mask.AddBindable(std::move(pVS));
			mask.AddBindable(std::make_shared<TransformCBuf>(gfx));
			outline.AddStep(mask);
		}
		{
			Step draw{ 2u };
			auto pVS = VertexShader::Resolve(gfx, L"Solid_VS.cso"s);
			draw.AddBindable(InputLayout::Resolve(gfx, mVertexLayout, pVS->GetByteCode()));
			draw.AddBindable(std::move(pVS));
			draw.AddBindable(PixelShader::Resolve(gfx, L"Solid_PS.cso"s));

			Dcb::RawLayout layout;
			layout.Add(Dcb::Type::Float3, L"MaterialColor"s);
			Dcb::Buffer buffer{ std::move(layout) };
			buffer[L"MaterialColor"s] = XMFLOAT3{ 1.0f, 0.4f, 0.4f };
			draw.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buffer));

			class TransformCbufScaling : public TransformCBuf
			{
			public:
				TransformCbufScaling(const Graphics& gfx, float scale = 1.04f)
					: TransformCBuf(gfx), mBuffer(MakeLayout())
				{
					mBuffer[L"Scale"s] = scale;
				}
				void Accept(TechniqueProbe& probe) override
				{
					probe.VisitBuffer(mBuffer);
				}
				void Bind(const Graphics& gfx) noexcept override
				{
					const float scale = mBuffer[L"Scale"s];
					const XMMATRIX scaleMatrix = XMMatrixScaling(scale, scale, scale);
					Transforms xf = GetTransforms(gfx);
					xf.World = xf.World * scaleMatrix;
					xf.WVP = xf.WVP * scaleMatrix;
					UpdateBindImpl(gfx, xf);
				}
				std::unique_ptr<CloningBindable> Clone() const noexcept override
				{
					return std::make_unique<TransformCbufScaling>(*this);
				}
			private:
				static Dcb::RawLayout MakeLayout()
				{
					Dcb::RawLayout layout;
					layout.Add(Dcb::Type::Float, L"Scale"s);
					return layout;
				}
			private:
				Dcb::Buffer mBuffer;
			};

			draw.AddBindable(std::make_shared<TransformCbufScaling>(gfx));
			outline.AddStep(std::move(draw));
		}
		mTechniques.push_back(std::move(outline));
	}
}

Dvtx::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { mVertexLayout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3u);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3u);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}

std::shared_ptr<VertexBuffer> Material::MakeVertexBindable(const Graphics& gfx, const aiMesh& mesh, float scale) const noexcept(!IS_DEBUG)
{
	Dvtx::VertexBuffer vtc = ExtractVertices(mesh);
	if (scale != 1.0f)
	{
		for (size_t i = 0u; i < vtc.Size(); i++)
		{
			XMFLOAT3& position = vtc[i].Attr<Dvtx::VertexLayout::ElementType::Position3D>();
			position.x *= scale;
			position.y *= scale;
			position.z *= scale;
		}
	}
	return VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vtc));
}

std::shared_ptr<IndexBuffer> Material::MakeIndexBindable(const Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG)
{
	return IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}

std::vector<Technique> Material::GetTechniques() const noexcept
{
	return mTechniques;
}

std::wstring Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return mModelPath + L"%"s + ToWide(mesh.mName.C_Str());
}
