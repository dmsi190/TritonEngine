// system.hpp

#pragma once

#include "object.hpp"
#include "scene.hpp"

namespace triton
{
	class cContext;
	template <typename TValue>
	class cStack;
}

namespace triton::ecs
{
	class cSystem : public triton::iObject
	{
		TRITON_OBJECT(cSystem)

		explicit cSystem(triton::cContext* context);
		virtual ~cSystem() override final = default;

	public:
		void OnFrameUpdate(triton::cStack<cScene>* scenes) = 0;
	};
}