#include "Stencil.h"

#include "BindableCodex.h"
#include "ChiliUtil.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
    Stencil::Stencil(const Graphics& gfx, Mode mode)
        : mMode(mode)
    {
        INFOMAN(gfx);
        D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

        if (mode == Mode::Write)
        {
            dsDesc.DepthEnable = FALSE;
            dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            dsDesc.StencilEnable = TRUE;
            dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        }
        else if (mode == Mode::Mask)
        {
            dsDesc.DepthEnable = FALSE;
            dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            dsDesc.StencilEnable = TRUE;
            dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
        }

        GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilState(&dsDesc, &mDepthStencilState));
    }

    void Stencil::Bind(const Graphics& gfx) noexcept
    {
        GetDeviceContext(gfx)->OMSetDepthStencilState(mDepthStencilState.Get(), 0xFF);
    }

    std::shared_ptr<Stencil> Stencil::Resolve(const Graphics& gfx, Mode mode) noexcept(!IS_DEBUG)
    {
        return Codex::Resolve<Stencil>(gfx, mode);
    }

    std::wstring Stencil::GenerateUID(Mode mode) noexcept
    {
        using namespace std::string_literals;
        const auto getModeName = [mode]() {
            switch (mode) {
            case Mode::Off: return L"off"s;
            case Mode::Write: return L"write"s;
            case Mode::Mask: return L"mask"s;
            default: return L"ERROR"s;
            }
        };
        return ChiliUtil::ToWide(typeid(Stencil).name()) + L"#"s + getModeName();
    }

    std::wstring Stencil::GetUID() const noexcept
    {
        return GenerateUID(mMode);
    }
}
