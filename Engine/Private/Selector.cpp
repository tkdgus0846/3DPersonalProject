#include "Selector.h"


// 몇번째 이터레이터 부터 돌껀지 정해주자.
CSelector::RESULT CSelector::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	vector<CBehavior*>::iterator beginIter;

	if (LastRunningList == nullptr || LastRunningList->size() == 0)
	{
		beginIter = m_ChildList.begin();
	}
	else
	{
		
		beginIter = find(m_ChildList.begin(), m_ChildList.end(), LastRunningList->front());
		LastRunningList->erase(LastRunningList->begin());
	}
		

	for (auto it = beginIter; it != m_ChildList.end(); it++)
	{
		RESULT result = (*it)->Run(TimeDelta, LastRunningList);

		if (result.eRet == SUCCESS)
			return result;
		if (result.eRet == RUNNING)
		{
			result.LastRunningList.push_back(this);
			return result;
		}
			
	}
	return RESULT(FAIL);
}

CSelector* CSelector::Create()
{
	return new CSelector;
}

void CSelector::Free()
{
	__super::Free();
}
