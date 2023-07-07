#include "RandomChooser.h"


// 몇번째 이터레이터 부터 돌껀지 정해주자.
CRandomChooser::RESULT CRandomChooser::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	_int index = -1;

	vector<CBehavior*>::iterator beginIter;

	if (LastRunningList == nullptr || LastRunningList->size() == 0)
	{
		beginIter = m_ChildList.begin();
		index = rand() % m_ChildList.size();
	}
	else
	{
		
		beginIter = find(m_ChildList.begin(), m_ChildList.end(), LastRunningList->front());
		LastRunningList->erase(LastRunningList->begin());
		index = beginIter - m_ChildList.begin();
	}
	

	RESULT result = m_ChildList[index]->Run(TimeDelta, LastRunningList);

	if (result.eRet == RUNNING)
		result.LastRunningList.push_back(this);

	return result;

}

CRandomChooser* CRandomChooser::Create()
{
	return new CRandomChooser;
}

void CRandomChooser::Free()
{
	__super::Free();
}
