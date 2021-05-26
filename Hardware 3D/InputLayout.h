#pragma once

#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout(const Graphics& gfx, Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderByteCode);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Bindable> Resolve(const Graphics& gfx, Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderByteCode);
		static std::wstring GenerateUID(Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderByteCode = nullptr);
		std::wstring GetUID() const noexcept override;

	private:
		Dvtx::VertexLayout mLayout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	};
}

