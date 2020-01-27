#pragma once

#include <cstdint>
#include <type_traits>
#include "Vector.h"

struct CRecvProxyData {
	int pad;
	union {
		float _float;
		long _int;
		char* _string;
		void* data;
		Vector vector;
		int64_t int64;
	} value;
};

using RecvVarProxyFn = std::add_pointer_t<void(CRecvProxyData&, void*, void*)>;
using ArrayLengthRecvProxyFn = std::add_pointer_t<void(void*, int, int)>;
using DataTableRecvVarProxyFn = std::add_pointer_t<void(struct RecvProp*, void**, void*, int)>;

struct RecvProp {
	const char*				m_pVarName;
	int						m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;
	bool					m_bInsideArray;
	const void*				m_pExtraData;
	RecvProp*				m_pArrayProp;
	ArrayLengthRecvProxyFn	m_ArrayLengthProxy;
	RecvVarProxyFn			m_ProxyFn;
	DataTableRecvVarProxyFn	m_DataTableProxyFn;
	struct RecvTable*		m_pDataTable;
	int						m_Offset;
	int						m_ElementStride;
	int						m_nElements;
	const char*				m_pParentArrayPropName;
};

struct RecvTable {
	RecvProp*		m_pProps;
	int				m_nProps;
	void*			m_pDecoder;
	const char*		m_pNetTableName;
private:
	bool			m_bInitialized;
	bool			m_bInMainList;
};