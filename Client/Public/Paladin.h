#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class CAnimInstance;
class CNavigation;
END

BEGIN(Client)

class CPaladin final : public CMonster
{
protected:
	CPaladin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPaladin(const CPaladin& rhs);
	virtual ~CPaladin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	//void Ride() override;

	virtual void OnCollisionEnter(const Collision* collision);
	virtual void OnCollisionStay(const Collision* collision); 
	virtual void OnCollisionExit(const Collision* collision); 

private:
	void Add_Animations();

	void Attack();
	void Damaged(const _float& TimeDelta);
	void SuperArmor(const _float& TimeDelta);


	// ������ �������� ��ü�� �Ű��ִ� �۾��� �Ѵ�.
	void State();
	void Select_DamagedKey();
	void Select_MoveKey();
	void Select_IdleKey();
	void Select_AttackKey();
	void Select_AnimationKey();

private:
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };	
	CTransform*				m_pTransformCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };

	CCollider*				m_pBodyColliderCom = { nullptr };
	CCollider*				m_pDetectionColliderCom = { nullptr };
	CCollider*				m_pAttackRangeColliderCom = { nullptr };
	CAnimInstance*			m_pAnimInstance = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };


	// AI ���� ������
	_bool					m_isAttack1 = { false };
	_bool					m_isWalk = { false };
	_bool					m_isRun = { false };
	_bool					m_isDamaged = { false };
	_bool					m_isSuperArmor = { false };

	// �ǰݰ� ���۾Ƹ� ������
	_float					m_DamagedStunTimeAcc = { 0.f };
	_float					m_SuperArmorTimeAcc = { 0.f };
	const _float			m_DamagedStunTime = { 0.5f };
	const _float			m_SuperArmorTime = { 4.f };

	// �̵� ���� ������
	const _float			m_WalkSpeed = { 2.0f };
	const _float			m_RunSpeed = { 5.0f };
	
	
	


public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* ������ �����Ѵ�. */
	static CPaladin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻(���� ����� ��ü)�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END