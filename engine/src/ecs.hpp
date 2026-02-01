#pragma once

#include <type_traits>
#include "object.hpp"
#include "cache.hpp"
#include "engine.hpp"
#include "application.hpp"
#include "types.hpp"

namespace triton
{
	class cEntity : public iObject
	{
		cHashTable<cTag> _componentIndices;
	};

	struct sComponent {};

	template <typename T>
	class cComponentStorage : public iObject
	{
		static_assert(std::is_base_of_v<sComponent, T>, "T must inherit from sComponent");

		cHashTable<T>* _components = nullptr;

	public:
		explicit cComponentStorage(cContext* context);
		virtual ~cComponentStorage() override final = default;

		void Add(const T&& component);
		T* Get(types::u32 index);
		void Remove(types::u32 index);
	};

	template <typename T>
	class cSystem : public iObject
	{
	public:
		explicit cSystem(cContext* context);
		virtual ~cSystem() override final = default;

		virtual void Init(const cComponentStorage<T>& storage) = 0;
		virtual void Update(const cComponentStorage<T>& storage) = 0;
		virtual void Shutdown(const cComponentStorage<T>& storage) = 0;
	};

	template <typename T>
	cComponentStorage<T>::cComponentStorage(cContext* context)
	{
		const sCapabilities* caps = _context->GetSubsystem<cEngine>()->GetApplication()->GetCapabilities();

		sChunkAllocatorDescriptor cad = {};
		cad.chunkByteSize = caps->hashTableChunkByteSize;
		cad.maxChunkCount = caps->hashTableMaxChunkCount;
		cad.hashTableSize = caps->hashTableSize;
		_components = _context->Create<cHashTable<T>>(_context, cad);
	}

	template <typename T>
	cComponentStorage<T>::~cComponentStorage()
	{
		_context->Destroy<cHashTable<T>>(_components);
	}

	template <typename T>
	void cComponentStorage<T>::Add(const T&& component)
	{
		_components->Insert(cIdentifier::Generate(T::GetTypeStatic()), component);
	}

	template <typename T>
	T* cComponentStorage<T>::Get(types::u32 index)
	{
		return _components->Find(index);
	}

	template <typename T>
	void cComponentStorage<T>::Remove(types::u32 index)
	{
		return _components->Erase(index);
	}

	template <typename T>
	cSystem<T>::cSystem(cContext* context) : iObject(context) {}
}