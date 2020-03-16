#pragma once

#include "utils/Singleton.h"
#include <unordered_map>
#include "SDK/Recv.h"

#define NETVAR(type, name, table, netvar)\
	type& name##() const {\
		static int _##name = NetVars::Get()[table][netvar];\
		return *(type*)((uintptr_t)this + _##name);\
	}\

#define PTRNETVAR(type, name, table, netvar)\
	type* name##() const {\
		static int _##name = NetVars::Get()[table][netvar];\
		return (type*)((uintptr_t)this + _##name);\
	}\

#define OFFSET(type, name, offset)\
	type& name##() const {\
		return *(type*)((uintptr_t)this + offset);\
	}\

/*#define NETVAR(type, name, table, netvar)\
	std::add_lvalue_reference_t<type> name##() {\
		static int _##name = NetVars::Get()[table][netvar];\
		return *reinterpret_cast<std::add_pointer_t<type>>(this + _##name);\
	}\

#define PTRNETVAR(type, name, table, netvar)\
	auto name##() {\
		static int _##name = NetVars::Get()[table][netvar];\
		return reinterpret_cast<std::add_pointer_t<type>>(this + _##name);\
	}\*/

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

