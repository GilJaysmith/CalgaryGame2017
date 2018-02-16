#pragma once

#include <set>

namespace Memory
{
	class FreeStoreAllocator : public Allocator
	{
	public:
		void* Allocate(std::size_t size) override;
		void Destroy(void* ptr) override;

	protected:
		std::set<void*> m_Allocations;
	};
}
