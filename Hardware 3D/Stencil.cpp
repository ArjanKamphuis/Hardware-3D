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
            dsDesc.StencilEnable = TRUE;
            dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        }
        else if (mode == Mode::Mask)
        {
            dsDesc.DepthEnable = FALSE;
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
        return ChiliUtil::ToWide(typeid(Stencil).name()) + L"#"s + (mode == Mode::Mask ? L"Mask"s : (mode == Mode::Write ? L"Write"s : (mode == Mode::Off ? L"Off"s : L"Error"));
    }

    std::wstring Stencil::GetUID() const noexcept
    {
        return GenerateUID(mMode);
    }
}
