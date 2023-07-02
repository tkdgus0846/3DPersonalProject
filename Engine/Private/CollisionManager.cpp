#include "..\Public\CollisionManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Check_Collision(COLGROUP ID1, COLGROUP ID2)
{
	if (Get_ColliderList(ID1) == nullptr) return;
	if (Get_ColliderList(ID2) == nullptr) return;

	if (Get_ColliderList(ID1)->empty())
		return;
	if (Get_ColliderList(ID2)->empty())
		return;

	for (auto iterID1 = Get_ColliderList(ID1)->begin(); iterID1 != Get_ColliderList(ID1)->end(); iterID1++)
	{
		if ((*iterID1)->m_bEnabled == false) continue;

		for (auto iterID2 = Get_ColliderList(ID2)->begin(); iterID2 != Get_ColliderList(ID2)->end(); iterID2++)
		{
			if ((*iterID2)->m_bEnabled == false) continue;
			if (ID1 == ID2 && iterID1 == iterID2) continue;

			CCollider* collider1 = *iterID1;
			CCollider* collider2 = *iterID2;

			if (collider1 == nullptr || collider2 == nullptr) break;

			if (collider1->m_pBounding == nullptr || collider2->m_pBounding == nullptr) break;

			auto* colliderList1 = &collider1->m_CollisionList;
			auto* colliderList2 = &collider2->m_CollisionList;
			auto findCollider = colliderList1->find(collider2);

			Collision collision1;
			Collision collision2;

			// 충돌했다면
			_bool result = collider1->Intersect(collider2);
			_bool result2 = collider2->Intersect(collider1);
			if (result && result2)
			{
				
				collision2 = collision1;

				collision1.MyCollider = collider1;
				collision1.OtherCollider = collider2;
				collision1.OtherGameObject = (CGameObject*)collider2->GetOwner();

				collision2.MyCollider = collider2;
				collision2.OtherCollider = collider1;
				collision2.OtherGameObject = (CGameObject*)collider1->GetOwner();

				// 둘이 충돌하고 있지 않았으면
				if (findCollider == colliderList1->end())
				{
					/* 레퍼런스 카운트 관리가 잘 될지 모르겠지만 일단 해보자.*/
					/* 레퍼런스 카운트 관리가 잘 될지 모르겠지만 일단 해보자.*/
					//Safe_AddRef(collider1);
					//Safe_AddRef(collider2);

					collider1->OnCollisionEnter(&collision1);
					collider1->OnCollisionStay(&collision1);

					collider2->OnCollisionEnter(&collision2);
					collider2->OnCollisionStay(&collision2);
				}
				// 둘이 충돌하고 있었으면
				else
				{
					collider1->OnCollisionStay(&collision1);
					collider2->OnCollisionStay(&collision2);
				}

				(*colliderList1)[collider2] = collision1;
				(*colliderList2)[collider1] = collision2;

				

			}
			// 충돌 하지 않았다면
			else
			{
				if (colliderList1->empty()) continue;
				if (colliderList2->empty()) continue;

				if (findCollider != colliderList1->end())
				{
					auto findCollider2 = colliderList2->find(collider1);

					if (findCollider2 == colliderList2->end()) continue;

					collider1->OnCollisionExit(&findCollider->second);
					collider2->OnCollisionExit(&findCollider2->second);

					/* 레퍼런스 카운트 관리가 잘 될지 모르겠지만 일단 해보자.*/
					/* 레퍼런스 카운트 관리가 잘 될지 모르겠지만 일단 해보자.*/
					/* 객체가 죽을때 내 콜라이더랑 충돌 하고 있던 애들을 다 Release 해주고 그 충돌하고있던 개수만큼 나를 Release 해줘야 할것 같다. (예상)*/
					/*Safe_Release(collider1);
					Safe_Release(collider2);*/

					colliderList1->erase(collider2);
					colliderList2->erase(collider1);
				}
			}
		}
	}
}

void CCollisionManager::Change_ColGroup(CCollider* collider, COLGROUP changeID)
{
	list<CCollider*>::iterator iter;
	_bool found = false;
	for (int i = 0; i < COL_END; i++)
	{
		for (iter = Get_ColliderList((COLGROUP)i)->begin(); iter != Get_ColliderList((COLGROUP)i)->end(); ++iter)
		{
			if (*iter == collider)
			{
				Get_ColliderList((COLGROUP)i)->erase(iter);
				found = true;
				break;
			}
		}
		if (found == true) break;
	}

	if (found == false) return;


	Get_ColliderList(changeID)->push_front(collider);
}

void CCollisionManager::Add_ColGroup(COLGROUP eID, CCollider* pCollider)
{
	if (COL_END <= eID || nullptr == pCollider)
		return;

	pCollider->AddRef();

	Get_ColliderList(eID)->push_back(pCollider);

	/*if (m_bIsRender != pCollider->m_bIsRender)
		m_bIsRender = pCollider->m_bIsRender;*/
}

HRESULT CCollisionManager::Remove_Collider(CCollider* collider, COLGROUP colID)
{
	for (auto it = Get_ColliderList(colID)->begin(); it != Get_ColliderList(colID)->end(); it++)
	{
		if (*it == collider)
		{
			Safe_Release(collider);
			Get_ColliderList(colID)->erase(it);
			break;
		}
	}
	return S_OK;
}

HRESULT CCollisionManager::Remove_Collider(CGameObject* gameObject)
{
	return S_OK;
}

void CCollisionManager::Reset_ColGroup()
{
	for (int i = 0; i < COL_END; i++)
	{
		for (auto& collider : m_ColliderList[i])
		{
			if (collider->GetRef() == 1)
			{
				collider->Erase_FromOtherCollisionList();
			}
			Safe_Release(collider);
		}
		m_ColliderList[i].clear();
	}
	
}

void CCollisionManager::Free()
{
	for (int i = 0; i < COL_END; i++)
	{
		for (auto& collider : m_ColliderList[i])
		{
			Safe_Release(collider);
		}
	}
}

#ifdef _DEBUG

void CCollisionManager::Render_Colliders()
{
	if (m_bIsRender == false) return;

	for (int i = 0; i < COL_END; i++)
	{
		for (auto& collider : m_ColliderList[i])
		{
			collider->Render();
		}
	}
}
#endif

list<CCollider*>* CCollisionManager::Get_ColliderList(COLGROUP colID)
{
	if (colID < COL_END)
	{
		return &m_ColliderList[colID];
	}
	else
		return nullptr;
}

void CCollisionManager::Find_Remove_Collider(CGameObject* gameObject, COLGROUP colID)
{
	if (Get_ColliderList(colID) == nullptr) return;

	for (auto it = Get_ColliderList(colID)->begin(); it != Get_ColliderList(colID)->end();)
	{
		if ((CGameObject*)(*it)->GetOwner() == gameObject)
		{
			it = Get_ColliderList(colID)->erase(it);
		}
		else
			it++;
	}
}
