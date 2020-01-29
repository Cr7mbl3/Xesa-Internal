#include "NetVars.h"

#include "Interfaces.h"

#include "SDK/ClientClass.h"

//https://www.unknowncheats.me/forum/counterstrike-global-offensive/206642-blazing-fast-netvar-scanner.html

void NetVars::Initialize()
{
	ClientClass* pClientData = Interfaces::Get().Client->GetAllClasses();

	for (auto pData = pClientData; pData != nullptr; pData = pData->m_pNext)
	{
		StoreProps(pData->m_pRecvTable->m_pNetTableName, pData->m_pRecvTable);
	}
}

void NetVars::StoreProps(const char* chGroupName, RecvTable* pTable, size_t childOffset)
{
	for (auto i = 0; i < pTable->m_nProps; ++i)
	{
		auto pCurrentProp = &pTable->m_pProps[i];
		auto pCurrentChild = pCurrentProp->m_pDataTable;

		if (pCurrentChild != nullptr && pCurrentChild->m_nProps > 0)
			StoreProps(chGroupName, pCurrentChild, pCurrentProp->m_Offset);

		if ((m_NetVars[chGroupName][pCurrentProp->m_pVarName] <= 0) && (
			pCurrentProp->m_RecvType == DPT_Float
			|| pCurrentProp->m_RecvType == DPT_Int
			|| pCurrentProp->m_RecvType == DPT_String
			|| pCurrentProp->m_RecvType == DPT_Vector
			|| pCurrentProp->m_RecvType == DPT_VectorXY))
		{
			m_NetVars[chGroupName][pCurrentProp->m_pVarName] = static_cast<size_t>(pCurrentProp->m_Offset) + childOffset;
			netvars++;
		}
	}
}
