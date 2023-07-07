#include "..\Public\BlackBoard.h"
#include "GameObject.h"

CBlackBoard::CBlackBoard()
{
	AddData("Target", (CGameObject*)nullptr);
	AddData("TargetDistance", -10.f);
	AddData("StandardDistance", 0.f);

	AddData("InAttackRange", false);
	AddData("InDetectionRange", false);

	AddData("IsAttack1", false);
	AddData("IsAttack2", false);
	AddData("IsAttack3", false);
	AddData("IsAttack4", false);

	AddData("IsAttack1Started", true);
	AddData("IsAttack2Started", true);
	AddData("IsAttack3Started", true);
	AddData("IsAttack4Started", true);

	AddData("IsAttacking", false);

	AddData("IsAttack1Finished", false);
	AddData("IsAttack2Finished", false);
	AddData("IsAttack3Finished", false);
	AddData("IsAttack4Finished", false);

	//AddData("IsAttack1FirstFrame", false);
	//AddData("IsAttack3FirstFrame", false);
	//AddData("IsAttack3FirstFrame", false);

	AddData("IsWalk", false);
	AddData("IsRun", false);
}

void CBlackBoard::ClearData()
{
	/*ChangeData("InAttackRange", false);
	ChangeData("InDetectionRange", false);*/

	ChangeData("IsAttack1", false);
	ChangeData("IsAttack2", false);
	ChangeData("IsAttack3", false);
	ChangeData("IsAttack4", false);

	/*ChangeData("IsAttack1Started", true);
	ChangeData("IsAttack2Started", true);
	ChangeData("IsAttack3Started", true);
	ChangeData("IsAttack4Started", true);*/


	ChangeData("IsAttacking", false);

	ChangeData("IsWalk", false);
	ChangeData("IsRun", false);

	ChangeData("IsAttack1Finished", false);
	ChangeData("IsAttack2Finished", false);
	ChangeData("IsAttack3Finished", false);
	ChangeData("IsAttack4Finished", false);

}

void CBlackBoard::AddData(const string& name, any Data)
{
	if (m_DataMap.find(name) == m_DataMap.end())
		m_DataMap[name] = Data;
}

void CBlackBoard::ChangeData(const string& name, any Data)
{
	if (m_DataMap.find(name) != m_DataMap.end())
		m_DataMap[name] = Data;	
}

CBlackBoard* CBlackBoard::Create()
{
	return new CBlackBoard;
}

void CBlackBoard::Free()
{
}

