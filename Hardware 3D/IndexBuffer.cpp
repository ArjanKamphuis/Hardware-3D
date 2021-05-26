#include "IndexBuffer.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	IndexBuffer::IndexBuffer(const Graphics& gfx, const std::vector<USHORT>& indices)
		: IndexBuffer(gfx, L"?", indices)
	{
	}

	IndexBuffer::IndexBuffer(const Graphics& gfx, const std::wstring& tag, const std::vector<USHORT>& indices)
		: mTag(tag), mCount(static_cast<UINT>(indices.size()))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.ByteWidth = static_cast<UINT>(mCount * sizeof(USHORT));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA iInitData = {};
		iInitData.pSysMem = indices.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &iInitData, &mBuffer));
	}

	void IndexBuffer::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->IASetIndexBuffer(mBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return mCount;
	}

	std::shared_ptr<Bindable> IndexBuffer::Resolve(const Graphics& gfx, const std::wstring& tag, const std::vector<USHORT>& indices)
	{
		return Codex::Resolve<IndexBuffer>(gfx, tag, indices);
	}

	std::wstring IndexBuffer::GetUID() const noexcept
	{
		return GenerateUID(mTag);
	}

	std::wstring IndexBuffer::GenerateUID_(const std::wstring& tag)
	{
		const std::string name{ typeid(IndexBuffer).name() };
		return std::wstring{ name.begin(), name.end() } + L"#" + tag;
	}
}
