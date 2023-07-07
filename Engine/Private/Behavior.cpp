#include "..\Public\Behavior.h"
#include "Decorator.h"
#include "BlackBoard.h"

void CBehavior::AddNode(CBehavior* pNode, _int iOrder)
{
	if (pNode == nullptr) return;
	
	pNode->m_pParent = this;

	if (iOrder == -1)
		m_ChildList.push_back(pNode);
	else
	{
		m_ChildList.insert(m_ChildList.begin() + iOrder, pNode);
	}


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
		result = result && deco->Is_Exec(m_pBlackBoard);
		if (result == false)
			return false;
	}

	if (m_ChildList.size() == 0)
		m_pBlackBoard->ClearData();

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

