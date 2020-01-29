#pragma once

#include "Singleton.h"
#include <unordered_map>
#include "SDK/Recv.h"

class NetVars : public Singleton<NetVars>
{
public:
	void Initialize();
	std::unordered_map<std::string, size_t> operator[] (std::string key) {
		return m_NetVars[key];
	}
	int netvars;
private:
	std::unordered_map<std::string, std::unordered_map<std::string, size_t>> m_NetVars;
	void StoreProps(const char*, RecvTable*, size_t childOffset = 0);
};

