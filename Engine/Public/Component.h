#pragma once

#include "Base.h"

/* ���� ������Ʈ���� �θ� �ȴ�. */

BEGIN(Engine)

class CGameObject;
class CComposite;

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual void Tick(_double TimeDelta) {}
	virtual void Late_Tick(_double TimeDelta) {}
	virtual HRESULT Render() { return S_OK; }

	const wstring GetName() const { return m_pName; }
	void SetName(const wstring& newName) { m_pName = newName; }
	CComposite* GetOwner() const { return m_pOwner; }
	void SetOwner(CComposite* pObj)
	{
		if (pObj == nullptr) 
		{
			MSG_BOX("Set Owner Failed!"); return; 
		}
		m_pOwner = pObj;
	}

protected:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	_bool							m_isCloned = { false };
	CComposite*						m_pOwner = { nullptr };
	wstring							m_pName = { L"" };

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

class ENGINE_DLL CComposite : public CComponent
{
protected:
	CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);

	list<class CComponent*> Get_ComponentsByList();
	unordered_map<wstring, CComponent*>* Get_ComponentsByUmap() { return &m_Components; }

protected:
	unordered_map<wstring, CComponent*>			m_Components;

public:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const wstring& pComponentTag, _Inout_ CComponent * *ppOut, void* pArg = nullptr);

	// ������Ʈ�� �����ؼ� �׳� ����ִ� ���� �۾��� �ϱ� �����̴�.
	HRESULT		Add_Component(CComponent* newComp, const wstring& componentName);

	// ���ӿ�����Ʈ ���� ������Ʈ���� �߰��ϱ� �����̴�.
	HRESULT		Add_Component(const wstring& PrototypeTag, const wstring& ObjectName, CGameObject** ppOut, void* pArg = nullptr);

	HRESULT		Delete_Component(const _tchar* pComponentTag);
	CComponent* Get_Component(const wstring& compName);

public:
	virtual void Free() override;
};

END