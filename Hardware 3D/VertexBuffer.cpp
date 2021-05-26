#include "VertexBuffer.h"

#include "BindableCodex.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(const Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
		: VertexBuffer(gfx, L"?", vbuf)
	{
	}

	VertexBuffer::VertexBuffer(const Graphics& gfx, const std::wstring& tag, const Dvtx::VertexBuffer& vbuf)
		: mTag(tag), mStride(static_cast<UINT>(vbuf.GetLayout().Size()))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.ByteWidth = static_cast<UINT>(vbuf.SizeBytes());
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA vInitData = {};
		vInitData.pSysMem = vbuf.GetData();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &vInitData, &mBuffer));
	}

	void VertexBuffer::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->IASetVertexBuffers(0u, 1u, mBuffer.GetAddressOf(), &mStride, &mOffset);
	}

	std::shared_ptr<Bindable> VertexBuffer::Resolve(const Graphics& gfx, const std::wstring& tag, const Dvtx::VertexBuffer& vbuf)
	{
		return Codex::Resolve<VertexBuffer>(gfx, tag, vbuf);
	}
	std::wstring VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID(mTag);
	}
	std::wstring VertexBuffer::GenerateUID_(const std::wstring& tag)
	{
		const std::string name{ typeid(VertexBuffer).name() };
		return std::wstring{ name.begin(), name.end() } + L"#" + tag;
	}
}