// engine.hpp

#pragma once

#include <unordered_map>
#include "object.hpp"
#include "types.hpp"

namespace triton
{
	class cContext;
	class iApplication;
	struct sCapabilities;

	class cEngine : public iObject
	{
		TRITON_OBJECT(cEngine)

	public:
		explicit cEngine(cContext* context, iApplication* app);
		virtual ~cEngine() override final = default;

		void Initialize();
		void Run();

		inline iApplication* GetApplication() const { return _app; }
		inline const sCapabilities* GetCapabilities() const { return _caps; }

	private:
		iApplication* _app = nullptr;
		const sCapabilities* _caps = nullptr;
	};
}