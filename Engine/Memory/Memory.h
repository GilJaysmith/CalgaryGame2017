#pragma once

#include <cstddef>
#include <list>
#include <vector>


namespace MemoryPool
{
	enum TYPE
	{
		Default,
		System,
		Physics,
		Rendering,
		Entities,
		Audio,
		COUNT
	};
}

namespace MemoryAllocatorThreadSafeness
{
	enum TYPE
	{
		SingleThreaded,
		Multithreaded,
	};
}

namespace Memory
{
	class Allocator
	{
	public:
		virtual void* Allocate(std::size_t) = 0;
		virtual void Destroy(void* raw) = 0;
	};

	extern std::list<MemoryPool::TYPE> s_AllocatorStack;

	class ScopedAllocatorObject
	{
	public:
		ScopedAllocatorObject(MemoryPool::TYPE allocator)
			: m_Allocator(allocator)
		{
			s_AllocatorStack.push_front(allocator);
		}
		~ScopedAllocatorObject()
		{
			s_AllocatorStack.pop_front();
		}
		MemoryPool::TYPE m_Allocator;
	};

	void Initialize();
	void Terminate();

	Allocator* CreateFreeStoreAllocator();
	Allocator* CreateDougLeaAllocator();

	void RegisterAllocator(MemoryPool::TYPE memory_pool_type, Allocator* allocator, MemoryAllocatorThreadSafeness::TYPE thread_safeness);
}

void* MemAlloc(MemoryPool::TYPE allocator, size_t size, const char* filename, int line);

void* operator new(size_t size, MemoryPool::TYPE allocator, const char* filename, int line);
void operator delete(void* ptr, MemoryPool::TYPE allocator, const char* filename, int line);


// Basic interface to memory.
void MemFreeNotForPublicUse(void* cooked_ptr);

// Interface for creating and deleting single instance of a class object.

#define MemNewBytes(allocator, size) MemAlloc(allocator, size, __FILE__, __LINE__)
#define MemNew(allocator, type) new(allocator, __FILE__, __LINE__) type

template<typename T> void MemDelete(T* cooked_ptr)
{
	int* ptr_i = (int*)cooked_ptr;
	MemoryPool::TYPE allocator = (MemoryPool::TYPE)(ptr_i[-4]);
	cooked_ptr->~T();
	MemFreeNotForPublicUse(cooked_ptr);
}

// Specialization without '~T' call.
template<> inline void MemDelete(void* cooked_ptr)
{
	int* ptr_i = (int*)cooked_ptr;
	MemoryPool::TYPE allocator = (MemoryPool::TYPE)(ptr_i[-4]);
	MemFreeNotForPublicUse(cooked_ptr);
}


//// Interface for creating and deleting arrays of class objects.
//
//template <class T>
//struct TypeAndCount
//{
//};
//
//template <class T, size_t N>
//struct TypeAndCount<T[N]>
//{
//	typedef T Type;
//	static const size_t Count = N;
//};
//
//template <typename T>
//T* NewArray(MemoryPool::TYPE allocator, size_t N, const char* filename, int line)
//{
//	union
//	{
//		void* as_void;
//		size_t* as_size_t;
//		T* as_T;
//	};
//
//	as_void = Memory::s_Allocators[allocator]->Allocate(sizeof(T)*N + 16);
//
//	// store number of instances in first size_t bytes
//	*as_size_t++ = allocator;
//	*as_size_t++ = N;
//
//	// construct instances using placement new
//	const T* const onePastLast = as_T + N;
//	while (as_T < onePastLast)
//		new (as_T++) T;
//
//	// hand user the pointer to the first instance
//	return (as_T - N);
//}
//
//#define MemNewArray(allocator, type)    NewArray<TypeAndCount<type>::Type>(allocator, TypeAndCount<type>::Count, __FILE__, __LINE__)
//
//template<typename T> void MemDeleteArray(T* cooked_ptr)
//{
//	size_t* raw_ptr = (size_t*)cooked_ptr;
//	raw_ptr -= 4;
//	MemoryPool::TYPE allocator = (MemoryPool::TYPE)(raw_ptr[0]);
//	int count = (int)raw_ptr[1];
//	T* ptr_t = (T*)cooked_ptr;
//	ptr_t += count - 1;
//	for (int i = count; i > 0; --i)
//	{
//		ptr_t--->~T();
//	}
//	Memory::s_Allocators[allocator]->Destroy(raw_ptr);
//}


#define SET_SCOPED_MEMORY_ALLOCATOR(p) Memory::ScopedAllocatorObject _dummy(p)
