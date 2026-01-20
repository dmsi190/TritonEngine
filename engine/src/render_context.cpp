// render_context.cpp

#include "render_context.hpp"

namespace triton
{
	cRenderPassGPU::cRenderPassGPU(cContext* context, cVertexArray* vertexArray, cShader* shader, cRenderTarget* renderTarget)
		: iObject(context), _vertexArray(vertexArray), _shader(shader), _renderTarget(renderTarget) {}
}