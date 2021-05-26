#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer(const Graphics& gfx, const Dvtx::VertexBuffer& vbuf);
		VertexBuffer(const Graphics& gfx, const std::wstring& tag, const Dvtx::VertexBuffer& vbuf);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<VertexBuffer> Resolve(const Graphics& gfx, const std::wstring& tag, const Dvtx::VertexBuffer& vbuf);
		template<typename ...Ignore>
		static std::wstring GenerateUID(const std::wstring& tag, Ignore&& ...ignore)
		{
			return GenerateUID_(tag);
		}
		std::wstring GetUID() const noexcept override;

	private:
		static std::wstring GenerateUID_(const std::wstring& tag);

	private:
		std::wstring mTag;
		UINT mStride;
		UINT mOffset = 0u;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
	};
}
