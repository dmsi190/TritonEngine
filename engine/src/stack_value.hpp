#pragma once

#include "types.hpp"

namespace triton
{
	template <typename TValue>
	class cStack;

	class cStackValue
	{
		template <typename>
		friend class cStack;

		types::u32 chunk = 0;
		types::u32 localPosition = 0;
		types::u32 globalPosition = 0;
	};
}