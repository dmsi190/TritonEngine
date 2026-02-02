// hash_table.hpp

#pragma once

#include <type_traits>
#include "object.hpp"
#include "context.hpp"
#include "memory_pool.hpp"
#include "stack.hpp"
#include "tag.hpp"
#include "math.hpp"
#include "types.hpp"

namespace triton
{
	template <typename TKey, typename TValue>
	class cHashTable;

	template <typename TKey, typename TValue>
	class cHashTablePair : public cStackValue
	{
		friend class cHashTable<TKey, TValue>;

		TKey key = {};
		TValue value = {};
	};

	template <typename TKey, typename TValue>
	class cHashTable : public iObject
	{
		TRITON_OBJECT(cHashTable)

	public:
		static_assert(std::is_base_of_v<cStackValue, TValue>, "TValue must inherit from cStackValue");

		explicit cHashTable(cContext* context, const sChunkAllocatorDescriptor& allocatorDesc);
		virtual ~cHashTable() override final;

		template<typename... Args>
		TValue* Insert(const TKey& key, Args&&... args);
		TValue* Insert(const TKey& key, const TValue&& value);
		TValue* Find(const TKey& key) const;
		TValue* Find(types::u32 index) const;
		void Erase(const TKey& key);
		void Erase(types::u32 index);

		inline types::usize GetSize() const { return _elements->GetSize(); }

	private:
		void HashPair(const TKey& key, const TValue* value);

		sChunkAllocatorDescriptor _allocatorDesc = {};
		cStack<cHashTablePair<TKey, TValue>>* _elements;
		types::usize _hashTableSize = 0;
		types::qword _hashMask = 0;
		cStackValue* _hashTable = nullptr;
	};

	template <typename TKey, typename TValue>
	cHashTable<TKey, TValue>::cHashTable(cContext* context, const sChunkAllocatorDescriptor& allocatorDesc) : iObject(context)
	{
		const sCapabilities* caps = _context->GetSubsystem<cEngine>()->GetApplication()->GetCapabilities();
		cMemoryAllocator* memoryAllocator = _context->GetMemoryAllocator();

		_allocatorDesc = allocatorDesc;
		_elements = _context->Create<cStack<cHashTablePair<TKey, TValue>>>(_context, _allocatorDesc);
		_hashTableSize = hashTableSize;
		_hashMask = MakeHashMask(hashTableSize);
		_hashTable = (cStackValue*)memoryAllocator->Allocate(_hashTableSize * sizeof(cStackValue), caps->memoryAlignment);
	}

	template <typename TKey, typename TValue>
	cHashTable<TKey, TValue>::~cHashTable()
	{
		cMemoryAllocator* memoryAllocator = _context->GetMemoryAllocator();
		memoryAllocator->Deallocate(_hashTable);

		_context->Destroy<cStack<cHashTablePair<TKey, TValue>>>(_elements);
	}

	template <typename TKey, typename TValue>
	template <typename... Args>
	TValue* cHashTable<TKey, TValue>::Insert(const TKey& key, Args&&... args)
	{
		cHashTablePair<TKey, TValue>* pair = _elements->Push(std::forward<Args>(args)...);

		if (pair == nullptr)
			return nullptr;

		TValue* object = &pair->value;

		HashPair(key, object);

		return object;
	}

	template <typename TKey, typename TValue>
	TValue* cHashTable<TKey, TValue>::Insert(const TKey& key, const TValue&& value)
	{
		cHashTablePair<TKey, TValue>* pair = _elements->Push(value);

		if (pair == nullptr)
			return nullptr;

		TValue* object = &pair->value;

		HashPair(key, object);

		return object;
	}

	template <typename TKey, typename TValue>
	TValue* cHashTable<TKey, TValue>::Find(const TKey& key) const
	{
		const types::qword hash = cMath::Hash<TKey>(key, _hashMask);
		const cStackValue& sv = _hashTable[hash];
		cHashTablePair<TKey, TValue>* pair = _elements->At(sv);
		if (pair != nullptr && key == pair->key)
			return &pair->value;

		for (types::usize i = 0; i < _elements->GetSize(); i++)
		{
			cHashTablePair<TKey, TValue>* pair = _elements->At(i);
			if (pair != nullptr && key == pair->key)
				return &pair->value;
		}

		return nullptr;
	}

	template <typename TKey, typename TValue>
	TValue* cHashTable<TKey, TValue>::Find(types::u32 index) const
	{
		cHashTablePair<TKey, TValue>* pair = _elements->At(index);

		if (pair == nullptr)
			return nullptr;
		else
			return &pair->value;
	}

	template <typename TKey, typename TValue>
	void cHashTable<TKey, TValue>::Erase(const TKey& key)
	{
		const types::qword hash = cMath::Hash<TKey>(key, _hashMask);
		const cStackValue& sv = _hashTable[hash];
		const cHashTablePair<TKey, TValue>* pair = _elements->At(sv);
		if (pair != nullptr && key == pair->key)
			_elements->Erase(sv.globalPosition);

		for (types::usize i = 0; i < _elements->GetSize(); i++)
		{
			const cHashTablePair<TKey, TValue>* pair = _elements->At(i);
			if (pair != nullptr && key == pair->key)
				_elements->Erase(i);
		}
	}

	template <typename TKey, typename TValue>
	void cHashTable<TKey, TValue>::Erase(types::u32 index)
	{
		_elements->Erase(index);
	}

	template <typename TKey, typename TValue>
	void cHashTable<TKey, TValue>::HashPair(const TKey& key, const TValue* value)
	{
		cStackValue sv = {};
		sv.chunk = value->chunk;
		sv.position = value->position;
		sv.globalPosition = value->globalPosition;

		const types::u32 hash = cMath::Hash<TKey>(key, _hashMask);
		_hashTable[hash] = sv;
	}
}