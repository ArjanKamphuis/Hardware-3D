#pragma once

#include "GraphicsResource.h"

class Drawable;
class TechniqueProbe;

namespace Bind
{
	class Bindable : public GraphicsResource
	{
	public:
		virtual ~Bindable() = default;
		virtual void InitializeParentReference(const Drawable& parent) noexcept;
		virtual void Accept(TechniqueProbe&);
		virtual void Bind(const Graphics& gfx) noexcept = 0;
		virtual std::wstring GetUID() const noexcept;
	};

	class CloningBindable : public Bindable
	{
	public:
		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
}
