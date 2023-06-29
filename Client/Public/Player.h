#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CAnimInstance;
class CActorComponent;

END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum COLLIDER { COLLIDER_AABB, COLLIDER_OBB, COLLIDER_SPHERE, COLLIDER_END };
	enum WEAPONTYPE { WEAPON_HAND, WEAPON_AXE, WEAPON_SLASHER, WEAPON_MACE, WEAPON_END};

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Move(_double TimeDelta);
	void PlayerRotate(_double TimeDelta);
	void AttackMove(_double TimeDelta);
	void ClimbNavMesh();
	void CameraRotate(_double TimeDelta);
	void CameraZoom(_double TimeDelta);
	void Dash(_double TimeDelta);

	// 무기 타입에 따라서 처리 해주면 좋을거같다. 일단은 임시로 Attack_Claw
	void Attack_Combo();
	void Add_Animations();

	// 스킬들
	void Skills(const _double& TimeDelta);
	void Skill_Q(const _double& TimeDelta);
	void Skill_C(const _double& TimeDelta);
	void Skill_R(const _double& TimeDelta);
	void Skill_E(const _double& TimeDelta);
	void Skill_T(const _double& TimeDelta);

	// 무기 교체 관련 함수들
	void Select_Weapon();
	void Change_Weapon();

	/// 애니메이션 키 선택해주는 함수들
	void Select_ChangingWeaponKey();
	void Select_MoveKey();
	void Select_DashKey();
	void Select_AttackKey();
	void Select_IdleKey();

	void Select_AnimationKey();

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };	
	CRenderer*					m_pRendererCom = { nullptr };
	CCollider*					m_pColliderCom[COLLIDER_END] = { nullptr };
	class CCamera_Player_Main*	m_pCameraCom = { nullptr };
	CTransform*					m_pTransformCom = { nullptr };
	CNavigation*				m_pNavigationCom = { nullptr };
	CAnimInstance*				m_pAnimInstance = { nullptr };
	
	// 애님 상태 제어 변수들
	_bool						m_bMove = { false };
	_bool						m_bRotate = { false };
	_bool						m_bAttack = { false };
	_bool						m_bAttackMove = { false };
	_bool						m_bDash = { false };
	_bool						m_bSkillQ = { false };
	_bool						m_bSkillC = { false };
	_bool						m_bSkillR = { false };
	_bool						m_bSkillE = { false };
	_bool						m_bSkillT = { false };

	// 카메라 제어 관련 변수들
	POINT						m_lastCursorPoint;
	_float						m_fCameraRotationX = { 0.f };
	_float						m_fCameraRotationY = { 0.f };
	_float						m_fCameraZoom = { 0.f };
	_float						m_fCurCameraZoom = { 0.f };
	_float3						m_OriginOffset = { 0.f, 12.f, -12.f };

	// 이동 관련 변수들
	_float						m_CurHeight = { 0.f };
	_int						m_CurIndex = { -1 };
	_float3						m_MoveDir;
	
	// 무기에 관한 정보들
	WEAPONTYPE					m_eWeaponType = { WEAPON_HAND };
	WEAPONTYPE					m_ePrevWeaponType = { WEAPON_HAND };
	vector<CActorComponent*>	m_Weapons[WEAPON_END];

	

	// Claw 무기에 대한 애니메이션 정보
	_int						m_Combo = { 0 };
	_int						m_PrevCombo = { 0 };
	const _int					m_ComboMax = { 3 };

	// Dash 관련 변수들
	_float3						m_DashDir;
	_double						m_DashTimeAcc = { 0.0 };
	const _double				m_DashAccel = { 4.7 };
	const _double				m_DashInitSpeed = { -0.2 };

	_double						m_AxeDashTimeAcc = { 0.0 };
	const _double				m_AxeDashAccel = { -2.1 };
	const _double				m_AxeDashInitSpeed = { 1.01 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	/* 원형을 생성한다. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본(실제 사용할 객체)을 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END