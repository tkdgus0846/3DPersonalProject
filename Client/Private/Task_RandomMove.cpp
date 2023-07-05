#include "stdafx.h"
#include "..\Public\Task_RandomMove.h"
#include "BlackBoard.h"
#include "GameObject.h"
#include "Paladin.h"
#include "Transform.h"

#include <random>

CTask_RandomMove::RESULT CTask_RandomMove::Run(const _float& TimeDelta, vector<CBehavior*>* LastRunningList)
{
	m_pBlackBoard->ChangeData("IsWalk", false);

	if (Pass_Decorator() == false)
		return RESULT(FAIL);

	//cout << "Turn ÁøÀÔ!!" << endl;
	if (m_TimeAcc == 0.0)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<_float> dir1(-1.0f, 1.0f);
		std::uniform_real_distribution<_float> dir2(-1.0f, 1.0f);

		m_RandomDir = _float3( dir1(gen), 0.f, dir2(gen));
		_vector dir = XMLoadFloat3(&m_RandomDir);
		XMStoreFloat3(&m_RandomDir, XMVector3Normalize(dir));
	}
	
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= m_MoveTime)
	{
		m_TimeAcc = 0.0f;
		return RESULT(SUCCESS);
	}

	
	
	CGameObject* owner = any_cast<CGameObject*>(m_pBlackBoard->FindData("Owner"));
	CComponent* pComp = owner->Get_Component(TRANSFORM_W);
	CTransform* pTransform = (CTransform*)pComp;

	pTransform->LookDir(XMLoadFloat3(&m_RandomDir), CTransform::LANDOBJECT);
	pTransform->Go_Dir(XMLoadFloat3(&m_RandomDir), TimeDelta, (CNavigation*)owner->Get_Component(NAVIGATION_W));

	m_pBlackBoard->ChangeData("IsWalk", true);
	
	RESULT result = RESULT(RUNNING);
	result.LastRunningList.push_back(this);

	return result;
}

CTask_RandomMove* CTask_RandomMove::Create()
{
	return new CTask_RandomMove;
}

void CTask_RandomMove::Free()
{
	__super::Free();
}
