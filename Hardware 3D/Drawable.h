#pragma once

#include "Graphics.h"
#include "Technique.h"

namespace Bind
{
	class IndexBuffer;
	class InputLayout;
	class Topology;
	class VertexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	Drawable& operator=(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void AddTechnique(Technique technique) noexcept;
	void Accept(TechniqueProbe& probe);
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(const Graphics& gfx) const noexcept;
	UINT GetIndexCount() const noexcept(!IS_DEBUG);

	virtual DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept = 0;

protected:
	std::shared_ptr<Bind::IndexBuffer> mIndexBuffer;
	std::shared_ptr<Bind::Topology> mTopology;
	std::shared_ptr<Bind::VertexBuffer> mVertexBuffer;
	std::vector<Technique> mTechniques;
};
