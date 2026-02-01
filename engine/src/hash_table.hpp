// hash_table.hpp

#pragma once

#include "object.hpp"
#include "context.hpp"
#include "memory_pool.hpp"
#include "types.hpp"

namespace triton
{
	struct sChunkElement
	{
		types::u32 chunk = 0;
		types::u32 position = 0;
	};

	struct sChunkAllocatorDescriptor
	{
		types::usize chunkByteSize = 16 * 1024;
		types::usize maxChunkCount = 256;
		types::usize hashTableSize = 4096;
	};

	template <typename TKey, typename TValue>
	class cHashTable : public iObject
	{
	public:
		explicit cHashTable(cContext* context, const sChunkAllocatorDescriptor& allocatorDesc);
		virtual ~cHashTable() override final;

		template<typename... Args>
		TValue* Insert(Args&&... args);
		TValue* Insert(const TKey& key, const TValue&& value);
		TValue* Find(const TKey& key) const;
		TValue* Find(types::u32 index) const;
		void Erase(const TKey& key);
		void Erase(types::u32 index);

		inline types::usize GetElementCount() const { return _elementCount; }

	private:
		types::u32 AllocateChunk();
		void DeallocateChunk(types::u32 chunkIndex);
		types::u32 GetChunkIndex(types::u32 globalPosition);
		types::u32 GetChunkLocalPosition(types::u32 chunkIndex, types::u32 globalPosition);
		types::cpuword MakeHashMask(types::usize size);

	private:
		sChunkAllocatorDescriptor _allocatorDesc = {};
		types::usize _chunkCount = 0;
		types::usize _objectByteSize = 0;
		types::usize _objectCountPerChunk = 0;
		types::usize _elementCount = 0;
		T** _chunks = nullptr;
		types::usize _hashTableSize = 0;
		types::cpuword _hashMask = 0;
		sChunkElement* _hashTable = nullptr;
	};

	template <typename TKey, typename TValue>
	cHashTable<TKey, TValue>::cHashTable(cContext* context, const sChunkAllocatorDescriptor& allocatorDesc) : iObject(context)
	{
		const sCapabilities* caps = _context->GetSubsystem<cEngine>()->GetApplication()->GetCapabilities();
		cMemoryAllocator* memoryAllocator = _context->GetMemoryAllocator();

		_allocatorDesc = allocatorDesc;
		_objectByteSize = sizeof(T);
		_objectCountPerChunk = chunkByteSize / _objectByteSize;
		_chunks = (TValue**)memoryAllocator->Allocate(_maxChunkCount * sizeof(TValue*), caps->memoryAlignment);
		_hashTableSize = hashTableSize;
		_hashMask = MakeHashMask(hashTableSize);
		_hashTable = (sChunkElement*)memoryAllocator->Allocate(_hashTableSize * sizeof(sChunkElement), caps->memoryAlignment);

		AllocateNewChunk();
	}

	template <typename TKey, typename TValue>
	cHashTable<TKey, TValue>::~cHashTable()
	{
		for (types::usize i = 0; i < _chunkCount; i++)
			DeallocateChunk(i);

		cMemoryAllocator* memoryAllocator = _context->GetMemoryAllocator();
		memoryAllocator->Deallocate(_hashTable);
		memoryAllocator->Deallocate(_chunks);
	}

	template <typename TKey, typename TValue>
	template <typename... Args>
	TValue* cHashTable<TKey, TValue>::Insert(Args&&... args)
	{
		types::u32 elementChunkIndex = GetChunkIndex(_elementCount);
		types::u32 elementLocalPosition = GetChunkLocalPosition(elementChunkIndex, _elementCount);

		const types::usize chunkObjectCount = GetChunkLocalPosition(_chunkCount - 1, _elementCount);
		if (chunkObjectCount >= _objectCountPerChunk)
		{
			elementChunkIndex = AllocateChunk();
			elementLocalPosition = 0;
		}

		_elementCount += 1;

		cHandle::index idx = (cHandle::index)elementLocalPosition;
		TValue* object = _context->Create<TValue>(_chunks[elementChunkIndex], idx, std::forward<Args>(args)...);

		sChunkElement ce;
		ce.chunk = elementChunkIndex;
		ce.position = elementLocalPosition;

		const uint32_t hash = Hash(object->GetID(), _hashMask);
		_hashTable[hash] = ce;

		return object;
	}

	template <typename TKey, typename TValue>
	TValue* cHashTable<TKey, TValue>::Insert(const TKey& key, const TValue&& value)
	{
		types::u32 elementChunkIndex = GetChunkIndex(_elementCount);
		types::u32 elementLocalPosition = GetChunkLocalPosition(elementChunkIndex, _elementCount);

		const types::usize chunkObjectCount = GetChunkLocalPosition(_chunkCount - 1, _elementCount);
		if (chunkObjectCount >= _objectCountPerChunk)
		{
			elementChunkIndex = AllocateChunk();
			elementLocalPosition = 0;
		}

		_elementCount += 1;

		cHandle::index idx = (cHandle::index)elementLocalPosition;
		_chunks[elementChunkIndex][idx] = std::move(value);
		TValue* object = &_chunks[elementChunkIndex][idx];
		object->_identifier = key;

		sChunkElement ce;
		ce.chunk = elementChunkIndex;
		ce.position = elementLocalPosition;

		const uint32_t hash = Hash(object->GetID(), _hashMask);
		_hashTable[hash] = ce;

		return object;
	}

	template <typename TKey, typename TValue>
	TValue* cHashTable<TKey, TValue>::Find(const TKey& key) const
	{
		const types::usize chunkObjectCount = GetChunkLocalPosition(_chunkCount - 1, _elementCount);
		const types::cpuword hash = Hash(key.GetData(), key.GetByteSize());
		const sChunkElement& ce = _hashTable[hash];
		if (ce.chunk < _chunkCount && ce.position < chunkObjectCount)
		{
			const T* object = &_chunks[ce.chunk][ce.position];
			if (key == object->GetID())
				return (const T*)object;
		}

		for (types::usize i = 0; i < _chunkCount; i++)
		{
			for (types::usize j = 0; j < _objectCountPerChunk; j++)
			{
				const T* object = &_chunks[i][j];
				if (key == object->GetID())
					return (const T*)object;
			}
		}

		return nullptr;
	}

	template <typename TKey, typename TValue>
	TValue* cHashTable<TKey, TValue>::Find(types::u32 index) const
	{
		const types::u32 chunkIndex = GetChunkIndex(index);

		if (chunkIndex >= _chunkCount)
			return nullptr;

		const types::usize chunkObjectCount = GetChunkLocalPosition(info.chunk, _elementCount);
		const types::u32 localPosition = GetChunkLocalPosition(chunkIndex, index);
		if (localPosition >= chunkObjectCount)
			return nullptr;

		return &_chunks[chunkIndex][localPosition];
	}

	template <typename TKey, typename TValue>
	void cHashTable<TKey, TValue>::Erase(const TKey& key)
	{
		for (types::usize i = 0; i < _chunkCount; i++)
		{
			for (types::usize j = 0; j < _objectCountPerChunk; j++)
			{
				if (key == _chunks[i][j]->GetID())
				{
					const types::u32 lastChunkIndex = GetChunkIndex(_elementCount - 1);
					const types::usize lastChunkObjectCount = GetChunkLocalPosition(lastChunkIndex, _elementCount);

					const types::u32 lastLocalPosition = lastChunkObjectCount - 1;
					_chunks[i][j] = _chunks[lastChunkIndex][lastLocalPosition];
					_elementCount -= 1;

					if (lastChunkObjectCount == 1)
						DeallocateChunk(lastChunkIndex);

					return;
				}
			}
		}
	}

	template <typename TKey, typename TValue>
	void cHashTable<TKey, TValue>::Erase(types::u32 index)
	{
		const types::u32 lastChunkIndex = _chunkCount - 1;
		const types::u32 chunkIndex = GetChunkIndex(index);
		const types::usize lastChunkObjectCount = GetChunkLocalPosition(lastChunkIndex, _elementCount);
		const types::u32 lastLocalPosition = lastChunkObjectCount - 1;
		const types::usize localPosition = GetChunkLocalPosition(chunkIndex, index);
		const types::boolean isLastChunk = chunkIndex == lastChunkIndex;

		if (chunkIndex >= _chunkCount || (isLastChunk == types::K_TRUE && localPosition >= lastChunkObjectCount))
			return;

		_chunks[chunkIndex][localPosition] = _chunks[lastChunkIndex][lastLocalPosition];
		_elementCount -= 1;

		if (lastChunkObjectCount == 1)
			DeallocateChunk(lastChunkIndex);
	}

	template <typename TKey, typename TValue>
	types::u32 cHashTable<TKey, TValue>::AllocateChunk()
	{
		if (_chunkCount >= _maxChunkCount)
			return 0;

		const sCapabilities* caps = _context->GetSubsystem<cEngine>()->GetApplication()->GetCapabilities();
		cMemoryAllocator* memoryAllocator = _context->GetMemoryAllocator();
		_chunks[_chunkCount] = (TValue*)memoryAllocator->Allocate(_chunkByteSize, caps->memoryAlignment);

		return _chunkCount++;
	}

	template <typename TKey, typename TValue>
	void cHashTable<TKey, TValue>::DeallocateChunk(types::u32 chunkIndex)
	{
		if (chunkIndex >= _chunkCount)
			return;

		cMemoryAllocator* memoryAllocator = _context->GetMemoryAllocator();
		memoryAllocator->Deallocate(_chunks[chunkIndex]);
		_chunkCount -= 1;

		return _chunkCount++;
	}

	template <typename TKey, typename TValue>
	types::u32 cHashTable<TKey, TValue>::GetChunkIndex(types::u32 globalPosition)
	{
		return globalPosition / _objectCountPerChunk;
	}

	template <typename TKey, typename TValue>
	types::u32 cHashTable<TKey, TValue>::GetChunkLocalPosition(types::u32 chunkIndex, types::u32 globalPosition)
	{
		const types::u32 chunkIndexBoundary = chunkIndex * _objectCountPerChunk;
		const types::u32 localPosition = globalPosition - chunkIndexBoundary;

		return localPosition;
	}

	template <typename TKey, typename TValue>
	types::cpuword cHashTable<TKey, TValue>::MakeHashMask(types::usize size)
	{
		unsigned int count = __lzcnt((unsigned int)size);
		types::cpuword mask = (types::cpuword)((1 << (31 - count)) - 1);

		return mask;
	}
}