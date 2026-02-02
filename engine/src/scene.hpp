// scene.hpp

#pragma once

#include "object.hpp"

namespace triton::ecs
{
	class cScene : public iObject
	{
		// TODO: Create component storages for each component type
		// cComponentStorage<sTransform> _transforms;
		// ...
		// cComponentStorage<sHealth> _healths;

	public:
		explicit cScene(cContext* context);
		virtual ~cScene() override final = default;
	};
}