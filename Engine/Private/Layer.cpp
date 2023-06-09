#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObjects(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);	

	return S_OK;
}

void CLayer::Tick(_float TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Tick(TimeDelta);
}

void CLayer::Late_Tick(_float TimeDelta)
{
	for (auto& it = m_GameObjects.begin(); it != m_GameObjects.end();)
	{
		_int result = (*it)->Late_Tick(TimeDelta);

		if (result == OBJ_DEAD)
		{
			Safe_Release(*it);
			it = m_GameObjects.erase(it);
		}
		else if (result == OBJ_RETPOOL)
		{

		}
		else if (result == OBJ_NOEVENT)
		{
			it++;
		}
	}
		
}

CGameObject* CLayer::Find_Object(const wstring& objName)
{
	CGameObject* result = nullptr;
	for (auto object : m_GameObjects)
	{
		if (object->GetName().compare(objName) == 0)
		{
			result = object;
			return result;
		}	
	}
	return result;
}

HRESULT CLayer::Delete_GameObject(const wstring& pObjName)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
	{
		if ((*it)->GetName().compare(pObjName) == 0)
		{
			Safe_Release(*it);
			it = m_GameObjects.erase(it);
			return S_OK;
		}
		else
			it++;
	}
	return E_FAIL;
}

HRESULT CLayer::Delete_GameObject_SameName(const wstring& pObjName)
{
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
	{
		if ((*it)->GetName().find(pObjName) != std::wstring::npos)
		{
			Safe_Release(*it);
			it = m_GameObjects.erase(it);
			return S_OK;
		}
		else
			it++;
	}
	return E_FAIL;
}

HRESULT CLayer::Clear_Layer()
{
	for (auto& obj : m_GameObjects)
	{
		Safe_Release(obj);
	}
	m_GameObjects.clear();
	return S_OK;
}

CLayer * CLayer::Create()
{
	return new CLayer();	
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}

