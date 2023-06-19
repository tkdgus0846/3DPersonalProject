#include "..\Public\Component.h"

#include "GameInstance.h"

CComponent::CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return	S_OK;
}

HRESULT CComponent::Initialize(void * pArg)
{
	return	S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


CComposite::CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CComposite::CComposite(const CComposite& rhs)
	: CComponent(rhs)
{
	for (auto item : rhs.m_Components)
	{
		void* pArg = nullptr;

		wstring compName = item.first;
		CComponent* newComp = nullptr;
		_bool bContinue = false;
		
		if (compName.compare(RENDERER_W) == 0)
		{
			bContinue = true;
		}
		else if (compName.compare(TRANSFORM_W) == 0)
		{
			bContinue = true;
		}
		else if (compName.compare(VIBUFFER_W) == 0)
		{

		}
		else if (compName.compare(SHADER_W) == 0)
		{

		}
		else if (compName.compare(TEXTURE_W) == 0)
		{

		}
		else if (compName.compare(MODEL_W) == 0)
		{

		}

		if (bContinue == true) continue;

		newComp = item.second->Clone(pArg);
		newComp->SetName(compName);
		newComp->SetOwner(this);

		m_Components.emplace(compName, newComp);
		
	}
}

HRESULT CComposite::Initialize_Prototype()
{
	HRESULT result = __super::Initialize_Prototype();

	return result;
}

HRESULT CComposite::Initialize(void* pArg)
{
	HRESULT result = __super::Initialize(pArg);

	return result;
}

void CComposite::Tick(_double TimeDelta)
{
	for (auto& comp : m_Components)
	{
		comp.second->Tick(TimeDelta);
	}
}

void CComposite::Late_Tick(_double TimeDelta)
{
	for (auto& comp : m_Components)
	{
		comp.second->Late_Tick(TimeDelta);
	}
}

list<CComponent*> CComposite::Get_ComponentsByList()
{
	list<CComponent*> componentList;

	// unordered_map의 값인 CComponent*들을 추출하여 list에 삽입
	for (const auto& pair : m_Components)
	{
		componentList.push_back(pair.second);
	}

	return componentList;
}

HRESULT CComposite::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const wstring& pComponentTag, _Inout_ CComponent** ppOut, CComposite* pOwner, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	pComponent->SetName(pComponentTag);
	pComponent->SetOwner(pOwner);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CComposite::Add_Component(CComponent* newComp)
{
	m_Components.emplace(newComp->GetName(), newComp);
	return S_OK;
}

HRESULT CComposite::Delete_Component(const _tchar* pComponentTag)
{
	auto iter = m_Components.find(pComponentTag);
	if (iter == m_Components.end())
	{
		MSG_BOX("컴포넌트를 찾지 못하였음");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	Safe_Release(iter->second);
	m_Components.erase(iter);

	Safe_Release(pGameInstance);
	return S_OK;
}

CComponent* CComposite::Get_Component(const wstring& compName)
{
	auto iter = m_Components.find(compName);

	if (iter == m_Components.end()) 
		return nullptr;
	else 
		return iter->second;
}


void CComposite::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	__super::Free();
}
