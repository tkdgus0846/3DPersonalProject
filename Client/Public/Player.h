#pragma once

#include "Client_Defines.h"
#include "Creature.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CAnimInstance;
END

BEGIN(Client)

class CPlayer final : public CCreature
{
	friend class CAxe;
	friend class CSlasher;
	friend class CMace;
	friend class CHand;
public:
	enum COLLIDER { COLLIDER_PLAYER, COLLIDER_ATTACK, COLLIDER_SPHERE, COLLIDER_END };
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

	virtual void OnCollisionEnter(const Collision* collision) override;
	virtual void OnCollisionStay(const Collision* collision) override;
	virtual void OnCollisionExit(const Collision* collision) override;

private:
	void Move(_double TimeDelta);
	void PlayerRotate(_double TimeDelta);
	void AttackMove(_double TimeDelta);
	void ClimbNavMesh();
	_float Compute_NavMesh_Height();
	void CameraRotate(_double TimeDelta);
	void CameraZoom(_double TimeDelta);
	void Dash(_double TimeDelta);

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
	_bool						m_bDash = { false };

	_bool						m_bSkillQ = { false };
	_bool						m_bSkillC = { false };
	_bool						m_bSkillR = { false };
	_bool						m_bSkillE = { false };
	_bool						m_bSkillT = { false };
	_bool						m_bSkill = { false };

	// 카메라 제어 관련 변수들
	POINT						m_lastCursorPoint;
	_float						m_fCameraRotationX = { 0.f };
	_float						m_fCameraRotationY = { 0.f };
	_float						m_fCameraZoom = { 0.f };
	_float						m_fCurCameraZoom = { 0.f };
	_float3						m_OriginOffset = { 0.f, 12.f, -12.f };
	_double						m_ObjectAttack_CameraShake_TimeAcc = { 0.0 };

	// 이동 관련 변수들
	_float						m_CurHeight = { 0.f };
	_int						m_CurIndex = { -1 };
	_float3						m_MoveDir;
	_bool						m_bClimbNavMesh = { true };
	
	// 무기에 관한 정보들
	WEAPONTYPE					m_eWeaponType = { WEAPON_HAND };
	WEAPONTYPE					m_ePrevWeaponType = { WEAPON_HAND };
	vector<class CWeapon*>		m_Weapons[WEAPON_END];

	

	// Claw 무기에 대한 애니메이션 정보
	_int						m_Combo = { 0 };
	_int						m_PrevCombo = { 0 };
	const _int					m_ComboMax = { 3 };

	// 움직이면서 공격하는 것에 대한 변수들
	_int						m_MoveAttackCheckCombo = { 0 };
	_double						m_MoveAttackTimeAcc = { 0.0 };
	const _double				m_MoveAttackAccel = { -1.7 };
	const _double				m_MoveAttackInitSpeed = { 0.5 };
	_bool						m_bAttackMove = { false };

	// Dash 관련 변수들
	_float3						m_DashDir;
	_double						m_DashTimeAcc = { 0.0 };
	const _double				m_DashAccel = { 5.7 };
	const _double				m_DashInitSpeed = { -0.01 };

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