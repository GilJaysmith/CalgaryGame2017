#include "Engine/Pch.h"

#include "Engine/Memory/Memory.h"
#include "Engine/Memory/Allocators/FreeStoreAllocator.h"

#include <set>

namespace Memory
{
	struct AllocatorDesc
	{
		MemoryPool::TYPE m_AllocatorId;
		Allocator* m_Allocator;
		MemoryAllocatorThreadSafeness::TYPE m_ThreadSafeness;
		CRITICAL_SECTION m_cs;
		std::map<void*, std::pair<const char*, int>> m_Allocations;
		AllocatorDesc(MemoryPool::TYPE allocator_id)
		{
			m_AllocatorId = allocator_id;
			InitializeCriticalSection(&m_cs);
			m_Allocator = nullptr;
			m_ThreadSafeness = MemoryAllocatorThreadSafeness::Multithreaded;
		}
		~AllocatorDesc()
		{
			DeleteCriticalSection(&m_cs);
			if (m_Allocator)
			{
				delete m_Allocator;
				m_Allocator = nullptr;
			}
		}
		unsigned int ReportLeaks()
		{
			unsigned int leaks = 0;
			for (auto allocation_it : m_Allocations)
			{
				int* ptr_i = (int*)allocation_it.first;
				int allocator = *ptr_i++;
				int size = *ptr_i++;
				const char* filename = allocation_it.second.first;
				int line = allocation_it.second.second;
				std::stringstream s;
				s << "Allocator " << allocator << ": leak at " << filename << "(" << line << ")" << " size " << size;
				Logging::Log("Memory", s.str());
				++leaks;
			}
			return leaks;
		}
		void LockAllocator()
		{
			if (m_ThreadSafeness == MemoryAllocatorThreadSafeness::Multithreaded)
			{
				EnterCriticalSection(&m_cs);
			}
		}
		void UnlockAllocator()
		{
			if (m_ThreadSafeness == MemoryAllocatorThreadSafeness::Multithreaded)
			{
				LeaveCriticalSection(&m_cs);
			}
		}
		void* MemAlloc(MemoryPool::TYPE allocator, size_t size, const char* filename, int line)
		{
			LockAllocator();
			void* raw_ptr = m_Allocator->Allocate(size + 4 * sizeof(int*));
			auto it = m_Allocations.find(raw_ptr);
			if (it != m_Allocations.end())
			{
				Logging::Log("Memory", "Address already registered as allocated!");
				exit(1);
			}
			m_Allocations[raw_ptr] = std::pair<const char*, int>(filename, line);
			int* mem_i = (int*)raw_ptr;
			*mem_i++ = int(allocator);
			*mem_i++ = (int)size;
			mem_i++;
			mem_i++;
			UnlockAllocator();
			return mem_i;
		}
		void MemFree(void* raw_ptr)
		{
			LockAllocator();
			MemoryPool::TYPE allocator = (MemoryPool::TYPE)(*((int*)raw_ptr));
			auto it = m_Allocations.find(raw_ptr);
			if (it == m_Allocations.end())
			{
				Logging::Log("Memory", "Freeing pointer not registered as allocated!");
				exit(1);
			}
			m_Allocations.erase(raw_ptr);
			m_Allocator->Destroy(raw_ptr);
			UnlockAllocator();
		}
	};

	std::vector<AllocatorDesc*> s_Allocators(MemoryPool::COUNT);
	std::list<MemoryPool::TYPE> s_AllocatorStack = { MemoryPool::Default };

	void Initialize()
	{
		for (int i = 0; i < s_Allocators.size(); ++i)
		{
			s_Allocators[i] = nullptr;
		}
	}

	void Terminate()
	{
		unsigned int leaks = 0;
		for (auto allocator : s_Allocators)
		{
			if (allocator)
			{
				leaks += allocator->ReportLeaks();
				delete allocator;
			}
		}
		if (!leaks)
		{
			Logging::Log("Memory", "No memory leaks on shutdown!");
		}

		s_Allocators.clear();
	}

	Allocator* CreateFreeStoreAllocator()
	{
		return new FreeStoreAllocator;
	}

	Allocator* CreateDougLeaAllocator()
	{
		return nullptr;
	}

	void RegisterAllocator(MemoryPool::TYPE memory_pool_type, Allocator* allocator, MemoryAllocatorThreadSafeness::TYPE thread_safeness)
	{
		s_Allocators[memory_pool_type] = new AllocatorDesc(memory_pool_type);
		s_Allocators[memory_pool_type]->m_Allocator = allocator;
		s_Allocators[memory_pool_type]->m_ThreadSafeness = thread_safeness;
	}
}

void* MemAlloc(MemoryPool::TYPE allocator, size_t size, const char* filename, int line)
{
	return Memory::s_Allocators[allocator]->MemAlloc(allocator, size, filename, line);
}

void MemFreeNotForPublicUse(void* cooked_ptr)
{
	int* raw_ptr = (int*)cooked_ptr;
	raw_ptr -= 4;
	MemoryPool::TYPE allocator = (MemoryPool::TYPE)(*raw_ptr);
	Memory::s_Allocators[allocator]->MemFree(raw_ptr);
}

void* operator new(size_t size, MemoryPool::TYPE allocator, const char* filename, int line)
{
	void* cooked_ptr = MemAlloc(allocator, size, filename, line);
	return cooked_ptr;
}

void operator delete(void*, MemoryPool::TYPE allocator, const char* filename, int line)
{

}
