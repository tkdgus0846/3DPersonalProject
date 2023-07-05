#include "..\Public\RootNode.h"

CRootNode::RESULT CRootNode::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	RESULT result;

	if (m_LastRunningList.size() != 0)
	{
		// 부모한테 
		m_LastRunningList.erase(m_LastRunningList.begin());
		result = m_ChildList.front()->Run(TimeDelta, &m_LastRunningList);

		if (result.eRet == RUNNING)
		{
			reverse(result.LastRunningList.begin(), result.LastRunningList.end());
			m_LastRunningList = result.LastRunningList;
		}
		else
		{
			m_LastRunningList.clear();
		}
	}
	else
	{
		result = m_ChildList.front()->Run(TimeDelta);

		if (result.eRet == RUNNING)
		{
			reverse(result.LastRunningList.begin(), result.LastRunningList.end());
			m_LastRunningList = result.LastRunningList;
		}
			
	}

	return result;
}

CRootNode* CRootNode::Create()
{
	return new CRootNode();
}

void CRootNode::Free()
{
	__super::Free();
}
