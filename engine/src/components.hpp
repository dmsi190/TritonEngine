#pragma once

#include "math.hpp"

namespace triton::ecs::components
{
	struct sCameraComponent
	{
        triton::cMatrix4 _view = triton::cMatrix4(1.0f);
        triton::cMatrix4 _projection = triton::cMatrix4(1.0f);
        triton::cMatrix4 _viewProjection = triton::cMatrix4(1.0f);
        types::f32 _fov = 60.0f;
        types::f32 _zNear = 0.01f;
        types::f32 _zFar = 100.0f;
	};
}