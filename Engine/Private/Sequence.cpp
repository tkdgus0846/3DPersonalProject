#include "..\Public\Sequence.h"

CSequence::RESULT CSequence::Run(const _double& TimeDelta, vector<CBehavior*>* LastRunningList)
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

		if (result.eRet == FAIL)
			return result;
		if (result.eRet == RUNNING)
		{
			result.LastRunningList.push_back(this);
			return result;
		}
	}
	return RESULT(SUCCESS);
}

CSequence* CSequence::Create()
{
	return new CSequence;
}

void CSequence::Free()
{
	__super::Free();
}
