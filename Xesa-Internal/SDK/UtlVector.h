#pragma once

template <typename T>
class CUtlVector {
public:
	T& operator[](int i) noexcept { return m_pMemory[i]; };

	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
	int m_Size;
	T* m_pElements;
};
