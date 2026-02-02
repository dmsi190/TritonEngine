#pragma once

#include <type_traits>
#include "object.hpp"
#include "engine.hpp"
#include "application.hpp"
#include "hash_table.hpp"
#include "stack.hpp"
#include "types.hpp"

namespace triton::ecs
{
	class cContext;

	using entity = types::u64;
	static constexpr entity kInvalidEntity = 0;

	struct sComponent {};

	template <typename TComponent>
	class cComponentStorage : public iObject
	{
		static_assert(std::is_base_of_v<sComponent, TComponent>, "TComponent must inherit from sComponent");

		cStack<TComponent>* _data = nullptr;
		cHashTable<entity, types::u32>* _indices = nullptr;

	public:
		explicit cComponentStorage(cContext* context);
		virtual ~cComponentStorage() override final = default;

		TComponent* Create(entity ent);
		TComponent* Get(entity ent);
		void Destroy(entity ent);
	};

	template <typename TComponent>
	cComponentStorage<TComponent>::cComponentStorage(cContext* context)
	{
		const sCapabilities* caps = _context->GetSubsystem<cEngine>()->GetApplication()->GetCapabilities();

		sChunkAllocatorDescriptor cad = {};
		cad.chunkByteSize = caps->hashTableChunkByteSize;
		cad.maxChunkCount = caps->hashTableMaxChunkCount;
		cad.hashTableSize = caps->hashTableSize;

		_data = _context->Create<cStack<TComponent>>(_context, cad);
		_indices = _context->Create<cHashTable<entity, types::u32>>(_context, cad);
	}

	template <typename TComponent>
	cComponentStorage<TComponent>::~cComponentStorage()
	{
		_context->Destroy<cHashTable<entity, types::u32>>(_indices);
		_context->Destroy<cStack<TComponent>>(_data);
	}

	template <typename TComponent>
	TComponent* cComponentStorage<TComponent>::Create(entity ent)
	{
		_indices->Insert(ent, _data->GetSize());
		TComponent* component = _data->Push();

		return component;
	}

	template <typename TComponent>
	TComponent* cComponentStorage<TComponent>::Get(entity ent)
	{
		types::u32 index = _indices->Find(ent);
		TComponent* component = _data->At(index);

		return component;
	}

	template <typename TComponent>
	void cComponentStorage<TComponent>::Destroy(entity ent)
	{
		types::u32 index = _indices->Find(ent);
		_data->Erase(index);
	}

	cScene::cScene(cContext* context) : iObject(context) {}

	entity cScene::CreateEntity() const
	{
		static entity next = kInvalidEntity + 1;

		return next;
	}
}