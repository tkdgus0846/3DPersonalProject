#pragma once

#include "Base.h"

/* 각종 컴포넌트들의 부모가 된다. */

BEGIN(Engine)

class CGameObject;

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

	const _tchar* GetName() const { return m_pName; }
	void SetName(const _tchar* newName) { m_pName = newName; }
	CGameObject* GetOwner() const { return m_pOwner; }
	void SetOwner(CGameObject* pObj)
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
	CGameObject*					m_pOwner = { nullptr };
	const _tchar*					m_pName = { nullptr };

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
	unordered_map<const _tchar*, CComponent*>* Get_ComponentsByUmap() { return &m_Components; }
	

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	unordered_map<const _tchar*, CComponent*>			m_Components;

public:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, _Inout_ CComponent * *ppOut, CGameObject* pOwner, void* pArg = nullptr);
	HRESULT Delete_Component(const _tchar* pComponentTag);

public:
	virtual void Free() override;
};

END