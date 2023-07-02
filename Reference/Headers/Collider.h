#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
	friend class CCollisionManager;

public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(class CBounding* pBounding, void* pArg);

	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;

	void OnCollisionEnter(const Collision* collision);
	void OnCollisionStay(const Collision* collision);
	void OnCollisionExit(const Collision* collision);

public:
	void Set_Enable(_bool bEnabled) { m_bEnabled = bEnabled; }
	_bool Get_Enable() const { return m_bEnabled; }

	void Change_ColGroup(COLGROUP eGroup) { m_eColGroup = eGroup; }
	void Erase_FromOtherCollisionList();
	

#ifdef _DEBUG
	HRESULT Render();
#endif

public:
	_bool Intersect(const CCollider* pCollider);

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*									m_pEffect = { nullptr };
	ID3D11InputLayout*								m_pInputLayout = { nullptr };
#endif

private:
	TYPE							m_eColliderType = { TYPE_END };
	class CBounding*				m_pBounding = { nullptr };
	_bool							m_bEnabled = { true };
	// 사실상 이 Enabled 변수가 콜리젼매니저에 넣을지 말지도 결정한다.
	
	class CTransform*				m_pOwnerTransform = { nullptr };
	class CGameObject*				m_pOwnerObject = { nullptr };

	unordered_map<CCollider*, Collision>	m_CollisionList;
	COLGROUP						m_eColGroup = { COL_END };



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END