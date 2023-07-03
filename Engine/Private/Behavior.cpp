#include "..\Public\Behavior.h"
#include "Decorator.h"

void CBehavior::AddNode(CBehavior* pNode)
{
	if (pNode == nullptr) return;
	
	pNode->m_pParent = this;
	m_ChildList.push_back(pNode);

	//if (m_ChildList.size() >= 2)
	//{
	//	(*(m_ChildList.end()-2))->m_NextNode = m_ChildList.end()-1;
	//	pNode->m_PrevNode = m_ChildList.end() - 2;
	//	pNode->m_NextNode = m_ChildList.end();
	//	pNode->m_CurNode = m_ChildList.end() - 1;
	//}
	////크기가 1보다 작을때
	//else
	//{
	//	pNode->m_PrevNode = m_ChildList.end();
	//	pNode->m_CurNode = m_ChildList.end() - 1;
	//	pNode->m_NextNode = m_ChildList.end();
	//}
}

void CBehavior::Set_BlackBoard(CBlackBoard* pBlackBoard)
{
	for (auto& deco : m_DecoratorList)
	{
		deco->Set_BlackBoard(pBlackBoard);
	}


	m_pBlackBoard = pBlackBoard;
	for (auto& item : m_ChildList)
	{
		item->Set_BlackBoard(pBlackBoard);
	}
}

void CBehavior::Add_Decorator(CDecorator* deco)
{
	m_DecoratorList.push_back(deco);
}

_bool CBehavior::Pass_Decorator()
{
	_bool result = true;

	for (auto deco : m_DecoratorList)
	{
		result = result && deco->Is_Exec();
		if (result == false)
			return false;
	}

	return result;
}

void CBehavior::Free()
{
	for (auto& item : m_DecoratorList)
	{
		Safe_Release(item);
	}

	for (auto& item : m_ChildList)
	{
		Safe_Release(item);
	}
}

